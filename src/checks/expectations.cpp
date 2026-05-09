#include "checks/expectations.h"

#include <algorithm>
#include <cmath>
#include <string>

#include <spdlog/spdlog.h>

namespace grader {
    namespace {
        std::string make_label(const std::string_view check_name, const std::string_view default_label) {
            return std::string(check_name.empty() ? default_label : check_name);
        }

        void log_format_diff(const std::string &label,
                             std::string_view expected_fmt,
                             const PrintfCall &call) {
            spdlog::error("  expected fmt: \"{}\"", expected_fmt);
            spdlog::error("  actual   fmt: \"{}\"", call.fmt);
            if (!call.rendered.empty()) {
                spdlog::error("  rendered:    \"{}\"", call.rendered);
            }
            if (!call.parsed.ok()) {
                spdlog::error("  [{}] format parse error: {}", label, call.parsed.error);
            }
        }

        void emit_format_hint(const std::string &label,
                              const std::size_t arg_index,
                              const FormatSpec &expected,
                              const FormatSpec &actual) {
            using L = LengthModifier;
            using C = Conversion;

            const bool int_family = (expected.conversion == C::d || expected.conversion == C::i ||
                                     expected.conversion == C::u || expected.conversion == C::o ||
                                     expected.conversion == C::x || expected.conversion == C::X);
            const bool float_family = (expected.conversion == C::f || expected.conversion == C::F ||
                                       expected.conversion == C::e || expected.conversion == C::E ||
                                       expected.conversion == C::g || expected.conversion == C::G);

            if (int_family && expected.length == L::l && actual.length == L::None) {
                spdlog::error("  hint: argument #{} requires a 'long' (32-bit) format such as %ld — "
                              "you wrote %{}.  On the host build int is 32 bits so this LOOKS fine, but "
                              "on the TI C2000 target int is only 16 bits and your printout will be "
                              "wrong (truncated or corrupt).", arg_index + 1,
                              conversion_name(actual.conversion));
            }
            if (int_family && expected.length == L::None && actual.length == L::l) {
                spdlog::error("  hint: argument #{} expects a 16-bit int (%d) but you used %ld. "
                              "If your variable is declared 'int' on TI C2000 it is 16 bits.",
                              arg_index + 1);
            }

            if (float_family && expected.precision != actual.precision) {
                if (expected.precision >= 0 && actual.precision == kFormatWidthUnset) {
                    spdlog::error("  hint: argument #{} requires precision .{} (e.g. \"%.{}{}\") — "
                                  "you didn't specify any precision so printf defaults to 6 digits.",
                                  arg_index + 1, expected.precision,
                                  expected.precision, conversion_name(expected.conversion));
                } else {
                    spdlog::error("  hint: argument #{} requires precision .{}, you wrote .{}.",
                                  arg_index + 1, expected.precision, actual.precision);
                }
            }

            if (expected.conversion != actual.conversion) {
                spdlog::error("  hint: argument #{} requires conversion '{}', you wrote '{}'.",
                              arg_index + 1,
                              conversion_name(expected.conversion),
                              conversion_name(actual.conversion));
            }
        }

        bool format_matches(const ParsedFormat &expected, const PrintfCall &call,
                            const std::string &label, const std::string_view expected_fmt) {
            const auto &actual = call.parsed;
            if (!actual.ok()) {
                spdlog::error("[{}] could not parse student's format string: {}", label, actual.error);
                log_format_diff(label, expected_fmt, call);
                return false;
            }

            if (actual.specs.size() != expected.specs.size()) {
                spdlog::error("[{}] expected {} format specifier(s), got {}",
                              label, expected.specs.size(), actual.specs.size());
                log_format_diff(label, expected_fmt, call);
                if (actual.specs.size() < expected.specs.size()) {
                    spdlog::error("  hint: you are missing {} argument(s) — likely you stopped printing early.",
                                  expected.specs.size() - actual.specs.size());
                } else {
                    spdlog::error("  hint: you are printing {} extra argument(s).",
                                  actual.specs.size() - expected.specs.size());
                }
                return false;
            }

            bool ok = true;
            for (std::size_t i = 0; i < actual.specs.size(); ++i) {
                const auto &exp = expected.specs[i];
                if (const auto &act = actual.specs[i]; !format_specs_equivalent(exp, act)) {
                    spdlog::error("[{}] argument #{}: format specifier mismatch", label, i + 1);
                    spdlog::error("  expected: \"{}\"   ({})",
                                  format_spec_canonical(exp),
                                  arg_type_name(expected_arg_type(exp)));
                    spdlog::error("  got:      \"{}\"   ({})",
                                  format_spec_canonical(act),
                                  arg_type_name(expected_arg_type(act)));
                    emit_format_hint(label, i, exp, act);
                    ok = false;
                }
            }
            if (!ok) {
                log_format_diff(label, expected_fmt, call);
            }
            return ok;
        }
    }

    bool expect_format(const PrintfCall *call,
                       std::string_view expected_fmt,
                       const std::string_view check_name) {
        const std::string label = make_label(check_name, "expect_format");
        if (!call) {
            spdlog::error("[{}] no captured printf call to compare against. Expected fmt: \"{}\"",
                          label, expected_fmt);
            spdlog::error("  hint: did you call resetPrintfCapture() *and* run the timer ISR for "
                "long enough to fire at least one print? Check that UARTPrint == 1 was "
                "set inside cpu_timer2_isr at the right cadence.");
            return false;
        }
        auto expected = parse_format(expected_fmt);
        if (!expected.ok()) {
            spdlog::error("[{}] internal error: expected fmt \"{}\" failed to parse: {}",
                          label, expected_fmt, expected.error);
            return false;
        }
        return format_matches(expected, *call, label, expected_fmt);
    }

    bool expect_format(const PrintfCall &call,
                       const std::string_view expected_fmt,
                       const std::string_view check_name) {
        return expect_format(&call, expected_fmt, check_name);
    }

    bool expect_arg_types(const PrintfCall *call,
                          std::initializer_list<ArgType> types,
                          const std::string_view check_name) {
        const std::string label = make_label(check_name, "expect_arg_types");
        if (!call) {
            spdlog::error("[{}] no captured printf call available", label);
            return false;
        }
        if (!call->parsed.ok()) {
            spdlog::error("[{}] student's format failed to parse: {}", label, call->parsed.error);
            return false;
        }
        if (call->parsed.specs.size() != types.size()) {
            spdlog::error("[{}] expected {} argument(s), the format string has {}",
                          label, types.size(), call->parsed.specs.size());
            spdlog::error("  fmt: \"{}\"", call->fmt);
            return false;
        }
        bool ok = true;
        auto it = types.begin();
        for (std::size_t i = 0; i < call->parsed.specs.size(); ++i, ++it) {
            if (const ArgType actual_arg = expected_arg_type(call->parsed.specs[i]); actual_arg != *it) {
                spdlog::error("[{}] argument #{}: expected {}, got {} (specifier \"{}\")",
                              label, i + 1,
                              arg_type_name(*it),
                              arg_type_name(actual_arg),
                              format_spec_canonical(call->parsed.specs[i]));
                ok = false;
            }
        }
        if (!ok) {
            spdlog::error("  fmt: \"{}\"", call->fmt);
        }
        return ok;
    }

    bool expect_arg_types(const PrintfCall &call,
                          const std::initializer_list<ArgType> types,
                          const std::string_view check_name) {
        return expect_arg_types(&call, types, check_name);
    }

    bool expect_print_cadence(const SerialPort port,
                              std::size_t expected_count,
                              const double tolerance_pct,
                              const std::string_view check_name) {
        const std::string label = make_label(check_name, "expect_print_cadence");
        const std::size_t actual = printfCallCount(port);

        if (expected_count == 0) {
            if (actual != 0) {
                spdlog::error("[{}] expected 0 prints on {}, got {}",
                              label, serial_port_name(port), actual);
                return false;
            }
            return true;
        }

        const double dexp = static_cast<double>(expected_count);
        const double low = std::floor(dexp * (1.0 - tolerance_pct));
        if (const double high = std::ceil(dexp * (1.0 + tolerance_pct));
            static_cast<double>(actual) < low || static_cast<double>(actual) > high) {
            spdlog::error("[{}] expected ~{} prints on {} (tolerance ±{:.0f}% → window [{}, {}]), got {}",
                          label, expected_count,
                          serial_port_name(port),
                          tolerance_pct * 100.0,
                          static_cast<std::size_t>(low),
                          static_cast<std::size_t>(high),
                          actual);
            if (actual == 0) {
                spdlog::error("  hint: zero printf calls were captured. Common causes:");
                spdlog::error("        - the SCI driver wasn't initialised (init_serialSCIA / serial_printf gated)");
                spdlog::error("        - UARTPrint == 1 is never set inside cpu_timer2_isr");
                spdlog::error("        - the print is gated by 'if (CpuTimer2.InterruptCount % N == 0)' but "
                    "InterruptCount never reaches a multiple of N during the test window");
            } else if (static_cast<double>(actual) < low) {
                spdlog::error("  hint: under-printing — the modulus inside cpu_timer2_isr is too large, "
                    "or you skip prints when UARTPrint is already set.");
            } else {
                spdlog::error("  hint: over-printing — UARTPrint may be set on every ISR call instead "
                    "of every Nth call as the spec requires.");
            }
            return false;
        }
        return true;
    }

    bool expect_print_cadence_window(const SerialPort port,
                                     std::size_t min_count,
                                     std::size_t max_count,
                                     const std::string_view check_name) {
        const std::string label = make_label(check_name, "expect_print_cadence_window");
        if (const std::size_t actual = printfCallCount(port); actual < min_count || actual > max_count) {
            spdlog::error("[{}] expected {}..{} prints on {}, got {}",
                          label, min_count, max_count,
                          serial_port_name(port), actual);
            return false;
        }
        return true;
    }

    bool expect_min_print_calls(const SerialPort port,
                                std::size_t min_count,
                                const std::string_view check_name) {
        const std::string label = make_label(check_name, "expect_min_print_calls");
        if (const std::size_t actual = printfCallCount(port); actual < min_count) {
            spdlog::error("[{}] expected at least {} prints on {}, got {}",
                          label, min_count, serial_port_name(port), actual);
            return false;
        }
        return true;
    }

    bool expect_format_any(const SerialPort port,
                           std::string_view expected_fmt,
                           const std::string_view check_name) {
        const std::string label = make_label(check_name, "expect_format_any");
        auto expected = parse_format(expected_fmt);
        if (!expected.ok()) {
            spdlog::error("[{}] internal: could not parse expected format \"{}\": {}",
                          label, expected_fmt, expected.error);
            return false;
        }
        const auto calls = getPrintfCallsForPort(port);
        if (calls.empty()) {
            spdlog::error("[{}] no calls captured on {} to compare against fmt \"{}\"",
                          label, serial_port_name(port), expected_fmt);
            return false;
        }
        for (const auto *c: calls) {
            if (!c->parsed.ok()) continue;
            if (c->parsed.specs.size() != expected.specs.size()) continue;
            bool match = true;
            for (std::size_t i = 0; i < expected.specs.size(); ++i) {
                if (!format_specs_equivalent(expected.specs[i], c->parsed.specs[i])) {
                    match = false;
                    break;
                }
            }
            if (match) return true;
        }
        spdlog::error("[{}] no captured call on {} matched expected fmt \"{}\"",
                      label, serial_port_name(port), expected_fmt);
        spdlog::error("  searched {} call(s):", calls.size());
        for (const auto *c: calls) {
            spdlog::error("    - \"{}\"", c->fmt);
        }
        return false;
    }
}