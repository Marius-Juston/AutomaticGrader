//
// Created by marius on 2/20/26.
//

#ifndef AUTOMATICGRADER_STATE_CHECKER_H
#define AUTOMATICGRADER_STATE_CHECKER_H
#include <functional>
#include <ranges>
#include <string>
#include <unordered_map>

class HardwareStateValidator {
public:
    using ZeroCheckFunc = std::function<bool()>;

    HardwareStateValidator() {
        populate_map();
    }

    void mark_as_used(const std::string &reg_name) {
        tracker_.erase(reg_name);
    }

    bool validate_unused_are_zero() {
        bool all_clean = true;
        for (const auto &check_func: tracker_ | std::views::values) {
            if (!check_func()) {
                all_clean = false;
            }
        }
        return all_clean;
    }

private:
    std::unordered_map<std::string, ZeroCheckFunc> tracker_;

    void populate_map();
};

#endif //AUTOMATICGRADER_STATE_CHECKER_H