// GTest coverage for include/checks/printf_capture.h. Exercises the capture
// pipeline through the real `serial_printf` stub (linked from grader_stubs).

#include <gtest/gtest.h>

#include "checks/format_parser.h"
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
        // Arrange
        // (SetUp resets capture)
        // Act
        const auto &calls = getPrintfCalls();
        // Assert
        EXPECT_TRUE(calls.empty());
        EXPECT_EQ(printfCallCount(SerialPort::SCIA), 0u);
        EXPECT_EQ(latestPrintfCall(SerialPort::SCIA), nullptr);
    }

    TEST_F(PrintfCaptureTest, SinglePrint_AppendsOneEntry) {
        // Arrange
        // (no prior state)
        // Act
        serial_printf(&SerialA, const_cast<char *>("x = %ld\r\n"), 7L);
        // Assert
        ASSERT_EQ(g_printfCalls.size(), 1u);
        EXPECT_EQ(g_printfCalls[0].port, SerialPort::SCIA);
        EXPECT_EQ(g_printfCalls[0].parsed.specs.size(), 1u);
        EXPECT_NE(g_printfCalls[0].rendered.find("x = 7"), std::string::npos);
    }

    TEST_F(PrintfCaptureTest, MultiplePrints_AreOrderedByCall) {
        // Arrange
        serial_printf(&SerialA, const_cast<char *>("a %ld\r\n"), 1L);
        synthetic_clock_advance(1000);
        // Act
        serial_printf(&SerialA, const_cast<char *>("b %ld\r\n"), 2L);
        // Assert
        ASSERT_EQ(g_printfCalls.size(), 2u);
        EXPECT_LT(g_printfCalls[0].sequence_index, g_printfCalls[1].sequence_index);
        EXPECT_LE(g_printfCalls[0].synthetic_timestamp_us,
                  g_printfCalls[1].synthetic_timestamp_us);
    }

    TEST_F(PrintfCaptureTest, GetPrintfCallsForPort_FiltersCorrectly) {
        // Arrange
        serial_printf(&SerialA, const_cast<char *>("hi %ld\r\n"), 1L);
        serial_printf(&SerialA, const_cast<char *>("yo %ld\r\n"), 2L);
        // Act
        auto scia = getPrintfCallsForPort(SerialPort::SCIA);
        auto scib = getPrintfCallsForPort(SerialPort::SCIB);
        // Assert
        EXPECT_EQ(scia.size(), 2u);
        EXPECT_TRUE(scib.empty());
    }

    TEST_F(PrintfCaptureTest, LatestPrintfCall_ReturnsMostRecent) {
        // Arrange
        serial_printf(&SerialA, const_cast<char *>("first %ld\r\n"), 1L);
        serial_printf(&SerialA, const_cast<char *>("second %ld\r\n"), 99L);
        // Act
        const auto *latest = latestPrintfCall(SerialPort::SCIA);
        // Assert
        ASSERT_NE(latest, nullptr);
        EXPECT_NE(latest->rendered.find("99"), std::string::npos);
    }

    TEST_F(PrintfCaptureTest, LatestPrintfCall_PortWithNoCalls_ReturnsNull) {
        // Arrange
        serial_printf(&SerialA, const_cast<char *>("scia %ld\r\n"), 1L);
        // Act
        const auto *p = latestPrintfCall(SerialPort::SCIB);
        // Assert
        EXPECT_EQ(p, nullptr);
    }

    TEST_F(PrintfCaptureTest, SerialPortName_AllPortsHaveNonEmptyName) {
        // Arrange
        // (none — pure name-lookup query)
        // Act / Assert
        EXPECT_FALSE(serial_port_name(SerialPort::SCIA).empty());
        EXPECT_FALSE(serial_port_name(SerialPort::SCIB).empty());
        EXPECT_FALSE(serial_port_name(SerialPort::SCIC).empty());
        EXPECT_FALSE(serial_port_name(SerialPort::SCID).empty());
        EXPECT_FALSE(serial_port_name(SerialPort::UART_LCD).empty());
    }

    TEST_F(PrintfCaptureTest, ResetPrintfCapture_AlsoResetsSyntheticClock) {
        // Arrange
        synthetic_clock_advance(99999);
        // Act
        resetPrintfCapture();
        // Assert
        EXPECT_EQ(synthetic_clock_now_us(), 0u);
        EXPECT_TRUE(getPrintfCalls().empty());
    }

    TEST_F(PrintfCaptureTest, RecordPrintfCall_DirectAPI_AppendsEntry) {
        // Arrange
        // (clean log)
        // Act
        recordPrintfCall(SerialPort::SCIB, "hello %d\n", "hello 5\n");
        // Assert: parsed specifier sequence is populated, not just the raw fmt.
        ASSERT_EQ(g_printfCalls.size(), 1u);
        EXPECT_EQ(g_printfCalls[0].port, SerialPort::SCIB);
        EXPECT_EQ(g_printfCalls[0].rendered, "hello 5\n");
        ASSERT_EQ(g_printfCalls[0].parsed.specs.size(), 1u);
        EXPECT_EQ(g_printfCalls[0].parsed.specs[0].conversion, Conversion::d);
        EXPECT_EQ(g_printfCalls[0].parsed.specs[0].length, LengthModifier::None);
    }

    // ---------- cross-port interleaving ----------

    TEST_F(PrintfCaptureTest, CrossPortInterleaving_SequenceIndexStrictlyMonotonic) {
        // Arrange: alternate SCIA / SCIB / SCIC prints — single global counter
        // means sequence_index must be strictly increasing across all ports.
        // (serial_printf's signature is `const serialSCIA_t *` but it
        // pointer-compares against &SerialB/&SerialC for port classification.)
        serial_printf(&SerialA, const_cast<char *>("a1\r\n"));
        serial_printf(reinterpret_cast<const serialSCIA_t *>(&SerialB), const_cast<char *>("b1\r\n"));
        serial_printf(reinterpret_cast<const serialSCIA_t *>(&SerialC), const_cast<char *>("c1\r\n"));
        serial_printf(&SerialA, const_cast<char *>("a2\r\n"));
        serial_printf(reinterpret_cast<const serialSCIA_t *>(&SerialB), const_cast<char *>("b2\r\n"));
        // Act
        const auto &calls = getPrintfCalls();
        // Assert
        ASSERT_EQ(calls.size(), 5u);
        for (size_t i = 1; i < calls.size(); ++i) {
            EXPECT_LT(calls[i - 1].sequence_index, calls[i].sequence_index)
                    << "non-monotonic sequence_index at i=" << i;
        }
    }

    // ---------- TI-C2000 16-bit-int rendering boundary ----------

    TEST_F(PrintfCaptureTest, PercentD_ParsesAsInt16ArgType_TI_C2000_Convention) {
        // Arrange: %d on host is 32-bit but on TI C2000 target is 16-bit.
        // The captured parsed-spec sequence must reflect Int16 (the canonical
        // bug class the suite is meant to catch).
        // Act
        serial_printf(&SerialA, const_cast<char *>("%d\r\n"), 0x10000);
        // Assert
        ASSERT_EQ(g_printfCalls.size(), 1u);
        ASSERT_EQ(g_printfCalls[0].parsed.specs.size(), 1u);
        EXPECT_EQ(expected_arg_type(g_printfCalls[0].parsed.specs[0]), ArgType::Int16);
        EXPECT_EQ(g_printfCalls[0].parsed.specs[0].length, LengthModifier::None);
    }
} // namespace