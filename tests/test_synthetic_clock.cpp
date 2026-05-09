// GTest coverage for include/checks/synthetic_clock.h.

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>

#include "checks/synthetic_clock.h"

using namespace grader;

namespace {

class SyntheticClockTest : public ::testing::Test {
protected:
    void SetUp() override { synthetic_clock_reset(); }
    void TearDown() override { synthetic_clock_reset(); }
};

TEST_F(SyntheticClockTest, FreshReset_IsZero) {
    EXPECT_EQ(synthetic_clock_now_us(), 0u);
}

TEST_F(SyntheticClockTest, AdvanceByZero_NoChange) {
    synthetic_clock_advance(0);
    EXPECT_EQ(synthetic_clock_now_us(), 0u);
}

TEST_F(SyntheticClockTest, AdvanceAccumulates) {
    synthetic_clock_advance(100);
    synthetic_clock_advance(50);
    EXPECT_EQ(synthetic_clock_now_us(), 150u);
}

TEST_F(SyntheticClockTest, ResetClearsAccumulated) {
    synthetic_clock_advance(1234);
    synthetic_clock_reset();
    EXPECT_EQ(synthetic_clock_now_us(), 0u);
}

TEST_F(SyntheticClockTest, AdvanceLargeValue_NoOverflow) {
    // Boundary: large but well within uint64_t range.
    const uint64_t big = std::numeric_limits<uint64_t>::max() / 4;
    synthetic_clock_advance(big);
    EXPECT_EQ(synthetic_clock_now_us(), big);
}

TEST_F(SyntheticClockTest, RunIsrForUs_FiresExpectedCount) {
    int count = 0;
    auto isr = [&] { ++count; };
    run_isr_for_us(isr, /*period_us=*/1000, /*total_us=*/5000);
    EXPECT_EQ(count, 5);
    EXPECT_EQ(synthetic_clock_now_us(), 5000u);
}

TEST_F(SyntheticClockTest, RunIsrForUs_TotalNotMultipleOfPeriod_FloorsCount) {
    // Boundary: total_us=5500, period_us=1000 -> 5 calls (floor).
    int count = 0;
    auto isr = [&] { ++count; };
    run_isr_for_us(isr, 1000, 5500);
    EXPECT_EQ(count, 5);
    EXPECT_EQ(synthetic_clock_now_us(), 5000u);
}

TEST_F(SyntheticClockTest, RunIsrForUs_PeriodZero_NoCallsNoCrash) {
    // Boundary: zero period must short-circuit (avoid div-by-zero).
    int count = 0;
    auto isr = [&] { ++count; };
    run_isr_for_us(isr, /*period_us=*/0, /*total_us=*/5000);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(synthetic_clock_now_us(), 0u);
}

TEST_F(SyntheticClockTest, RunIsrForUs_TotalLessThanPeriod_NoCalls) {
    int count = 0;
    auto isr = [&] { ++count; };
    run_isr_for_us(isr, 1000, 500);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(synthetic_clock_now_us(), 0u);
}

TEST_F(SyntheticClockTest, RunIsrCalls_FiresExactlyNTimes) {
    int count = 0;
    auto isr = [&] { ++count; };
    run_isr_calls(isr, 250, 7);
    EXPECT_EQ(count, 7);
    EXPECT_EQ(synthetic_clock_now_us(), 250u * 7);
}

TEST_F(SyntheticClockTest, RunIsrCalls_ZeroIterations_NoCalls) {
    int count = 0;
    auto isr = [&] { ++count; };
    run_isr_calls(isr, 1000, 0);
    EXPECT_EQ(count, 0);
}

}  // namespace
