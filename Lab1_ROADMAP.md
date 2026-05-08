# Lab1 Grader Roadmap

**Authoritative spec:** `context/lab/Lab1.txt`
**Reference fixture:** `context/code_solutions/SE423_Lab_phuriy2_lourdes3_emilyah3/Lab1/`
**Implementation:** `src/checks/lab1.cpp` (deferred), `include/checks/lab1.h` (deferred)

## Spec inventory (Lab1.txt)

| Topic                  | Behavior                                                                                                  |
| ---------------------- | --------------------------------------------------------------------------------------------------------- |
| GPIO outputs           | LEDs on GPIOs 22, 34, 94, 95, 97, 111 — `GPIO_SetupPinMux` + `GPIO_SetupPinOptions` (output, push-pull). |
| Timers                 | CpuTimer0 @ 10 ms, CpuTimer2 @ 40 ms.                                                                    |
| LED toggle/set/clear   | Via `GPIO*TOGGLE`, `GPIO*SET`, `GPIO*CLEAR` registers.                                                   |
| UART init              | `init_serialSCIA(&SerialA, 115200)`.                                                                     |
| Print cadence          | `serial_printf` modulus on `CpuTimer2.InterruptCount` — every ~400 ms (10 × 40 ms).                       |
| UART RX                | `RXAINT_recv_ready` parses input characters: 'a' or 'b' to toggle red LED (GPIO34).                       |
| Print format           | `UART_printfLine` with `%ld` (int32_t) and `%d` (int16_t) — verify both correct.                          |

## Stubs needed

- [ ] All HW1-style GPIO + timer stubs (already in place).
- [ ] **Shared:** `serial_printf` capture (with port detection) and `inject_serial_rx(SCIA, char)` to simulate keyboard input via `RXAINT_recv_ready` invocation.

## Checks to implement

- [ ] `check_initialization` — `gpiosSetup[]` for all 6 LED pins; `CpuTimer0.PeriodInUSec == 10000`, `CpuTimer2.PeriodInUSec == 40000`; `PieVectTable.{TIMER0_INT, TIMER2_INT, SCIA_RX_INT}` registered.
- [ ] `check_timer0` — drive ISR; assert `numTimer0calls` increments; LED activity per spec.
- [ ] `check_timer2` — drive ISR for 1 second synthetic; assert print count matches modulus (e.g. 25 prints if mod 1, 2-3 if mod 10).
- [ ] `check_uart_rx_echo` — `inject_serial_rx(SCIA, 'a')`, drive `RXAINT_recv_ready()`, assert `GpioDataRegs.GPBTOGGLE.bit.GPIO34 == 1` (red LED toggle). Same for 'b'. Inject other chars → no toggle.
- [ ] `check_print_cadence` — assert spec'd rate (~400 ms); ±10%.
- [ ] `check_print_format` — `expect_arg_types` matches `int32_t` (`%ld`) and `int16_t` (`%d`) variables.

## Validation matrix (deep)

- [ ] Reference (`SE423_Lab_phuriy2_lourdes3_emilyah3/Lab1/`) → all pass, zero warnings, exit 0
- [ ] Mutation A: timer period change → fails on `CpuTimer*.PeriodInUSec`
- [ ] Mutation B: drop UART init → fails on the SCI-A baseline check
- [ ] Mutation C: `%d` → `%ld` swap → `check_print_format` fails on arg type
- [ ] Mutation D: change RX echo from 'a' to 'A' → `check_uart_rx_echo` fails
- [ ] Run twice → identical

## Skip — demo only

- Oscilloscope verification of GPIO timing
- Manual LED visual inspection ("have your TA show you")

## Open questions / [ambiguous] from spec

- [ ] Exact print modulus — spec says "10th Timer2 call" → 400 ms. Confirm against reference.
- [ ] UART baud rate verification — `init_serialSCIA(&SerialA, 115200)` is captured but stubbed; spec says 115200. Verify with explicit assertion.
