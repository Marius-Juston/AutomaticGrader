# Using stimulus

Stimulus helpers drive the peripherals the student firmware is supposed
to react to. They live in
[`include/checks/stimulus.hpp`](https://github.com/Marius-Juston/AutomaticGrader/blob/master/include/checks/stimulus.hpp)
and are introduced in
[Stimulus injection](../architecture/stimulus-injection.md) — this
page is the cookbook.

## Recipe: button → LED toggle

Most HW1 / Lab1-style checks have the form "when PB{n} is held, LED{k}
toggles." Use this template:

```cpp
const Hw1Phase3Snapshot baseline = take_snapshot();

GpioDataRegs = baseline.data;
CpuTimer2.InterruptCount = 0;
clear_all_toggle_regs();

// 1. Primer — release every relevant button and fire one ISR, so any
//    edge-detector student code latches `prev = 1`.
grader::release_button(4);
grader::release_button(7);
cpu_timer2_isr();
clear_all_toggle_regs();

// 2. Stimulus — drive the button line low for the window.
grader::press_button(4);

// 3. Drive — invoke the ISR enough times to span the spec'd window.
const size_t ticks = 100000u / CpuTimer2.PeriodInUSec + 2;
for (size_t i = 0; i < ticks; ++i) cpu_timer2_isr();

// 4. Assert — the toggle bit must be set.
success &= report(toggle_bit_set(GpioDataRegs.GPBTOGGLE.all, 61 - 32),
                  "PB1: LED12 (GPIO61) didn't toggle",
                  "spec Ex.9 quote");

restore_snapshot(baseline);
```

The primer is **the** non-obvious step. Without it, an edge-detector
student implementation never sees an edge (`prev` starts at 0, button
goes 0 → 0 = no edge).

## Recipe: ADC scaling

Many HW2 / Lab2 checks have the form "when the ADC reads X, the
student-computed voltage should be Y." Use this template:

```cpp
const HwNPhase3Snapshot baseline = take_snapshot();

// 1. Inject the raw ADC result.
grader::inject_adc_result(grader::AdcModule::A, /*soc_idx=*/0, /*value=*/4095);

// 2. Drive the ISR that consumes the result.
grader::run_isr_for_us(adca1_isr, period_us, /*total_us=*/1000);

// 3. Assert the post-scaling value the student computed.
HardwareStateValidator v;
v.register_custom_copy("computed_voltage", student_voltage_global, /*expected=*/3.30f);
success &= v.validate();

restore_snapshot(baseline);
```

`AdcModule::{A,B,C,D}` matches the four ADC peripherals on the C2000.
`soc_idx` is the SOC slot the student configured.

## Recipe: SPI RX (MPU-9250 etc.)

```cpp
// Push a sequence of words into the SPI module's RX FIFO.
const uint16_t mpu_response[] = {0x0042, 0xFF00, 0x1234, 0x5678};
grader::inject_spi_rx(grader::SpiModule::B, mpu_response);

// Drive the ISR / poll cycle that drains the FIFO.
grader::run_isr_for_us(spib_rx_isr, period_us, /*total_us=*/200);

// Assert.
success &= ...
```

`spi_rx_fifo_size(module)` tells you how many words are still queued —
useful when asserting that the student consumed exactly the spec'd
number.

## Recipe: encoder velocity

```cpp
// Encoder reads as 1500 → 2000 over one isr period.
grader::inject_encoder_count(grader::EqepModule::Eqep1, 1500);
eqep1_velocity_isr();
grader::inject_encoder_count(grader::EqepModule::Eqep1, 2000);
eqep1_velocity_isr();

// Assert the student-computed velocity matches the spec'd value.
```

## Recipe: LIDAR frame

`Lab6` consumes 228-element float arrays. Build them with `std::array`:

```cpp
std::array<float, grader::LIDAR_FRAME_LEN> frame{};
for (size_t i = 0; i < frame.size(); ++i) {
    frame[i] = 2.0f + 0.01f * static_cast<float>(i);
}
grader::inject_lidar_frame(frame);

// Drive the consumer.
grader::run_isr_for_us(lidar_isr, period_us, total_us);

// Assert the student's parsed result.
```

`inject_lidar_ping` / `inject_lidar_pong` write only one of the two
double-buffer halves; `_frame` writes both, which is what you want
unless you're specifically testing the ping/pong swap logic.

## Recipe: between-check teardown

If your check leaves the system in a non-default state and you don't
want to author every snapshot field manually, call
`grader::reset_all_stimulus()` at the very end of the check. This
clears every stimulus-side register; it does **not** restore the
register state the student's init code set (use the snapshot/restore
pattern for that).

## Reference

The full API list is at
[`stimulus.hpp`](../architecture/stimulus-injection.md#api-surface).
When a helper you need doesn't exist yet:

1. Check the [deferred list](../architecture/stimulus-injection.md#deferred-stimulus-slice-1-scope)
   — your stimulus may already be on the roadmap.
2. If not, file an issue + open a PR that adds the helper next to the
   existing pattern (read register, write field, return). Stimulus
   helpers should never log; their side effect is the register write.
