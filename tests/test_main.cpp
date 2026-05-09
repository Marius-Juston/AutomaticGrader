// Test runner main() and stubs for symbols normally provided by the patched
// student source / per-assignment checker.

#include <gtest/gtest.h>

extern "C" int grader_main_loop_iterations(void);

// Test-only state observed by test_main_loop_driver.cpp.
int g_temp_main_call_count = 0;
int g_temp_main_last_iters = -1;

// Stub for the symbol the real grader pulls from the patched student source.
// We don't care about reproducing the init-guard semantics; the unit tests
// for main_loop_driver track call count and the iter-count argument.
extern "C" int temp_main() {
    ++g_temp_main_call_count;
    g_temp_main_last_iters = grader_main_loop_iterations();
    return 0;
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
