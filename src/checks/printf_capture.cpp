#include "checks/printf_capture.h"

#include <mutex>
#include <utility>

#include <spdlog/spdlog.h>

#include "checks/synthetic_clock.h"

std::vector<grader::PrintfCall> g_printfCalls;

namespace grader {
    namespace {
        std::mutex &capture_mutex() {
            static std::mutex m;
            return m;
        }
    }

    std::string_view serial_port_name(SerialPort p) noexcept {
        switch (p) {
            case SerialPort::SCIA: return "SCIA";
            case SerialPort::SCIB: return "SCIB";
            case SerialPort::SCIC: return "SCIC";
            case SerialPort::SCID: return "SCID";
            case SerialPort::UART_LCD: return "UART_LCD";
            case SerialPort::Unknown: return "Unknown";
        }
        return "?";
    }

    void resetPrintfCapture() noexcept {
        std::lock_guard<std::mutex> lk(capture_mutex());
        g_printfCalls.clear();
        synthetic_clock_reset();
    }

    void clearPrintfCalls() noexcept {
        std::lock_guard<std::mutex> lk(capture_mutex());
        g_printfCalls.clear();
    }

    const std::vector<PrintfCall> &getPrintfCalls() noexcept {
        return g_printfCalls;
    }

    std::vector<const PrintfCall *> getPrintfCallsForPort(SerialPort port) {
        std::vector<const PrintfCall *> out;
        out.reserve(g_printfCalls.size());
        for (const auto &call: g_printfCalls) {
            if (call.port == port) {
                out.push_back(&call);
            }
        }
        return out;
    }

    const PrintfCall *latestPrintfCall(SerialPort port) noexcept {
        for (auto it = g_printfCalls.rbegin(); it != g_printfCalls.rend(); ++it) {
            if (it->port == port) {
                return &(*it);
            }
        }
        return nullptr;
    }

    std::size_t printfCallCount(SerialPort port) noexcept {
        std::size_t n = 0;
        for (const auto &call: g_printfCalls) {
            if (call.port == port) {
                ++n;
            }
        }
        return n;
    }

    void recordPrintfCall(SerialPort port,
                          std::string_view fmt,
                          std::string_view rendered,
                          uint16_t lcd_line) {
        std::lock_guard<std::mutex> lk(capture_mutex());
        PrintfCall call;
        call.port = port;
        call.fmt.assign(fmt);
        call.rendered.assign(rendered);
        call.parsed = parse_format(fmt);
        call.synthetic_timestamp_us = synthetic_clock_now_us();
        call.sequence_index = g_printfCalls.size();
        call.lcd_line = lcd_line;
        if (!call.parsed.ok()) {
            spdlog::debug("recordPrintfCall: format string '{}' failed to parse: {}",
                          call.fmt, call.parsed.error);
        }
        g_printfCalls.push_back(std::move(call));
    }

    void dumpPrintfCalls() {
        spdlog::info("=== captured printf log ({} calls) ===", g_printfCalls.size());
        for (const auto &c: g_printfCalls) {
            spdlog::info("  [{}] t={}us {} fmt=\"{}\" rendered=\"{}\"",
                         c.sequence_index,
                         c.synthetic_timestamp_us,
                         serial_port_name(c.port),
                         c.fmt,
                         c.rendered);
        }
    }
}