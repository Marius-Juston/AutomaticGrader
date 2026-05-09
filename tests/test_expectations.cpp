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
    serial_printf(&SerialA, const_cast<char*>("v=%ld, f=%.2f\r\n"), 1L, 2.0);
    auto* call = latestPrintfCall(SerialPort::SCIA);
    ASSERT_NE(call, nullptr);
    EXPECT_TRUE(expect_format(call, "v=%ld, f=%.2f\r\n", "test_match"));
}

TEST_F(ExpectationsTest, ExpectFormat_LengthModifierMismatch_ReturnsFalse) {
    serial_printf(&SerialA, const_cast<char*>("v=%d\r\n"), 1);
    auto* call = latestPrintfCall(SerialPort::SCIA);
    ASSERT_NE(call, nullptr);
    EXPECT_FALSE(expect_format(call, "v=%ld\r\n", "test_mismatch"));
}

TEST_F(ExpectationsTest, ExpectFormat_NullCall_ReturnsFalse) {
    EXPECT_FALSE(expect_format(static_cast<const PrintfCall*>(nullptr),
                               "anything %ld\r\n", "null_call"));
}

TEST_F(ExpectationsTest, ExpectFormat_RefOverload_Works) {
    serial_printf(&SerialA, const_cast<char*>("hello %ld\r\n"), 5L);
    auto* call = latestPrintfCall(SerialPort::SCIA);
    ASSERT_NE(call, nullptr);
    EXPECT_TRUE(expect_format(*call, "hello %ld\r\n", "ref_match"));
}

// ---------- expect_arg_types ----------

TEST_F(ExpectationsTest, ExpectArgTypes_Match_ReturnsTrue) {
    serial_printf(&SerialA, const_cast<char*>("%d %.2f\r\n"), 1, 2.0);
    auto* call = latestPrintfCall(SerialPort::SCIA);
    ASSERT_NE(call, nullptr);
    EXPECT_TRUE(expect_arg_types(call, {ArgType::Int16, ArgType::Float}, "argtypes_ok"));
}

TEST_F(ExpectationsTest, ExpectArgTypes_Mismatch_ReturnsFalse) {
    serial_printf(&SerialA, const_cast<char*>("%d %.2f\r\n"), 1, 2.0);
    auto* call = latestPrintfCall(SerialPort::SCIA);
    ASSERT_NE(call, nullptr);
    EXPECT_FALSE(expect_arg_types(call, {ArgType::Int32, ArgType::Float}, "argtypes_bad"));
}

// ---------- expect_print_cadence ----------

TEST_F(ExpectationsTest, ExpectPrintCadence_ExactCount_ReturnsTrue) {
    auto isr = [] {
        serial_printf(&SerialA, const_cast<char*>("tick %ld\r\n"), 1L);
    };
    run_isr_for_us(isr, /*period_us=*/5000, /*total_us=*/1'000'000);
    EXPECT_EQ(printfCallCount(SerialPort::SCIA), 200u);
    EXPECT_TRUE(expect_print_cadence(SerialPort::SCIA, 200, 0.0, "exact"));
}

TEST_F(ExpectationsTest, ExpectPrintCadence_WithinTolerance_ReturnsTrue) {
    auto isr = [] {
        serial_printf(&SerialA, const_cast<char*>("tick %ld\r\n"), 1L);
    };
    run_isr_for_us(isr, 5000, 1'000'000);
    EXPECT_TRUE(expect_print_cadence(SerialPort::SCIA, 180, 0.20, "within_tolerance"));
}

TEST_F(ExpectationsTest, ExpectPrintCadence_TooFew_ReturnsFalse) {
    auto isr = [] {
        serial_printf(&SerialA, const_cast<char*>("tick %ld\r\n"), 1L);
    };
    run_isr_for_us(isr, 5000, 1'000'000);
    EXPECT_FALSE(expect_print_cadence(SerialPort::SCIA, 50, 0.10, "too_few"));
}

TEST_F(ExpectationsTest, ExpectPrintCadence_TooMany_ReturnsFalse) {
    auto isr = [] {
        serial_printf(&SerialA, const_cast<char*>("tick %ld\r\n"), 1L);
    };
    run_isr_for_us(isr, 5000, 1'000'000);
    EXPECT_FALSE(expect_print_cadence(SerialPort::SCIA, 500, 0.10, "too_many"));
}

// ---------- expect_print_cadence_window ----------

TEST_F(ExpectationsTest, ExpectPrintCadenceWindow_InRange_ReturnsTrue) {
    auto isr = [] {
        serial_printf(&SerialA, const_cast<char*>("tick %ld\r\n"), 1L);
    };
    run_isr_for_us(isr, 5000, 1'000'000);
    EXPECT_TRUE(expect_print_cadence_window(SerialPort::SCIA, 150, 250, "win_ok"));
}

TEST_F(ExpectationsTest, ExpectPrintCadenceWindow_BelowMin_ReturnsFalse) {
    auto isr = [] {
        serial_printf(&SerialA, const_cast<char*>("tick %ld\r\n"), 1L);
    };
    run_isr_for_us(isr, 5000, 1'000'000);
    EXPECT_FALSE(expect_print_cadence_window(SerialPort::SCIA, 0, 100, "win_low"));
}

// ---------- expect_min_print_calls ----------

TEST_F(ExpectationsTest, ExpectMinPrintCalls_AboveMin_ReturnsTrue) {
    serial_printf(&SerialA, const_cast<char*>("a\r\n"));
    serial_printf(&SerialA, const_cast<char*>("b\r\n"));
    EXPECT_TRUE(expect_min_print_calls(SerialPort::SCIA, 2, "min_ok"));
}

TEST_F(ExpectationsTest, ExpectMinPrintCalls_BelowMin_ReturnsFalse) {
    serial_printf(&SerialA, const_cast<char*>("only\r\n"));
    EXPECT_FALSE(expect_min_print_calls(SerialPort::SCIA, 5, "min_bad"));
}

// ---------- expect_format_any ----------

TEST_F(ExpectationsTest, ExpectFormatAny_OneOfManyMatches_ReturnsTrue) {
    serial_printf(&SerialA, const_cast<char*>("noise\r\n"));
    serial_printf(&SerialA, const_cast<char*>("v=%ld, f=%.2f\r\n"), 1L, 2.0);
    EXPECT_TRUE(expect_format_any(SerialPort::SCIA, "v=%ld, f=%.2f\r\n", "any_match"));
}

TEST_F(ExpectationsTest, ExpectFormatAny_NoneMatch_ReturnsFalse) {
    serial_printf(&SerialA, const_cast<char*>("hello %ld\r\n"), 1L);
    EXPECT_FALSE(expect_format_any(SerialPort::SCIA, "totally %ld different %.2f\r\n",
                                   "any_miss"));
}

}  // namespace
