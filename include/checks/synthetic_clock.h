#ifndef AUTOMATICGRADER_CHECKS_SYNTHETIC_CLOCK_H
#define AUTOMATICGRADER_CHECKS_SYNTHETIC_CLOCK_H

#include <cstdint>
#include <utility>

namespace grader {
    [[nodiscard]] uint64_t synthetic_clock_now_us() noexcept;

    void synthetic_clock_reset() noexcept;

    void synthetic_clock_advance(uint64_t us) noexcept;

    template<typename Fn>
    inline void run_isr_for_us(Fn &&isr, uint64_t period_us, uint64_t total_us) noexcept(noexcept(isr())) {
        if (period_us == 0) {
            return;
        }
        const uint64_t calls = total_us / period_us;
        for (uint64_t i = 0; i < calls; ++i) {
            synthetic_clock_advance(period_us);
            std::forward<Fn>(isr)();
        }
    }

    template<typename Fn>
    inline void run_isr_calls(Fn &&isr, uint64_t period_us, uint64_t calls) noexcept(noexcept(isr())) {
        for (uint64_t i = 0; i < calls; ++i) {
            synthetic_clock_advance(period_us);
            std::forward<Fn>(isr)();
        }
    }
}

#endif