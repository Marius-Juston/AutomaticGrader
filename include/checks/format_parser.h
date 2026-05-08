#ifndef AUTOMATICGRADER_CHECKS_FORMAT_PARSER_H
#define AUTOMATICGRADER_CHECKS_FORMAT_PARSER_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace grader {
    enum class LengthModifier : uint8_t {
        None,
        hh,
        h,
        l,
        ll,
        j,
        z,
        t,
        L,
    };

    enum class Conversion : uint8_t {
        Invalid,
        d,
        i,
        o,
        u,
        x,
        X,
        e,
        E,
        f,
        F,
        g,
        G,
        a,
        A,
        c,
        s,
        p,
        n,
        Percent,
    };

    namespace flag_bits {
        constexpr uint8_t Minus = 0x01;
        constexpr uint8_t Plus = 0x02;
        constexpr uint8_t Space = 0x04;
        constexpr uint8_t Hash = 0x08;
        constexpr uint8_t Zero = 0x10;
        constexpr uint8_t Apostrophe = 0x20;
    }

    enum class ArgType : uint8_t {
        Unknown,
        None,
        Int16,
        Int32,
        Int64,
        UInt16,
        UInt32,
        UInt64,
        Float,
        Double,
        Char,
        String,
        Pointer,
        SizeT,
        PtrDiffT,
    };

    constexpr int kFormatWidthUnset = -1;
    constexpr int kFormatWidthStar = -2;

    struct FormatSpec {
        std::string raw;
        uint8_t flags = 0;
        int width = kFormatWidthUnset;
        int precision = kFormatWidthUnset;
        LengthModifier length = LengthModifier::None;
        Conversion conversion = Conversion::Invalid;
        std::size_t fmt_offset = 0;
        std::size_t arg_index = 0;
    };

    struct LiteralSegment {
        std::string text;
        std::size_t fmt_offset = 0;
    };

    struct FormatSegment {
        enum class Kind : uint8_t { Literal, Spec };

        Kind kind = Kind::Literal;
        LiteralSegment literal{};
        FormatSpec spec{};
    };

    struct ParsedFormat {
        std::vector<FormatSegment> segments;
        std::vector<FormatSpec> specs;
        std::string error;

        [[nodiscard]] bool ok() const noexcept { return error.empty(); }
    };

    [[nodiscard]] ParsedFormat parse_format(std::string_view fmt);

    [[nodiscard]] ArgType expected_arg_type(const FormatSpec &spec) noexcept;

    [[nodiscard]] std::string_view length_modifier_name(LengthModifier lm) noexcept;

    [[nodiscard]] std::string_view conversion_name(Conversion c) noexcept;

    [[nodiscard]] std::string_view arg_type_name(ArgType t) noexcept;

    [[nodiscard]] std::string format_spec_canonical(const FormatSpec &spec);

    [[nodiscard]] bool format_specs_equivalent(const FormatSpec &a, const FormatSpec &b) noexcept;
}

#endif