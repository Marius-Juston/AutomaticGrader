# GTest/GMock unit tests for the grader harness.
#
# Build is gated by GRADER_BUILD_TESTS (default OFF) so existing CI is unaffected.
#
# Linkage strategy:
#   - Reuse object libraries grader_infra and grader_stubs (already built by
#     the parent CMakeLists). These give us format_parser/synthetic_clock/
#     printf_capture/expectations/stimulus + ti_stubs.cpp + main_loop_driver.cpp.
#   - Compile a fresh, test-local copy of src/checks/validator.cpp with HW=1
#     so the #include "checks/hw1.h" resolves. We do NOT link the per-assignment
#     checker source nor the patched student source; the weak `checker()` falls
#     back to {} and `temp_main` is provided as a stub in tests/test_main.cpp.

# Allow standalone use (cmake -S tests -B build) but normally invoked from
# the parent.
cmake_minimum_required(VERSION 3.21)

find_package(GTest REQUIRED)

# Test-local validator.cpp recompile: we need the same flags as the production
# grader_state_check object library (HW=1, AUTO_GRADER, -include ti_stubs.h,
# -Wno-volatile) but isolated so it doesn't depend on the per-assignment .cpp.
add_library(grader_validator_for_tests OBJECT
        ${PROJECT_SOURCE_DIR}/src/checks/validator.cpp
)
target_include_directories(grader_validator_for_tests PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/common/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include
        ${PROJECT_SOURCE_DIR}/lib/default
)
target_link_libraries(grader_validator_for_tests PUBLIC spdlog::spdlog)
target_compile_definitions(grader_validator_for_tests PRIVATE
        AUTO_GRADER HW=1 ASSIGNMENT_HW
)
target_compile_options(grader_validator_for_tests PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile>
        -include ti_stubs.h
)

add_executable(grader_tests
        test_main.cpp
        test_cli_args.cpp
        test_json_escape.cpp
        test_validator.cpp
        test_format_parser.cpp
        test_synthetic_clock.cpp
        test_printf_capture.cpp
        test_expectations.cpp
        test_stimulus.cpp
        test_main_loop_driver.cpp
        $<TARGET_OBJECTS:grader_infra>
        $<TARGET_OBJECTS:grader_stubs>
        $<TARGET_OBJECTS:grader_validator_for_tests>
)

target_include_directories(grader_tests BEFORE PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)
target_include_directories(grader_tests PRIVATE
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/common/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/driverlib/f2837xd/driverlib
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/libraries/dsp/FPU/c28/include
        ${PROJECT_SOURCE_DIR}/lib/C2000Ware_4_01_00_00/device_support/f2837xd/headers/include
        ${PROJECT_SOURCE_DIR}/lib/default
)

target_compile_definitions(grader_tests PRIVATE
        AUTO_GRADER HW=1 ASSIGNMENT_HW
)
target_compile_options(grader_tests PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile>
        -include ti_stubs.h
)

target_link_libraries(grader_tests PRIVATE
        GTest::gtest
        spdlog::spdlog
        ${CMAKE_DL_LIBS}
)

# Export symbols so dladdr in resolve_check_name finds free-function check fakes.
if (NOT MINGW)
    target_link_options(grader_tests PRIVATE -rdynamic)
endif ()

include(GoogleTest)
gtest_discover_tests(grader_tests
        DISCOVERY_TIMEOUT 30
        PROPERTIES
        ENVIRONMENT "GTEST_SHUFFLE=1"
)
