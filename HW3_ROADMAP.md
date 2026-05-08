# HW3 Grader Roadmap

**Authoritative spec:** `context/homeworks/HW3.tex`
**Reference fixture (non-authoritative):** `context/code_solutions/myHW_Yingyord_First/HW3/`
**Implementation:** `src/checks/hw3.cpp` (to create), `include/checks/hw3.h` (to create)

## Spec inventory (HW3.tex)

| Topic               | Behavior                                                                                          |
| ------------------- | ------------------------------------------------------------------------------------------------- |
| Buzzer EPWM9A       | GPIO16 mux → EPWM9A; CLKDIV=1; `AQCTLA.ZRO=2`, `AQCTLA.CAU=1` (50% duty square wave); `TBPRD` driven by `songarray[]` from `song.h`. |
| Song playback       | `cpu_timer1_isr` (125 ms) advances `songarray[]` index; after 96 notes, mux GPIO16 back to GPIO and `GPACLEAR.bit.GPIO16 = 1`. |
| SPIB pinmux         | GPIO63 (SIMOB, mux 10), GPIO64 (SOMIB, mux 8), GPIO65 (CLKB, mux 9), GPIO66 (CS, GPIO output).    |
| SPIB config         | `SPICCR.SPICHAR == 15` (16-bit); `SPIBRR == 49` (1 MHz @ 50 MHz LSPCLK); `SPICTL.CLK_PHASE == 1`; `SPICCR.CLKPOLARITY == 0` (Mode 01); FIFO enabled. |
| MPU-9250 single read | `cpu_timer2_isr` (40 ms) writes `0x8000 \| (0x46 << 8)` then `0x0000` to `SpibRegs.SPITXBUF` (gyroZ_H read). |
| `SPIB_isr`          | Reads RX FIFO, accumulates `gyrozraw` (int16), scales `gyroz = gyrozraw * 250.0/32768.0` deg/s. |
| Full IMU init       | `setupSpib()` writes registers per spec: `CONFIG=0x02`, `ACCEL_CONFIG=0x08` (±4g), `GYRO_CONFIG=0x00` (±250 dps), accel offsets `XA_OFFS=-2679`, `YA_OFFS=2173`, `ZA_OFFS=4264`. |
| Print Ex. 5 single  | gyroZ raw int16 every 100 ms. Format: spec uses `%d` for `int16_t`; correct.                      |
| Print Ex. 5 final   | 6 floats (accel X/Y/Z g, gyro X/Y/Z deg/s) every 200 ms.                                          |
| Print format        | Floats with `%f` or `%.2f` (**[ambiguous]**); raw gyro `%d` (16-bit).                             |

## Stubs needed (`src/ti_stubs.cpp`)

- [ ] All HW2 EPWM stubs (reused for EPWM9 buzzer)
- [ ] `SPI_disableModule` / `SPI_enableModule` → `SpiXRegs.SPICCR.bit.SPISWRESET`
- [ ] `SPI_setConfig` → `SPICCR.SPICHAR`, `SPIBRR`, `SPICTL.{CLK_PHASE, MASTER_SLAVE, TALK, SPIINTENA}`
- [ ] `SPI_setEmulationMode` → `SPIPRI.bit.{SOFT, FREE}`
- [ ] `SPI_enableFIFO`, `SPI_disableFIFO` → `SPIFFTX.bit.SPIRST`/`SPIFFENA`
- [ ] `SPI_setFIFOInterruptLevel` → `SPIFFTX.TXFFIL`, `SPIFFRX.RXFFIL`
- [ ] `SPI_enableInterrupt` → `SPIFFRX.RXFFIENA`
- [ ] **Deferred (slice 3):** `spibTxLog[]` capture — extend SPI write paths to log the 16-bit words written to `SpibRegs.SPITXBUF`. Slice 1 ships the RX side (`grader::inject_spi_rx(SpiModule::B, word)` writes `SPIRXBUF` and bumps `SPIFFRX.RXFFST`); the TX-log capture is still TBD because struct-field writes can't be intercepted without a wrapper around `SPITXBUF`.
- [x] **Shared (shipped):** `serial_printf` / `UART_printfLine` capture → `g_printfCalls`. Auto-wired in `src/ti_stubs.cpp`; consume via `include/checks/printf_capture.h`.
- [x] **Shared (shipped):** stimulus helpers — `include/checks/stimulus.hpp` (`press_button`, `inject_adc_result`, `inject_spi_rx`, `inject_encoder_count`, `inject_lidar_*`).
- [x] **Shared (shipped):** synthetic clock + `run_isr_for_us` — `include/checks/synthetic_clock.h`.
- [x] **Shared (shipped):** format parser + `expect_format` / `expect_arg_types` / `expect_print_cadence` — `include/checks/format_parser.h` + `include/checks/expectations.h`. Run `./AutomaticGrader --selftest` to verify the infra after edits.

## Checks to implement (`src/checks/hw3.cpp`)

- [x] `check_initialization` — verifies:
  - GPIO16 mux=5 (EPWM9A), GPIO63/64/65 mux=15 (SPISIMOB/SPISOMIB/SPICLKB; lenient toward older 10/8/9 wording in legacy docs), GPIO66 = CS output.
  - CpuTimer1 = 125 ms, CpuTimer2 = 40 ms (T0 free).
  - `PieVectTable.SPIB_RX_INT == &SPIB_isr`.
  - `EPwm9Regs.AQCTLA.ZRO == 3` (toggle on zero), `TBCTL.CTRMODE == 0`.
  - `SpibRegs.{SPICCR.SPICHAR=15, SPIBRR.SPI_BIT_RATE=49, SPICTL.CLK_PHASE=1, SPICCR.CLKPOLARITY=0, SPICTL.MASTER_SLAVE=1, SPIFFTX.SPIFFENA=1}`.
  - Skipped registers: HW2 set + `SpibRegs`, `EPwm9Regs`, all `CpuTimer*` and `CpuTimer*Regs`, `PieVectTable`, `PieCtrlRegs`.
  - **Required workaround:** the reference's `setupSpib()` busy-waits on `SpibRegs.SPIFFRX.bit.RXFFST` (values 7, 4, 1, 1, …); without a real FIFO model the student thread spins forever. `hw3_unblock_setup_spib()` walks RXFFST through the expected sequence with 2 ms steps and a 5× retry per value to defeat the spin-wait. **This is a slice-3 hack** — when the FIFO shadow lands in slice 4, remove the watchdog.
- [ ] `check_song_playback` — **deferred**; the Yingyord reference has the entire song-playback block inside `cpu_timer1_isr` commented out. Re-enable when a spec-compliant fixture lands.
- [ ] `check_spib_request_format` — **DEFERRED (slice 4)**. Stub returns `1` with `[HW3] check_spib_request_format SKIPPED — SPI TX-side capture (spibTxLog[]) not yet implemented in slice 1`. Requires a TX-side word log that intercepts struct-field writes to `SpibRegs.SPITXBUF`.
- [x] `check_spib_isr_scaling` — three injection cases (`+half-scale`, `-half-scale`, `zero`) via `grader::inject_spi_rx(SpiModule::B, word)` then `SPIB_isr()`. Asserts `gyroz_raw == word` and `gyroz_val ≈ word * 250.0/32767.0` ±0.05. Snapshot/restore of `gyroz_raw`, `gyroz_val`, `SPIBCount`. **Limitation:** the existing single-field SPIRXBUF means all 8 reads inside `SPIB_isr` return the same value — only the LAST-read field (`gyroz_val`) is verified. Per-axis ordering checks land in slice 4.
- [ ] `check_imu_full_init` — **DEFERRED (slice 4)**. Same TX-log dependency as `check_spib_request_format`.
- [x] `check_print_cadence` — drives `SPIB_isr()` 1000× in 5 bursts of 200 (each burst should fire `UARTPrint=1` once); accepts `[4, 14]` printf calls (covers 1–3 prints per UARTPrint event — the reference issues two: Acc + Gyro).
- [x] `check_print_format` — accepts any of: `{Float×3}` (3-axis form), `{Float×6}` (combined 6-axis form), or `{Int16}` (Ex.5 single raw-gyro form). Tolerates the spec's ambiguity about which print is "latest."

## Validation matrix (deep)

- [x] Reference (`myHW_Yingyord_First/HW3/HW3fy_main.c`) → `Succeeded all tests!`, exit 0 (4 active checks pass + 2 SKIPPED markers for TX-log-dependent checks).
- [x] Negative test: `STUDENT_SRC=student_work/LABstarter_main.c` → all 4 active checks correctly fail or skip with explicit `missing student symbols` errors (`SPIB_isr`, `gyroz_raw`, `gyroz_val`).
- [ ] Mutation A: `SPIBRR` 49→24 (would be 2 MHz) → `check_initialization` fails on `SPIBRR != 49`
- [ ] Mutation B: flip `CLKPOLARITY` 0→1 → fails on `SPICCR.CLKPOLARITY != 0`
- [ ] Mutation C: read wrong gyro register (0x46 → 0x47) — **deferred** until `check_spib_request_format` ships in slice 4.
- [ ] Mutation D: drop the `mux back to GPIO` step at end of song — **deferred** (`check_song_playback` not active).
- [ ] Mutation E: `%d` instead of `%f` for accel/gyro print → `check_print_format` falls through all 3 acceptance modes and fails with the TI-C2000 16-bit `int` hint.
- [x] Cross-regression: HW1, HW2 still pass after HW3 changes (only added new files; sole shared-infra change was making `setupSpib` weak in `src/SE423Lib.c` so HW3 reference's strong override wins).

## Open questions / [ambiguous] from spec

- [ ] `%.2f` vs `%f` for IMU floats — spec ambiguous; accept any precision.
- [ ] Whether `setupSpib()` must actually write the IMU register init sequence or whether stubbing the SPI peripheral alone is sufficient. Spec implies the former; check Ex. 5 wording carefully.
- [ ] Buzzer pin — spec says GPIO16 mux 5 (EPWM9A); confirm against reference.
- [ ] Final 6-axis cadence — spec says "every 200 ms"; pin to integer ratio with `cpu_timer2_isr` (40 ms × 5).
