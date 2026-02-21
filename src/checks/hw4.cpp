//
// Created by marius on 2/20/26.
//

#include "checks/hw1.h"

#include <iostream>
#include <ostream>
#include <cmath>

#include "checks/validator.h"


int check_initialization() {
    std::cout << "Check initialization" << std::endl;

    std::vector<GpioSetup> values = {};

    return 0;
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
    std::cout << "Checking HW4" << std::endl;

    return {
        &check_initialization,
        &check_timer0,
        &check_timer1,
        &check_timer2,
        &check_saturate
    };
}