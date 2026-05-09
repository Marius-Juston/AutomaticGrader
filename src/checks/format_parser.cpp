#include "checks/format_parser.h"

#include <cctype>
#include <utility>

namespace grader {
    namespace {
        bool is_decimal_digit(const char c) noexcept {
            return c >= '0' && c <= '9';
        }

        bool consume_flag(const char c, uint8_t &flags) noexcept {
            switch (c) {
                case '-': flags |= flag_bits::Minus;
                    return true;
                case '+': flags |= flag_bits::Plus;
                    return true;
                case ' ': flags |= flag_bits::Space;
                    return true;
                case '#': flags |= flag_bits::Hash;
                    return true;
                case '0': flags |= flag_bits::Zero;
                    return true;
                case '\'': flags |= flag_bits::Apostrophe;
                    return true;
                default: return false;
            }
        }

        Conversion classify_conversion(const char c) noexcept {
            switch (c) {
                case 'd': return Conversion::d;
                case 'i': return Conversion::i;
                case 'o': return Conversion::o;
                case 'u': return Conversion::u;
                case 'x': return Conversion::x;
                case 'X': return Conversion::X;
                case 'e': return Conversion::e;
                case 'E': return Conversion::E;
                case 'f': return Conversion::f;
                case 'F': return Conversion::F;
                case 'g': return Conversion::g;
                case 'G': return Conversion::G;
                case 'a': return Conversion::a;
                case 'A': return Conversion::A;
                case 'c': return Conversion::c;
                case 's': return Conversion::s;
                case 'p': return Conversion::p;
                case 'n': return Conversion::n;
                case '%': return Conversion::Percent;
                default: return Conversion::Invalid;
            }
        }
    }

    ParsedFormat parse_format(std::string_view fmt) {
        ParsedFormat result;
        result.segments.reserve(8);
        result.specs.reserve(4);

        std::string literal;
        literal.reserve(fmt.size());
        std::size_t literal_start = 0;
        std::size_t arg_idx = 0;

        auto flush_literal = [&](const std::size_t until_offset) {
            if (literal.empty()) {
                literal_start = until_offset;
                return;
            }
            FormatSegment seg;
            seg.kind = FormatSegment::Kind::Literal;
            seg.literal.text = std::move(literal);
            seg.literal.fmt_offset = literal_start;
            result.segments.push_back(std::move(seg));
            literal.clear();
            literal_start = until_offset;
        };

        std::size_t i = 0;
        while (i < fmt.size()) {
            if (const char c = fmt[i]; c != '%') {
                literal.push_back(c);
                ++i;
                continue;
            }

            const std::size_t spec_start = i;
            flush_literal(spec_start);

            ++i;
            if (i >= fmt.size()) {
                result.error = "format string ends with stray '%' (no conversion specifier follows)";
                return result;
            }

            FormatSpec spec;
            spec.fmt_offset = spec_start;

            while (i < fmt.size() && consume_flag(fmt[i], spec.flags)) {
                ++i;
            }

            if (i < fmt.size() && fmt[i] == '*') {
                spec.width = kFormatWidthStar;
                ++i;
            } else if (i < fmt.size() && is_decimal_digit(fmt[i]) && fmt[i] != '0') {
                int w = 0;
                while (i < fmt.size() && is_decimal_digit(fmt[i])) {
                    w = w * 10 + (fmt[i] - '0');
                    ++i;
                }
                spec.width = w;
            }

            if (i < fmt.size() && fmt[i] == '.') {
                ++i;
                if (i < fmt.size() && fmt[i] == '*') {
                    spec.precision = kFormatWidthStar;
                    ++i;
                } else {
                    int p = 0;
                    bool any = false;
                    while (i < fmt.size() && is_decimal_digit(fmt[i])) {
                        p = p * 10 + (fmt[i] - '0');
                        ++i;
                        any = true;
                    }
                    spec.precision = any ? p : 0;
                }
            }

            if (i + 1 < fmt.size() && fmt[i] == 'h' && fmt[i + 1] == 'h') {
                spec.length = LengthModifier::hh;
                i += 2;
            } else if (i < fmt.size() && fmt[i] == 'h') {
                spec.length = LengthModifier::h;
                ++i;
            } else if (i + 1 < fmt.size() && fmt[i] == 'l' && fmt[i + 1] == 'l') {
                spec.length = LengthModifier::ll;
                i += 2;
            } else if (i < fmt.size() && fmt[i] == 'l') {
                spec.length = LengthModifier::l;
                ++i;
            } else if (i < fmt.size()) {
                switch (fmt[i]) {
                    case 'j': spec.length = LengthModifier::j;
                        ++i;
                        break;
                    case 'z': spec.length = LengthModifier::z;
                        ++i;
                        break;
                    case 't': spec.length = LengthModifier::t;
                        ++i;
                        break;
                    case 'L': spec.length = LengthModifier::L;
                        ++i;
                        break;
                    default: break;
                }
            }

            if (i >= fmt.size()) {
                result.error = "format spec '" + std::string(fmt.substr(spec_start)) +
                               "' ended before a conversion specifier";
                return result;
            }

            const char conv_char = fmt[i++];
            spec.conversion = classify_conversion(conv_char);

            if (spec.conversion == Conversion::Invalid) {
                result.error = std::string("unknown conversion specifier '") + conv_char +
                               "' at offset " + std::to_string(spec_start);
                return result;
            }

            spec.raw.assign(fmt.substr(spec_start, i - spec_start));
            spec.arg_index = (spec.conversion == Conversion::Percent) ? 0 : arg_idx++;

            FormatSegment seg;
            seg.kind = FormatSegment::Kind::Spec;
            seg.spec = spec;
            result.segments.push_back(seg);

            if (spec.conversion != Conversion::Percent) {
                result.specs.push_back(std::move(spec));
            }

            literal_start = i;
        }
        flush_literal(fmt.size());
        return result;
    }

    ArgType expected_arg_type(const FormatSpec &spec) noexcept {
        using L = LengthModifier;
        using C = Conversion;
        switch (spec.conversion) {
            case C::d:
            case C::i:
                switch (spec.length) {
                    case L::hh:
                    case L::h:
                    case L::None: return ArgType::Int16;
                    case L::l: return ArgType::Int32;
                    case L::ll:
                    case L::j: return ArgType::Int64;
                    case L::z: return ArgType::SizeT;
                    case L::t: return ArgType::PtrDiffT;
                    default: return ArgType::Unknown;
                }
            case C::o:
            case C::u:
            case C::x:
            case C::X:
                switch (spec.length) {
                    case L::hh:
                    case L::h:
                    case L::None: return ArgType::UInt16;
                    case L::l: return ArgType::UInt32;
                    case L::ll:
                    case L::j: return ArgType::UInt64;
                    case L::z: return ArgType::SizeT;
                    case L::t: return ArgType::PtrDiffT;
                    default: return ArgType::Unknown;
                }
            case C::e:
            case C::E:
            case C::f:
            case C::F:
            case C::g:
            case C::G:
            case C::a:
            case C::A:
                switch (spec.length) {
                    case L::None: return ArgType::Float;
                    case L::l:
                    case L::L: return ArgType::Double;
                    default: return ArgType::Unknown;
                }
            case C::c: return spec.length == L::l ? ArgType::Int32 : ArgType::Char;
            case C::s: return ArgType::String;
            case C::p: return ArgType::Pointer;
            case C::n: return ArgType::Pointer;
            case C::Percent: return ArgType::None;
            default: return ArgType::Unknown;
        }
    }

    std::string_view length_modifier_name(const LengthModifier lm) noexcept {
        switch (lm) {
            case LengthModifier::None: return "(none)";
            case LengthModifier::hh: return "hh";
            case LengthModifier::h: return "h";
            case LengthModifier::l: return "l";
            case LengthModifier::ll: return "ll";
            case LengthModifier::j: return "j";
            case LengthModifier::z: return "z";
            case LengthModifier::t: return "t";
            case LengthModifier::L: return "L";
        }
        return "?";
    }

    std::string_view conversion_name(const Conversion c) noexcept {
        switch (c) {
            case Conversion::d: return "d";
            case Conversion::i: return "i";
            case Conversion::o: return "o";
            case Conversion::u: return "u";
            case Conversion::x: return "x";
            case Conversion::X: return "X";
            case Conversion::e: return "e";
            case Conversion::E: return "E";
            case Conversion::f: return "f";
            case Conversion::F: return "F";
            case Conversion::g: return "g";
            case Conversion::G: return "G";
            case Conversion::a: return "a";
            case Conversion::A: return "A";
            case Conversion::c: return "c";
            case Conversion::s: return "s";
            case Conversion::p: return "p";
            case Conversion::n: return "n";
            case Conversion::Percent: return "%";
            case Conversion::Invalid: return "?";
        }
        return "?";
    }

    std::string_view arg_type_name(const ArgType t) noexcept {
        switch (t) {
            case ArgType::Unknown: return "Unknown";
            case ArgType::None: return "(no arg)";
            case ArgType::Int16: return "int (16-bit on TI C2000)";
            case ArgType::Int32: return "int32_t / long";
            case ArgType::Int64: return "int64_t / long long";
            case ArgType::UInt16: return "unsigned int (16-bit on TI C2000)";
            case ArgType::UInt32: return "uint32_t / unsigned long";
            case ArgType::UInt64: return "uint64_t / unsigned long long";
            case ArgType::Float: return "float (32-bit)";
            case ArgType::Double: return "double";
            case ArgType::Char: return "char";
            case ArgType::String: return "const char*";
            case ArgType::Pointer: return "pointer";
            case ArgType::SizeT: return "size_t";
            case ArgType::PtrDiffT: return "ptrdiff_t";
        }
        return "?";
    }

    std::string format_spec_canonical(const FormatSpec &spec) {
        std::string out;
        out.reserve(8);
        out.push_back('%');

        if (spec.flags & flag_bits::Minus) out.push_back('-');
        if (spec.flags & flag_bits::Plus) out.push_back('+');
        if (spec.flags & flag_bits::Space) out.push_back(' ');
        if (spec.flags & flag_bits::Hash) out.push_back('#');
        if (spec.flags & flag_bits::Zero) out.push_back('0');
        if (spec.flags & flag_bits::Apostrophe) out.push_back('\'');

        if (spec.width == kFormatWidthStar) {
            out.push_back('*');
        } else if (spec.width != kFormatWidthUnset) {
            out += std::to_string(spec.width);
        }

        if (spec.precision == kFormatWidthStar) {
            out.append(".*");
        } else if (spec.precision != kFormatWidthUnset) {
            out.push_back('.');
            out += std::to_string(spec.precision);
        }

        const auto lm = length_modifier_name(spec.length);
        if (spec.length != LengthModifier::None) {
            out.append(lm);
        }
        out.append(conversion_name(spec.conversion));
        return out;
    }

    bool format_specs_equivalent(const FormatSpec &a, const FormatSpec &b) noexcept {
        return a.conversion == b.conversion
               && a.length == b.length
               && a.precision == b.precision
               && a.width == b.width
               && a.flags == b.flags;
    }
}