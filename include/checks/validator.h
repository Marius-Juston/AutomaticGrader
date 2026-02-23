//
// Created by marius on 2/20/26.
//

#ifndef AUTOMATICGRADER_VALIDATOR_H
#define AUTOMATICGRADER_VALIDATOR_H
#include "pthread.h"
#include <vector>

class Validator;

typedef int (*CheckFunction)(Validator *);

typedef int (*MainFunction)();

typedef std::vector<CheckFunction> CheckFunctions;


class Validator {
public:
    explicit Validator(const CheckFunctions &checkFunctions);

    [[nodiscard]] int check();

    void start_main_thread();

private:
    CheckFunctions checkFunctions;
    pthread_t main_thread;
};

CheckFunctions checker();

Validator get_validator();

#endif //AUTOMATICGRADER_VALIDATOR_H
