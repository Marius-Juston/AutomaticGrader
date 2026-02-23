//
// Created by marius on 2/20/26.
//

#include "checks/hw3.h"

#include <spdlog/spdlog.h>

#include "checks/validator.h"


int check_initialization(Validator *) {
    spdlog::info("Check initialization");

    std::vector<GpioSetup> values = {};

    return 0;
}

int check_timer0(Validator *) {
    spdlog::info("Check timer 0");
    return 0;
}

int check_timer1(Validator *) {
    spdlog::info("Check timer 1");
    return 0;
}

int check_timer2(Validator *) {
    spdlog::info("Check timer 2");
    return 0;
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
    spdlog::info("Checking HW 3");

    return {
        &check_initialization,
        &check_timer0,
        &check_timer1,
        &check_timer2,
        &check_saturate
    };
}
