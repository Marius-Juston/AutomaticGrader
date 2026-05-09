#ifndef AUTOMATICGRADER_CHECKS_MAIN_LOOP_DRIVER_H
#define AUTOMATICGRADER_CHECKS_MAIN_LOOP_DRIVER_H

#include <cstdint>

#include "checks/synthetic_clock.h"

extern "C" int temp_main();

namespace grader {
    // Run student init once. Invokes temp_main(), which under AUTO_GRADER
    // is patched so its `while(1)` loop becomes a separate function: main
    // runs init code and returns. All peripheral globals (SerialA,
    // CpuTimer*, IER, PieCtrlRegs, ...) are populated. Idempotent.
    void run_student_init();

    // Force the next call to run_student_init() to actually re-run init.
    void reset_student_init() noexcept;

    // No-op stub retained for forward-compat; the new driver does not need
    // to set an iteration count on the student side.
    void set_main_loop_iterations(int n) noexcept;

    // Step the student's main-loop body N times. Each call invokes
    // `grader_main_body()` (the function the patched student source extracts
    // from the original while(1) body). No init re-run, no thread, no race.
    void step_main_loop(int iterations = 1);

    // Drive `total_ticks` ISR calls, interleaving one main-loop iteration
    // after each ISR. Synthetic clock advances by `period_us` per ISR.
    // Single-threaded, no real-time sleeps, no races with a detached thread.
    template <typename IsrFn>
    inline void drive_isr_with_main_pump(IsrFn&& isr,
                                         std::uint64_t period_us,
                                         std::uint64_t total_ticks) {
        for (std::uint64_t i = 0; i < total_ticks; ++i) {
            synthetic_clock_advance(period_us);
            isr();
            step_main_loop(1);
        }
    }
}

#endif
