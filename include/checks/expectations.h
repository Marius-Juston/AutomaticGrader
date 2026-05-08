#ifndef AUTOMATICGRADER_CHECKS_EXPECTATIONS_H
#define AUTOMATICGRADER_CHECKS_EXPECTATIONS_H

#include <cstddef>
#include <initializer_list>
#include <string_view>

#include "checks/format_parser.h"
#include "checks/printf_capture.h"

namespace grader {

[[nodiscard]] bool expect_format(const PrintfCall* call,
                                 std::string_view expected_fmt,
                                 std::string_view check_name = {});

[[nodiscard]] bool expect_format(const PrintfCall& call,
                                 std::string_view expected_fmt,
                                 std::string_view check_name = {});

[[nodiscard]] bool expect_arg_types(const PrintfCall* call,
                                    std::initializer_list<ArgType> types,
                                    std::string_view check_name = {});

[[nodiscard]] bool expect_arg_types(const PrintfCall& call,
                                    std::initializer_list<ArgType> types,
                                    std::string_view check_name = {});

[[nodiscard]] bool expect_print_cadence(SerialPort port,
                                        std::size_t expected_count,
                                        double tolerance_pct = 0.10,
                                        std::string_view check_name = {});

[[nodiscard]] bool expect_print_cadence_window(SerialPort port,
                                               std::size_t min_count,
                                               std::size_t max_count,
                                               std::string_view check_name = {});

[[nodiscard]] bool expect_min_print_calls(SerialPort port,
                                          std::size_t min_count,
                                          std::string_view check_name = {});

[[nodiscard]] bool expect_format_any(SerialPort port,
                                     std::string_view expected_fmt,
                                     std::string_view check_name = {});

}

#endif
