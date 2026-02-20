//
// Created by marius on 2/20/26.
//
#include "checks/validator.h"
#include <iostream>
#include <type_traits>
#include <array>
#include <string>

Validator::Validator(const std::vector<CheckFunction> &checkFunctions) : checkFunctions(checkFunctions) {
}

int Validator::check() const {
    int result = 0;

    for (const auto &checkFunction: checkFunctions) {
        result = checkFunction();
    }

    return result;
}


Validator get_validator() {
    return Validator(checker());
}
