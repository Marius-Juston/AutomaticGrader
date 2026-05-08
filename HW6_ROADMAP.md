# HW6 Grader Roadmap

**Authoritative spec:** `context/homeworks/HW6.tex`
**Reference fixture:** none in `context/code_solutions/`
**Implementation:** **none in the firmware grader.** Routed elsewhere.

## Decision: out-of-scope for `AutomaticGrader`

HW6.tex covers (a) A* path planning over a course map (12 fill-in items in `astar.cpp`, modify `mapCourseStart[176]`) and (b) Kalman filter tuning in MATLAB (Q/R 3×3 matrices, OptiTrack + wheel + gyro fusion). Neither targets a TMS320F28379D peripheral, so the existing register-state validator infrastructure is not applicable.

> The slice-1 firmware-grader infrastructure (printf capture, stimulus, synthetic clock, `expect_*`) does **not** apply to HW6 — HW6 is a host-side algorithm grader with no peripheral state to intercept and no `temp_main`/`serial_printf` flow. Re-using the slice-1 headers in a separate Option-B harness would only add link-time bloat without giving any of the test categories a useful hook.

**Recommended:** the parent CI repo's workspace manifest routes HW6 to a **separate desktop/Python harness** that:

- Compiles the student's `astar.cpp` standalone (no TI stubs, no `temp_main`).
- Runs `pathfind(start, goal)` against canned `mapCourseStart[]` test maps.
- Asserts path length, presence of expected waypoints, and the no-path return value.

Kalman tuning is left to manual review (MATLAB outputs and plots).

## If a desktop harness is added (Option B)

If course staff want HW6 inside this repo:

- [ ] Add a separate target `AutomaticGraderHW6` in `CMakeLists.txt` (no `temp_main`, no `ti_stubs.h`)
- [ ] Add `src/checks/hw6.cpp` and `include/checks/hw6.h`
- [ ] Conditionally link the student's `astar.cpp` from the workspace path
- [ ] `check_astar_basic` — known map + known start/goal → assert returned path length matches reference
- [ ] `check_astar_obstacle` — block direct route → assert path detours
- [ ] `check_astar_no_path` — fully blocked → assert documented no-path sentinel (likely `-1` or empty path)
- [ ] `check_astar_initialization` — verify all 12 fill-in items are present (string-grep heuristic on student source)

## Validation matrix (only applies to Option B)

- [ ] Primary reference solution if/when added under `context/code_solutions/.../HW6/`
- [ ] Mutation A: break heuristic (Manhattan vs Euclidean) → still finds path but suboptimal length → fails on length
- [ ] Mutation B: comment out `closedSet.add()` → infinite loop → fails (use a timeout)
- [ ] Mutation C: return path on impossible map → `check_astar_no_path` fails
- [ ] No snapshot/restore concerns (no shared state)

## Open questions

- [ ] Confirm with course staff: is HW6 graded automatically, or is it review-only?
- [ ] If automated: should the harness live in this repo (Option B) or a sibling Python project? Recommend sibling for clean separation of firmware-grader vs. algorithm-grader.
- [ ] What's the canonical `mapCourseStart[]` fixture set? Need at least 3: open map, single-obstacle, fully blocked.
- [ ] Kalman portion: confirm grading method (manual review of plots vs. tolerance-checked MATLAB asserts).

## Note on the `ASSIGNMENT` selector

`ASSIGNMENT=HW6` will fail at the CMake mapping step (`hw6.cpp not found`) until Option B lands. This is intentional — fail-fast is preferable to silently building a no-op grader.
