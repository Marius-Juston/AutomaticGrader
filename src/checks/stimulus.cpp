#include "checks/stimulus.hpp"

#include <algorithm>
#include <cstring>

#include <spdlog/spdlog.h>

#include "ti_stubs.h"
#include "lidar.h"

extern "C" {
extern datapts ladar_data[228];
}

namespace grader {

namespace {

constexpr int kMaxGpio = 168;

struct GpioBank {
    int low;
    int high;
    volatile uint32_t* dat;
};

GpioBank gpio_bank_for(int gpio) {
    if (gpio >= 0 && gpio <= 31) {
        return {0, 31, &GpioDataRegs.GPADAT.all};
    }
    if (gpio >= 32 && gpio <= 63) {
        return {32, 63, &GpioDataRegs.GPBDAT.all};
    }
    if (gpio >= 64 && gpio <= 95) {
        return {64, 95, &GpioDataRegs.GPCDAT.all};
    }
    if (gpio >= 96 && gpio <= 127) {
        return {96, 127, &GpioDataRegs.GPDDAT.all};
    }
    if (gpio >= 128 && gpio <= 159) {
        return {128, 159, &GpioDataRegs.GPEDAT.all};
    }
    if (gpio >= 160 && gpio <= kMaxGpio) {
        return {160, kMaxGpio, &GpioDataRegs.GPFDAT.all};
    }
    return {-1, -1, nullptr};
}

bool write_gpio_dat_bit(int gpio, bool high) {
    const GpioBank bank = gpio_bank_for(gpio);
    if (!bank.dat) {
        spdlog::error("stimulus: GPIO {} is out of range [0,{}]", gpio, kMaxGpio);
        return false;
    }
    const uint32_t mask = uint32_t{1} << (gpio - bank.low);
    if (high) {
        *bank.dat |= mask;
    } else {
        *bank.dat &= ~mask;
    }
    return true;
}

bool read_gpio_dat_bit(int gpio) {
    const GpioBank bank = gpio_bank_for(gpio);
    if (!bank.dat) {
        spdlog::error("stimulus: GPIO {} is out of range [0,{}]", gpio, kMaxGpio);
        return false;
    }
    const uint32_t mask = uint32_t{1} << (gpio - bank.low);
    return (*bank.dat & mask) != 0;
}

volatile ADC_RESULT_REGS* adc_result_regs(AdcModule m) {
    switch (m) {
        case AdcModule::A: return &AdcaResultRegs;
        case AdcModule::B: return &AdcbResultRegs;
        case AdcModule::C: return &AdccResultRegs;
        case AdcModule::D: return &AdcdResultRegs;
    }
    return nullptr;
}

const char* adc_module_name(AdcModule m) noexcept {
    switch (m) {
        case AdcModule::A: return "ADCA";
        case AdcModule::B: return "ADCB";
        case AdcModule::C: return "ADCC";
        case AdcModule::D: return "ADCD";
    }
    return "?";
}

volatile uint16_t* adc_result_slot(volatile ADC_RESULT_REGS* regs, int soc_idx) {
    if (!regs || soc_idx < 0 || soc_idx > 15) return nullptr;
    return &(&regs->ADCRESULT0)[soc_idx];
}

volatile SPI_REGS* spi_regs(SpiModule m) {
    switch (m) {
        case SpiModule::A: return &SpiaRegs;
        case SpiModule::B: return &SpibRegs;
        case SpiModule::C: return &SpicRegs;
    }
    return nullptr;
}

const char* spi_module_name(SpiModule m) noexcept {
    switch (m) {
        case SpiModule::A: return "SPIA";
        case SpiModule::B: return "SPIB";
        case SpiModule::C: return "SPIC";
    }
    return "?";
}

volatile EQEP_REGS* eqep_regs(EqepModule m) {
    switch (m) {
        case EqepModule::Eqep1: return &EQep1Regs;
        case EqepModule::Eqep2: return &EQep2Regs;
        case EqepModule::Eqep3: return &EQep3Regs;
    }
    return nullptr;
}

const char* eqep_module_name(EqepModule m) noexcept {
    switch (m) {
        case EqepModule::Eqep1: return "EQep1";
        case EqepModule::Eqep2: return "EQep2";
        case EqepModule::Eqep3: return "EQep3";
    }
    return "?";
}

}

void press_button(int gpio_num) {
    spdlog::debug("stimulus: press_button(GPIO{})", gpio_num);
    write_gpio_dat_bit(gpio_num, false);
}

void release_button(int gpio_num) {
    spdlog::debug("stimulus: release_button(GPIO{})", gpio_num);
    write_gpio_dat_bit(gpio_num, true);
}

bool button_state(int gpio_num) {
    return read_gpio_dat_bit(gpio_num);
}

void set_gpio_input(int gpio_num, bool high) {
    spdlog::debug("stimulus: set_gpio_input(GPIO{}, {})", gpio_num, high ? "HIGH" : "LOW");
    write_gpio_dat_bit(gpio_num, high);
}

bool read_gpio_input(int gpio_num) {
    return read_gpio_dat_bit(gpio_num);
}

void inject_adc_result(AdcModule module, int soc_idx, uint16_t value) {
    auto* regs = adc_result_regs(module);
    auto* slot = adc_result_slot(regs, soc_idx);
    if (!slot) {
        spdlog::error("stimulus: inject_adc_result({}, soc={}): SOC out of range [0,15]",
                      adc_module_name(module), soc_idx);
        return;
    }
    *slot = value;
    spdlog::debug("stimulus: {}.ADCRESULT{} = 0x{:04x} ({})",
                  adc_module_name(module), soc_idx, value, value);
}

uint16_t read_adc_result(AdcModule module, int soc_idx) {
    auto* regs = adc_result_regs(module);
    auto* slot = adc_result_slot(regs, soc_idx);
    if (!slot) return 0;
    return *slot;
}

void clear_adc_results(AdcModule module) {
    auto* regs = adc_result_regs(module);
    if (!regs) return;
    for (int i = 0; i <= 15; ++i) {
        if (auto* slot = adc_result_slot(regs, i)) {
            *slot = 0;
        }
    }
}

void inject_spi_rx(SpiModule module, uint16_t word) {
    auto* regs = spi_regs(module);
    if (!regs) return;
    regs->SPIRXBUF = word;
    if (regs->SPIFFRX.bit.RXFFST < 16) {
        regs->SPIFFRX.bit.RXFFST = static_cast<uint16_t>(regs->SPIFFRX.bit.RXFFST + 1);
    }
    spdlog::debug("stimulus: {}.SPIRXBUF = 0x{:04x}", spi_module_name(module), word);
}

void inject_spi_rx(SpiModule module, std::span<const uint16_t> words) {
    if (words.empty()) return;
    auto* regs = spi_regs(module);
    if (!regs) return;
    regs->SPIRXBUF = words.back();
    const std::size_t depth = std::min<std::size_t>(words.size(), 16);
    regs->SPIFFRX.bit.RXFFST = static_cast<uint16_t>(depth);
    spdlog::debug("stimulus: {}: pushed {} words; SPIRXBUF=0x{:04x}",
                  spi_module_name(module), words.size(), words.back());
}

std::size_t spi_rx_fifo_size(SpiModule module) {
    auto* regs = spi_regs(module);
    if (!regs) return 0;
    return regs->SPIFFRX.bit.RXFFST;
}

void clear_spi_state(SpiModule module) {
    auto* regs = spi_regs(module);
    if (!regs) return;
    regs->SPIRXBUF = 0;
    regs->SPITXBUF = 0;
    regs->SPIFFRX.bit.RXFFST = 0;
    regs->SPIFFTX.bit.TXFFST = 0;
}

void clear_spi_state() {
    clear_spi_state(SpiModule::A);
    clear_spi_state(SpiModule::B);
    clear_spi_state(SpiModule::C);
}

void inject_encoder_count(EqepModule module, int32_t count) {
    auto* regs = eqep_regs(module);
    if (!regs) return;
    regs->QPOSCNT = static_cast<uint32_t>(count);
    spdlog::debug("stimulus: {}.QPOSCNT = {}", eqep_module_name(module), count);
}

int32_t read_encoder_count(EqepModule module) {
    auto* regs = eqep_regs(module);
    if (!regs) return 0;
    return static_cast<int32_t>(regs->QPOSCNT);
}

void inject_lidar_ping(std::span<const float, LIDAR_FRAME_LEN> distances) {
    for (int i = 0; i < LIDAR_FRAME_LEN; ++i) {
        ladar_data[i].distance_ping = distances[i];
    }
    spdlog::debug("stimulus: lidar ping frame loaded ({} samples)", LIDAR_FRAME_LEN);
}

void inject_lidar_pong(std::span<const float, LIDAR_FRAME_LEN> distances) {
    for (int i = 0; i < LIDAR_FRAME_LEN; ++i) {
        ladar_data[i].distance_pong = distances[i];
    }
    spdlog::debug("stimulus: lidar pong frame loaded ({} samples)", LIDAR_FRAME_LEN);
}

void inject_lidar_frame(std::span<const float, LIDAR_FRAME_LEN> distances) {
    inject_lidar_ping(distances);
    inject_lidar_pong(distances);
}

void clear_lidar_frame() {
    for (int i = 0; i < LIDAR_FRAME_LEN; ++i) {
        ladar_data[i].distance_ping = 0.0f;
        ladar_data[i].distance_pong = 0.0f;
    }
}

void reset_all_stimulus() {
    clear_adc_results(AdcModule::A);
    clear_adc_results(AdcModule::B);
    clear_adc_results(AdcModule::C);
    clear_adc_results(AdcModule::D);
    clear_spi_state();
    clear_lidar_frame();
}

}
