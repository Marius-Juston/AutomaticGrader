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


HardwareStateValidator validator;

int check_initialization() {
    std::cout << "Check initialization" << std::endl;

    int success = 0;

    success &= validator.validate();

    // temp_main();

    GpioSetup expectedGpioSetup[MAX_GPIO];
    for (size_t i = 0; i < MAX_GPIO; ++i) {
        expectedGpioSetup[i] = gpiosSetup[i];
    }

    expectedGpioSetup[31] = {
        GPIO_MUX_CPU2, 1, GPIO_INPUT, GPIO_PULLUP
    };

    // check_compare(gpiosSetup, expectedGpioSetup, "Hello");

    CPUTIMER_VARS expected = {};
    expected.CPUFreqInMHz = 1;

    validator.register_comparison("GpioSetup", gpiosSetup, expectedGpioSetup);
    validator.register_comparison("CpuTimer0", CpuTimer0, expected);

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
