#ifndef AUTOMATICGRADER_CHECKS_PRINTF_CAPTURE_H
#define AUTOMATICGRADER_CHECKS_PRINTF_CAPTURE_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "checks/format_parser.h"

namespace grader {
    enum class SerialPort : uint8_t {
        Unknown,
        SCIA,
        SCIB,
        SCIC,
        SCID,
        UART_LCD,
    };

    [[nodiscard]] std::string_view serial_port_name(SerialPort p) noexcept;

    struct PrintfCall {
        SerialPort port = SerialPort::Unknown;
        std::string fmt;
        ParsedFormat parsed;
        std::string rendered;
        uint64_t synthetic_timestamp_us = 0;
        std::size_t sequence_index = 0;
        uint16_t lcd_line = 0;

        [[nodiscard]] const std::vector<FormatSpec> &specs() const noexcept { return parsed.specs; }
    };

    void resetPrintfCapture() noexcept;

    void clearPrintfCalls() noexcept;

    [[nodiscard]] const std::vector<PrintfCall> &getPrintfCalls() noexcept;

    [[nodiscard]] std::vector<const PrintfCall *> getPrintfCallsForPort(SerialPort port);

    [[nodiscard]] const PrintfCall *latestPrintfCall(SerialPort port) noexcept;

    [[nodiscard]] std::size_t printfCallCount(SerialPort port) noexcept;

    void recordPrintfCall(SerialPort port,
                          std::string_view fmt,
                          std::string_view rendered,
                          uint16_t lcd_line = 0);

    void dumpPrintfCalls();
}

extern std::vector<grader::PrintfCall> g_printfCalls;

#endif