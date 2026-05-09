//
// Created by marius on 2/20/26.
//

#ifndef AUTOMATICGRADER_VALIDATOR_H
#define AUTOMATICGRADER_VALIDATOR_H
#include <string>
#include <thread>
#include <vector>

class Validator;

typedef int (*CheckFunction)(Validator *);

typedef int (*MainFunction)();

typedef std::vector<CheckFunction> CheckFunctions;

struct CheckResult {
    std::string name;
    bool passed;
    std::vector<std::string> messages;
};


class Validator {
public:
    explicit Validator(const CheckFunctions &checkFunctions);

    [[nodiscard]] int check();

    void start_main_thread();

    void set_json_report_path(std::string path) { json_report_path = std::move(path); }

    [[nodiscard]] const std::vector<CheckResult>& results() const { return check_results; }

private:
    CheckFunctions checkFunctions;
    std::vector<CheckResult> check_results;
    std::string json_report_path;
    std::jthread main_thread;
};

CheckFunctions checker();

Validator get_validator();

#endif //AUTOMATICGRADER_VALIDATOR_H