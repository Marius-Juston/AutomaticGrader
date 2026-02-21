//
// Created by marius on 2/20/26.
//

#ifndef AUTOMATICGRADER_STATE_CHECKER_H
#define AUTOMATICGRADER_STATE_CHECKER_H
#include <functional>
#include <ranges>
#include <string>
#include <unordered_map>

#include "checks/compare_generated.hpp"

class HardwareStateValidator {
public:
    using ZeroCheckFunc = std::function<bool()>;

    HardwareStateValidator() {
        populate_map();
    }

    void mark_as_used(const std::string &reg_name) {
        tracker_.erase(reg_name);
    }

    template<typename T>
    void register_comparison(const std::string &reg_name, const T &actual_reg, const T &expected_state) {
        tracker_[reg_name] = [&actual_reg, expected_state = std::move(expected_state), reg_name]() -> bool {
            return check_compare(actual_reg, expected_state, reg_name);
        };
    }

    template<typename T, std::size_t N>
    void register_comparison(const std::string &reg_name, const T (&actual_reg)[N], const T (&expected_state)[N]) {
        tracker_[reg_name] = [&actual_reg, expected_state = std::to_array(expected_state), reg_name]() -> bool {
            return check_compare(actual_reg, expected_state, reg_name);
        };
    }

    bool validate() {
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