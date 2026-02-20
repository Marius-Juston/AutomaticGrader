//
// Created by marius on 2/20/26.
//

#ifndef AUTOMATICGRADER_VALIDATOR_H
#define AUTOMATICGRADER_VALIDATOR_H
#include <vector>

typedef int (*CheckFunction)();

typedef std::vector<CheckFunction> CheckFunctions;

class Validator {
public:
    explicit Validator(const CheckFunctions &checkFunctions);

    [[nodiscard]] int check() const;

private:
    CheckFunctions checkFunctions;
};

CheckFunctions checker();

Validator get_validator();

#endif //AUTOMATICGRADER_VALIDATOR_H
