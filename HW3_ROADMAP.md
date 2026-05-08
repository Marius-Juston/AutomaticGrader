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

- [ ] `check_initialization` — HW1 baseline + HW3 SPI/EPWM9 register state. Verify GPIO63/64/65/66 muxes; `SpibRegs.{SPICCR.SPICHAR, SPIBRR, SPICTL.CLK_PHASE, SPICCR.CLKPOLARITY, SPIPRI, SPIFFTX, SPIFFRX}`; EPWM9 AQCTLA. `PieVectTable.SPIB_RX_INT == &SPIB_isr`. CpuTimer1=125000 µs, CpuTimer2=40000 µs.
- [ ] `check_song_playback` — `resetPrintfCapture()`, snapshot `EPwm9Regs`, drive `cpu_timer1_isr()` 96×; after each tick assert `EPwm9Regs.TBPRD == songarray[i]`. After tick 97, assert `GpioCtrlRegs.GPAGMUX2.bit.GPIO16 == 0` and `GPACLEAR.bit.GPIO16 == 1`.
- [ ] `check_spib_request_format` — clear `spibTxLog`. Drive `cpu_timer2_isr()` once. Assert `spibTxLog` contains exactly `{0x8000 | (0x46 << 8), 0x0000}` in that order.
- [ ] `check_spib_isr_scaling` — `grader::inject_spi_rx(grader::SpiModule::B, 0x4000)` (=16384, half-scale positive); call `SPIB_isr()`. Assert `gyrozraw == 16384`; `gyroz ≈ 125.0` deg/s ±1e-3. Test negative: `inject_spi_rx(SpiModule::B, 0xC000)` (=-16384) → `gyroz ≈ -125.0`.
- [ ] `check_imu_full_init` — clear `spibTxLog`, run `setupSpib()` (or wait for it during main-thread init). Assert the spec'd init sequence appears in `spibTxLog`: writes to CONFIG=0x02, ACCEL_CONFIG=0x08, GYRO_CONFIG=0x00, XA_OFFSH/L=-2679, YA_OFFSH/L=2173, ZA_OFFSH/L=4264.
- [ ] `check_print_cadence_ex5_single` — drive ISR for 1000 ms synthetic; assert 10 ± 1 prints (100 ms cadence).
- [ ] `check_print_cadence_ex5_final` — drive ISR for 1000 ms synthetic; assert 5 ± 1 prints (200 ms cadence).
- [ ] `check_print_format_ex5_single` — `expect_arg_types(latest, {grader::ArgType::Int16})` with format containing `%d` (not `%ld`).
- [ ] `check_print_format_ex5_final` — `expect_arg_types(latest, {grader::ArgType::Float, grader::ArgType::Float, grader::ArgType::Float, grader::ArgType::Float, grader::ArgType::Float, grader::ArgType::Float})`.

## Validation matrix (deep)

- [ ] Reference (`myHW_Yingyord_First/HW3/`) → all pass, zero warnings, exit 0
- [ ] Mutation A: `SPIBRR` 49→24 (would be 2 MHz) → fails on `SpibRegs SPIBRR`
- [ ] Mutation B: flip `CLKPOLARITY` 0→1 → fails on `SpibRegs SPICCR.CLKPOLARITY`
- [ ] Mutation C: read wrong gyro register (0x46 → 0x47) → `check_spib_request_format` fails on first word
- [ ] Mutation D: drop the `mux back to GPIO` step at end of song → `check_song_playback` fails on `GPAGMUX2`
- [ ] Mutation E: `%d` instead of `%f` for accel/gyro print → `check_print_format_ex5_final` fails
- [ ] Run twice → identical
- [ ] Cross-regression: HW1, HW2 still pass

## Open questions / [ambiguous] from spec

- [ ] `%.2f` vs `%f` for IMU floats — spec ambiguous; accept any precision.
- [ ] Whether `setupSpib()` must actually write the IMU register init sequence or whether stubbing the SPI peripheral alone is sufficient. Spec implies the former; check Ex. 5 wording carefully.
- [ ] Buzzer pin — spec says GPIO16 mux 5 (EPWM9A); confirm against reference.
- [ ] Final 6-axis cadence — spec says "every 200 ms"; pin to integer ratio with `cpu_timer2_isr` (40 ms × 5).
