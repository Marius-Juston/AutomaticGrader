//
// HW3 grader.
//
// Spec: context/homeworks/HW3.tex
// Reference: context/code_solutions/myHW_Yingyord_First/HW3/HW3fy_main.c
//
// Slice-3 deferred items:
//   - `spibTxLog[]` capture (TX-side word log) — would be required for
//     `check_spib_request_format` and `check_imu_full_init`. Not in slice 1;
//     SPITXBUF struct-field writes can't be intercepted without a heavier
//     ti_stubs rewrite. Both checks are stubbed below with a clear "skipped"
//     marker so the contract is documented.
//

#include "checks/hw3.h"

#include <chrono>
#include <cmath>
#include <string>
#include <thread>

#include <spdlog/spdlog.h>

#include "checks/expectations.h"
#include "checks/printf_capture.h"
#include "checks/state_checker.h"
#include "checks/stimulus.hpp"
#include "checks/synthetic_clock.h"
#include "checks/validator.h"

#define LAUNCHPAD_CPU_FREQUENCY 200

namespace {
    bool report(bool cond, const char *check, const std::string &msg, const char *hint = nullptr) {
        if (!cond) {
            spdlog::error("[{}] {}", check, msg);
            if (hint) spdlog::error("  spec: {}", hint);
        }
        return cond;
    }

    bool approx_eq(float a, float b, float tol = 1e-3f) noexcept {
        return std::fabs(a - b) <= tol;
    }
}

// The HW3 reference's setupSpib() busy-waits on SpibRegs.SPIFFRX.bit.RXFFST
// reaching specific values (7, 4, then several 1s). Without a real SPI FIFO
// shadow we have to drive those values from the test thread; otherwise the
// student's main thread spins forever and check_initialization never sees the
// post-init register state.
static void hw3_unblock_setup_spib() {
    using namespace std::chrono_literals;
    const std::vector<uint16_t> sequence = {7, 4, 1, 1, 1, 1, 1, 1, 1, 1};
    for (uint16_t v: sequence) {
        for (int retry = 0; retry < 5; ++retry) {
            SpibRegs.SPIFFRX.bit.RXFFST = v;
            std::this_thread::sleep_for(2ms);
        }
    }
    SpibRegs.SPIFFRX.bit.RXFFST = 0;
}

int check_initialization(Validator *val) {
    HardwareStateValidator validator;
    validator.populate_all_zero();

    for (const char *name: {
             "AdcaRegs", "AdcaResultRegs",
             "EPwm9Regs", "EPwm12Regs",
             "GpioCtrlRegs", "GpioDataRegs",
             "DacaRegs", "SpibRegs",
             "PieVectTable", "PieCtrlRegs",
             "CpuTimer0", "CpuTimer1", "CpuTimer2",
             "CpuTimer0Regs", "CpuTimer1Regs", "CpuTimer2Regs"
         }) {
        validator.mark_as_used(name);
    }

    spdlog::info("[HW3] check_initialization");
    int success = 1;
    success &= validator.validate();

    val->start_main_thread();
    // Drive the SPI RX FIFO state register through the sequence the reference's
    // setupSpib() busy-waits on; without this the student thread hangs forever.
    hw3_unblock_setup_spib();
    // HW3 init has more steps than HW1; give the main thread a moment to reach
    // the EPWM9/SPIB initialisation block before sampling registers.
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    // ---- GPIO baseline (focus on HW3-specific pins) ----
    {
        success &= report(gpiosSetup[16].muxPosition == 5,
                          "HW3/init", "GPIO16 mux != 5 (EPWM9A buzzer)")
                       ? 1
                       : 0;
        success &= report(gpiosSetup[16].cpu == GPIO_MUX_CPU1,
                          "HW3/init", "GPIO16 not assigned to CPU1")
                       ? 1
                       : 0;
        // Note: the F2837xD MUX option for SPISIMOB/SPISOMIB/SPICLKB is 15
        // (per F2837xD_gpio.h tables); the original HW3.tex roadmap's
        // "10/8/9" wording was based on an older reference. We accept either
        // 15 (new) or 10/8/9 (old) for back-compat.
        success &= report(gpiosSetup[63].muxPosition == 15
                          || gpiosSetup[63].muxPosition == 10,
                          "HW3/init", "GPIO63 mux not set to SPISIMOB (15)")
                       ? 1
                       : 0;
        success &= report(gpiosSetup[64].muxPosition == 15
                          || gpiosSetup[64].muxPosition == 8,
                          "HW3/init", "GPIO64 mux not set to SPISOMIB (15)")
                       ? 1
                       : 0;
        success &= report(gpiosSetup[65].muxPosition == 15
                          || gpiosSetup[65].muxPosition == 9,
                          "HW3/init", "GPIO65 mux not set to SPICLKB (15)")
                       ? 1
                       : 0;
        success &= report(gpiosSetup[66].cpu == GPIO_MUX_CPU1
                          && gpiosSetup[66].output == GPIO_OUTPUT,
                          "HW3/init", "GPIO66 not configured as a CS output")
                       ? 1
                       : 0;
    }

    // ---- Timers (HW3 spec: T1 @ 125 ms, T2 @ 40 ms; T0 unspecified) ----
    {
        CPUTIMER_VARS expected = {};
        ConfigCpuTimer(&expected, LAUNCHPAD_CPU_FREQUENCY, 125000);
        validator.register_comparison("CpuTimer1", CpuTimer1, expected);
        ConfigCpuTimer(&expected, LAUNCHPAD_CPU_FREQUENCY, 40000);
        validator.register_comparison("CpuTimer2", CpuTimer2, expected);
        // T0 period is left to the student; assert only that it was configured.
        success &= report(CpuTimer0.PeriodInUSec > 0,
                          "HW3/init", "CpuTimer0 period not configured (expected > 0 us)")
                       ? 1
                       : 0;
    }

    // ---- PieVectTable.SPIB_RX_INT must point at SPIB_isr ----
    success &= report(&SPIB_isr != nullptr,
                      "HW3/init", "SPIB_isr is not defined by the student")
                   ? 1
                   : 0;
    if (&SPIB_isr) {
        success &= report(PieVectTable.SPIB_RX_INT == &SPIB_isr,
                          "HW3/init", "PieVectTable.SPIB_RX_INT != &SPIB_isr",
                          "the SPIB RX interrupt vector must point at SPIB_isr")
                       ? 1
                       : 0;
    }

    // ---- EPWM9: square-wave for buzzer (AQCTLA.ZRO=3 = toggle on zero) ----
    success &= report(EPwm9Regs.AQCTLA.bit.ZRO == 3,
                      "HW3/init", "EPwm9Regs.AQCTLA.ZRO != 3 (must toggle output on counter == 0)")
                   ? 1
                   : 0;
    success &= report(EPwm9Regs.TBCTL.bit.CTRMODE == 0,
                      "HW3/init", "EPwm9Regs.TBCTL.CTRMODE != 0 (count-up)")
                   ? 1
                   : 0;

    // ---- SPIB config (spec: 16-bit, mode 01, 1 MHz, FIFO enabled) ----
    success &= report(SpibRegs.SPICCR.bit.SPICHAR == 15,
                      "HW3/init", "SpibRegs.SPICCR.SPICHAR != 15 (must transmit/receive 16-bit)",
                      "SPICHAR = 15 yields 16-bit characters")
                   ? 1
                   : 0;
    success &= report(SpibRegs.SPIBRR.bit.SPI_BIT_RATE == 49,
                      "HW3/init", "SpibRegs.SPIBRR.SPI_BIT_RATE != 49 (must be 1 MHz @ 50 MHz LSPCLK)",
                      "SPIBRR = 49 yields 50 MHz / 50 = 1 MHz SCLK")
                   ? 1
                   : 0;
    success &= report(SpibRegs.SPICTL.bit.CLK_PHASE == 1,
                      "HW3/init", "SpibRegs.SPICTL.CLK_PHASE != 1 (Mode 01)")
                   ? 1
                   : 0;
    success &= report(SpibRegs.SPICCR.bit.CLKPOLARITY == 0,
                      "HW3/init", "SpibRegs.SPICCR.CLKPOLARITY != 0 (Mode 01)")
                   ? 1
                   : 0;
    success &= report(SpibRegs.SPICTL.bit.MASTER_SLAVE == 1,
                      "HW3/init", "SpibRegs.SPICTL.MASTER_SLAVE != 1 (must be SPI master)")
                   ? 1
                   : 0;
    success &= report(SpibRegs.SPIFFTX.bit.SPIFFENA == 1,
                      "HW3/init", "SpibRegs.SPIFFTX.SPIFFENA != 1 (FIFO must be enabled)")
                   ? 1
                   : 0;

    success &= validator.validate();
    return success;
}

// Inject SPI RX, call SPIB_isr, verify the IMU scaling math.
//
// LIMITATION: the existing ti_stubs SPI shim has a single SPIRXBUF field
// (no FIFO read-side dequeue), so all 8 reads inside SPIB_isr return the same
// value. We therefore validate the LAST-read field (`gyroz_val`) and the
// shared scaling factor; per-axis ordering checks land alongside the FIFO
// shadow in slice 4.
int check_spib_isr_scaling(Validator *) {
    spdlog::info("[HW3] check_spib_isr_scaling");

    if (!&SPIB_isr || !&gyroz_raw || !&gyroz_val) {
        spdlog::error("[HW3/spib_scaling] missing student symbols (SPIB_isr / gyroz_raw / gyroz_val)");
        return 0;
    }

    int success = 1;
    const int16_t saved_gyroz_raw = gyroz_raw;
    const float saved_gyroz_val = gyroz_val;
    const int32_t saved_count = (&SPIBCount) ? SPIBCount : 0;

    auto run_case = [&](uint16_t spi_word, float expected_gyroz_dps, const char *label) {
        grader::clear_spi_state(grader::SpiModule::B);
        grader::inject_spi_rx(grader::SpiModule::B, spi_word);
        SPIB_isr();
        success &= report(gyroz_raw == static_cast<int16_t>(spi_word),
                          "HW3/spib_scaling",
                          std::string(label) + ": gyroz_raw not equal to injected SPIRXBUF")
                       ? 1
                       : 0;
        success &= report(approx_eq(gyroz_val, expected_gyroz_dps, 0.05f),
                          "HW3/spib_scaling",
                          std::string(label) + ": gyroz_val scaling off",
                          "gyroz_val = gyroz_raw * 250.0 / 32767.0")
                       ? 1
                       : 0;
    };

    run_case(0x4000, 16384.0f * 250.0f / 32767.0f, "+half-scale");
    run_case(0xC000, static_cast<int16_t>(0xC000) * 250.0f / 32767.0f, "-half-scale");
    run_case(0x0000, 0.0f, "zero");

    gyroz_raw = saved_gyroz_raw;
    gyroz_val = saved_gyroz_val;
    if (&SPIBCount) SPIBCount = saved_count;
    return success;
}

// Spec roadmap: print cadence "every 200 ms" via SPIB_isr modulus.
int check_print_cadence(Validator *) {
    spdlog::info("[HW3] check_print_cadence");

    if (!&SPIB_isr) {
        spdlog::error("[HW3/print_cadence] SPIB_isr not defined by student");
        return 0;
    }

    grader::clear_spi_state(grader::SpiModule::B);
    grader::inject_spi_rx(grader::SpiModule::B, 0x1234);
    grader::resetPrintfCapture();
    UARTPrint = 0;
    const int32_t initialCount = (&SPIBCount) ? SPIBCount : 0;

    // 1000 SPIB_isr calls in 5 bursts of 200 (each burst should fire UARTPrint).
    for (int burst = 0; burst < 5; ++burst) {
        for (int i = 0; i < 200; ++i) SPIB_isr();
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    if (&SPIBCount) SPIBCount = initialCount;

    // Reference issues TWO printf calls per UARTPrint event (Acc + Gyro).
    // Accept the [4, 14] window which covers 5 events × {1, 2, 3} prints each.
    return grader::expect_print_cadence_window(grader::SerialPort::SCIA, 4, 14,
                                               "HW3/print_cadence")
               ? 1
               : 0;
}

int check_print_format(Validator *) {
    spdlog::info("[HW3] check_print_format");

    if (!&SPIB_isr) {
        spdlog::error("[HW3/print_format] SPIB_isr not defined by student");
        return 0;
    }

    grader::clear_spi_state(grader::SpiModule::B);
    grader::inject_spi_rx(grader::SpiModule::B, 0x4000);
    grader::resetPrintfCapture();
    UARTPrint = 0;

    for (int i = 0; i < 400; ++i) SPIB_isr();
    std::this_thread::sleep_for(std::chrono::milliseconds(80));

    const grader::PrintfCall *latest = grader::latestPrintfCall(grader::SerialPort::SCIA);
    if (!latest) {
        spdlog::error("[HW3/print_format] no SCIA prints captured");
        return 0;
    }
    bool ok = grader::expect_arg_types(latest,
                                       {grader::ArgType::Float, grader::ArgType::Float, grader::ArgType::Float},
                                       "HW3/print_format[3-float]");
    if (!ok) {
        ok = grader::expect_arg_types(latest,
                                      {
                                          grader::ArgType::Float, grader::ArgType::Float, grader::ArgType::Float,
                                          grader::ArgType::Float, grader::ArgType::Float, grader::ArgType::Float
                                      },
                                      "HW3/print_format[6-float]");
    }
    if (!ok) {
        ok = grader::expect_arg_types(latest, {grader::ArgType::Int16},
                                      "HW3/print_format[Ex.5-single-int16]");
    }
    return ok ? 1 : 0;
}

// Slice-3 deferred: requires SPI TX-side word log (`spibTxLog[]`).
int check_spib_request_format(Validator *) {
    spdlog::warn("[HW3] check_spib_request_format SKIPPED — SPI TX-side capture "
        "(spibTxLog[]) not yet implemented in slice 1. See HW3_ROADMAP.md.");
    return 1;
}

int check_imu_full_init(Validator *) {
    spdlog::warn("[HW3] check_imu_full_init SKIPPED — SPI TX-side capture "
        "(spibTxLog[]) not yet implemented in slice 1. See HW3_ROADMAP.md.");
    return 1;
}

CheckFunctions checker() {
    spdlog::info("Checking HW 3");
    return {
        &check_initialization,
        &check_spib_isr_scaling,
        &check_spib_request_format,
        &check_imu_full_init,
        &check_print_cadence,
        &check_print_format,
    };
}
