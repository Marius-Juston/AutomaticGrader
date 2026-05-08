# Lab5 Grader Roadmap

**Authoritative spec:** `context/lab/Lab5.txt`
**Reference fixture:** `context/code_solutions/SE423_Lab_phuriy2_lourdes3_emilyah3/Lab5/`
**Implementation:** `src/checks/lab5.cpp` (deferred), `include/checks/lab5.h` (deferred)

## Spec inventory (Lab5.txt)

| Topic                | Behavior                                                                                                                                                   |
| -------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Code merge           | Lab3 (EPWM motors, EQEP encoders) + Lab4 (ADCC gyro sampling).                                                                                             |
| PI control law       | `errLeft = Vref - velLeft`. `IKLeft += 0.5*(errLeft + errLeft_old)*dt` (trapezoidal). `uLeft = Kp*errLeft + Ki*IKLeft`.                                    |
| Decoupled            | Independent left/right PI loops. Anti-windup: `if (fabs(uLeft) > 10) IKLeft = IKLeft_old;`                                                                  |
| Coupled              | `errLeft = Vref - velLeft + Kp_turn*(turn - (velRight-velLeft)/wheelbase)`; analogous for right.                                                            |
| Gyro bearing         | LPR510 (or MPU-9250 in Lab6) integrated trapezoidal in radians for `bearing` global.                                                                       |
| Friction comp        | At 60% scaling (coefficients from Lab3).                                                                                                                    |
| Vref source          | Encoder knob (EQEP3) divided by 20.                                                                                                                         |
| Defaults             | `Kp=3.0, Ki=5.0` baseline (pre-tuning).                                                                                                                     |
| Print cadence        | 100 ms.                                                                                                                                                     |
| Print format         | Velocities `%.2f`, Vref `%.2f`, bearing `%.2f`. **[ambiguous]** — confirm precision against spec.                                                           |

## Stubs needed

- [ ] All Lab3 EPWM + EQEP stubs.
- [ ] All Lab4 ADCC stubs.
- [ ] **Shared:** `serial_printf` capture; `inject_encoder_count`.

## Checks to implement

- [ ] `check_initialization` — Lab3 + Lab4 register baseline; PI globals (`Kp, Ki, IKLeft, IKRight, errLeft, errRight, uLeft, uRight, velLeft, velRight, Vref, turn, bearing`) all default.
- [ ] `check_PI_decoupled_step` — set `Vref=2.0`, `velLeft=1.0` → `errLeft=1.0`. After one ISR call, assert `IKLeft` advanced by `0.5*1.0*dt`, `uLeft = 3.0*1.0 + 5.0*IKLeft`. Sweep over multiple steps to verify integrator advances correctly.
- [ ] `check_PI_anti_windup` — drive `errLeft` to push `uLeft` > 10. Assert `IKLeft` is held at previous value (not advanced) when saturated.
- [ ] `check_PI_coupled` — set `velLeft != velRight` and `turn != 0`; verify the differential term in `errLeft` matches `Kp_turn*(turn - (velRight-velLeft)/wheelbase)`.
- [ ] `check_bearing_integration` — inject constant gyro voltage corresponding to known angular rate; drive ISR for 1 s synthetic; assert `bearing` integrated to `rate * 1.0` ±tolerance.
- [ ] `check_Vref_from_encoder_knob` — `inject_encoder_count(EQEP3, 200)`; trigger ISR; assert `Vref == 200/20 = 10.0`.
- [ ] `check_print_cadence` — 100 ms; ±10%.
- [ ] `check_print_format` — `expect_arg_types(latest, {ARG_FLOAT, ARG_FLOAT, ARG_FLOAT, ARG_FLOAT})` with the spec'd order.

## Validation matrix (deep)

- [ ] Reference → all pass
- [ ] Mutation A: forget anti-windup branch → `check_PI_anti_windup` fails (integrator overflows)
- [ ] Mutation B: rectangle-rule integration instead of trapezoidal → `check_PI_decoupled_step` fails
- [ ] Mutation C: drop coupled term → `check_PI_coupled` fails
- [ ] Mutation D: `%d` for `Vref` → `check_print_format` fails
- [ ] Mutation E: gyro integration sign flip → `check_bearing_integration` fails on sign
- [ ] Run twice → identical
- [ ] Cross-regression

## Skip — demo only

- Motor amplifier ON/OFF physical switch
- Floor test of robot tracking
- Subjective tuning ("quicker transient")

## Open questions / [ambiguous] from spec

- [ ] `wheelbase` value — student-measured; test asserts presence not value.
- [ ] `Kp_turn` for coupled control — student-tuned baseline. Test against the spec'd default if any.
- [ ] Direction sign of `setEPWM1A`/`setEPWM2A` — implicit motor wiring; assume + drives forward.
- [ ] LPR510 vs MPU-9250 source — Lab5 uses LPR510 (analog gyro); Lab6 swaps to MPU-9250 SPI. Verify against reference.
