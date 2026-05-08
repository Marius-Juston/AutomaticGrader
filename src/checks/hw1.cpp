//
// Created by marius on 2/20/26.
//

#include "checks/hw1.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <ostream>
#include <thread>

#include "checks/compare_generated.hpp"
#include "checks/expectations.h"
#include "checks/printf_capture.h"
#include "checks/state_checker.h"
#include "checks/stimulus.hpp"
#include "checks/synthetic_clock.h"
#include "checks/validator.h"

#define LAUNCHPAD_CPU_FREQUENCY 200

extern "C" {
extern uint32_t numTimer0calls;
extern uint16_t LEDdisplaynum;
}


int check_initialization(Validator *val) {
    HardwareStateValidator validator;
    validator.populate_all_zero();

    spdlog::info("Check initialization");

    int success = 1;

    success &= validator.validate();

    val->start_main_thread();

    {
        GpioSetup expected[MAX_GPIO];
        for (auto &i: expected) {
            i = {};
        }

        expected[31] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[34] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[22] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[94] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[95] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[97] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[111] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[130] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[131] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[25] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[26] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[27] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[60] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[61] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[157] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[158] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[159] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[160] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[0] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[1] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[19] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[29] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[32] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[9] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[66] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[125] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
        expected[4] = {GPIO_MUX_CPU1, 0, GPIO_INPUT, GPIO_PULLUP};
        expected[5] = {GPIO_MUX_CPU1, 0, GPIO_INPUT, GPIO_PULLUP};
        expected[6] = {GPIO_MUX_CPU1, 0, GPIO_INPUT, GPIO_PULLUP};
        expected[7] = {GPIO_MUX_CPU1, 0, GPIO_INPUT, GPIO_PULLUP};
        expected[8] = {GPIO_MUX_CPU1, 0, GPIO_INPUT, GPIO_PULLUP};

        validator.register_comparison("GpioSetup", gpiosSetup, expected);
    }

    {
        CPUTIMER_VARS expected = {};
        ConfigCpuTimer(&expected, LAUNCHPAD_CPU_FREQUENCY, 10000);
        validator.register_comparison("CpuTimer0", CpuTimer0, expected);

        ConfigCpuTimer(&expected, LAUNCHPAD_CPU_FREQUENCY, 20000);
        validator.register_comparison("CpuTimer1", CpuTimer1, expected);

        ConfigCpuTimer(&expected, LAUNCHPAD_CPU_FREQUENCY, 5000);
        validator.register_comparison("CpuTimer2", CpuTimer2, expected);
    }

    {
        CPUTIMER_REGS expected = {};
        expected.TCR.all = 0x4000;
        validator.register_comparison("CpuTimer0Regs", CpuTimer0Regs, expected);
        validator.register_comparison("CpuTimer1Regs", CpuTimer1Regs, expected);
        validator.register_comparison("CpuTimer2Regs", CpuTimer2Regs, expected);
    }


    {
        constexpr uint16_t expected = M_INT1 | M_INT8 | M_INT9 | M_INT12 | M_INT13 | M_INT14;
        validator.register_custom("IER", IER, expected);
    }

    {
        PIE_CTRL_REGS expected = {};
        expected.PIEIER1.bit.INTx7 = 1;
        expected.PIEIER12.bit.INTx9 = 1;
        validator.register_comparison("PieCtrlRegs", PieCtrlRegs, expected);
    }

    {
        PIE_VECT_TABLE expected = {};

        expected.TIMER0_INT = &cpu_timer0_isr;
        expected.TIMER1_INT = &cpu_timer1_isr;
        expected.TIMER2_INT = &cpu_timer2_isr;
        expected.SCIA_RX_INT = &RXAINT_recv_ready;
        expected.SCIB_RX_INT = &RXBINT_recv_ready;
        expected.SCIC_RX_INT = &RXCINT_recv_ready;
        expected.SCID_RX_INT = &RXDINT_recv_ready;
        expected.SCIA_TX_INT = &TXAINT_data_sent;
        expected.SCIB_TX_INT = &TXBINT_data_sent;
        expected.SCIC_TX_INT = &TXCINT_data_sent;
        expected.SCID_TX_INT = &TXDINT_data_sent;
        expected.EMIF_ERROR_INT = &SWI_isr;

        validator.register_comparison("PieVectTable", PieVectTable, expected);
    }

    {
        GPIO_DATA_REGS expected = {};

        expected.GPASET.bit.GPIO31 = 1;
        expected.GPBSET.bit.GPIO34 = 1;
        expected.GPACLEAR.bit.GPIO22 = 1;
        expected.GPCCLEAR.bit.GPIO94 = 1;
        expected.GPCCLEAR.bit.GPIO95 = 1;
        expected.GPDCLEAR.bit.GPIO97 = 1;
        expected.GPDCLEAR.bit.GPIO111 = 1;
        expected.GPECLEAR.bit.GPIO130 = 1;
        expected.GPECLEAR.bit.GPIO131 = 1;
        expected.GPACLEAR.bit.GPIO25 = 1;
        expected.GPACLEAR.bit.GPIO26 = 1;
        expected.GPACLEAR.bit.GPIO27 = 1;
        expected.GPBCLEAR.bit.GPIO60 = 1;
        expected.GPBCLEAR.bit.GPIO61 = 1;
        expected.GPECLEAR.bit.GPIO157 = 1;
        expected.GPECLEAR.bit.GPIO158 = 1;
        expected.GPECLEAR.bit.GPIO159 = 1;
        expected.GPFCLEAR.bit.GPIO160 = 1;
        expected.GPASET.bit.GPIO0 = 1;
        expected.GPASET.bit.GPIO1 = 1;
        expected.GPASET.bit.GPIO19 = 1;
        expected.GPASET.bit.GPIO29 = 1;
        expected.GPBSET.bit.GPIO32 = 1;
        expected.GPASET.bit.GPIO9 = 1;
        expected.GPCSET.bit.GPIO66 = 1;
        expected.GPDSET.bit.GPIO125 = 1;

        validator.register_comparison_copy("GpioDataRegs", GpioDataRegs, expected);
    }

    success &= validator.validate();

    {
        bool state_changed = updateGPIOState();

        spdlog::debug("GPIO state changed: {}", state_changed);

        GPIO_DATA_REGS expected = {};

        expected.GPADAT.bit.GPIO31 = 1;
        expected.GPBDAT.bit.GPIO34 = 1;
        expected.GPADAT.bit.GPIO0 = 1;
        expected.GPADAT.bit.GPIO1 = 1;
        expected.GPADAT.bit.GPIO19 = 1;
        expected.GPADAT.bit.GPIO29 = 1;
        expected.GPBDAT.bit.GPIO32 = 1;
        expected.GPADAT.bit.GPIO9 = 1;
        expected.GPCDAT.bit.GPIO66 = 1;
        expected.GPDDAT.bit.GPIO125 = 1;

        validator.register_comparison_copy("GpioDataRegs Set Data", GpioDataRegs, expected);
    }

    success &= validator.validate();

    return success;
}

int check_timer0(Validator *) {
    HardwareStateValidator validator;
    spdlog::info("Check timer 0");

    const GPIO_DATA_REGS initialGpio = GpioDataRegs;
    const uint32_t initialNumTimer0Calls = numTimer0calls;
    const uint16_t initialLEDdisplaynum = LEDdisplaynum;
    const uint32_t initialInterruptCount = CpuTimer0.InterruptCount;

    validator.mark_as_used("GpioDataRegs");

    constexpr uint32_t calls = 250;

    for (uint32_t i = 0; i < calls; ++i) {
        cpu_timer0_isr();
    }

    {
        const uint32_t expected = initialNumTimer0Calls + calls;
        validator.register_custom_copy("numTimer0calls", numTimer0calls, expected);
    }
    {
        const uint32_t expected = initialInterruptCount + calls;
        validator.register_custom_copy("CpuTimer0.InterruptCount", CpuTimer0.InterruptCount, expected);
    }
    {
        // Reference increments LEDdisplaynum once per 250 ticks (HW1fy_main.c:344).
        const uint16_t expected = initialLEDdisplaynum + (calls / 250);
        validator.register_custom_copy("LEDdisplaynum", LEDdisplaynum, expected);
    }
    {
        // Red LED on GPIO34 must be toggled — reference writes
        // GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1 every call (HW1fy_main.c:353).
        // The TOGGLE bit is sticky in the stub; after any call it must be 1.
        GPIO_DATA_REGS expected = initialGpio;
        expected.GPBTOGGLE.bit.GPIO34 = 1;
        validator.register_custom_copy("Timer0 GPIO34 toggle", GpioDataRegs, expected);
    }

    numTimer0calls = initialNumTimer0Calls;
    LEDdisplaynum = initialLEDdisplaynum;
    CpuTimer0.InterruptCount = initialInterruptCount;
    GpioDataRegs = initialGpio;

    return validator.validate();
}

int check_timer1(Validator *) {
    HardwareStateValidator validator;
    spdlog::info("Check timer 1");

    const uint32_t initialInterruptCount = CpuTimer1.InterruptCount;

    constexpr uint32_t calls = 100;

    for (uint32_t i = 0; i < calls; ++i) {
        cpu_timer1_isr();
    }

    {
        // Reference's cpu_timer1_isr only increments InterruptCount (HW1fy_main.c:364).
        const uint32_t expected = initialInterruptCount + calls;
        validator.register_custom_copy("CpuTimer1.InterruptCount", CpuTimer1.InterruptCount, expected);
    }

    CpuTimer1.InterruptCount = initialInterruptCount;

    return validator.validate();
}

namespace {
    bool toggle_bit_set(uint32_t reg_all, int bit_in_bucket) noexcept {
        return (reg_all & (uint32_t{1} << bit_in_bucket)) != 0;
    }

    void clear_all_toggle_regs() noexcept {
        GpioDataRegs.GPATOGGLE.all = 0;
        GpioDataRegs.GPBTOGGLE.all = 0;
        GpioDataRegs.GPCTOGGLE.all = 0;
        GpioDataRegs.GPDTOGGLE.all = 0;
        GpioDataRegs.GPETOGGLE.all = 0;
        GpioDataRegs.GPFTOGGLE.all = 0;
    }

    struct Hw1Phase3Snapshot {
        GPIO_DATA_REGS data{};
        uint16_t uart_print{0};
        uint32_t interrupt_count{0};
    };

    Hw1Phase3Snapshot take_snapshot() {
        return Hw1Phase3Snapshot{GpioDataRegs, UARTPrint, CpuTimer2.InterruptCount};
    }

    void restore_snapshot(const Hw1Phase3Snapshot &s) {
        GpioDataRegs = s.data;
        UARTPrint = s.uart_print;
        CpuTimer2.InterruptCount = s.interrupt_count;
    }

    bool report(bool cond, const char *label, const char *hint = nullptr) {
        if (!cond) {
            spdlog::error("[check_timer2] {}", label);
            if (hint) {
                spdlog::error("  spec: {}", hint);
            }
        }
        return cond;
    }
}

// Spec-strict check_timer2 (HW1.tex Exercise 9):
//   - Always toggle blue LED10 (GPIO27, GPATOGGLE) and LED11 (GPIO60, GPBTOGGLE)
//     every 100 ms tick of CpuTimer2.
//   - GPIO31 (the "BLUE" status LED) must toggle every ISR invocation.
//   - If PB1 (GPIO4, active-low) is pressed at the 100 ms boundary, also toggle
//     LED12 (GPIO61, GPBTOGGLE) and LED13 (GPIO157, GPETOGGLE).
//   - If PB4 (GPIO7) is pressed, toggle LED14 (GPIO158) and LED15 (GPIO159).
//
// We accept either a level-based or edge-on-press student implementation: each
// sub-check runs a single primer ISR call with the buttons released so that any
// student edge-detector latches `prev_button=1`, then presses the button and
// drives 100 ms worth of ticks. A spec-compliant implementation must emit the
// LED toggle within that window; the LABstarter reference's edge-on-RELEASE
// logic is correctly caught as a spec deviation.
int check_timer2(Validator *) {
    spdlog::info("Check timer 2 (spec-strict)");

    const Hw1Phase3Snapshot baseline = take_snapshot();
    int success = 1;

    const uint32_t period_us = static_cast<uint32_t>(CpuTimer2.PeriodInUSec);
    if (period_us == 0) {
        spdlog::error("[check_timer2] CpuTimer2.PeriodInUSec is 0 — student did not call ConfigCpuTimer");
        restore_snapshot(baseline);
        return 0;
    }
    const size_t ticks_per_100ms = 100000u / period_us;
    if (ticks_per_100ms == 0) {
        spdlog::error("[check_timer2] CpuTimer2 period {} us yields zero ticks per 100 ms boundary",
                      period_us);
        restore_snapshot(baseline);
        return 0;
    }

    auto run_window = [&](const char *label, bool press_pb1, bool press_pb4) {
        // Reset registers and counter for a clean window.
        GpioDataRegs = baseline.data;
        CpuTimer2.InterruptCount = 0;
        UARTPrint = 0;
        clear_all_toggle_regs();

        // Primer: one ISR call with all buttons released — lets edge-detector
        // students latch `prev_button = 1`.
        grader::release_button(4);
        grader::release_button(5);
        grader::release_button(6);
        grader::release_button(7);
        grader::release_button(8);
        cpu_timer2_isr();
        clear_all_toggle_regs();
        CpuTimer2.InterruptCount = 0;

        if (press_pb1) grader::press_button(4);
        else grader::release_button(4);
        if (press_pb4) grader::press_button(7);
        else grader::release_button(7);

        // Drive enough ticks to span at least one 100 ms boundary.
        for (size_t i = 0; i < ticks_per_100ms + 2; ++i) {
            cpu_timer2_isr();
        }
        spdlog::debug("[check_timer2/{}] CpuTimer2.InterruptCount = {} after window",
                      label, static_cast<uint32_t>(CpuTimer2.InterruptCount));
    };

    // ----- Sub-check A: all buttons released through the boundary -----
    run_window("released", false, false);
    success &= report(toggle_bit_set(GpioDataRegs.GPATOGGLE.all, 31),
                      "released-buttons: GPIO31 (status BLUE LED) toggle bit not set",
                      "every ISR call must execute GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1")
                   ? 1
                   : 0;
    success &= report(toggle_bit_set(GpioDataRegs.GPATOGGLE.all, 27),
                      "released-buttons: GPIO27 (LED10) toggle bit not set at 100 ms boundary",
                      "every 100 ms boundary must toggle LED10 (GPIO27)")
                   ? 1
                   : 0;
    success &= report(toggle_bit_set(GpioDataRegs.GPBTOGGLE.all, 60 - 32),
                      "released-buttons: GPIO60 (LED11) toggle bit not set at 100 ms boundary",
                      "every 100 ms boundary must toggle LED11 (GPIO60)")
                   ? 1
                   : 0;
    success &= report(!toggle_bit_set(GpioDataRegs.GPBTOGGLE.all, 61 - 32),
                      "released-buttons: GPIO61 (LED12) toggled when PB1 was released",
                      "LED12/13 must toggle ONLY when PB1 (GPIO4 == 0) is held")
                   ? 1
                   : 0;
    success &= report(!toggle_bit_set(GpioDataRegs.GPETOGGLE.all, 157 - 128),
                      "released-buttons: GPIO157 (LED13) toggled when PB1 was released",
                      "LED12/13 must toggle ONLY when PB1 is held")
                   ? 1
                   : 0;
    success &= report(!toggle_bit_set(GpioDataRegs.GPETOGGLE.all, 158 - 128),
                      "released-buttons: GPIO158 (LED14) toggled when PB4 was released",
                      "LED14/15 must toggle ONLY when PB4 (GPIO7 == 0) is held")
                   ? 1
                   : 0;
    success &= report(!toggle_bit_set(GpioDataRegs.GPETOGGLE.all, 159 - 128),
                      "released-buttons: GPIO159 (LED15) toggled when PB4 was released",
                      "LED14/15 must toggle ONLY when PB4 is held")
                   ? 1
                   : 0;

    // ----- Sub-check B: PB1 held pressed -----
    run_window("PB1-pressed", true, false);
    success &= report(toggle_bit_set(GpioDataRegs.GPBTOGGLE.all, 61 - 32),
                      "PB1-pressed: GPIO61 (LED12) toggle bit not set",
                      "spec Ex.9: 'If PB1 (GPIO4) pressed (DAT==0), toggle LED12 (GPIO61)'")
                   ? 1
                   : 0;
    success &= report(toggle_bit_set(GpioDataRegs.GPETOGGLE.all, 157 - 128),
                      "PB1-pressed: GPIO157 (LED13) toggle bit not set",
                      "spec Ex.9: '...toggle LED13 (GPIO157)'")
                   ? 1
                   : 0;
    success &= report(!toggle_bit_set(GpioDataRegs.GPETOGGLE.all, 158 - 128),
                      "PB1-pressed: GPIO158 toggled while PB4 was released",
                      "PB4 LEDs must remain idle while only PB1 is pressed")
                   ? 1
                   : 0;
    success &= report(!toggle_bit_set(GpioDataRegs.GPETOGGLE.all, 159 - 128),
                      "PB1-pressed: GPIO159 toggled while PB4 was released",
                      "PB4 LEDs must remain idle while only PB1 is pressed")
                   ? 1
                   : 0;

    // ----- Sub-check C: PB4 held pressed -----
    run_window("PB4-pressed", false, true);
    success &= report(toggle_bit_set(GpioDataRegs.GPETOGGLE.all, 158 - 128),
                      "PB4-pressed: GPIO158 (LED14) toggle bit not set",
                      "spec Ex.9: 'If PB4 (GPIO7) pressed, toggle LED14 (GPIO158)'")
                   ? 1
                   : 0;
    success &= report(toggle_bit_set(GpioDataRegs.GPETOGGLE.all, 159 - 128),
                      "PB4-pressed: GPIO159 (LED15) toggle bit not set",
                      "spec Ex.9: '...toggle LED15 (GPIO159)'")
                   ? 1
                   : 0;
    success &= report(!toggle_bit_set(GpioDataRegs.GPBTOGGLE.all, 61 - 32),
                      "PB4-pressed: GPIO61 toggled while PB1 was released",
                      "PB1 LEDs must remain idle while only PB4 is pressed")
                   ? 1
                   : 0;
    success &= report(!toggle_bit_set(GpioDataRegs.GPETOGGLE.all, 157 - 128),
                      "PB4-pressed: GPIO157 toggled while PB1 was released",
                      "PB1 LEDs must remain idle while only PB4 is pressed")
                   ? 1
                   : 0;

    restore_snapshot(baseline);
    return success;
}

// Spec Ex.5: serial_printf must fire every 250 ms via CpuTimer2's modulus.
int check_print_cadence(Validator *) {
    spdlog::info("Check print cadence");

    const Hw1Phase3Snapshot baseline = take_snapshot();
    const uint32_t period_us = static_cast<uint32_t>(CpuTimer2.PeriodInUSec);
    if (period_us == 0) {
        spdlog::error("[check_print_cadence] CpuTimer2.PeriodInUSec is 0");
        restore_snapshot(baseline);
        return 0;
    }

    grader::release_button(4);
    grader::release_button(7);

    grader::resetPrintfCapture();
    UARTPrint = 0;
    CpuTimer2.InterruptCount = 0;

    // Drive 1 s of synthetic time in four 250 ms bursts; sleep briefly between
    // bursts to let the main thread process each UARTPrint=1 transition.
    for (int burst = 0; burst < 4; ++burst) {
        grader::run_isr_for_us(cpu_timer2_isr, period_us, 250'000);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    const bool ok = grader::expect_print_cadence(grader::SerialPort::SCIA, 4, 0.25,
                                                 "check_print_cadence");
    if (!ok) {
        spdlog::error("  spec Ex.5: serial_printf must fire every 250 ms — that's 4 prints in 1 s");
    }

    restore_snapshot(baseline);
    return ok ? 1 : 0;
}

// Spec Ex.8: the print format must be
//   "Timeint = %ld, Time = %.2f sec, Input = %.3f, SatOut = %.2f\r\n"
// with arguments (timeint, time, sinvalue, satvalue). The most common student
// bug is %d-for-int32_t — `expect_format` flags that explicitly.
int check_print_format(Validator *) {
    spdlog::info("Check print format");

    const Hw1Phase3Snapshot baseline = take_snapshot();
    const uint32_t period_us = static_cast<uint32_t>(CpuTimer2.PeriodInUSec);
    if (period_us == 0) {
        spdlog::error("[check_print_format] CpuTimer2.PeriodInUSec is 0");
        restore_snapshot(baseline);
        return 0;
    }

    grader::release_button(4);
    grader::release_button(7);

    grader::resetPrintfCapture();
    UARTPrint = 0;
    CpuTimer2.InterruptCount = 0;

    grader::run_isr_for_us(cpu_timer2_isr, period_us, 500'000);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    const grader::PrintfCall *latest = grader::latestPrintfCall(grader::SerialPort::SCIA);
    if (!latest) {
        spdlog::error("[check_print_format] no SCIA prints captured — main thread may not be servicing UARTPrint");
        restore_snapshot(baseline);
        return 0;
    }

    const bool ex8_ok = grader::expect_format(
        latest,
        "Timeint = %ld, Time = %.2f sec, Input = %.3f, SatOut = %.2f\r\n",
        "check_print_format[Ex.8]");
    bool overall = ex8_ok;
    if (!ex8_ok) {
        // Ex.5 fallback: spec also allows the original Ex.5 format if the student
        // hasn't reached Ex.8.
        const bool ex5_ok = grader::expect_format(
            latest,
            "Num Timer2:%ld Num SerialRX: %ld\r\n",
            "check_print_format[Ex.5-fallback]");
        if (ex5_ok) {
            spdlog::warn("[check_print_format] format matches the Ex.5 placeholder, not the Ex.8 spec");
            overall = true;
        }
    }

    if (overall) {
        const bool args_ok = grader::expect_arg_types(
            latest,
            {grader::ArgType::Int32, grader::ArgType::Float, grader::ArgType::Float, grader::ArgType::Float},
            "check_print_format[Ex.8 arg types]");
        if (!args_ok) {
            // Tolerate Ex.5 fallback (two %ld) — re-check.
            const bool ex5_args = grader::expect_arg_types(
                latest, {grader::ArgType::Int32, grader::ArgType::Int32}, "check_print_format[Ex.5 args]");
            overall = ex5_args;
        }
    }

    restore_snapshot(baseline);
    return overall ? 1 : 0;
}

int check_saturate(Validator *) {
    spdlog::info("Check saturate");

    int success = 1;

    if (saturate) {
        const std::vector<float> values = {-10., -2., -1., -0.5, 0, 0.5, -1, -2, -10};
        const std::vector<float> saturatation_vals = {0.25, 1.0, 1.5, 2.0, 15.0};

        for (const float &sat: saturatation_vals) {
            for (const float &value: values) {
                const float res = saturate(value, sat);

                float actual = value;

                if (value > sat) {
                    actual = sat;
                } else if (value < -sat) {
                    actual = -sat;
                }


                if (actual != res) {
                    spdlog::error("Expecting saturate({}, {}) = {} got {}", value, sat, actual, res);

                    if (-actual == res) {
                        spdlog::error("Check that you have the right output sign for your function");
                    }

                    if (res == value) {
                        if (value > 0) {
                            spdlog::error("Check that you bound the positive saturation value");
                        } else if (value < 0) {
                            spdlog::error("Check that you bound the negative saturation value");
                        }
                    }

                    success = 0;
                }
            }
        }
    } else {
        spdlog::error("float saturate(float, float) not available");
        success = 0;
    }

    return success;
}

CheckFunctions checker() {
    spdlog::info("Checking HW 1");

    return {
        &check_initialization,
        &check_timer0,
        &check_timer1,
        &check_timer2,
        &check_saturate,
        &check_print_cadence,
        &check_print_format,
    };
}
