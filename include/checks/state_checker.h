//
// Created by marius on 2/20/26.
//

#ifndef AUTOMATICGRADER_STATE_CHECKER_H
#define AUTOMATICGRADER_STATE_CHECKER_H
#include <functional>
#include <iostream>
#include <ranges>
#include <string>
#include <unordered_map>

#include "checks/compare_generated.hpp"

#define DEFAULT_NUM_SPOTS 200 // Needed otherwise we have string hash collisions


using ZeroCheckFunc = std::function<bool()>;

template<typename T>
using CheckFunc = std::function<bool(const T &, const T &, const std::string &)>;


template<typename T>
const inline CheckFunc<T> defaultComparator = [
        ](const auto &a, const auto &b, const auto &name)-> bool {
    return check_compare(a, b, name);
};

class HardwareStateValidator {
public:
    HardwareStateValidator(): tracker_(DEFAULT_NUM_SPOTS) {
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

    template<typename T>
    void register_comparison_copy(const std::string &reg_name, const T actual_reg, const T expected_state) {
        if (tracker_.contains(reg_name)) {
            std::cout << "Be careful as " << reg_name << " is already used as a key" << std::endl;
        }

        tracker_[reg_name] = [actual_reg = std::move(actual_reg), expected_state = std::move(expected_state), reg_name
                ]() -> bool {
                    return check_compare(actual_reg, expected_state, reg_name);
                };
    }

    template<typename T, std::size_t N>
    void register_comparison(const std::string &reg_name, const T (&actual_reg)[N], const T (&expected_state)[N]) {
        tracker_[reg_name] = [actual_reg = std::to_array(actual_reg), expected_state = std::to_array(expected_state),
                    reg_name]() -> bool {
                    return check_compare(actual_reg, expected_state, reg_name);
                };
    }

    template<typename T>
    void register_custom(const std::string &reg_name, const T &actual_reg, const T &expected_state,
                         const CheckFunc<T> &check_func = defaultComparator<T>) {
        tracker_[reg_name] = [&actual_reg, expected_state = std::move(expected_state), reg_name, &check_func
                ]() -> bool {
                    return check_func(actual_reg, expected_state, reg_name);
                };
    }

    template<typename T>
    void register_custom_copy(const std::string &reg_name, const T actual_reg, const T expected_state,
                              const CheckFunc<T> &check_func = defaultComparator<T>) {

        if (tracker_.contains(reg_name)) {
            std::cout << "Be careful as " << reg_name << " is already used as a key" << std::endl;
        }

        tracker_[reg_name] = [actual_reg= std::move(actual_reg), expected_state = std::move(expected_state),
                    reg_name, &check_func
                ]() -> bool {
                    return check_func(actual_reg, expected_state, reg_name);
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
