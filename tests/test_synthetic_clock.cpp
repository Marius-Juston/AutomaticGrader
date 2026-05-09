// GTest coverage for include/checks/synthetic_clock.h.

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <random>

#include "checks/synthetic_clock.h"

using namespace grader;

namespace {
    class SyntheticClockTest : public ::testing::Test {
    protected:
        void SetUp() override { synthetic_clock_reset(); }
        void TearDown() override { synthetic_clock_reset(); }
    };

    TEST_F(SyntheticClockTest, FreshReset_IsZero) {
        // Arrange
        // (SetUp resets clock)
        // Act
        const auto now = synthetic_clock_now_us();
        // Assert
        EXPECT_EQ(now, 0u);
    }

    TEST_F(SyntheticClockTest, AdvanceByZero_NoChange) {
        // Arrange
        // Act
        synthetic_clock_advance(0);
        // Assert
        EXPECT_EQ(synthetic_clock_now_us(), 0u);
    }

    TEST_F(SyntheticClockTest, AdvanceAccumulates) {
        // Arrange
        synthetic_clock_advance(100);
        // Act
        synthetic_clock_advance(50);
        // Assert
        EXPECT_EQ(synthetic_clock_now_us(), 150u);
    }

    TEST_F(SyntheticClockTest, ResetClearsAccumulated) {
        // Arrange
        synthetic_clock_advance(1234);
        // Act
        synthetic_clock_reset();
        // Assert
        EXPECT_EQ(synthetic_clock_now_us(), 0u);
    }

    TEST_F(SyntheticClockTest, AdvanceLargeValue_NoOverflow) {
        // Arrange: large but well within uint64_t range.
        const uint64_t big = std::numeric_limits<uint64_t>::max() / 4;
        // Act
        synthetic_clock_advance(big);
        // Assert
        EXPECT_EQ(synthetic_clock_now_us(), big);
    }

    TEST_F(SyntheticClockTest, RunIsrForUs_FiresExpectedCount) {
        // Arrange
        int count = 0;
        auto isr = [&] { ++count; };
        // Act
        run_isr_for_us(isr, /*period_us=*/1000, /*total_us=*/5000);
        // Assert
        EXPECT_EQ(count, 5);
        EXPECT_EQ(synthetic_clock_now_us(), 5000u);
    }

    TEST_F(SyntheticClockTest, RunIsrForUs_TotalNotMultipleOfPeriod_FloorsCount) {
        // Arrange: total_us=5500, period_us=1000 -> 5 calls (floor).
        int count = 0;
        auto isr = [&] { ++count; };
        // Act
        run_isr_for_us(isr, 1000, 5500);
        // Assert
        EXPECT_EQ(count, 5);
        EXPECT_EQ(synthetic_clock_now_us(), 5000u);
    }

    TEST_F(SyntheticClockTest, RunIsrForUs_PeriodZero_NoCallsNoCrash) {
        // Arrange: zero period must short-circuit (avoid div-by-zero).
        int count = 0;
        auto isr = [&] { ++count; };
        // Act
        run_isr_for_us(isr, /*period_us=*/0, /*total_us=*/5000);
        // Assert
        EXPECT_EQ(count, 0);
        EXPECT_EQ(synthetic_clock_now_us(), 0u);
    }

    TEST_F(SyntheticClockTest, RunIsrForUs_TotalLessThanPeriod_NoCalls) {
        // Arrange
        int count = 0;
        auto isr = [&] { ++count; };
        // Act
        run_isr_for_us(isr, 1000, 500);
        // Assert
        EXPECT_EQ(count, 0);
        EXPECT_EQ(synthetic_clock_now_us(), 0u);
    }

    TEST_F(SyntheticClockTest, RunIsrCalls_FiresExactlyNTimes) {
        // Arrange
        int count = 0;
        auto isr = [&] { ++count; };
        // Act
        run_isr_calls(isr, 250, 7);
        // Assert
        EXPECT_EQ(count, 7);
        EXPECT_EQ(synthetic_clock_now_us(), 250u * 7);
    }

    TEST_F(SyntheticClockTest, RunIsrCalls_ZeroIterations_NoCalls) {
        // Arrange
        int count = 0;
        auto isr = [&] { ++count; };
        // Act
        run_isr_calls(isr, 1000, 0);
        // Assert
        EXPECT_EQ(count, 0);
    }

    // ---------- property: floor-semantics invariant ----------

    TEST_F(SyntheticClockTest, RunIsrForUs_RandomFuzz_FloorSemanticsInvariant) {
        // Arrange: deterministic seed; for each (period, total), the ISR must
        // fire exactly floor(total/period) times and the clock must advance by
        // exactly that count * period.
        std::mt19937 rng(0xC0FFEE);
        // Act + Assert per-iteration (single logical property check).
        for (int trial = 0; trial < 500; ++trial) {
            synthetic_clock_reset();
            const uint64_t period_us = 1u + (rng() % 10000u); // [1, 10000]
            const uint64_t total_us = rng() % 1'000'001u; // [0, 1e6]
            int count = 0;
            auto isr = [&] { ++count; };
            run_isr_for_us(isr, period_us, total_us);
            const uint64_t expected_calls = total_us / period_us;
            const uint64_t expected_now = expected_calls * period_us;
            ASSERT_EQ(static_cast<uint64_t>(count), expected_calls)
                    << "trial=" << trial << " period=" << period_us << " total=" << total_us;
            ASSERT_EQ(synthetic_clock_now_us(), expected_now)
                    << "trial=" << trial << " period=" << period_us << " total=" << total_us;
        }
    }
} // namespace