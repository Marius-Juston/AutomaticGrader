// GTest coverage for include/checks/printf_capture.h. Exercises the capture
// pipeline through the real `serial_printf` stub (linked from grader_stubs).

#include <gtest/gtest.h>

#include "checks/printf_capture.h"
#include "checks/synthetic_clock.h"
#include "F28379dSerial.h"

using namespace grader;

namespace {
    class PrintfCaptureTest : public ::testing::Test {
    protected:
        void SetUp() override { resetPrintfCapture(); }
        void TearDown() override { resetPrintfCapture(); }
    };

    TEST_F(PrintfCaptureTest, FreshReset_LogIsEmpty) {
        EXPECT_TRUE(getPrintfCalls().empty());
        EXPECT_EQ(printfCallCount(SerialPort::SCIA), 0u);
        EXPECT_EQ(latestPrintfCall(SerialPort::SCIA), nullptr);
    }

    TEST_F(PrintfCaptureTest, SinglePrint_AppendsOneEntry) {
        serial_printf(&SerialA, const_cast<char *>("x = %ld\r\n"), 7L);
        ASSERT_EQ(g_printfCalls.size(), 1u);
        EXPECT_EQ(g_printfCalls[0].port, SerialPort::SCIA);
        EXPECT_EQ(g_printfCalls[0].parsed.specs.size(), 1u);
        EXPECT_NE(g_printfCalls[0].rendered.find("x = 7"), std::string::npos);
    }

    TEST_F(PrintfCaptureTest, MultiplePrints_AreOrderedByCall) {
        serial_printf(&SerialA, const_cast<char *>("a %ld\r\n"), 1L);
        synthetic_clock_advance(1000);
        serial_printf(&SerialA, const_cast<char *>("b %ld\r\n"), 2L);
        ASSERT_EQ(g_printfCalls.size(), 2u);
        EXPECT_LT(g_printfCalls[0].sequence_index, g_printfCalls[1].sequence_index);
        EXPECT_LE(g_printfCalls[0].synthetic_timestamp_us,
                  g_printfCalls[1].synthetic_timestamp_us);
    }

    TEST_F(PrintfCaptureTest, GetPrintfCallsForPort_FiltersCorrectly) {
        serial_printf(&SerialA, const_cast<char *>("hi %ld\r\n"), 1L);
        serial_printf(&SerialA, const_cast<char *>("yo %ld\r\n"), 2L);
        auto scia = getPrintfCallsForPort(SerialPort::SCIA);
        EXPECT_EQ(scia.size(), 2u);
        auto scib = getPrintfCallsForPort(SerialPort::SCIB);
        EXPECT_TRUE(scib.empty());
    }

    TEST_F(PrintfCaptureTest, LatestPrintfCall_ReturnsMostRecent) {
        serial_printf(&SerialA, const_cast<char *>("first %ld\r\n"), 1L);
        serial_printf(&SerialA, const_cast<char *>("second %ld\r\n"), 99L);
        const auto *latest = latestPrintfCall(SerialPort::SCIA);
        ASSERT_NE(latest, nullptr);
        EXPECT_NE(latest->rendered.find("99"), std::string::npos);
    }

    TEST_F(PrintfCaptureTest, LatestPrintfCall_PortWithNoCalls_ReturnsNull) {
        serial_printf(&SerialA, const_cast<char *>("scia %ld\r\n"), 1L);
        EXPECT_EQ(latestPrintfCall(SerialPort::SCIB), nullptr);
    }

    TEST_F(PrintfCaptureTest, SerialPortName_AllPortsHaveNonEmptyName) {
        EXPECT_FALSE(serial_port_name(SerialPort::SCIA).empty());
        EXPECT_FALSE(serial_port_name(SerialPort::SCIB).empty());
        EXPECT_FALSE(serial_port_name(SerialPort::SCIC).empty());
        EXPECT_FALSE(serial_port_name(SerialPort::SCID).empty());
        EXPECT_FALSE(serial_port_name(SerialPort::UART_LCD).empty());
    }

    TEST_F(PrintfCaptureTest, ResetPrintfCapture_AlsoResetsSyntheticClock) {
        synthetic_clock_advance(99999);
        resetPrintfCapture();
        EXPECT_EQ(synthetic_clock_now_us(), 0u);
        EXPECT_TRUE(getPrintfCalls().empty());
    }

    TEST_F(PrintfCaptureTest, RecordPrintfCall_DirectAPI_AppendsEntry) {
        recordPrintfCall(SerialPort::SCIB, "hello %d\n", "hello 5\n");
        ASSERT_EQ(g_printfCalls.size(), 1u);
        EXPECT_EQ(g_printfCalls[0].port, SerialPort::SCIB);
        EXPECT_EQ(g_printfCalls[0].rendered, "hello 5\n");
    }
} // namespace