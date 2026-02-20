//
// Created by marius on 2/20/26.
//
#include "checks/validator.h"

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

int Validator::check() const {
    int result = 0;

    for (const auto &checkFunction: checkFunctions) {
        result = checkFunction();
    }

    return result;
}

__attribute__((weak))
CheckFunctions checker() {
    return {};
}

Validator get_validator() {
    return Validator(checker());
}
