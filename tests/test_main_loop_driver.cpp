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
        // Arrange
        // (SetUp resets state)
        // Act
        grader::run_student_init();
        // Assert
        EXPECT_EQ(g_temp_main_call_count, 1);
        EXPECT_EQ(g_temp_main_last_iters, 0); // init pass -> iterations=0
    }

    TEST_F(MainLoopDriverTest, RunStudentInit_Idempotent_SecondCallNoOp) {
        // Arrange
        grader::run_student_init();
        // Act
        grader::run_student_init();
        // Assert
        EXPECT_EQ(g_temp_main_call_count, 1);
    }

    TEST_F(MainLoopDriverTest, ResetStudentInit_AllowsReinit) {
        // Arrange
        grader::run_student_init();
        grader::reset_student_init();
        // Act
        grader::run_student_init();
        // Assert
        EXPECT_EQ(g_temp_main_call_count, 2);
    }

    TEST_F(MainLoopDriverTest, StepMainLoop_NonZeroIters_InvokesTempMain) {
        // Arrange
        grader::run_student_init();
        g_temp_main_call_count = 0;
        // Act
        grader::step_main_loop(3);
        // Assert
        EXPECT_EQ(g_temp_main_call_count, 1);
        EXPECT_EQ(g_temp_main_last_iters, 3);
    }

    TEST_F(MainLoopDriverTest, StepMainLoop_ZeroIters_NoOp) {
        // Arrange: iterations=0 must NOT enter temp_main again.
        grader::run_student_init();
        g_temp_main_call_count = 0;
        // Act
        grader::step_main_loop(0);
        // Assert
        EXPECT_EQ(g_temp_main_call_count, 0);
    }

    TEST_F(MainLoopDriverTest, StepMainLoop_NegativeIters_NoOp) {
        // Arrange: negative iterations must NOT enter temp_main.
        grader::run_student_init();
        g_temp_main_call_count = 0;
        // Act
        grader::step_main_loop(-5);
        // Assert
        EXPECT_EQ(g_temp_main_call_count, 0);
    }

    TEST_F(MainLoopDriverTest, StepMainLoop_ResetsItersToZeroAfter) {
        // Arrange
        grader::run_student_init();
        // Act
        grader::step_main_loop(7);
        // Assert: after step, the iter counter is reset to 0.
        EXPECT_EQ(grader_main_loop_iterations(), 0);
    }

    TEST_F(MainLoopDriverTest, StepMainLoop_LargeIterCount_PropagatesExactly) {
        // Arrange: BVA — large iteration count (1,000,000).
        grader::run_student_init();
        g_temp_main_call_count = 0;
        // Act
        grader::step_main_loop(1'000'000);
        // Assert: temp_main entered once, with iter count exactly 1,000,000.
        EXPECT_EQ(g_temp_main_call_count, 1);
        EXPECT_EQ(g_temp_main_last_iters, 1'000'000);
    }

    TEST_F(MainLoopDriverTest, DriveIsrWithMainPump_AdvancesClockAndPumpsLoop) {
        // Arrange
        grader::run_student_init();
        g_temp_main_call_count = 0;
        int isr_calls = 0;
        auto isr = [&] { ++isr_calls; };
        // Act
        grader::drive_isr_with_main_pump(isr, /*period_us=*/1000, /*total_ticks=*/4);
        // Assert
        EXPECT_EQ(isr_calls, 4);
        EXPECT_EQ(g_temp_main_call_count, 4);
        EXPECT_EQ(grader::synthetic_clock_now_us(), 4000u);
    }

    TEST_F(MainLoopDriverTest, DriveIsrWithMainPump_ZeroTicks_NoCalls) {
        // Arrange
        grader::run_student_init();
        g_temp_main_call_count = 0;
        int isr_calls = 0;
        // Act
        grader::drive_isr_with_main_pump([&] { ++isr_calls; }, 1000, 0);
        // Assert
        EXPECT_EQ(isr_calls, 0);
        EXPECT_EQ(g_temp_main_call_count, 0);
    }

    TEST_F(MainLoopDriverTest, DriveIsrWithMainPump_LargeTickCount_NoClockOverflow) {
        // Arrange: BVA — drive 100_000 ticks at 1_000 us each => 1e8 us.
        // Well within uint64_t range; verifies clock arithmetic does not overflow.
        grader::run_student_init();
        g_temp_main_call_count = 0;
        int isr_calls = 0;
        constexpr std::uint64_t total_ticks = 100'000;
        constexpr std::uint64_t period_us = 1000;
        // Act
        grader::drive_isr_with_main_pump([&] { ++isr_calls; }, period_us, total_ticks);
        // Assert
        EXPECT_EQ(static_cast<std::uint64_t>(isr_calls), total_ticks);
        EXPECT_EQ(grader::synthetic_clock_now_us(), total_ticks * period_us);
    }

    TEST_F(MainLoopDriverTest, SetMainLoopIterations_PropagatesToQuery) {
        // Arrange
        // Act
        grader::set_main_loop_iterations(42);
        // Assert
        EXPECT_EQ(grader_main_loop_iterations(), 42);
        grader::set_main_loop_iterations(0);
    }
} // namespace