#include "checks/synthetic_clock.h"

#include <atomic>

namespace grader {
    namespace {
        std::atomic<uint64_t> g_clock_us{0};
    }

    uint64_t synthetic_clock_now_us() noexcept {
        return g_clock_us.load(std::memory_order_relaxed);
    }

    void synthetic_clock_reset() noexcept {
        g_clock_us.store(0, std::memory_order_relaxed);
    }

    void synthetic_clock_advance(uint64_t us) noexcept {
        g_clock_us.fetch_add(us, std::memory_order_relaxed);
    }
}