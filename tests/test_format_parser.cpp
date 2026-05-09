// GTest coverage for include/checks/format_parser.h. Mirrors selftest
// coverage and extends it with boundary cases and a hand-rolled fuzz loop.

#include <gtest/gtest.h>

#include <random>
#include <string>

#include "checks/format_parser.h"

using namespace grader;

namespace {

// ---------- basic parse ----------

TEST(FormatParser, EmptyString_ParsesOkWithNoSpecs) {
    auto p = parse_format("");
    EXPECT_TRUE(p.ok());
    EXPECT_TRUE(p.specs.empty());
}

TEST(FormatParser, LiteralOnly_ParsesOkWithNoSpecs) {
    auto p = parse_format("hello world\r\n");
    EXPECT_TRUE(p.ok());
    EXPECT_TRUE(p.specs.empty());
}

TEST(FormatParser, LiteralPercent_ParsesAsZeroSpecs) {
    auto p = parse_format("100%% utilization");
    EXPECT_TRUE(p.ok());
    EXPECT_TRUE(p.specs.empty());
}

// ---------- conversions ----------

TEST(FormatParser, BasicIntConversion) {
    auto p = parse_format("%d");
    ASSERT_TRUE(p.ok());
    ASSERT_EQ(p.specs.size(), 1u);
    EXPECT_EQ(p.specs[0].conversion, Conversion::d);
    EXPECT_EQ(p.specs[0].length, LengthModifier::None);
}

TEST(FormatParser, LongIntConversion_ProducesInt32ArgType) {
    auto p = parse_format("%ld");
    ASSERT_TRUE(p.ok());
    ASSERT_EQ(p.specs.size(), 1u);
    EXPECT_EQ(p.specs[0].length, LengthModifier::l);
    EXPECT_EQ(expected_arg_type(p.specs[0]), ArgType::Int32);
}

TEST(FormatParser, PlainPercentD_ProducesInt16ArgType_TI_C2000_Convention) {
    // Critical for SE 423: TI C2000 has 16-bit `int`, so `%d` => Int16.
    auto p = parse_format("%d");
    ASSERT_TRUE(p.ok());
    EXPECT_EQ(expected_arg_type(p.specs[0]), ArgType::Int16);
}

TEST(FormatParser, FloatPrecision_Captured) {
    auto p = parse_format("%.3f");
    ASSERT_TRUE(p.ok());
    ASSERT_EQ(p.specs.size(), 1u);
    EXPECT_EQ(p.specs[0].conversion, Conversion::f);
    EXPECT_EQ(p.specs[0].precision, 3);
}

TEST(FormatParser, HexUpper_LongModifier_UInt32) {
    auto p = parse_format("0x%08lX");
    ASSERT_TRUE(p.ok());
    ASSERT_EQ(p.specs.size(), 1u);
    EXPECT_EQ(p.specs[0].conversion, Conversion::X);
    EXPECT_EQ(p.specs[0].length, LengthModifier::l);
    EXPECT_EQ(expected_arg_type(p.specs[0]), ArgType::UInt32);
}

TEST(FormatParser, StringSpec_ProducesStringArgType) {
    auto p = parse_format("%s");
    ASSERT_TRUE(p.ok());
    EXPECT_EQ(expected_arg_type(p.specs[0]), ArgType::String);
}

// ---------- flags / width ----------

TEST(FormatParser, MinusFlag_Recognized) {
    auto p = parse_format("%-10.3lf");
    ASSERT_TRUE(p.ok());
    EXPECT_NE(p.specs[0].flags & flag_bits::Minus, 0);
    EXPECT_EQ(p.specs[0].width, 10);
}

TEST(FormatParser, ZeroFlag_Recognized) {
    auto p = parse_format("%05d");
    ASSERT_TRUE(p.ok());
    EXPECT_NE(p.specs[0].flags & flag_bits::Zero, 0);
    EXPECT_EQ(p.specs[0].width, 5);
}

// ---------- error paths ----------

TEST(FormatParser, TrailingBarePercent_IsError) {
    auto p = parse_format("hello %");
    EXPECT_FALSE(p.ok());
    EXPECT_FALSE(p.error.empty());
}

TEST(FormatParser, UnknownConversion_IsError) {
    auto p = parse_format("hi %q there");
    EXPECT_FALSE(p.ok());
}

// ---------- equivalence ----------

TEST(FormatSpecsEquivalent, ExactMatch_ReturnsTrue) {
    auto a = parse_format("%.2f");
    auto b = parse_format("%.2f");
    ASSERT_TRUE(a.ok() && b.ok());
    EXPECT_TRUE(format_specs_equivalent(a.specs[0], b.specs[0]));
}

TEST(FormatSpecsEquivalent, PrecisionDiffers_ReturnsFalse) {
    auto a = parse_format("%.2f");
    auto b = parse_format("%.3f");
    ASSERT_TRUE(a.ok() && b.ok());
    EXPECT_FALSE(format_specs_equivalent(a.specs[0], b.specs[0]));
}

TEST(FormatSpecsEquivalent, LengthModifierDiffers_ReturnsFalse) {
    // The whole point of this checker: %d vs %ld must NOT match.
    auto a = parse_format("%d");
    auto b = parse_format("%ld");
    ASSERT_TRUE(a.ok() && b.ok());
    EXPECT_FALSE(format_specs_equivalent(a.specs[0], b.specs[0]));
}

TEST(FormatSpecCanonical, ProducesStableForm) {
    auto p = parse_format("%-10.3lf");
    ASSERT_TRUE(p.ok());
    EXPECT_EQ(format_spec_canonical(p.specs[0]), "%-10.3lf");
}

// ---------- fuzz: parser must never crash on random input ----------

TEST(FormatParser, RandomBytes_ParserNeverCrashes) {
    std::mt19937 rng(0xC0FFEE);  // deterministic seed
    for (int trial = 0; trial < 500; ++trial) {
        std::string s;
        const int len = rng() % 64;
        for (int i = 0; i < len; ++i) {
            s.push_back(static_cast<char>(rng() % 256));
        }
        // Property: must terminate, must never throw, error string must be
        // either empty or non-empty (no UB).
        auto p = parse_format(s);
        (void)p.ok();
        for (const auto& spec : p.specs) {
            (void)expected_arg_type(spec);
            (void)format_spec_canonical(spec);
        }
    }
}

}  // namespace
