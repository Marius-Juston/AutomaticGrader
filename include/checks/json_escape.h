#ifndef AUTOMATICGRADER_CHECKS_JSON_ESCAPE_H
#define AUTOMATICGRADER_CHECKS_JSON_ESCAPE_H

// JSON string-content escaper, extracted from validator.cpp so it can be
// unit-tested directly. Header-only.
//
// Contract: produces the contents of a JSON string (caller wraps with quotes).
// All bytes < 0x20 are emitted as either a special escape (\b \f \n \r \t)
// or a \uXXXX escape; bytes >= 0x20 are emitted as-is except for `"` and `\`
// which become \" and \\ respectively. Bytes >= 0x80 are passed through
// unchanged (the resulting JSON is valid UTF-8 iff the input was).

#include <cstdio>
#include <string>

namespace grader_json {
    inline std::string json_escape(const std::string &s) {
        std::string out;
        out.reserve(s.size() + 2);
        for (const unsigned char c: s) {
            switch (c) {
                case '"': out += "\\\"";
                    break;
                case '\\': out += "\\\\";
                    break;
                case '\b': out += "\\b";
                    break;
                case '\f': out += "\\f";
                    break;
                case '\n': out += "\\n";
                    break;
                case '\r': out += "\\r";
                    break;
                case '\t': out += "\\t";
                    break;
                default:
                    if (c < 0x20) {
                        char buf[8];
                        std::snprintf(buf, sizeof(buf), "\\u%04x", c);
                        out += buf;
                    } else {
                        out += static_cast<char>(c);
                    }
            }
        }
        return out;
    }
} // namespace grader_json

#endif  // AUTOMATICGRADER_CHECKS_JSON_ESCAPE_H