# Lab2 Grader Roadmap

**Authoritative spec:** `context/lab/Lab2.txt`
**Reference fixture:** `context/code_solutions/SE423_Lab_phuriy2_lourdes3_emilyah3/Lab2/`
**Implementation:** `src/checks/lab2.cpp` (deferred), `include/checks/lab2.h` (deferred)

## Spec inventory (Lab2.txt)

| Topic               | Behavior                                                                                                           |
| ------------------- | ------------------------------------------------------------------------------------------------------------------ |
| Switch inputs       | GPIOs 157–160 as input + pullup; read via `GP?DAT.bit.GPIO?`. (Pull-ups via `GPIOxPUDx`.)                          |
| Worker functions    | `void SetLEDsOnOff(int16_t LEDvalue)` — bits 0..4 of LEDvalue map to 5 LEDs. `int16_t ReadSwitches(void)` returns bitfield from the 5 switches. |
| Bitwise operators   | Both functions must use bitwise AND/OR (not branches).                                                              |
| Timer rate          | Variable per exercise: 0.1 s, 0.25 s, 1 ms.                                                                         |
| Print cadence       | Every 100 ms (modulus 10 at 1 ms timer rate).                                                                       |
| LabVIEW TCP/IP      | Demo only — not auto-graded.                                                                                         |

## Stubs needed

- [ ] GPIO pullup-disable / pullup-enable stubs (`GpioCtrlRegs.GP*PUD` writes).
- [x] **Shared (shipped):** `serial_printf` / `UART_printfLine` capture → `g_printfCalls`. Auto-wired in `src/ti_stubs.cpp`; consume via `include/checks/printf_capture.h`.
- [x] **Shared (shipped):** stimulus helpers — `include/checks/stimulus.hpp` (`press_button`, `inject_adc_result`, `inject_spi_rx`, `inject_encoder_count`, `inject_lidar_*`).
- [x] **Shared (shipped):** synthetic clock + `run_isr_for_us` — `include/checks/synthetic_clock.h`.
- [x] **Shared (shipped):** format parser + `expect_format` / `expect_arg_types` / `expect_print_cadence` — `include/checks/format_parser.h` + `include/checks/expectations.h`. Run `./AutomaticGrader --selftest` to verify the infra after edits.

## Checks to implement

- [ ] `check_initialization` — `gpiosSetup[157..160]` as input + pullup. Pull-up enabled via `GPxPUD` bits clear (=enabled on TI).
- [ ] `check_SetLEDsOnOff` — direct call sweep all 32 combinations of 5-bit input. For each, snapshot `GpioDataRegs` and assert exactly the bits matching `LEDvalue & 0x1F` are set on the corresponding LED GPIOs (per spec mapping).
- [ ] `check_ReadSwitches` — for each of 32 switch combos, set `GP?DAT` for GPIO157..160 (and the 5th switch — confirm GPIO from spec), call `ReadSwitches()`, assert returned int16_t equals the bitfield.
- [ ] `check_timer_rate_arbitrary` — verify the student's `ConfigCpuTimer` reflects the exercise's rate (test with each of 0.1 s, 0.25 s, 1 ms — if the reference has the final 1 ms, only that case is asserted).
- [ ] `check_print_cadence` — drive ISR at 1 ms for 1 s synthetic; assert ~10 prints (100 ms cadence) ±10%.
- [ ] `check_print_format` — `expect_arg_types(latest, {...})` for the documented variables (use `grader::ArgType::*` tags).
- [ ] `check_button_response` — wrap `grader::press_button(N)` / `grader::release_button(N)` for each switch; drive ISR; assert LED state mirrors switch state (since `SetLEDsOnOff(ReadSwitches())` is the typical idiom).

## Validation matrix (deep)

- [ ] Reference → all pass, zero warnings
- [ ] Mutation A: change LED mapping (swap bits) → `check_SetLEDsOnOff` fails on the swapped bit
- [ ] Mutation B: `ReadSwitches` uses branches → still passes (we test behavior, not implementation); if spec demands bitwise, add a string-grep heuristic on source
- [ ] Mutation C: drop a pullup config → `gpiosSetup[157].flags` mismatch
- [ ] Run twice → identical

## Skip — demo only

- LabVIEW GUI creation
- Raspberry Pi echo server / TCPIP socket verification
- Device Manager COM port discovery

## Open questions / [ambiguous] from spec

- [ ] 5th switch GPIO — spec lists 157–160 (4 pins). Confirm whether a 5th switch is on a different pin.
- [ ] LED-to-GPIO mapping for the 5 output LEDs — pin from reference.
- [ ] Print rate at the 1 ms timer (mod 10 = 100 ms) vs the 0.25 s timer (mod 1 = 250 ms) — multiple valid configurations; the test should match whichever the reference uses, or accept any of the spec'd rates.
