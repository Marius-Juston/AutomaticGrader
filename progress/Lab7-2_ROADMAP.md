# Lab7-2 Grader Roadmap (Robot Vision)

**Authoritative spec:** `context/lab/Lab7-2.txt`
**Reference fixture:** `context/code_solutions/SE423_Lab_phuriy2_lourdes3_emilyah3/Lab7-2/`
**Implementation:** `src/checks/lab7-2.cpp` (deferred), `include/checks/lab7-2.h` (deferred)

## Spec inventory (Lab7-2.txt)

| Topic                   | Behavior                                                                                                                                                                                                                                             |
|-------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| OpenMV camera           | RPI4-relayed; `find_blobs()` with LAB color thresholds for two colors (orange, purple). Output: `area`, `row centroid`, `col centroid` for up to 3 blobs per color.                                                                                  |
| Distance estimation     | Empirical curve fit: `row_centroid → distance_ft` (data collected at known distances 1–4.5 ft). Student implements non-linear equation in SWI1.                                                                                                      |
| Ball-follow SM (orange) | RobotState 20: if `area > MaxAreaThreshold1`, set `Vref = 0.75`, `turn = kpvision*(0 - colcentroid)`. State 22: stop 1 s (gripper open). State 24: reverse 0.5 ft/s for 1 s. State 26: stop 1 s (gripper close). Return to State 1 with 2 s inhibit. |
| Ball-follow SM (purple) | RobotState 30/32/34/36 — analogous structure for the second color.                                                                                                                                                                                   |
| XY waypoint navigation  | RobotState 1: call `xy_control()` to drive to target; switch to ball-follow if blob area grows past threshold.                                                                                                                                       |
| OptiTrack Kalman fusion | Merges motion-capture pose with dead-reckoned pose; updates `ROBOTps`.                                                                                                                                                                               |
| Print cadence           | 100 ms: state, velocities, ball stats (row/area), robot x/y/θ.                                                                                                                                                                                       |
| Print format            | Wide format string. Floats and one int (`RobotState` int16). **[ambiguous]** — confirm precision and order against reference.                                                                                                                        |

## Stubs needed

- [ ] All Lab6 stubs (LADAR, dead-reckoning baseline).
- [x] **Shared (shipped):** `serial_printf` / `UART_printfLine` capture → `g_printfCalls`. Auto-wired in
  `src/ti_stubs.cpp`; consume via `include/checks/printf_capture.h`.
- [x] **Shared (shipped):** stimulus helpers — `include/checks/stimulus.hpp` (`press_button`, `inject_adc_result`,
  `inject_spi_rx`, `inject_encoder_count`, `inject_lidar_*`).
- [x] **Shared (shipped):** synthetic clock + `run_isr_for_us` — `include/checks/synthetic_clock.h`.
- [x] **Shared (shipped):** format parser + `expect_format` / `expect_arg_types` / `expect_print_cadence` —
  `include/checks/format_parser.h` + `include/checks/expectations.h`. Run `./AutomaticGrader --selftest` to verify the
  infra after edits.
- [ ] **Deferred:** OpenMV / RPI4 stub — `inject_blob(color_idx, area, row, col)` helper that writes into the
  student-readable blob arrays.
- [ ] **Deferred:** OptiTrack stub — `inject_optitrack(x, y, theta)` for the Kalman fusion test.

## Checks to implement

- [ ] `check_initialization` — Lab6 baseline + vision globals (`RobotState`, `MaxAreaThreshold1/2`,
  `MaxColThreshold1/2`, `MaxRowThreshold1/2`, `kpvision`, distance fit coefficients) defaults.
- [ ] `check_distance_curve_fit` — for each calibration point in spec (rows at known distances), assert
  `student's row→distance fn` returns expected ft within tolerance.
- [ ] `check_ball_follow_orange_state_20_to_22` — set `RobotState = 20`,
  `inject_blob(orange, area=large, row=mid, col=0)` (deferred — write the blob globals directly until `inject_blob`
  ships), drive SWI1; assert `Vref ≈ 0.75`, `turn ≈ 0`. Increase `area` past trigger; assert state transitions to 22 and
  motors stop.
- [ ] `check_ball_follow_orange_state_22_to_24_to_26` — drive timer for 1 s; assert state advance 22→24 (reverse), then
  after another second 24→26 (stop), then to 1 with 2 s inhibit.
- [ ] `check_ball_follow_purple` — same but with RobotState 30/32/34/36.
- [ ] `check_centering_proportional_control` — set `colcentroid = +50` and -50; assert `turn = -50*kpvision` and
  `+50*kpvision` respectively.
- [ ] `check_xy_waypoint_state_1` — set `RobotState = 1`, target (5, 0); call `xy_control(5, 0)`; assert returns 0 (not
  at target). With current pose at (5, 0), assert returns 1.
- [ ] `check_optitrack_fusion` — `inject_optitrack(2, 3, 0.5)` (deferred — write `Optitrackdata[]` directly until the
  helper ships); drive Kalman update; assert `ROBOTps` shifts toward injected pose (relative to dead-reckoned estimate).
- [ ] `check_print_cadence` — 100 ms.
- [ ] `check_print_format` — wide
  `expect_arg_types(latest, {grader::ArgType::Int16, grader::ArgType::Float, grader::ArgType::Float, ...})` per spec.

## Validation matrix (deep)

- [ ] Reference → all pass
- [ ] Mutation A: distance curve coefficient off → `check_distance_curve_fit` fails
- [ ] Mutation B: missing state-22 transition → `check_ball_follow_orange_state_*` fails on stuck state
- [ ] Mutation C: `kpvision` sign flipped → centering steers wrong way
- [ ] Mutation D: drop OptiTrack fusion → `check_optitrack_fusion` fails on expected `ROBOTps` shift
- [ ] Mutation E: `%d` for `Vref` → `check_print_format` fails
- [ ] Run twice → identical
- [ ] Cross-regression

## Skip — demo only

- Live OpenMV image capture and color calibration
- Actual robot floor test of ball-following
- OptiTrack motion capture system setup and marker placement
- LabVIEW live graphical updates during robot motion

## Open questions / [ambiguous] from spec

- [ ] Exact thresholds `MaxAreaThreshold1/2`, `MaxColThreshold1/2` — student-tuned. Test asserts transitions happen, not
  specific values.
- [ ] Distance curve fit form — quadratic? exponential? Spec gives data; student picks form. Test against the published
  calibration data points.
- [ ] OpenMV blob arrays — confirm storage format (struct array vs parallel arrays) from reference.
- [ ] State-machine inhibit duration — spec says 2 s. Confirm against reference.
- [ ] Kalman gain values for OptiTrack fusion — student-tuned; test verifies output is between dead-reckoned and
  OptiTrack values, not specific gain.
