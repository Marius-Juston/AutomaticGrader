# CLI flags

The `AutomaticGrader` binary takes a small number of flags. They are
parsed by `grader_main::parse_args()` in
[`include/checks/cli_args.h`](https://github.com/Marius-Juston/AutomaticGrader/blob/master/include/checks/cli_args.h),
which is header-only so the parser is unit-testable in isolation.

## Flags

| Flag | Effect |
|---|---|
| `--selftest` | Exercise the shared slice-1 infrastructure (format parser, synthetic clock, printf capture, stimulus, expectations) end-to-end. Reports a failure count; exits non-zero on any regression. |
| `--report-json <path>` | After running all checks, write a structured JSON summary to `<path>`. Used by CI. |
| `--report-json=<path>` | Equivalent to the above (single-token form). |

Unknown flags are silently ignored — the harness has no other flags
today.

## Environment variables

| Variable | Effect |
|---|---|
| `GRADER_SELFTEST` | If set to a non-empty, non-`0` value, behaves as if `--selftest` was passed. |

## Examples

=== "Normal run"

    ```bash
    ./AutomaticGrader
    ```

    Runs every check function in the assignment's `checker()` list,
    logs to stderr via spdlog, exits 0 on success.

=== "JSON output for CI"

    ```bash
    ./AutomaticGrader --report-json /tmp/HW1.json
    ```

    Same behaviour as the bare run, plus a JSON file you can feed into
    `tools/render_report.py`.

=== "Self-test"

    ```bash
    ./AutomaticGrader --selftest
    # equivalent:
    GRADER_SELFTEST=1 ./AutomaticGrader
    ```

    Skips the per-assignment check list entirely.

## Report JSON schema

```json
{
  "assignment": "HW1",
  "passed": 6,
  "failed": 1,
  "checks": [
    {
      "name": "check_initialization",
      "status": "pass",
      "messages": []
    },
    {
      "name": "check_timer2",
      "status": "fail",
      "messages": [
        "[error] [check_timer2] PB1-pressed: GPIO61 (LED12) toggle bit not set",
        "[error]   spec: spec Ex.9: 'If PB1 (GPIO4) pressed (DAT==0), toggle LED12 (GPIO61)'"
      ]
    }
  ]
}
```

`check.name` is the C++ function name resolved at runtime via
`dladdr` — that's why `CMakeLists.txt` passes `-rdynamic` for non-MinGW
builds.

The renderer (`tools/render_report.py`) augments this with `commit`,
`folder`, `slot`, and `timestamp` derived from the workflow's
`<slot>.meta.json` sidecar. See
[Reports & traceability](../workflow/reports.md).
