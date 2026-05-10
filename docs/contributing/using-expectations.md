# Using expectations

The `expect_*` family lives in
[`include/checks/expectations.h`](https://github.com/Marius-Juston/AutomaticGrader/blob/master/include/checks/expectations.h)
and is what turns "the student should print every 250 ms with this
format" into a deterministic assertion against the
[capture pipeline](../architecture/capture-pipeline.md).

Every API returns `bool` (with `[[nodiscard]]`) — there's no
exception, no abort, no global side-effect. The caller decides what
"this check failed" means.

## `expect_format`

Asserts that a `PrintfCall` matches the expected format string by
parsing both into specifier sequences and comparing them tolerantly:
whitespace and static text differ → pass; specifier-vs-required-type
differ → fail with a spec-quality error.

```cpp
const grader::PrintfCall *latest =
    grader::latestPrintfCall(grader::SerialPort::SCIA);
if (!latest) {
    spdlog::error("no SCIA prints captured — main thread isn't pumping");
    return 0;
}

const bool ok = grader::expect_format(
    latest,
    "Timeint = %ld, Time = %.2f sec, Input = %.3f, SatOut = %.2f\r\n",
    "check_print_format[Ex.8]");
```

On failure, the error message includes:

- the offending argument index,
- the canonical spec form (`%ld` rather than `%l3d`),
- a TI-C2000-specific hint when the student wrote `%d` where `%ld`
  was required (the 16-bit `int` issue).

There's also `expect_format(call, ...)` (reference overload) for when
you already have a `PrintfCall &` and `expect_format_any(port, fmt,
...)` to match against any call on the port.

## `expect_arg_types`

When the spec pins the type of each argument (e.g. "the first argument
must be `int32_t`"):

```cpp
const bool args_ok = grader::expect_arg_types(
    latest,
    {grader::ArgType::Int32,
     grader::ArgType::Float,
     grader::ArgType::Float,
     grader::ArgType::Float},
    "check_print_format[Ex.8 arg types]");
```

`ArgType` enumerates `Int16`, `Int32`, `UInt16`, `UInt32`, `Float`,
`Double`, `String`, `Pointer`, `Char`. The parser inspects each
specifier's length modifier and conversion to derive the argument
type.

## `expect_print_cadence`

Asserts that a port saw exactly `N` calls during the synthetic window:

```cpp
const bool cad_ok = grader::expect_print_cadence(
    grader::SerialPort::SCIA, /*expected_count=*/4, /*tolerance=*/0.10,
    "check_print_cadence");
```

- Default tolerance is **±10%**. Tighter is better; looser hides bugs.
- The previous code's ±25–30% slack only existed to mask races that
  the cooperative driver eliminates ([details](../architecture/cooperative-driver.md)).
- If you need a window-based assertion ("between 3 and 5 calls"), use
  `expect_print_cadence_window(port, min, max, name)`.
- If you need a minimum-only assertion ("at least 3 calls"), use
  `expect_min_print_calls(port, min, name)`.

## When the check should be skipped, not failed

When a precondition isn't met (e.g. `CpuTimer2.PeriodInUSec == 0`,
meaning the student never called `ConfigCpuTimer`), prefer a clear
`spdlog::error` + early return over a misleading "expected 4 prints,
got 0" failure:

```cpp
const uint32_t period_us = static_cast<uint32_t>(CpuTimer2.PeriodInUSec);
if (period_us == 0) {
    spdlog::error("[check_print_cadence] CpuTimer2.PeriodInUSec is 0 — "
                  "student did not call ConfigCpuTimer");
    restore_snapshot(baseline);
    return 0;
}
```

The student gets a single actionable error instead of a stack of
downstream noise.

## Order matters

The conventional ordering in a Phase 4 check is:

1. `resetPrintfCapture()` (clears `g_printfCalls` **and** the synthetic
   clock).
2. `UARTPrint = 0; CpuTimer*.InterruptCount = 0;` (any other globals
   the cadence depends on).
3. `grader::drive_isr_with_main_pump(isr, period_us, total_ticks)`.
4. `expect_print_cadence` — fail-fast: if the cadence is wrong, the
   format check will be noise.
5. `latestPrintfCall(port)` + `expect_format` + `expect_arg_types`.
6. Restore snapshot, return.

Step (3) is non-negotiable for any cadence check. `run_isr_for_us`
alone won't fire flag-gated prints because the student's `while(1)`
body never runs — see
[Cooperative driver](../architecture/cooperative-driver.md#when-to-use-which).

## Reading the auto-generated reference

All the `expect_*` signatures are also surfaced in
[the C++ API reference](../reference/cpp-api.md). For day-to-day work
the prose on this page plus the working
[`src/checks/hw1.cpp`](https://github.com/Marius-Juston/AutomaticGrader/blob/master/src/checks/hw1.cpp)
example is enough; the API ref exists for symbol-by-symbol lookup.
