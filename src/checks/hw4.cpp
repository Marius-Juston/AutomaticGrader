//
// HW4 grader.
//
// Spec: context/homeworks/HW4.tex
// Reference: context/code_solutions/myHW_Yingyord_First/HW4/HW4fy_main.c
//
// Reference uses GPIO14/GPIO15 (mux 1) for EPWM8A/EPWM8B and binds
// XINT1 -> GPIO6 (PB3), XINT2 -> GPIO7 (PB4) via GPIO_SetupXINT{1,2}Gpio.
// Servo math: CMPA/CMPB = ((0.04 * (angle + 90) / 90) + 0.04) * 62500
//   i.e. -90 -> 2500, 0 -> 5000, +90 -> 7500.
//

#include "checks/hw4.h"

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <string>
#include <thread>

#include <spdlog/spdlog.h>

#include "checks/expectations.h"
#include "checks/main_loop_driver.h"
#include "checks/printf_capture.h"
#include "checks/state_checker.h"
#include "checks/stimulus.hpp"
#include "checks/synthetic_clock.h"
#include "checks/validator.h"

#define LAUNCHPAD_CPU_FREQUENCY 200

extern uint16_t xintPinMap[8];

namespace {
    bool report(const bool cond, const char *check, const std::string &msg, const char *hint = nullptr) {
        if (!cond) {
            spdlog::error("[{}] {}", check, msg);
            if (hint) spdlog::error("  spec: {}", hint);
        }
        return cond;
    }
}

int check_initialization(Validator *val) {
    HardwareStateValidator validator;
    validator.populate_all_zero();

    for (const char *name: {
             "AdcaRegs", "AdcaResultRegs",
             "EPwm4Regs", "EPwm8Regs", "EPwm12Regs",
             "GpioCtrlRegs", "GpioDataRegs",
             "DacaRegs", "XintRegs", "InputXbarRegs",
             "PieVectTable", "PieCtrlRegs",
             "CpuTimer0", "CpuTimer1", "CpuTimer2",
             "CpuTimer0Regs", "CpuTimer1Regs", "CpuTimer2Regs"
         }) {
        validator.mark_as_used(name);
    }

    spdlog::info("[HW4] check_initialization");
    int success = 1;
    success &= validator.validate();

    val->start_main_thread();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    {
        CPUTIMER_VARS expected = {};
        ConfigCpuTimer(&expected, LAUNCHPAD_CPU_FREQUENCY, 10000);
        validator.register_comparison("CpuTimer0", CpuTimer0, expected);
        ConfigCpuTimer(&expected, LAUNCHPAD_CPU_FREQUENCY, 20000);
        validator.register_comparison("CpuTimer1", CpuTimer1, expected);
        ConfigCpuTimer(&expected, LAUNCHPAD_CPU_FREQUENCY, 40000);
        validator.register_comparison("CpuTimer2", CpuTimer2, expected);
    }

    // ---- EPWM8: 50 Hz RC-servo carrier ----
    success &= report(EPwm8Regs.TBPRD == 62500,
                      "HW4/init", "EPwm8Regs.TBPRD != 62500 (50 Hz @ EPWMCLK/16)",
                      "TBPRD = 62500 with CLKDIV=4 yields 50 MHz/16/62500 = 50 Hz")
                   ? 1
                   : 0;
    success &= report(EPwm8Regs.TBCTL.bit.CLKDIV == 4,
                      "HW4/init", "EPwm8Regs.TBCTL.CLKDIV != 4 (divide by 16)")
                   ? 1
                   : 0;
    success &= report(EPwm8Regs.TBCTL.bit.CTRMODE == 0,
                      "HW4/init", "EPwm8Regs.TBCTL.CTRMODE != 0 (count-up)")
                   ? 1
                   : 0;
    success &= report(EPwm8Regs.AQCTLA.bit.ZRO == 2,
                      "HW4/init", "EPwm8Regs.AQCTLA.ZRO != 2 (set high on counter==0)")
                   ? 1
                   : 0;
    success &= report(EPwm8Regs.AQCTLA.bit.CAU == 1,
                      "HW4/init", "EPwm8Regs.AQCTLA.CAU != 1 (clear on counter==CMPA)")
                   ? 1
                   : 0;
    success &= report(EPwm8Regs.AQCTLB.bit.ZRO == 2,
                      "HW4/init", "EPwm8Regs.AQCTLB.ZRO != 2 (set high on counter==0)")
                   ? 1
                   : 0;
    success &= report(EPwm8Regs.AQCTLB.bit.CBU == 1,
                      "HW4/init", "EPwm8Regs.AQCTLB.CBU != 1 (clear on counter==CMPB)")
                   ? 1
                   : 0;
    success &= report(EPwm8Regs.CMPA.bit.CMPA >= 2500 && EPwm8Regs.CMPA.bit.CMPA <= 7500,
                      "HW4/init", "EPwm8Regs.CMPA.CMPA out of [-90,+90] servo range",
                      "valid CMPA range is 2500..7500 (4%..12% duty)")
                   ? 1
                   : 0;
    success &= report(EPwm8Regs.CMPB.bit.CMPB >= 2500 && EPwm8Regs.CMPB.bit.CMPB <= 7500,
                      "HW4/init", "EPwm8Regs.CMPB.CMPB out of [-90,+90] servo range")
                   ? 1
                   : 0;

    // ---- GPIO mux for EPWM8A/B: GPIO14 / GPIO15 with mux 1 (reference) ----
    success &= report(gpiosSetup[14].muxPosition == 1 && gpiosSetup[14].cpu == GPIO_MUX_CPU1,
                      "HW4/init", "GPIO14 mux not set to EPWM8A (mux=1, CPU1)")
                   ? 1
                   : 0;
    success &= report(gpiosSetup[15].muxPosition == 1 && gpiosSetup[15].cpu == GPIO_MUX_CPU1,
                      "HW4/init", "GPIO15 mux not set to EPWM8B (mux=1, CPU1)")
                   ? 1
                   : 0;

    // ---- PB3/PB4 inputs with pullup ----
    success &= report(gpiosSetup[6].output == GPIO_INPUT && gpiosSetup[6].flags == GPIO_PULLUP,
                      "HW4/init", "GPIO6 (PB3) must be input + pullup for XINT1")
                   ? 1
                   : 0;
    success &= report(gpiosSetup[7].output == GPIO_INPUT && gpiosSetup[7].flags == GPIO_PULLUP,
                      "HW4/init", "GPIO7 (PB4) must be input + pullup for XINT2")
                   ? 1
                   : 0;

    // ---- XINT bindings (XINT1->GPIO6, XINT2->GPIO7) ----
    success &= report(xintPinMap[1] == 6,
                      "HW4/init", "GPIO_SetupXINT1Gpio(6) not called — XINT1 must trigger on GPIO6 (PB3)")
                   ? 1
                   : 0;
    success &= report(xintPinMap[2] == 7,
                      "HW4/init", "GPIO_SetupXINT2Gpio(7) not called — XINT2 must trigger on GPIO7 (PB4)")
                   ? 1
                   : 0;

    // ---- XintRegs: falling-edge polarity, enabled ----
    success &= report(XintRegs.XINT1CR.bit.POLARITY == 0,
                      "HW4/init", "XintRegs.XINT1CR.POLARITY != 0 (must be falling edge)")
                   ? 1
                   : 0;
    success &= report(XintRegs.XINT1CR.bit.ENABLE == 1,
                      "HW4/init", "XintRegs.XINT1CR.ENABLE != 1")
                   ? 1
                   : 0;
    success &= report(XintRegs.XINT2CR.bit.POLARITY == 0,
                      "HW4/init", "XintRegs.XINT2CR.POLARITY != 0 (must be falling edge)")
                   ? 1
                   : 0;
    success &= report(XintRegs.XINT2CR.bit.ENABLE == 1,
                      "HW4/init", "XintRegs.XINT2CR.ENABLE != 1")
                   ? 1
                   : 0;

    // ---- GPIO qualification: 6-sample debounce on GPIO6/GPIO7 ----
    success &= report(GpioCtrlRegs.GPAQSEL1.bit.GPIO6 == 2,
                      "HW4/init", "GpioCtrlRegs.GPAQSEL1.GPIO6 != 2 (6-sample debounce)")
                   ? 1
                   : 0;
    success &= report(GpioCtrlRegs.GPAQSEL1.bit.GPIO7 == 2,
                      "HW4/init", "GpioCtrlRegs.GPAQSEL1.GPIO7 != 2 (6-sample debounce)")
                   ? 1
                   : 0;
    success &= report(GpioCtrlRegs.GPACTRL.bit.QUALPRD0 != 0,
                      "HW4/init", "GpioCtrlRegs.GPACTRL.QUALPRD0 must be set for XINT debounce")
                   ? 1
                   : 0;

    // ---- PIE vectors + enables ----
    if (&xint1_isr) {
        success &= report(PieVectTable.XINT1_INT == &xint1_isr,
                          "HW4/init", "PieVectTable.XINT1_INT != &xint1_isr")
                       ? 1
                       : 0;
    }
    if (&xint2_isr) {
        success &= report(PieVectTable.XINT2_INT == &xint2_isr,
                          "HW4/init", "PieVectTable.XINT2_INT != &xint2_isr")
                       ? 1
                       : 0;
    }
    success &= report(PieCtrlRegs.PIEIER1.bit.INTx4 == 1,
                      "HW4/init", "PieCtrlRegs.PIEIER1.INTx4 != 1 (XINT1 enable)")
                   ? 1
                   : 0;
    success &= report(PieCtrlRegs.PIEIER1.bit.INTx5 == 1,
                      "HW4/init", "PieCtrlRegs.PIEIER1.INTx5 != 1 (XINT2 enable)")
                   ? 1
                   : 0;

    success &= validator.validate();
    return success;
}

namespace {
    template<typename ReadFn>
    int sweep_axis(const char *label, void (*fn)(float), ReadFn read) {
        int success = 1;
        struct Case {
            float angle;
            uint16_t expected;
            const char *hint;
        };
        const Case cases[] = {
            {0.0f, 5000, "0 deg -> 8% duty (5000)"},
            {-90.0f, 2500, "-90 deg -> 4% duty (2500)"},
            {90.0f, 7500, "+90 deg -> 12% duty (7500)"},
            {200.0f, 7500, "+200 must saturate to +90 (7500)"},
            {-200.0f, 2500, "-200 must saturate to -90 (2500)"},
            {45.0f, 6250, "+45 deg -> 10% duty (6250) linearity"},
        };
        for (const auto &c: cases) {
            fn(c.angle);
            const uint16_t got = read();
            if (const bool ok = std::abs(static_cast<int>(got) - static_cast<int>(c.expected)) <= 2; !ok) {
                spdlog::error("[HW4/{}] {}({:.1f}) -> {} != expected {} ({})",
                              label, label, c.angle, got, c.expected, c.hint);
                success = 0;
            }
        }
        return success;
    }
}

int check_setEPWM8A_RCServo(Validator *) {
    spdlog::info("[HW4] check_setEPWM8A_RCServo");
    if (!&setEPWM8A_RCServo) {
        spdlog::error("[HW4/EPWM8A] setEPWM8A_RCServo not defined by student");
        return 0;
    }
    const uint16_t saved = EPwm8Regs.CMPA.bit.CMPA;
    const int s = sweep_axis("EPWM8A", &setEPWM8A_RCServo,
                             []() -> uint16_t { return EPwm8Regs.CMPA.bit.CMPA; });
    EPwm8Regs.CMPA.bit.CMPA = saved;
    return s;
}

int check_setEPWM8B_RCServo(Validator *) {
    spdlog::info("[HW4] check_setEPWM8B_RCServo");
    if (!&setEPWM8B_RCServo) {
        spdlog::error("[HW4/EPWM8B] setEPWM8B_RCServo not defined by student");
        return 0;
    }
    const uint16_t saved = EPwm8Regs.CMPB.bit.CMPB;
    const int s = sweep_axis("EPWM8B", &setEPWM8B_RCServo,
                             []() -> uint16_t { return EPwm8Regs.CMPB.bit.CMPB; });
    EPwm8Regs.CMPB.bit.CMPB = saved;
    return s;
}

int check_xint1_button(Validator *) {
    spdlog::info("[HW4] check_xint1_button");
    if (!&xint1_isr || !&Xint1Count) {
        spdlog::error("[HW4/xint1] xint1_isr or Xint1Count missing");
        return 0;
    }
    const uint32_t before = Xint1Count;
    constexpr int presses = 5;
    for (int i = 0; i < presses; ++i) {
        grader::press_button(6);
        xint1_isr();
        grader::release_button(6);
    }
    const uint32_t got = Xint1Count - before;
    if (got != presses) {
        spdlog::error("[HW4/xint1] Xint1Count advanced by {} (expected {})", got, presses);
        return 0;
    }
    return 1;
}

int check_xint2_button(Validator *) {
    spdlog::info("[HW4] check_xint2_button");
    if (!&xint2_isr || !&Xint2Count) {
        spdlog::error("[HW4/xint2] xint2_isr or Xint2Count missing");
        return 0;
    }
    const uint32_t before = Xint2Count;
    constexpr int presses = 5;
    for (int i = 0; i < presses; ++i) {
        grader::press_button(7);
        xint2_isr();
        grader::release_button(7);
    }
    if (const uint32_t got = Xint2Count - before; got != presses) {
        spdlog::error("[HW4/xint2] Xint2Count advanced by {} (expected {})", got, presses);
        return 0;
    }
    return 1;
}

// HW4 print format: reference uses "NumXInt1=%ld,NumXInt2=%ld\r\n".
// Spec is loose on cadence so only verify format/arg-types when a print exists.
int check_print_format(Validator *) {
    spdlog::info("[HW4] check_print_format");
    grader::resetPrintfCapture();
    UARTPrint = 0;
    const uint32_t period_us = static_cast<uint32_t>(CpuTimer2.PeriodInUSec);
    if (period_us == 0) {
        spdlog::error("[HW4/print_format] CpuTimer2.PeriodInUSec is 0");
        return 0;
    }
    // Cooperative driver: 8 s of synthetic time, one main-loop iteration per ISR.
    grader::drive_isr_with_main_pump(cpu_timer2_isr, period_us, 8'000'000ull / period_us);

    const grader::PrintfCall *latest = grader::latestPrintfCall(grader::SerialPort::SCIA);
    if (!latest) {
        spdlog::warn("[HW4/print_format] no SCIA prints captured — spec is ambiguous on Ex.4 cadence");
        return 1;
    }
    const bool ok = grader::expect_arg_types(latest,
                                             {grader::ArgType::Int32, grader::ArgType::Int32},
                                             "HW4/print_format[two-int32]");
    return ok ? 1 : 0;
}

CheckFunctions checker() {
    spdlog::info("Checking HW 4");
    return {
        &check_initialization,
        &check_setEPWM8A_RCServo,
        &check_setEPWM8B_RCServo,
        &check_xint1_button,
        &check_xint2_button,
        &check_print_format,
    };
}