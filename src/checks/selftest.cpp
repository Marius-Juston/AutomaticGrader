#include <cmath>
#include <cstring>
#include <string>

#include <spdlog/spdlog.h>

#include "checks/expectations.h"
#include "checks/format_parser.h"
#include "checks/printf_capture.h"
#include "checks/stimulus.hpp"
#include "checks/synthetic_clock.h"
#include "ti_stubs.h"

namespace grader {
    namespace {
        int g_failures = 0;

#define EXPECT(cond)                                                                        \
    do {                                                                                    \
        if (!(cond)) {                                                                      \
            spdlog::error("[selftest] FAIL at {}:{}: {}", __FILE__, __LINE__, #cond);       \
            ++g_failures;                                                                    \
        }                                                                                    \
    } while (0)

        void test_format_parser_basic() {
            const auto p = parse_format("Timeint = %ld, Time = %.2f sec, Input = %.3f, SatOut = %.2f\r\n");
            EXPECT(p.ok());
            EXPECT(p.specs.size() == 4u);
            EXPECT(p.specs[0].conversion == Conversion::d);
            EXPECT(p.specs[0].length == LengthModifier::l);
            EXPECT(p.specs[1].conversion == Conversion::f);
            EXPECT(p.specs[1].precision == 2);
            EXPECT(p.specs[1].length == LengthModifier::None);
            EXPECT(p.specs[2].precision == 3);
            EXPECT(p.specs[3].precision == 2);
            EXPECT(expected_arg_type(p.specs[0]) == ArgType::Int32);
            EXPECT(expected_arg_type(p.specs[1]) == ArgType::Float);
        }

        void test_format_parser_edge_cases() {
            auto empty = parse_format("");
            EXPECT(empty.ok());
            EXPECT(empty.specs.empty());

            auto trailing_pct = parse_format("hello %");
            EXPECT(!trailing_pct.ok());

            auto literal_pct = parse_format("100%% utilization");
            EXPECT(literal_pct.ok());
            EXPECT(literal_pct.specs.empty());

            auto width_prec = parse_format("%-10.3lf");
            EXPECT(width_prec.ok());
            EXPECT(width_prec.specs.size() == 1u);
            EXPECT(width_prec.specs[0].width == 10);
            EXPECT(width_prec.specs[0].precision == 3);
            EXPECT(width_prec.specs[0].length == LengthModifier::l);
            EXPECT(width_prec.specs[0].conversion == Conversion::f);
            EXPECT((width_prec.specs[0].flags & flag_bits::Minus) != 0);

            auto zero_pad = parse_format("%05d");
            EXPECT(zero_pad.ok());
            EXPECT(zero_pad.specs.size() == 1u);
            EXPECT((zero_pad.specs[0].flags & flag_bits::Zero) != 0);
            EXPECT(zero_pad.specs[0].width == 5);
            EXPECT(zero_pad.specs[0].length == LengthModifier::None);

            auto hex_upper = parse_format("0x%08lX");
            EXPECT(hex_upper.ok());
            EXPECT(hex_upper.specs.size() == 1u);
            EXPECT(hex_upper.specs[0].conversion == Conversion::X);
            EXPECT(hex_upper.specs[0].length == LengthModifier::l);
            EXPECT(expected_arg_type(hex_upper.specs[0]) == ArgType::UInt32);

            auto string_pct = parse_format("name=%s id=%lu\n");
            EXPECT(string_pct.ok());
            EXPECT(string_pct.specs.size() == 2u);
            EXPECT(string_pct.specs[0].conversion == Conversion::s);
            EXPECT(string_pct.specs[1].conversion == Conversion::u);
            EXPECT(string_pct.specs[1].length == LengthModifier::l);

            auto unknown = parse_format("hi %q there");
            EXPECT(!unknown.ok());

            auto canon = format_spec_canonical(width_prec.specs[0]);
            EXPECT(canon == "%-10.3lf");
        }

        void test_format_specs_equivalent() {
            auto a = parse_format("%.2f");
            auto b = parse_format("%.2f");
            auto c = parse_format("%f");
            auto d = parse_format("%.3f");
            auto e = parse_format("%ld");
            auto f = parse_format("%d");
            EXPECT(a.ok() && b.ok() && c.ok() && d.ok() && e.ok() && f.ok());
            EXPECT(format_specs_equivalent(a.specs[0], b.specs[0]));
            EXPECT(!format_specs_equivalent(a.specs[0], c.specs[0]));
            EXPECT(!format_specs_equivalent(a.specs[0], d.specs[0]));
            EXPECT(!format_specs_equivalent(e.specs[0], f.specs[0]));
        }

        void test_synthetic_clock() {
            synthetic_clock_reset();
            EXPECT(synthetic_clock_now_us() == 0u);
            synthetic_clock_advance(123);
            EXPECT(synthetic_clock_now_us() == 123u);
            synthetic_clock_advance(0);
            EXPECT(synthetic_clock_now_us() == 123u);

            int counter = 0;
            auto isr = [&counter]() { ++counter; };
            run_isr_for_us(isr, 1000, 5500);
            EXPECT(counter == 5);
            EXPECT(synthetic_clock_now_us() == 5123u);

            synthetic_clock_reset();
            EXPECT(synthetic_clock_now_us() == 0u);
        }

        void test_printf_capture_pipeline() {
            resetPrintfCapture();
            EXPECT(getPrintfCalls().empty());

            serial_printf(&SerialA, const_cast<char *>("Timeint = %ld, value = %.2f\r\n"), 42L, 3.14);
            EXPECT(g_printfCalls.size() == 1u);
            EXPECT(g_printfCalls[0].port == SerialPort::SCIA);
            EXPECT(g_printfCalls[0].parsed.specs.size() == 2u);
            EXPECT(g_printfCalls[0].parsed.specs[0].length == LengthModifier::l);
            EXPECT(g_printfCalls[0].parsed.specs[0].conversion == Conversion::d);
            EXPECT(g_printfCalls[0].parsed.specs[1].conversion == Conversion::f);
            EXPECT(g_printfCalls[0].parsed.specs[1].precision == 2);
            EXPECT(g_printfCalls[0].rendered.find("Timeint = 42") != std::string::npos);

            synthetic_clock_advance(50000);
            serial_printf(&SerialA, const_cast<char *>("Second print %ld\r\n"), 7L);
            EXPECT(g_printfCalls.size() == 2u);
            EXPECT(g_printfCalls[1].synthetic_timestamp_us > g_printfCalls[0].synthetic_timestamp_us);
            EXPECT(printfCallCount(SerialPort::SCIA) == 2u);

            const auto sciaCalls = getPrintfCallsForPort(SerialPort::SCIA);
            EXPECT(sciaCalls.size() == 2u);
            auto *latest = latestPrintfCall(SerialPort::SCIA);
            EXPECT(latest != nullptr);
            EXPECT(latest && latest->parsed.specs.size() == 1u);

            EXPECT(latestPrintfCall(SerialPort::SCIB) == nullptr);
        }

        void test_expect_format_pointed_errors() {
            resetPrintfCapture();
            serial_printf(&SerialA, const_cast<char *>("Timeint = %d, Time = %.2f\r\n"), 42, 3.14);
            auto *call = latestPrintfCall(SerialPort::SCIA);
            EXPECT(call != nullptr);

            const bool ok_match = expect_format(call, "Timeint = %d, Time = %.2f\r\n", "selftest_match");
            EXPECT(ok_match);

            const bool ok_mismatch = expect_format(call, "Timeint = %ld, Time = %.2f\r\n", "selftest_mismatch");
            EXPECT(!ok_mismatch);

            const bool ok_argtype_match = expect_arg_types(call, {ArgType::Int16, ArgType::Float}, "selftest_arg");
            EXPECT(ok_argtype_match);

            const bool ok_argtype_mismatch = expect_arg_types(call, {ArgType::Int32, ArgType::Float},
                                                              "selftest_argmismatch");
            EXPECT(!ok_argtype_mismatch);
        }

        void test_expect_print_cadence() {
            resetPrintfCapture();
            auto isr = []() {
                serial_printf(&SerialA, const_cast<char *>("tick %ld\r\n"), 1L);
            };
            run_isr_for_us(isr, 5000, 1'000'000);
            EXPECT(printfCallCount(SerialPort::SCIA) == 200u);
            EXPECT(expect_print_cadence(SerialPort::SCIA, 200, 0.0, "selftest_cadence_exact"));
            EXPECT(expect_print_cadence(SerialPort::SCIA, 180, 0.20, "selftest_cadence_window"));
            EXPECT(!expect_print_cadence(SerialPort::SCIA, 50, 0.10, "selftest_cadence_fail_low"));
            EXPECT(!expect_print_cadence(SerialPort::SCIA, 500, 0.10, "selftest_cadence_fail_high"));
            EXPECT(expect_print_cadence_window(SerialPort::SCIA, 150, 250, "selftest_window"));
            EXPECT(!expect_print_cadence_window(SerialPort::SCIA, 0, 100, "selftest_window_fail"));
        }

        void test_stimulus_buttons() {
            GpioDataRegs.GPADAT.all = 0;
            press_button(4);
            EXPECT(button_state(4) == false);
            release_button(4);
            EXPECT(button_state(4) == true);
            press_button(7);
            release_button(4);
            EXPECT(button_state(4) == true);
            EXPECT(button_state(7) == false);

            set_gpio_input(34, true);
            EXPECT(read_gpio_input(34) == true);
            set_gpio_input(34, false);
            EXPECT(read_gpio_input(34) == false);

            GpioDataRegs.GPADAT.all = 0;
            GpioDataRegs.GPBDAT.all = 0;
        }

        void test_stimulus_adc() {
            clear_adc_results(AdcModule::A);
            inject_adc_result(AdcModule::A, 0, 4095);
            EXPECT(read_adc_result(AdcModule::A, 0) == 4095);
            inject_adc_result(AdcModule::A, 0, 0);
            EXPECT(read_adc_result(AdcModule::A, 0) == 0);
            inject_adc_result(AdcModule::B, 5, 1234);
            EXPECT(read_adc_result(AdcModule::B, 5) == 1234);
            inject_adc_result(AdcModule::A, 99, 1);
            inject_adc_result(AdcModule::A, -1, 1);
        }

        void test_stimulus_spi() {
            clear_spi_state(SpiModule::B);
            inject_spi_rx(SpiModule::B, 0xBEEF);
            EXPECT(SpibRegs.SPIRXBUF == 0xBEEF);
            EXPECT(spi_rx_fifo_size(SpiModule::B) == 1);
            clear_spi_state(SpiModule::B);
            EXPECT(spi_rx_fifo_size(SpiModule::B) == 0);
            constexpr uint16_t words[] = {0x1, 0x2, 0x3};
            inject_spi_rx(SpiModule::B, std::span<const uint16_t>(words, 3));
            EXPECT(SpibRegs.SPIRXBUF == 0x3);
            EXPECT(spi_rx_fifo_size(SpiModule::B) == 3);
            clear_spi_state();
        }

        void test_stimulus_encoder() {
            inject_encoder_count(EqepModule::Eqep1, 12345);
            EXPECT(read_encoder_count(EqepModule::Eqep1) == 12345);
            inject_encoder_count(EqepModule::Eqep1, -1);
            EXPECT(read_encoder_count(EqepModule::Eqep1) == -1);
        }
    }

    int run_selftest() {
        g_failures = 0;
        spdlog::info("=== running grader infrastructure self-test ===");
        test_format_parser_basic();
        test_format_parser_edge_cases();
        test_format_specs_equivalent();
        test_synthetic_clock();
        test_printf_capture_pipeline();
        test_expect_format_pointed_errors();
        test_expect_print_cadence();
        test_stimulus_buttons();
        test_stimulus_adc();
        test_stimulus_spi();
        test_stimulus_encoder();

        resetPrintfCapture();
        reset_all_stimulus();

        if (g_failures == 0) {
            spdlog::info("=== self-test PASSED ===");
            return 0;
        }
        spdlog::error("=== self-test FAILED ({} assertion failures) ===", g_failures);
        return g_failures;
    }
}