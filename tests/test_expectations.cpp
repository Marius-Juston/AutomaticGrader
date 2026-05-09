// GTest coverage for include/checks/expectations.h.

#include <gtest/gtest.h>

#include "checks/expectations.h"
#include "checks/printf_capture.h"
#include "checks/synthetic_clock.h"
#include "F28379dSerial.h"

using namespace grader;

namespace {
    class ExpectationsTest : public ::testing::Test {
    protected:
        void SetUp() override { resetPrintfCapture(); }
        void TearDown() override { resetPrintfCapture(); }
    };

    // ---------- expect_format ----------

    TEST_F(ExpectationsTest, ExpectFormat_ExactMatch_ReturnsTrue) {
        // Arrange
        serial_printf(&SerialA, const_cast<char *>("v=%ld, f=%.2f\r\n"), 1L, 2.0);
        auto *call = latestPrintfCall(SerialPort::SCIA);
        ASSERT_NE(call, nullptr);
        // Act
        const bool ok = expect_format(call, "v=%ld, f=%.2f\r\n", "test_match");
        // Assert
        EXPECT_TRUE(ok);
    }

    TEST_F(ExpectationsTest, ExpectFormat_LengthModifierMismatch_ReturnsFalse) {
        // Arrange
        serial_printf(&SerialA, const_cast<char *>("v=%d\r\n"), 1);
        auto *call = latestPrintfCall(SerialPort::SCIA);
        ASSERT_NE(call, nullptr);
        // Act
        const bool ok = expect_format(call, "v=%ld\r\n", "test_mismatch");
        // Assert
        EXPECT_FALSE(ok);
    }

    TEST_F(ExpectationsTest, ExpectFormat_NullCall_ReturnsFalse) {
        // Arrange
        // (no call recorded)
        // Act
        const bool ok = expect_format(static_cast<const PrintfCall *>(nullptr),
                                      "anything %ld\r\n", "null_call");
        // Assert
        EXPECT_FALSE(ok);
    }

    TEST_F(ExpectationsTest, ExpectFormat_RefOverload_Works) {
        // Arrange
        serial_printf(&SerialA, const_cast<char *>("hello %ld\r\n"), 5L);
        auto *call = latestPrintfCall(SerialPort::SCIA);
        ASSERT_NE(call, nullptr);
        // Act
        const bool ok = expect_format(*call, "hello %ld\r\n", "ref_match");
        // Assert
        EXPECT_TRUE(ok);
    }

    // ---------- expect_arg_types ----------

    TEST_F(ExpectationsTest, ExpectArgTypes_Match_ReturnsTrue) {
        // Arrange
        serial_printf(&SerialA, const_cast<char *>("%d %.2f\r\n"), 1, 2.0);
        auto *call = latestPrintfCall(SerialPort::SCIA);
        ASSERT_NE(call, nullptr);
        // Act
        const bool ok = expect_arg_types(call, {ArgType::Int16, ArgType::Float}, "argtypes_ok");
        // Assert
        EXPECT_TRUE(ok);
    }

    TEST_F(ExpectationsTest, ExpectArgTypes_Mismatch_ReturnsFalse) {
        // Arrange
        serial_printf(&SerialA, const_cast<char *>("%d %.2f\r\n"), 1, 2.0);
        auto *call = latestPrintfCall(SerialPort::SCIA);
        ASSERT_NE(call, nullptr);
        // Act
        const bool ok = expect_arg_types(call, {ArgType::Int32, ArgType::Float}, "argtypes_bad");
        // Assert
        EXPECT_FALSE(ok);
    }

    // ---------- expect_print_cadence ----------

    TEST_F(ExpectationsTest, ExpectPrintCadence_ExactCount_ReturnsTrue) {
        // Arrange
        auto isr = [] {
            serial_printf(&SerialA, const_cast<char *>("tick %ld\r\n"), 1L);
        };
        run_isr_for_us(isr, /*period_us=*/5000, /*total_us=*/1'000'000);
        // Act
        const bool ok = expect_print_cadence(SerialPort::SCIA, 200, 0.0, "exact");
        // Assert
        EXPECT_EQ(printfCallCount(SerialPort::SCIA), 200u);
        EXPECT_TRUE(ok);
    }

    TEST_F(ExpectationsTest, ExpectPrintCadence_WithinTolerance_ReturnsTrue) {
        // Arrange
        auto isr = [] {
            serial_printf(&SerialA, const_cast<char *>("tick %ld\r\n"), 1L);
        };
        run_isr_for_us(isr, 5000, 1'000'000);
        // Act
        const bool ok = expect_print_cadence(SerialPort::SCIA, 180, 0.20, "within_tolerance");
        // Assert
        EXPECT_TRUE(ok);
    }

    TEST_F(ExpectationsTest, ExpectPrintCadence_TooFew_ReturnsFalse) {
        // Arrange
        auto isr = [] {
            serial_printf(&SerialA, const_cast<char *>("tick %ld\r\n"), 1L);
        };
        run_isr_for_us(isr, 5000, 1'000'000);
        // Act
        const bool ok = expect_print_cadence(SerialPort::SCIA, 50, 0.10, "too_few");
        // Assert
        EXPECT_FALSE(ok);
    }

    TEST_F(ExpectationsTest, ExpectPrintCadence_TooMany_ReturnsFalse) {
        // Arrange
        auto isr = [] {
            serial_printf(&SerialA, const_cast<char *>("tick %ld\r\n"), 1L);
        };
        run_isr_for_us(isr, 5000, 1'000'000);
        // Act
        const bool ok = expect_print_cadence(SerialPort::SCIA, 500, 0.10, "too_many");
        // Assert
        EXPECT_FALSE(ok);
    }

    // ---------- expect_print_cadence_window ----------

    TEST_F(ExpectationsTest, ExpectPrintCadenceWindow_InRange_ReturnsTrue) {
        // Arrange
        auto isr = [] {
            serial_printf(&SerialA, const_cast<char *>("tick %ld\r\n"), 1L);
        };
        run_isr_for_us(isr, 5000, 1'000'000);
        // Act
        const bool ok = expect_print_cadence_window(SerialPort::SCIA, 150, 250, "win_ok");
        // Assert
        EXPECT_TRUE(ok);
    }

    TEST_F(ExpectationsTest, ExpectPrintCadenceWindow_BelowMin_ReturnsFalse) {
        // Arrange
        auto isr = [] {
            serial_printf(&SerialA, const_cast<char *>("tick %ld\r\n"), 1L);
        };
        run_isr_for_us(isr, 5000, 1'000'000);
        // Act
        const bool ok = expect_print_cadence_window(SerialPort::SCIA, 0, 100, "win_low");
        // Assert
        EXPECT_FALSE(ok);
    }

    TEST_F(ExpectationsTest, ExpectPrintCadenceWindow_NoCalls_ReturnsFalseDefensively) {
        // Arrange: no prints. Mirrors the nullptr defensive case for expect_format.
        // Act
        const bool ok = expect_print_cadence_window(SerialPort::SCIA, 1, 5, "win_none");
        // Assert
        EXPECT_FALSE(ok);
    }

    // ---------- expect_min_print_calls ----------

    TEST_F(ExpectationsTest, ExpectMinPrintCalls_AboveMin_ReturnsTrue) {
        // Arrange
        serial_printf(&SerialA, const_cast<char *>("a\r\n"));
        serial_printf(&SerialA, const_cast<char *>("b\r\n"));
        // Act
        const bool ok = expect_min_print_calls(SerialPort::SCIA, 2, "min_ok");
        // Assert
        EXPECT_TRUE(ok);
    }

    TEST_F(ExpectationsTest, ExpectMinPrintCalls_BelowMin_ReturnsFalse) {
        // Arrange
        serial_printf(&SerialA, const_cast<char *>("only\r\n"));
        // Act
        const bool ok = expect_min_print_calls(SerialPort::SCIA, 5, "min_bad");
        // Assert
        EXPECT_FALSE(ok);
    }

    // ---------- expect_format_any ----------

    TEST_F(ExpectationsTest, ExpectFormatAny_OneOfManyMatches_ReturnsTrue) {
        // Arrange
        serial_printf(&SerialA, const_cast<char *>("noise\r\n"));
        serial_printf(&SerialA, const_cast<char *>("v=%ld, f=%.2f\r\n"), 1L, 2.0);
        // Act
        const bool ok = expect_format_any(SerialPort::SCIA, "v=%ld, f=%.2f\r\n", "any_match");
        // Assert
        EXPECT_TRUE(ok);
    }

    TEST_F(ExpectationsTest, ExpectFormatAny_NoneMatch_ReturnsFalse) {
        // Arrange
        serial_printf(&SerialA, const_cast<char *>("hello %ld\r\n"), 1L);
        // Act
        const bool ok = expect_format_any(SerialPort::SCIA, "totally %ld different %.2f\r\n",
                                          "any_miss");
        // Assert
        EXPECT_FALSE(ok);
    }

    TEST_F(ExpectationsTest, ExpectFormatAny_OneMatchAmongFive_ReturnsTrue) {
        // Arrange: five calls, exactly one matches the searched format.
        serial_printf(&SerialA, const_cast<char *>("noise1\r\n"));
        serial_printf(&SerialA, const_cast<char *>("noise2 %ld\r\n"), 1L);
        serial_printf(&SerialA, const_cast<char *>("target=%ld %.3f\r\n"), 9L, 1.5);
        serial_printf(&SerialA, const_cast<char *>("noise3\r\n"));
        serial_printf(&SerialA, const_cast<char *>("noise4 %.2f\r\n"), 2.0);
        // Act
        const bool ok = expect_format_any(SerialPort::SCIA, "target=%ld %.3f\r\n", "any_one_of_five");
        // Assert
        EXPECT_TRUE(ok);
    }

    TEST_F(ExpectationsTest, ExpectFormatAny_FiveCallsZeroMatches_ReturnsFalse) {
        // Arrange: five calls, none match.
        for (int i = 0; i < 5; ++i) {
            serial_printf(&SerialA, const_cast<char *>("noise %ld\r\n"), static_cast<long>(i));
        }
        // Act
        const bool ok = expect_format_any(SerialPort::SCIA, "completely_different=%.4f\r\n", "any_zero");
        // Assert
        EXPECT_FALSE(ok);
    }

    // ---------- cross-port isolation ----------

    TEST_F(ExpectationsTest, ExpectPrintCadence_CrossPortInterleaved_FiltersByPort) {
        // Arrange: alternate SCIA and SCIB calls (3 SCIA, 2 SCIB).
        serial_printf(&SerialA, const_cast<char *>("a1\r\n"));
        serial_printf(reinterpret_cast<const serialSCIA_t *>(&SerialB), const_cast<char *>("b1\r\n"));
        serial_printf(&SerialA, const_cast<char *>("a2\r\n"));
        serial_printf(reinterpret_cast<const serialSCIA_t *>(&SerialB), const_cast<char *>("b2\r\n"));
        serial_printf(&SerialA, const_cast<char *>("a3\r\n"));
        // Act
        const bool scia_ok = expect_print_cadence(SerialPort::SCIA, 3, 0.0, "scia_3");
        const bool scib_ok = expect_print_cadence(SerialPort::SCIB, 2, 0.0, "scib_2");
        // Assert: per-port filtering must not contaminate counts.
        EXPECT_TRUE(scia_ok);
        EXPECT_TRUE(scib_ok);
        EXPECT_EQ(printfCallCount(SerialPort::SCIA), 3u);
        EXPECT_EQ(printfCallCount(SerialPort::SCIB), 2u);
    }
} // namespace