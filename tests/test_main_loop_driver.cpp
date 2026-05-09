// GTest coverage for include/checks/main_loop_driver.h.
//
// `temp_main` is provided as a test-only stub in tests/test_main.cpp. It tracks
// how many times it has been entered and what value `grader_main_loop_iterations()`
// returns at entry, simulating the patched-student-source contract:
//   - First entry: init runs, body runs `iters` times, function returns.
//   - Subsequent entries with iters > 0: body runs `iters` times.
// Without the real init guard we just track inits/bodies coarsely.

#include <gtest/gtest.h>

#include <cstdint>

#include "checks/main_loop_driver.h"
#include "checks/synthetic_clock.h"

extern "C" int grader_main_loop_iterations(void);

// Test-only counters owned by the temp_main stub (defined in test_main.cpp).
extern int g_temp_main_call_count;
extern int g_temp_main_last_iters;

namespace {
    class MainLoopDriverTest : public ::testing::Test {
    protected:
        void SetUp() override {
            grader::reset_student_init();
            g_temp_main_call_count = 0;
            g_temp_main_last_iters = -1;
            grader::synthetic_clock_reset();
        }
    };

    TEST_F(MainLoopDriverTest, RunStudentInit_FirstCall_InvokesTempMainOnce) {
        grader::run_student_init();
        EXPECT_EQ(g_temp_main_call_count, 1);
        EXPECT_EQ(g_temp_main_last_iters, 0); // init pass -> iterations=0
    }

    TEST_F(MainLoopDriverTest, RunStudentInit_Idempotent_SecondCallNoOp) {
        grader::run_student_init();
        grader::run_student_init();
        EXPECT_EQ(g_temp_main_call_count, 1);
    }

    TEST_F(MainLoopDriverTest, ResetStudentInit_AllowsReinit) {
        grader::run_student_init();
        grader::reset_student_init();
        grader::run_student_init();
        EXPECT_EQ(g_temp_main_call_count, 2);
    }

    TEST_F(MainLoopDriverTest, StepMainLoop_NonZeroIters_InvokesTempMain) {
        grader::run_student_init();
        g_temp_main_call_count = 0;
        grader::step_main_loop(3);
        EXPECT_EQ(g_temp_main_call_count, 1);
        EXPECT_EQ(g_temp_main_last_iters, 3);
    }

    TEST_F(MainLoopDriverTest, StepMainLoop_ZeroIters_NoOp) {
        // Boundary: iterations=0 must NOT enter temp_main again.
        grader::run_student_init();
        g_temp_main_call_count = 0;
        grader::step_main_loop(0);
        EXPECT_EQ(g_temp_main_call_count, 0);
    }

    TEST_F(MainLoopDriverTest, StepMainLoop_NegativeIters_NoOp) {
        // Boundary: negative iterations must NOT enter temp_main.
        grader::run_student_init();
        g_temp_main_call_count = 0;
        grader::step_main_loop(-5);
        EXPECT_EQ(g_temp_main_call_count, 0);
    }

    TEST_F(MainLoopDriverTest, StepMainLoop_ResetsItersToZeroAfter) {
        grader::run_student_init();
        grader::step_main_loop(7);
        // After step_main_loop, the iter counter is reset to 0 so a subsequent
        // direct query (which is what the patched student source does on the
        // first re-entry of its loop macro) sees zero.
        EXPECT_EQ(grader_main_loop_iterations(), 0);
    }

    TEST_F(MainLoopDriverTest, DriveIsrWithMainPump_AdvancesClockAndPumpsLoop) {
        grader::run_student_init();
        g_temp_main_call_count = 0;

        int isr_calls = 0;
        auto isr = [&] { ++isr_calls; };
        grader::drive_isr_with_main_pump(isr, /*period_us=*/1000, /*total_ticks=*/4);

        EXPECT_EQ(isr_calls, 4);
        // step_main_loop is invoked once per tick; each step enters temp_main once.
        EXPECT_EQ(g_temp_main_call_count, 4);
        EXPECT_EQ(grader::synthetic_clock_now_us(), 4000u);
    }

    TEST_F(MainLoopDriverTest, DriveIsrWithMainPump_ZeroTicks_NoCalls) {
        grader::run_student_init();
        g_temp_main_call_count = 0;
        int isr_calls = 0;
        grader::drive_isr_with_main_pump([&] { ++isr_calls; }, 1000, 0);
        EXPECT_EQ(isr_calls, 0);
        EXPECT_EQ(g_temp_main_call_count, 0);
    }

    TEST_F(MainLoopDriverTest, SetMainLoopIterations_PropagatesToQuery) {
        grader::set_main_loop_iterations(42);
        EXPECT_EQ(grader_main_loop_iterations(), 42);
        grader::set_main_loop_iterations(0);
    }
} // namespace