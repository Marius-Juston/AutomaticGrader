//
// Created by marius on 2/20/26.
//

#include "checks/hw1.h"

#include <iostream>
#include <ostream>
#include <cmath>

#include "checks/compare_generated.hpp"
#include "checks/state_checker.h"
#include "checks/validator.h"
#include "checks/state_checker.h"

#define LAUNCHPAD_CPU_FREQUENCY 200

HardwareStateValidator validator;

int check_initialization() {
    std::cout << "Check initialization" << std::endl;

    int success = 0;

    success &= validator.validate();

    temp_main();

    {
        GpioSetup expected[MAX_GPIO];
        for (size_t i = 0; i < MAX_GPIO; ++i) {
            expected[i] = gpiosSetup[i];
        }

        expected[31] = {GPIO_MUX_CPU1, 0, GPIO_OUTPUT, GPIO_PUSHPULL};
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
        const uint16_t expected = M_INT1 | M_INT8 | M_INT9 | M_INT12 | M_INT13 | M_INT14;
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

        validator.register_comparison("GpioDataRegs", GpioDataRegs, expected);
    }

    success &= validator.validate();

    return success;
}

int check_timer0() {
    std::cout << "Check timer 0" << std::endl;
    return 0;
}

int check_timer1() {
    std::cout << "Check timer 1" << std::endl;
    return 0;
}

int check_timer2() {
    std::cout << "Check timer 2" << std::endl;
    return 0;
}

int check_saturate() {
    std::cout << "Check saturate" << std::endl;

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
                    std::cerr << "Expecting saturate(" << value << "," << sat << ")=" << actual << " got " << res <<
                            std::endl;

                    if (-actual == res) {
                        std::cerr << "Check that you have the right output sign for your funciton" << std::endl;
                    }

                    if (res == value) {
                        if (value > 0) {
                            std::cerr << "Check that you bound the positive saturation value" << std::endl;
                        } else if (value < 0) {
                            std::cerr << "Check that you bound the negative saturation value" << std::endl;
                        }
                    }

                    success = 0;
                }
            }
        }
    } else {
        std::cerr << "float saturate(float, float) not available\n";
        success = 0;
    }

    return success;
}

CheckFunctions checker() {
    std::cout << "Checking HW1" << std::endl;

    return {
        &check_initialization,
        &check_timer0,
        &check_timer1,
        &check_timer2,
        &check_saturate
    };
}
