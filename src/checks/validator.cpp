//
// Created by marius on 2/20/26.
//
#include "checks/validator.h"

#include <functional>
#include <thread>
#include <chrono>
#include <csignal>
#include <iostream>
#include <bits/sigthread.h>

#if HW == 1
#include "checks/hw1.h"
#elif HW == 2
#include "checks/hw2.h"
#elif HW == 3
#include "checks/hw3.h"
#elif HW == 4
#include "checks/hw4.h"
#else
#error "Invalid HW selection"
#endif


Validator::Validator(const std::vector<CheckFunction> &checkFunctions) : checkFunctions(checkFunctions) {
}

void Validator::start_main_thread() {
    main_thread = std::jthread(temp_main);
    main_thread.detach();

    // Wait for 1 second for us to reach the main internal loop
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

int Validator::check() {
    int result = 0;

    for (const auto &checkFunction: checkFunctions) {
        result = checkFunction(this);
    }

    return result;
}

__attribute__ ((weak))

CheckFunctions checker() {
    return {};
}

Validator get_validator() {
    return Validator(checker());
}
