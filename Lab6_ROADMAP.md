# Lab6 Grader Roadmap

**Authoritative spec:** `context/lab/Lab6.txt`
**Reference fixture:** `context/code_solutions/SE423_Lab_phuriy2_lourdes3_emilyah3/Lab6/`
**Implementation:** `src/checks/lab6.cpp` (deferred), `include/checks/lab6.h` (deferred)

## Spec inventory (Lab6.txt)

| Topic                       | Behavior                                                                                                                                       |
| --------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| LADAR (URG-04LX)            | UARTC (SCIC) at 19200→115200 baud; `G_command` request every 100 ms via `cpu_timer1_isr`; 228 distance readings (20–5700 mm); ping/pong buffers. |
| LADAR processing            | `LADARfront = min(samples 111..115)` (5 readings around index 113 = 0°); `LADARrightfront = min(samples ~90)` (~62°). Convert each to x,y in `ladar_pts[228]`. |
| MPU-9250 SPI merge          | SPIB from HW3; PI control moves into SWI1 posted by SPIB_ISR; ADCC trigger maintains 1 ms cadence.                                              |
| Gyro zero-cal (4 s)         | 0–2 s ignore; 2–4 s sum & average; 4 s+ subtract. (Same SM as Lab4 but for MPU-9250.)                                                          |
| Dead reckoning              | `ROBOTps.{x, y, theta}` updated via trapezoidal integration of wheel velocities and gyro Z rate.                                                |
| Wall-following SM           | SWI1, two states: (1) Left Turn if front < threshold, (2) Right Follow else. Proportional control on `turn`.                                    |
| Linux UART CLI              | UARTD at 2083332 baud; `LinuxCommands[]` array of 11 floats for parameter tuning.                                                              |
| LabVIEW interface           | 8 floats out + `ROBOTps` in every 250 ms via shared-memory RPI4 relay.                                                                          |
| Print cadence/format        | 100 ms: state, Vref, turn, gyro, velocities, x/y. Floats `%.2f`. **[ambiguous]** — large field; confirm against reference.                      |
| Three SWIs                  | SWI1 (highest), SWI2 (middle), SWI3 (lowest); priority hierarchy enforced.                                                                      |

## Stubs needed

- [ ] All HW3 SPI stubs (MPU-9250).
- [ ] All Lab3 EPWM/EQEP + Lab4 ADCC stubs.
- [ ] LADAR stubs: SCIC RX/TX + `inject_lidar_frame(uint16_t distances[228])` helper that fills the ping/pong buffer.
- [ ] UARTD stubs (or shared SCI capture extended to D).
- [ ] SWI post stubs: `PostSWI1`, `PostSWI3` — record post events.
- [ ] **Shared:** `serial_printf` capture per port (extend to SCID).

## Checks to implement

- [ ] `check_initialization` — Lab3+4+5 baseline + LADAR/SPIB/UARTD config. Verify SCIC baud, SPIB at 1 MHz mode 01, UARTD at 2083332. SWI vector entries.
- [ ] `check_lidar_parsing` — `inject_lidar_frame()` with synthetic distances (e.g. all `1000` mm); drive `cpu_timer1_isr()`; assert `LADARfront == 1000`, `LADARrightfront == 1000`. Test with a wall on one side: distances `[3000]*113 + [200]*5 + [3000]*110` → `LADARfront ≈ 200`.
- [ ] `check_lidar_xy_conversion` — known robot pose (0,0,0); LADAR reading at index 113 = 1000 mm → `ladar_pts[113] ≈ (1.0/0.3048, 0)` ft (forward, x-axis). Verify trig for 90° offset etc.
- [ ] `check_mpu9250_full_init` — same as HW3 `check_imu_full_init`.
- [ ] `check_dead_reckoning` — drive ISR with `velLeft = 1.0`, `velRight = 1.0`, gyro = 0 for 1 s synthetic. Assert `ROBOTps.x ≈ 1.0` ft, `theta ≈ 0`. Set gyro to constant rate; verify theta integrates.
- [ ] `check_wall_follow_state_left_turn` — `inject_lidar_frame()` with `LADARfront < threshold`; drive `SWI1`; assert state transitions to "Left Turn", `turn` adjusted accordingly.
- [ ] `check_wall_follow_state_right_follow` — similar with front clear, right wall present.
- [ ] `check_swi_priority` — verify SWI1 ISR is in highest-priority PIE group, SWI2 middle, SWI3 lowest. Inspect `PieVectTable` and IFR registers.
- [ ] `check_print_cadence` — 100 ms; ±10%.
- [ ] `check_print_format` — wide `expect_arg_types({...})` per spec.

## Validation matrix (deep)

- [ ] Reference → all pass
- [ ] Mutation A: LADAR threshold change → `check_wall_follow_*` fails on transitioning at wrong distance
- [ ] Mutation B: dead-reckoning sign flip on gyro Z → `check_dead_reckoning` theta wrong direction
- [ ] Mutation C: wrong LADAR index for "front" (113→0) → `check_lidar_parsing` wrong sample
- [ ] Mutation D: SWI priority swap → `check_swi_priority` fails
- [ ] Mutation E: format wide string with wrong type → `check_print_format` fails
- [ ] Run twice → identical
- [ ] Cross-regression: prior labs/HWs pass

## Skip — demo only

- Oscilloscope GPIO61 SWI1 timing
- Physical LADAR alignment / mounting
- WiFi connectivity + motion-capture infrastructure
- LabVIEW grid-line drawing

## Open questions / [ambiguous] from spec

- [ ] Exact LADAR sample indices for "right front" — spec says ~62°. Compute from 228-element array span (240° / 228 ≈ 1.05° per sample, so index 0 = -120°, index 113 = 0°, index ~172 = +62°). Confirm.
- [ ] Wall-follow gain `Kp_right_wall` and threshold values — student-tuned. Test transitions, not specific gains.
- [ ] LinuxCommands[] indices for which-parameter — spec table; test array length and presence not specific values.
