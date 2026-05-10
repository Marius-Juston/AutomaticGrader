# Validation checklist

Before opening a PR for a new checker, walk this checklist. It exists
because every previous "looked fine, merged, broke later" was caused by
skipping one of these.

The per-assignment roadmaps under
[`progress/`](https://github.com/Marius-Juston/AutomaticGrader/tree/master/progress)
encode the same matrix with assignment-specific specifics — link to
your roadmap from the PR description so the reviewer can verify
boxes.

## 1. Reference solution passes (or fails *only* on documented spec deviations)

Run the grader against the reference fixture in
`context/code_solutions/.../{HW,Lab}{N}/`:

```bash
cmake -S . -B cmake-build-debug \
    -DASSIGNMENT=HW{N} \
    -DSTUDENT_SRC=context/code_solutions/myHW_Yingyord_First/HW{N}/HW{N}fy_main.c
cmake --build cmake-build-debug -j
./cmake-build-debug/AutomaticGrader
```

Expected outcome: all checks pass, zero `spdlog::warn` lines, exit 0.

If the reference deviates from spec — e.g. HW1's `check_timer2`
correctly fails the LABstarter reference because LABstarter
implements edge-on-release LED toggle instead of the spec's
edge-on-press — that's fine. **Document the deviation in the roadmap**
so the next reviewer doesn't think the failure is a checker bug.

## 2. Mutation matrix — ≥5 broken copies fail on the *exact* field

Hand-author at least 5 mutations of the reference, each targeting a
different category. Each must fail on the *exact* register/field
name that diverged — not generically.

| Category | Example mutation | What should fail |
|---|---|---|
| Numeric constant | Change `ConfigCpuTimer(..., 10000)` → `5000` | `CpuTimer0.PeriodInUSec` |
| Structural | Drop a `GPIO_SetupPinMux(31, ...)` call | `gpiosSetup[31]` |
| Format-string | Change `%ld` → `%d` | `expect_format` arg-type mismatch |
| Cadence | Change the gating `if (Counter % 50 == 0)` to `% 100` | `expect_print_cadence` count |
| Stimulus reaction | Drop the PB1 branch | the PB1-specific `report()` line |

A mutation that fails on the wrong field means the check is too
coarse — it's catching the symptom but not the cause.

## 3. Run twice without rebuilding → identical pass result

```bash
./cmake-build-debug/AutomaticGrader        # first run: PASS
./cmake-build-debug/AutomaticGrader        # second run: must also PASS
```

This verifies Phase 3 / 4 snapshot/restore. If the second run fails,
some volatile global was mutated and not restored, and the next check
in `checker()` saw polluted state on run 2.

## 4. Re-run prior assignments → no regressions

If you added a new capturing stub to `src/ti_stubs.cpp`, rebuild and
run every prior assignment to confirm the new stub didn't change
existing capture behaviour:

```bash
for asn in HW1 HW2 HW3 HW4 HW5; do
  cmake -S . -B cmake-build-debug -DASSIGNMENT=$asn \
        -DSTUDENT_SRC=context/code_solutions/.../$asn/...
  cmake --build cmake-build-debug -j
  ./cmake-build-debug/AutomaticGrader || echo "$asn: REGRESSION"
done
```

This is the most-skipped step. Skipping it is how a new stub for HW3
silently breaks HW1's GPIO comparisons three weeks later.

## 5. Self-test still green

```bash
./cmake-build-debug/AutomaticGrader --selftest
```

If you touched anything under `src/checks/` that belongs to the shared
infrastructure (`format_parser.cpp`, `synthetic_clock.cpp`,
`printf_capture.cpp`, `stimulus.cpp`, `expectations.cpp`,
`main_loop_driver.cpp`), the self-test is the regression gate.

## 6. Python tests green (if you touched anything under `tools/`)

```bash
python3 tools/select_assignments.py --self-test
python3 -m unittest discover -s tests/python -v
```

## 7. Roadmap boxes checked

The PR includes an update to `progress/HW{N}_ROADMAP.md` /
`progress/Lab{N}_ROADMAP.md` reflecting the checks that now pass and
any new stubs added. Reviewer will look here first to confirm scope.

## 8. CI green

The PR's `tests.yml` workflow run passes:

- `unit-tests` (CTest)
- `selftest` (`./AutomaticGrader --selftest`)
- `python-tools` (matrix Python 3.12 + 3.13)

A failing CI is the merge gate. Don't merge around it.

---

If every box is ticked, the PR is mergeable. If you find yourself
skipping a step "because it's tedious," that step is exactly the one
that catches the next bug.
