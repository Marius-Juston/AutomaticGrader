//
// Created by marius on 2/20/26.
//
#include "checks/validator.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cxxabi.h>
#include <dlfcn.h>
#include <fstream>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/base_sink.h"

#include "checks/main_loop_driver.h"

#if HW == 1
#include "checks/hw1.h"
#elif HW == 2
#include "checks/hw2.h"
#elif HW == 3
#include "checks/hw3.h"
#elif HW == 4
#include "checks/hw4.h"
#elif HW == 5
#include "checks/hw5.h"
#else
#error "Invalid HW selection"
#endif

namespace {
    class CaptureSink final : public spdlog::sinks::base_sink<std::mutex> {
    public:
        std::vector<std::string> messages;

    protected:
        void sink_it_(const spdlog::details::log_msg &msg) override {
            spdlog::memory_buf_t formatted;
            formatter_->format(msg, formatted);
            messages.emplace_back(fmt::to_string(formatted));
        }

        void flush_() override {
        }
    };

    std::string resolve_check_name(const CheckFunction fp, const std::size_t fallback_index) {
        Dl_info info{};
        if (dladdr(reinterpret_cast<void *>(fp), &info) != 0 && info.dli_sname != nullptr) {
            int status = 0;
            char *demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
            std::string name = (status == 0 && demangled != nullptr)
                                   ? std::string(demangled)
                                   : std::string(info.dli_sname);
            std::free(demangled);
            // Strip parameter list and any namespace prefix for a friendlier display.
            if (const auto paren = name.find('('); paren != std::string::npos) {
                name.resize(paren);
            }
            if (const auto colon = name.rfind("::"); colon != std::string::npos) {
                name = name.substr(colon + 2);
            }
            return name;
        }
        return "check_" + std::to_string(fallback_index);
    }

    std::string json_escape(const std::string &s) {
        std::string out;
        out.reserve(s.size() + 2);
        for (const unsigned char c: s) {
            switch (c) {
                case '"': out += "\\\"";
                    break;
                case '\\': out += "\\\\";
                    break;
                case '\b': out += "\\b";
                    break;
                case '\f': out += "\\f";
                    break;
                case '\n': out += "\\n";
                    break;
                case '\r': out += "\\r";
                    break;
                case '\t': out += "\\t";
                    break;
                default:
                    if (c < 0x20) {
                        char buf[8];
                        std::snprintf(buf, sizeof(buf), "\\u%04x", c);
                        out += buf;
                    } else {
                        out += static_cast<char>(c);
                    }
            }
        }
        return out;
    }

    void write_json_report(const std::string &path,
                           const std::vector<CheckResult> &results,
                           const int hw_id) {
        std::ofstream f(path);
        if (!f) {
            spdlog::warn("Could not open --report-json path '{}'", path);
            return;
        }
        int passed = 0;
        int failed = 0;
        for (const auto &r: results) (r.passed ? passed : failed)++;
        f << "{\n";
        f << "  \"assignment\": \"HW" << hw_id << "\",\n";
        f << "  \"passed\": " << passed << ",\n";
        f << "  \"failed\": " << failed << ",\n";
        f << "  \"checks\": [\n";
        for (std::size_t i = 0; i < results.size(); ++i) {
            const auto &[name, passed, messages] = results[i];
            f << "    {\n";
            f << "      \"name\": \"" << json_escape(name) << "\",\n";
            f << "      \"status\": \"" << (passed ? "pass" : "fail") << "\",\n";
            f << "      \"messages\": [";
            for (std::size_t j = 0; j < messages.size(); ++j) {
                if (j) f << ", ";
                f << "\"" << json_escape(messages[j]) << "\"";
            }
            f << "]\n";
            f << "    }" << (i + 1 == results.size() ? "" : ",") << "\n";
        }
        f << "  ]\n";
        f << "}\n";
    }
} // namespace


Validator::Validator(const std::vector<CheckFunction> &checkFunctions) : checkFunctions(checkFunctions) {
}

void Validator::start_main_thread() {
    // Cooperative model: run the student's main() once with the patched
    // GRADER_MAIN_LOOP set to 0 iterations. Init code populates all
    // peripheral globals, while(1)-body executes zero times, main returns.
    // No detached thread = no race against ISR-driving test code.
    grader::run_student_init();
}

int Validator::check() {
    int result = 1;

    // Install a per-run capture sink alongside the existing default sinks so all
    // spdlog output keeps streaming to the console while we record it per-check.
    const auto capture = std::make_shared<CaptureSink>();
    const auto logger = spdlog::default_logger();
    logger->sinks().push_back(capture);

    check_results.reserve(checkFunctions.size());

    for (std::size_t i = 0; i < checkFunctions.size(); ++i) {
        const std::size_t before = capture->messages.size();
        int rc;
        try {
            rc = checkFunctions[i](this);
        } catch (const std::exception &e) {
            spdlog::error("Check threw std::exception: {}", e.what());
            rc = 0;
        } catch (...) {
            spdlog::error("Check threw non-std exception");
            rc = 0;
        }
        result &= rc;

        std::vector<std::string> per_check_msgs(
            capture->messages.begin() + static_cast<std::ptrdiff_t>(before),
            capture->messages.end());

        check_results.push_back({
            resolve_check_name(checkFunctions[i], i),
            rc != 0,
            std::move(per_check_msgs),
        });
    }

    // Remove the capture sink so subsequent activity doesn't keep growing it.
    auto &sinks = logger->sinks();
    std::erase(sinks,
               std::static_pointer_cast<spdlog::sinks::sink>(capture));

    if (!json_report_path.empty()) {
        write_json_report(json_report_path, check_results, HW);
    }

    return result;
}

__attribute__ ((weak))

CheckFunctions checker() {
    return {};
}

Validator get_validator() {
    return Validator(checker());
}
