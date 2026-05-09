//
// HW2 grader.
//
// Spec: context/homeworks/HW2.tex
// Reference: context/code_solutions/myHW_Yingyord_First/HW2/HW2fy_main.c
//
// All required register state is captured directly via the existing TI
// register-struct stubs — HW2 students write registers directly (no driverlib),
// so no additional stubs are needed beyond `AdcSetMode` (already shipped).
//

#include "checks/hw2.h"

#include <chrono>
#include <cmath>
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
    bool report(const bool cond, const char *check, const char *msg, const char *hint = nullptr) {
        if (!cond) {
            spdlog::error("[{}] {}", check, msg);
            if (hint) spdlog::error("  spec: {}", hint);
        }
        return cond;
    }

    bool approx_eq(const float a, const float b, const float tol = 1e-3f) noexcept {
        return std::fabs(a - b) <= tol;
    }
}

int check_initialization(Validator *val) {
    HardwareStateValidator validator;
    validator.populate_all_zero();

    // Student is expected to write these registers during HW2 init; we verify
    // their fields explicitly below via the report() helper rather than the
    // populate_all_zero zero-check.
    for (const char *name: {
             "AdcaRegs", "AdcaResultRegs",
             "EPwm4Regs", "EPwm12Regs",
             "GpioCtrlRegs", "GpioDataRegs",
             "DacaRegs"
         }) {
        validator.mark_as_used(name);
    }

    spdlog::info("[HW2] check_initialization");

    int success = 1;
    success &= validator.validate();

    val->start_main_thread();

    // ---- GPIO baseline (LEDs + buttons + EPWM12A pin mux) ----
    {
        GpioSetup expected[MAX_GPIO];
        for (auto &i: expected) i = {};
        // EPWM12A on GPIO22 (mux 5).
        expected[22] = {GPIO_MUX_CPU1, 5, GPIO_OUTPUT, GPIO_PUSHPULL};
        // LEDs (output, push-pull).
        for (const int g: {
                 31, 34, 94, 95, 97, 111, 130, 131,
                 25, 26, 27, 60, 61, 157, 158, 159, 160,
                 0, 1, 19, 29, 32, 9, 66, 125
             }) {
            if (g != 22) {
                expected[g] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
            }
        }
        // Pushbuttons + joystick (input, pullup).
        for (const int g: {4, 5, 6, 7, 8}) {
            expected[g] = {GPIO_MUX_CPU1, 0, GPIO_INPUT, GPIO_PULLUP};
        }
        validator.register_comparison("GpioSetup", gpiosSetup, expected);
    }

    // ---- Timers ----
    {
        CPUTIMER_VARS expected = {};
        ConfigCpuTimer(&expected, LAUNCHPAD_CPU_FREQUENCY, 10000);
        validator.register_comparison("CpuTimer0", CpuTimer0, expected);
        ConfigCpuTimer(&expected, LAUNCHPAD_CPU_FREQUENCY, 20000);
        validator.register_comparison("CpuTimer1", CpuTimer1, expected);
        // HW2 reference uses 1000 us (1 kHz). Spec says timer2 unspecified; the
        // 1 ms tick lines up with the 1 ms ADC SOC trigger.
        ConfigCpuTimer(&expected, LAUNCHPAD_CPU_FREQUENCY, 1000);
        validator.register_comparison("CpuTimer2", CpuTimer2, expected);
    }
    {
        CPUTIMER_REGS expected = {};
        expected.TCR.all = 0x4000;
        validator.register_comparison("CpuTimer0Regs", CpuTimer0Regs, expected);
        validator.register_comparison("CpuTimer1Regs", CpuTimer1Regs, expected);
        validator.register_comparison("CpuTimer2Regs", CpuTimer2Regs, expected);
    }

    // ---- IER + PIE ----
    {
        // M_INT1 (PIE group 1: ADCA1, TIMER0)
        // M_INT8 (SCIC/SCID), M_INT9 (SCIA), M_INT12 (SWI), M_INT13 (TIMER1), M_INT14 (TIMER2)
        constexpr uint16_t expected = M_INT1 | M_INT8 | M_INT9 | M_INT12 | M_INT13 | M_INT14;
        validator.register_custom("IER", IER, expected);
    }
    {
        PIE_CTRL_REGS expected = {};
        expected.PIEIER1.bit.INTx7 = 1; // TIMER0
        expected.PIEIER12.bit.INTx9 = 1; // SWI
        expected.PIEIER1.bit.INTx1 = 1; // ADCA1
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
        expected.ADCA1_INT = &ADCA_ISR;
        expected.EMIF_ERROR_INT = &SWI_isr;
        validator.register_comparison("PieVectTable", PieVectTable, expected);
    }

    // ---- EPWM12A: 5 kHz, AQCTLA configured for square output ----
    {
        const auto &cmpa = EPwm12Regs.CMPA.bit;
        success &= report(EPwm12Regs.TBPRD >= 9000 && EPwm12Regs.TBPRD <= 11000,
                          "HW2/init", "EPwm12Regs.TBPRD not ~10000 (expected 50 MHz / 5 kHz)",
                          "TBPRD = 10000 yields 50 MHz / 10000 = 5 kHz")
                       ? 1
                       : 0;
        success &= report(EPwm12Regs.AQCTLA.bit.CAU == 1,
                          "HW2/init", "EPwm12Regs.AQCTLA.CAU != 1",
                          "AQCTLA.CAU = 1 (clear output on counter == CMPA)")
                       ? 1
                       : 0;
        success &= report(EPwm12Regs.AQCTLA.bit.ZRO == 2,
                          "HW2/init", "EPwm12Regs.AQCTLA.ZRO != 2",
                          "AQCTLA.ZRO = 2 (set output high on counter == 0)")
                       ? 1
                       : 0;
        success &= report(EPwm12Regs.TBCTL.bit.CTRMODE == 0,
                          "HW2/init", "EPwm12Regs.TBCTL.CTRMODE != 0 (count-up)")
                       ? 1
                       : 0;
        success &= report(EPwm12Regs.TBCTL.bit.CLKDIV == 0,
                          "HW2/init", "EPwm12Regs.TBCTL.CLKDIV != 0 (divide by 1)")
                       ? 1
                       : 0;
        // The reference initialises CMPA=0; ADCA_ISR may have updated it by now.
        (void) cmpa;
    }

    // ---- EPWM4: 1 kHz SOCA trigger ----
    {
        success &= report(EPwm4Regs.TBPRD >= 49000 && EPwm4Regs.TBPRD <= 51000,
                          "HW2/init", "EPwm4Regs.TBPRD not ~50000 (expected 50 MHz / 1 kHz)",
                          "TBPRD = 50000 yields 50 MHz / 50000 = 1 kHz")
                       ? 1
                       : 0;
        success &= report(EPwm4Regs.ETSEL.bit.SOCAEN == 1,
                          "HW2/init", "EPwm4Regs.ETSEL.SOCAEN != 1 (must enable SOCA trigger)")
                       ? 1
                       : 0;
        success &= report(EPwm4Regs.ETSEL.bit.SOCASEL == 2,
                          "HW2/init", "EPwm4Regs.ETSEL.SOCASEL != 2 (must trigger on counter == TBPRD)")
                       ? 1
                       : 0;
        success &= report(EPwm4Regs.ETPS.bit.SOCAPRD == 1,
                          "HW2/init", "EPwm4Regs.ETPS.SOCAPRD != 1 (trigger every 1st event)")
                       ? 1
                       : 0;
    }

    // ---- ADCA: SOC0 = ADCINA4 (photodiode), SOC1 = ADCINA2 (joystick X), SOC2 = ADCINA3 (joystick Y) ----
    {
        success &= report(AdcaRegs.ADCSOC0CTL.bit.CHSEL == 4,
                          "HW2/init", "AdcaRegs.ADCSOC0CTL.CHSEL != 4 (ADCINA4 = photodiode)")
                       ? 1
                       : 0;
        success &= report(AdcaRegs.ADCSOC1CTL.bit.CHSEL == 2,
                          "HW2/init", "AdcaRegs.ADCSOC1CTL.CHSEL != 2 (ADCINA2 = joystick X)")
                       ? 1
                       : 0;
        success &= report(AdcaRegs.ADCSOC2CTL.bit.CHSEL == 3,
                          "HW2/init", "AdcaRegs.ADCSOC2CTL.CHSEL != 3 (ADCINA3 = joystick Y)")
                       ? 1
                       : 0;
        // EPWM4 SOCA trigger source = 0x0B per TI reference.
        success &= report(AdcaRegs.ADCSOC0CTL.bit.TRIGSEL == 0x0B,
                          "HW2/init", "AdcaRegs.ADCSOC0CTL.TRIGSEL != 0x0B (EPWM4 ADCSOCA)")
                       ? 1
                       : 0;
        success &= report(AdcaRegs.ADCINTSEL1N2.bit.INT1SEL == 2,
                          "HW2/init", "ADCINTSEL1N2.INT1SEL != 2 (interrupt on last SOC)")
                       ? 1
                       : 0;
        success &= report(AdcaRegs.ADCINTSEL1N2.bit.INT1E == 1,
                          "HW2/init", "ADCINTSEL1N2.INT1E != 1 (must enable ADCA1 interrupt)")
                       ? 1
                       : 0;
        success &= report(AdcaRegs.ADCCTL1.bit.ADCPWDNZ == 1,
                          "HW2/init", "AdcaRegs.ADCCTL1.ADCPWDNZ != 1 (ADCA must be powered up)")
                       ? 1
                       : 0;
    }

    success &= validator.validate();
    return success;
}

// Inject ADCA results, call ADCA_ISR, verify scaling math.
int check_adc_isr_scaling(Validator *) {
    spdlog::info("[HW2] check_adc_isr_scaling");
    int success = 1;

    if (!&ADCA_ISR || !&Adca4result || !&JoystickXresult || !&JoystickYresult
        || !&Adca4voltage || !&JoystickXvoltage || !&JoystickYvoltage) {
        spdlog::error("[HW2/adc_scaling] missing student symbols (ADCA_ISR, Adca4result, "
            "JoystickX/Yresult, Adca4voltage, JoystickX/Yvoltage)");
        return 0;
    }

    const struct Snapshot {
        int16_t adca4r, jxr, jyr;
        float adca4v, jxv, jyv;
        int32_t count;
    } snap{
        Adca4result, JoystickXresult, JoystickYresult,
        Adca4voltage, JoystickXvoltage, JoystickYvoltage,
        &AdcaInterruptCount ? AdcaInterruptCount : 0
    };

    auto run_case = [&](const uint16_t r0, const uint16_t r1, const uint16_t r2, const char *label) {
        grader::inject_adc_result(grader::AdcModule::A, 0, r0);
        grader::inject_adc_result(grader::AdcModule::A, 1, r1);
        grader::inject_adc_result(grader::AdcModule::A, 2, r2);
        const int32_t before = AdcaInterruptCount;
        ADCA_ISR();

        success &= report(static_cast<uint16_t>(Adca4result) == r0,
                          "HW2/adc_scaling", (std::string(label) + ": Adca4result mismatch").c_str())
                       ? 1
                       : 0;
        success &= report(static_cast<uint16_t>(JoystickXresult) == r1,
                          "HW2/adc_scaling", (std::string(label) + ": JoystickXresult mismatch").c_str())
                       ? 1
                       : 0;
        success &= report(static_cast<uint16_t>(JoystickYresult) == r2,
                          "HW2/adc_scaling", (std::string(label) + ": JoystickYresult mismatch").c_str())
                       ? 1
                       : 0;
        success &= report(approx_eq(Adca4voltage, r0 * 3.0f / 4096.0f, 5e-3f),
                          "HW2/adc_scaling", (std::string(label) + ": Adca4voltage scaling off").c_str(),
                          "voltage = result * 3.0f / 4096.0f")
                       ? 1
                       : 0;
        success &= report(approx_eq(JoystickXvoltage, r1 * 3.0f / 4096.0f, 5e-3f),
                          "HW2/adc_scaling", (std::string(label) + ": JoystickXvoltage scaling off").c_str())
                       ? 1
                       : 0;
        success &= report(approx_eq(JoystickYvoltage, r2 * 3.0f / 4096.0f, 5e-3f),
                          "HW2/adc_scaling", (std::string(label) + ": JoystickYvoltage scaling off").c_str())
                       ? 1
                       : 0;
        success &= report(AdcaInterruptCount == before + 1,
                          "HW2/adc_scaling",
                          (std::string(label) + ": AdcaInterruptCount did not advance").c_str())
                       ? 1
                       : 0;
        success &= report(AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 == 1,
                          "HW2/adc_scaling",
                          "ADCINTFLGCLR.ADCINT1 not set — ISR must clear the interrupt flag")
                       ? 1
                       : 0;
    };

    run_case(4095, 2048, 0, "max/half/zero");
    run_case(0, 4095, 2048, "rotated");
    run_case(1024, 1024, 1024, "uniform-quarter");

    Adca4result = snap.adca4r;
    JoystickXresult = snap.jxr;
    JoystickYresult = snap.jyr;
    Adca4voltage = snap.adca4v;
    JoystickXvoltage = snap.jxv;
    JoystickYvoltage = snap.jyv;
    if (&AdcaInterruptCount) AdcaInterruptCount = snap.count;
    return success;
}

// Spec: voltages printed every 100 ms via UARTPrint set inside ADCA_ISR (every 100th call).
int check_print_cadence(Validator *) {
    spdlog::info("[HW2] check_print_cadence");

    if (!&ADCA_ISR) {
        spdlog::error("[HW2/print_cadence] ADCA_ISR not defined by student");
        return 0;
    }

    grader::inject_adc_result(grader::AdcModule::A, 0, 1000);
    grader::inject_adc_result(grader::AdcModule::A, 1, 1500);
    grader::inject_adc_result(grader::AdcModule::A, 2, 2500);

    grader::resetPrintfCapture();
    UARTPrint = 0;
    const int32_t initialCount = (&AdcaInterruptCount) ? AdcaInterruptCount : 0;

    // Drive ADCA_ISR 1000 times in 10 bursts of 100 (each burst should trigger one print).
    for (int burst = 0; burst < 10; ++burst) {
        for (int i = 0; i < 100; ++i) {
            ADCA_ISR();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    if (&AdcaInterruptCount) AdcaInterruptCount = initialCount;

    return grader::expect_print_cadence(grader::SerialPort::SCIA, 10, 0.30,
                                        "HW2/print_cadence")
               ? 1
               : 0;
}

int check_print_format(Validator *) {
    spdlog::info("[HW2] check_print_format");

    if (!&ADCA_ISR) {
        spdlog::error("[HW2/print_format] ADCA_ISR not defined by student");
        return 0;
    }

    grader::inject_adc_result(grader::AdcModule::A, 0, 1234);
    grader::inject_adc_result(grader::AdcModule::A, 1, 2345);
    grader::inject_adc_result(grader::AdcModule::A, 2, 3456);

    grader::resetPrintfCapture();
    UARTPrint = 0;
    const int32_t initialCount = (&AdcaInterruptCount) ? AdcaInterruptCount : 0;

    for (int i = 0; i < 200; ++i) ADCA_ISR();
    std::this_thread::sleep_for(std::chrono::milliseconds(80));

    if (&AdcaInterruptCount) AdcaInterruptCount = initialCount;

    const grader::PrintfCall *latest = grader::latestPrintfCall(grader::SerialPort::SCIA);
    if (!latest) {
        spdlog::error("[HW2/print_format] no SCIA prints captured — UARTPrint may not be set "
            "by ADCA_ISR or main thread isn't servicing it");
        return 0;
    }
    // Spec (paraphrased): three floats — photodiode voltage, joystick X voltage, joystick Y voltage.
    return grader::expect_arg_types(latest,
                                    {grader::ArgType::Float, grader::ArgType::Float, grader::ArgType::Float},
                                    "HW2/print_format")
               ? 1
               : 0;
}

CheckFunctions checker() {
    spdlog::info("Checking HW 2");
    return {
        &check_initialization,
        &check_adc_isr_scaling,
        &check_print_cadence,
        &check_print_format,
    };
}
