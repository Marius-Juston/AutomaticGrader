//
// HW5 grader.
//
// Spec: context/homeworks/HW5.tex (sparse — most behavior is "extend HW4")
// Reference: context/code_solutions/myHW_Yingyord_First/HW5/HW5fy_main.c
//
// Reference layout:
//   - EPWM4 trigger @ 1 ms feeds AdcaSOC0/1 (joystick X/Y) -> ADCA_isr.
//   - EPWM7 trigger @ 0.1 ms feeds AdcbSOC0 (mic) -> DMA channel 1 -> ping/pong.
//   - DMA src = &AdcbResultRegs.ADCRESULT0; trigger source = ADCBINT1 (sel=6);
//     transfer size = 1023 (RFFT_SIZE-1); MODE=0x8901; CONTROL=0x0091.
//   - DMA_isr toggles iPingPong, sets pingFFT/pongFFT, increments DMAcount.
//   - EPWM8A/B servo functions inherited from HW4 (same math).
//
// Spec vs roadmap: roadmap mentions ADCA, but HW5 reference uses ADCB for mic.
// We accept either A or B for the DMA source register to match spec ambiguity.
//

#include "checks/hw5.h"

#include <chrono>
#include <cmath>
#include <cstdint>
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
             "AdcaRegs", "AdcaResultRegs", "AdcbRegs", "AdcbResultRegs",
             "AdccRegs", "AdcdRegs",
             "EPwm4Regs", "EPwm7Regs", "EPwm8Regs", "EPwm12Regs",
             "GpioCtrlRegs", "GpioDataRegs",
             "DacaRegs", "DmaRegs", "DmaClaSrcSelRegs",
             "PieVectTable", "PieCtrlRegs",
             "CpuTimer0", "CpuTimer1", "CpuTimer2",
             "CpuTimer0Regs", "CpuTimer1Regs", "CpuTimer2Regs"
         }) {
        validator.mark_as_used(name);
    }

    spdlog::info("[HW5] check_initialization");
    int success = 1;
    success &= validator.validate();

    val->start_main_thread();
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    {
        CPUTIMER_VARS expected = {};
        ConfigCpuTimer(&expected, LAUNCHPAD_CPU_FREQUENCY, 10000);
        validator.register_comparison("CpuTimer0", CpuTimer0, expected);
        ConfigCpuTimer(&expected, LAUNCHPAD_CPU_FREQUENCY, 20000);
        validator.register_comparison("CpuTimer1", CpuTimer1, expected);
        ConfigCpuTimer(&expected, LAUNCHPAD_CPU_FREQUENCY, 40000);
        validator.register_comparison("CpuTimer2", CpuTimer2, expected);
    }

    // ---- EPWM8 (servo carryover) ----
    success &= report(EPwm8Regs.TBPRD == 62500,
                      "HW5/init", "EPwm8Regs.TBPRD != 62500 (50 Hz servo carrier)")
                   ? 1
                   : 0;
    success &= report(EPwm8Regs.TBCTL.bit.CLKDIV == 4,
                      "HW5/init", "EPwm8Regs.TBCTL.CLKDIV != 4")
                   ? 1
                   : 0;
    success &= report(EPwm8Regs.AQCTLA.bit.ZRO == 2 && EPwm8Regs.AQCTLA.bit.CAU == 1,
                      "HW5/init", "EPwm8Regs.AQCTLA not configured for servo PWM")
                   ? 1
                   : 0;
    success &= report(EPwm8Regs.AQCTLB.bit.ZRO == 2 && EPwm8Regs.AQCTLB.bit.CBU == 1,
                      "HW5/init", "EPwm8Regs.AQCTLB not configured for servo PWM")
                   ? 1
                   : 0;

    // ---- EPWM7 SOC trigger for mic ADC (reference: 5000 = 100 us = 10 kHz) ----
    success &= report(EPwm7Regs.TBPRD > 0,
                      "HW5/init", "EPwm7Regs.TBPRD not configured (mic SOC trigger)")
                   ? 1
                   : 0;
    success &= report(EPwm7Regs.ETSEL.bit.SOCAEN == 1,
                      "HW5/init", "EPwm7Regs.ETSEL.SOCAEN != 1 (mic ADC trigger must be enabled)")
                   ? 1
                   : 0;

    // ---- DMA channel 1: source must be one of the ADC result regs ----
    {
        // Reference casts pointer to Uint32 — on the 64-bit Linux host that
        // truncates the address, so compare only the low 32 bits.
        const uint32_t src = static_cast<uint32_t>(DmaRegs.CH1.SRC_ADDR_SHADOW);
        const uint32_t adca_addr =
                static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&AdcaResultRegs.ADCRESULT0));
        const uint32_t adcb_addr =
                static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&AdcbResultRegs.ADCRESULT0));
        success &= report(src == adca_addr || src == adcb_addr,
                          "HW5/init",
                          "DmaRegs.CH1.SRC_ADDR_SHADOW must point at AdcaResultRegs.ADCRESULT0 "
                          "or AdcbResultRegs.ADCRESULT0",
                          "DMA source = address of the mic ADC result register")
                       ? 1
                       : 0;
    }
    // Destination should land on ping or pong buffer (we can't symbol-resolve from here,
    // so verify the channel was at least configured with a non-zero dest).
    success &= report(DmaRegs.CH1.DST_ADDR_SHADOW != 0,
                      "HW5/init", "DmaRegs.CH1.DST_ADDR_SHADOW not configured")
                   ? 1
                   : 0;

    // RFFT_SIZE = 1024 -> TRANSFER_SIZE = 1023 (bursts per transfer = N-1 in TI DMA)
    success &= report(DmaRegs.CH1.TRANSFER_SIZE == 1023 || DmaRegs.CH1.TRANSFER_SIZE == 1024,
                      "HW5/init",
                      "DmaRegs.CH1.TRANSFER_SIZE must be 1023 or 1024 (RFFT_SIZE samples)")
                   ? 1
                   : 0;

    // Trigger source: 1=ADCAINT1, 6=ADCBINT1 — accept either.
    {
        const uint16_t sel = DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH1;
        success &= report(sel == 1 || sel == 6,
                          "HW5/init",
                          "DmaClaSrcSelRegs.DMACHSRCSEL1.CH1 must be 1 (ADCAINT1) or 6 (ADCBINT1)")
                       ? 1
                       : 0;
    }

    // MODE: CHINTE=1 + PERINTE=1 + CONTINUOUS=1 + channel=1 -> 0x8901
    success &= report((DmaRegs.CH1.MODE.all & 0x8901) == 0x8901,
                      "HW5/init", "DmaRegs.CH1.MODE missing CHINTE|PERINTE|CONTINUOUS|CH1 bits",
                      "expected MODE = 0x8901")
                   ? 1
                   : 0;
    // CONTROL: RUN=1
    success &= report((DmaRegs.CH1.CONTROL.all & 0x0001) == 0x0001,
                      "HW5/init", "DmaRegs.CH1.CONTROL.RUN != 1 (DMA channel must be started)")
                   ? 1
                   : 0;

    // ---- PIE vectors ----
    if (&DMA_isr) {
        success &= report(PieVectTable.DMA_CH1_INT == &DMA_isr,
                          "HW5/init", "PieVectTable.DMA_CH1_INT != &DMA_isr")
                       ? 1
                       : 0;
    }
    success &= report(PieCtrlRegs.PIEIER7.bit.INTx1 == 1,
                      "HW5/init", "PieCtrlRegs.PIEIER7.INTx1 != 1 (DMA_CH1 enable)")
                   ? 1
                   : 0;
    if (&ADCA_isr) {
        success &= report(PieVectTable.ADCA1_INT == &ADCA_isr,
                          "HW5/init", "PieVectTable.ADCA1_INT != &ADCA_isr (joystick ISR)")
                       ? 1
                       : 0;
    }

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
        };
        const Case cases[] = {
            {0.0f, 5000},
            {-90.0f, 2500},
            {90.0f, 7500},
            {200.0f, 7500},
            {-200.0f, 2500},
            {45.0f, 6250},
        };
        for (const auto &c: cases) {
            fn(c.angle);
            const uint16_t got = read();
            if (const bool ok = std::abs(static_cast<int>(got) - static_cast<int>(c.expected)) <= 2; !ok) {
                spdlog::error("[HW5/{}] {}({:.1f}) -> {} != expected {}",
                              label, label, c.angle, got, c.expected);
                success = 0;
            }
        }
        return success;
    }
}

int check_servo_carryover(Validator *) {
    spdlog::info("[HW5] check_servo_carryover");
    int success = 1;
    if (!&setEPWM8A_RCServo || !&setEPWM8B_RCServo) {
        spdlog::error("[HW5/servo] setEPWM8A_RCServo / setEPWM8B_RCServo missing");
        return 0;
    }
    const uint16_t saveA = EPwm8Regs.CMPA.bit.CMPA;
    const uint16_t saveB = EPwm8Regs.CMPB.bit.CMPB;
    success &= sweep_axis("EPWM8A", &setEPWM8A_RCServo,
                          []() -> uint16_t { return EPwm8Regs.CMPA.bit.CMPA; });
    success &= sweep_axis("EPWM8B", &setEPWM8B_RCServo,
                          []() -> uint16_t { return EPwm8Regs.CMPB.bit.CMPB; });
    EPwm8Regs.CMPA.bit.CMPA = saveA;
    EPwm8Regs.CMPB.bit.CMPB = saveB;
    return success;
}

// DMA_isr alternates which ping/pong buffer is processed. Reference uses
// `iPingPong` as the toggle and sets pingFFT/pongFFT for the main loop to
// consume. Verify two consecutive DMA_isr calls advance the state cleanly
// (one should set pingFFT, the other pongFFT — order undefined by spec).
int check_pingpong_swap(Validator *) {
    spdlog::info("[HW5] check_pingpong_swap");
    if (!&DMA_isr) {
        spdlog::error("[HW5/pingpong] DMA_isr missing");
        return 0;
    }

    const int16_t saveCount = (&DMAcount) ? DMAcount : 0;
    const uint16_t savePing = (&pingFFT) ? pingFFT : 0;
    const uint16_t savePong = (&pongFFT) ? pongFFT : 0;
    const uint16_t saveTog = (&iPingPong) ? iPingPong : 0;

    if (&DMAcount) DMAcount = 0;
    if (&pingFFT) pingFFT = 0;
    if (&pongFFT) pongFFT = 0;
    if (&iPingPong) iPingPong = 0;

    DMA_isr();
    DMA_isr();
    DMA_isr();

    int success = 1;
    if (&DMAcount) {
        success &= report(DMAcount == 3, "HW5/pingpong",
                          std::string("DMAcount = ") + std::to_string(DMAcount) +
                          " after 3 DMA_isr calls (expected 3)")
                       ? 1
                       : 0;
    }
    if (&pingFFT && &pongFFT) {
        success &= report(pingFFT == 1 || pongFFT == 1,
                          "HW5/pingpong",
                          "neither pingFFT nor pongFFT set after 3 DMA_isr calls — "
                          "ping/pong dispatch missing")
                       ? 1
                       : 0;
    }

    if (&DMAcount) DMAcount = saveCount;
    if (&pingFFT) pingFFT = savePing;
    if (&pongFFT) pongFFT = savePong;
    if (&iPingPong) iPingPong = saveTog;
    return success;
}

// HW5 print format: reference uses "Power: %.3f Freq: %.0f\r\n" + joystick volts.
int check_print_format(Validator *) {
    spdlog::info("[HW5] check_print_format");
    grader::resetPrintfCapture();
    UARTPrint = 0;
    const uint32_t period_us = static_cast<uint32_t>(CpuTimer2.PeriodInUSec);
    if (period_us == 0) {
        spdlog::error("[HW5/print_format] CpuTimer2.PeriodInUSec is 0");
        return 0;
    }
    // Reference sets UARTPrint via Timer2 mod 50 (~2 s) — drive 8 s of ticks
    // via the cooperative driver so multiple prints land deterministically.
    grader::drive_isr_with_main_pump(cpu_timer2_isr, period_us, 8'000'000ull / period_us);

    const grader::PrintfCall *latest = grader::latestPrintfCall(grader::SerialPort::SCIA);
    if (!latest) {
        spdlog::warn("[HW5/print_format] no SCIA prints captured — spec is sparse on cadence");
        return 1;
    }
    // Two floats (power, frequency) — most common.
    bool ok = grader::expect_arg_types(latest,
                                       {grader::ArgType::Float, grader::ArgType::Float},
                                       "HW5/print_format[two-float]");
    if (!ok) {
        ok = grader::expect_arg_types(latest,
                                      {
                                          grader::ArgType::Float, grader::ArgType::Float,
                                          grader::ArgType::Float, grader::ArgType::Float
                                      },
                                      "HW5/print_format[four-float]");
    }
    return ok ? 1 : 0;
}

CheckFunctions checker() {
    spdlog::info("Checking HW 5");
    return {
        &check_initialization,
        &check_servo_carryover,
        &check_pingpong_swap,
        &check_print_format,
    };
}
