# Common gotchas

The full list of things to avoid. None of these are theoretical — every
entry is something that's bitten a previous contributor.

## Format-string gotchas

### `%d` vs `%ld` (the single biggest bug class)

The TI C2000 target has a **16-bit `int`**. The grader host has a
32-bit `int`. A student writing `%d` for an `int32_t` produces
correct-looking output on the grader and broken output on hardware.

| Wrong | Right |
|---|---|
| `%d` for `int32_t` | `%ld` |
| `%u` for `uint32_t` | `%lu` |
| `%x` for `uint32_t` | `%lx` |
| `%f` for any non-`double` | `%f` is *fine* but pin precision: `%.2f` |

`expect_format` parses both the spec and the student's format string
into typed specifier sequences and flags mismatches with a TI-specific
hint. Use the exact spec string verbatim in
`expect_format(call, "...spec string...")`.

### Whitespace in the format string is ignored, but the rest isn't

`expect_format` tolerates whitespace differences, but every static
character outside whitespace must match. `Time = %.2f sec` and
`Time=%.2f s` are **different** for the matcher — the trailing static
text differs (`sec` vs `s`).

If your spec doesn't pin the static text, use the more permissive
`expect_arg_types` instead — it only checks the typed specifier
sequence.

## Snapshot/restore gotchas

### Forgetting to restore mid-check

Phase 3 / 4 must restore every volatile global it mutated. The
canonical pattern (HW1):

```cpp
const Hw1Phase3Snapshot baseline = take_snapshot();
// ... mutations ...
restore_snapshot(baseline);
return ok ? 1 : 0;
```

If you skip the restore, the next check in `checker()` sees polluted
state. The validation step "run twice without rebuilding → identical
pass result" is the regression test for this.

### Not snapshotting enough

A check that toggles LED bits but forgets to snapshot `GpioDataRegs`
leaks state. If you mutate it, snapshot it. The HW1
`Hw1Phase3Snapshot` captures `GpioDataRegs`, `UARTPrint`, and
`CpuTimer2.InterruptCount` — extend it for any new volatile you
touch.

## Cooperative driver gotchas

### Using `run_isr_for_us` for a print check

`run_isr_for_us` only runs the ISR. If the print is gated by a flag
the **main loop body** sets (`if (UARTPrint) serial_printf(...)`),
no print fires because the main body never runs. Use
`grader::drive_isr_with_main_pump(...)` for any print/cadence
assertion.

### Calling `start_main_thread()` more than once per check

`Validator::start_main_thread()` invokes `grader::run_student_init()`,
which is idempotent (runs init only on the first call). Calling it
multiple times is harmless but wasteful. Once per check function is
the convention.

If you genuinely need to re-run init (uncommon), call
`grader::reset_student_init()` first.

### Student source has `for(;;)` or `while(true)` instead of `while(1)`

`tools/patch_student_source.py` only rewrites `while (1)`. The fixture
will compile but the main-loop body won't be reachable from
`step_main_loop`. Symptom: every cadence check reports 0 calls.

The fix is either editing the fixture or extending the patcher's
regex; the latter is the right answer if it's a common pattern in
your student population.

## Stimulus gotchas

### Skipping the primer

When testing "button held → LED toggles", many student implementations
use an edge detector (`if prev==1 && cur==0`). A naïve test that just
asserts `press_button(4)` before driving the ISR doesn't produce an
edge because `prev` starts at 0. Always fire one ISR with the buttons
released first — see
[Using stimulus](using-stimulus.md#recipe-button-led-toggle).

### Mixing active-high and active-low

Buttons on the C2000 LaunchPad are **active-low**. `press_button(n)`
writes `GPxDAT.bit.GPIO{n} = 0`. If your check sees no reaction,
verify the spec's polarity and check what bit `press_button` actually
sets versus what the student reads.

## Cadence tolerance gotchas

### Loose tolerances mask real bugs

Anything ≥ ±25% is a smell. The cooperative driver makes cadence
deterministic; if you need >10% slack, the check is probably wrong
(wrong ISR period? wrong driver?). Investigate, don't loosen.

### Window-based when the spec says "at least"

The spec says "fire at least every 100 ms" → use
`expect_min_print_calls(port, expected, name)`. The spec says "fire
exactly every 250 ms" → use `expect_print_cadence(port, count, 0.10)`.
Match the assertion to the wording.

## Stub gotchas

### Adding a new stub without the `-include ti_stubs.h` knowledge

Every TU is force-compiled with `-include ti_stubs.h` (see
[CMakeLists.txt:217](https://github.com/Marius-Juston/AutomaticGrader/blob/master/CMakeLists.txt#L217)).
TI-only keywords (`__interrupt`, `EALLOW`, `EDIS`, `DATA_SECTION`,
`Uint16`) are mapped to no-ops or standard types. If your new stub
needs a TI-only macro that isn't in `ti_stubs.h`, add it there too.

### Inventing a shadow array instead of writing to `*Regs`

The existing comparison overloads operate on the `*Regs` globals (and
`gpiosSetup[]` for the GPIO pair). If you invent a new shadow array
for your stub's state, you also have to wire it into
`generated.cpp` / `compare_generated.cpp` / `populate_all_zero()`.
Prefer writing to the existing `*Regs` global so the comparison
machinery picks it up for free.

## Workflow gotchas

### Duplicate-id manifest without a slot-aware workflow

If you maintain a fork of the reusable workflow, make sure
`out/${SLOT}.json` (not `out/${ID}.json`) is the upload path and the
artifact name includes the slot. Single-id manifests still produce
`slot == id`, so no migration is needed, but two `HW1` rows will
overwrite each other's artifact if the path is keyed by id alone.

### Forgetting the `<slot>.meta.json` sidecar

`render_report.py` falls back to filename heuristics when the sidecar
is missing, but the fallback can't recover the commit SHA. Always
write the sidecar — it's three lines of shell and one Python heredoc.

See [Manifest & slots](../workflow/manifest-and-slots.md#meta-sidecar-what-makes-results-traceable)
for the contract.
