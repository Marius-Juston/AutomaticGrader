#include "checks/main_loop_driver.h"

namespace {
    int g_iters = 0;
    bool g_inited = false;
}

extern "C" int grader_main_loop_iterations(void) {
    return g_iters;
}

namespace grader {
    void set_main_loop_iterations(int n) noexcept {
        g_iters = n;
    }

    void reset_student_init() noexcept {
        g_inited = false;
    }

    void run_student_init() {
        if (g_inited) return;
        g_iters = 0;
        // First entry runs the student's init (the patched main contains a
        // GRADER_MAIN_INIT_GUARD that latches a static flag). With iters=0
        // the loop body executes zero times, main returns immediately.
        (void) temp_main();
        g_inited = true;
    }

    void step_main_loop(int iterations) {
        if (iterations <= 0) return;
        g_iters = iterations;
        // Re-entry: GRADER_MAIN_INIT_GUARD jumps past init code via goto, so
        // only the loop body executes. No re-init, no busy-waits, no thread.
        (void) temp_main();
        g_iters = 0;
    }
}
