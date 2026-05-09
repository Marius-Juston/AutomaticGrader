#ifndef AUTOMATICGRADER_CHECKS_CLI_ARGS_H
#define AUTOMATICGRADER_CHECKS_CLI_ARGS_H

// Pure argv-parsing helper extracted from main() for unit testability.
// Header-only so tests can include it without linking the grader binary.

#include <cstring>
#include <string>

namespace grader_main {

struct ParsedArgs {
    bool selftest = false;
    std::string report_json_path;  // valid only if report_json_set == true
    bool report_json_set = false;
};

// Parses argv. Pure: no env reads, no I/O.
//   --selftest                 -> selftest = true
//   --report-json <path>       -> report_json_path = <path>, report_json_set = true
//   --report-json=<path>       -> same
// Unknown args are silently ignored (the harness has no other flags today).
// argv[0] is always skipped.
inline ParsedArgs parse_args(int argc, char** argv) {
    ParsedArgs out;
    if (argc <= 0 || argv == nullptr) return out;
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == nullptr) continue;
        if (std::strcmp(argv[i], "--selftest") == 0) {
            out.selftest = true;
        } else if (std::strcmp(argv[i], "--report-json") == 0 && i + 1 < argc &&
                   argv[i + 1] != nullptr) {
            out.report_json_path = argv[++i];
            out.report_json_set = true;
        } else if (std::strncmp(argv[i], "--report-json=", 14) == 0) {
            out.report_json_path = argv[i] + 14;
            out.report_json_set = true;
        }
    }
    return out;
}

// Returns true iff env points to a non-null, non-empty string whose first byte
// is not '0'. Mirrors the original behavior in main.cpp.
inline bool selftest_env_active(const char* env) {
    return env != nullptr && env[0] != '\0' && env[0] != '0';
}

}  // namespace grader_main

#endif  // AUTOMATICGRADER_CHECKS_CLI_ARGS_H
