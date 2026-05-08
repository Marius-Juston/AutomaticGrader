#ifndef AUTOMATICGRADER_CHECKS_STIMULUS_HPP
#define AUTOMATICGRADER_CHECKS_STIMULUS_HPP

#include <cstddef>
#include <cstdint>
#include <span>

namespace grader {
    void press_button(int gpio_num);

    void release_button(int gpio_num);

    [[nodiscard]] bool button_state(int gpio_num);

    void set_gpio_input(int gpio_num, bool high);

    [[nodiscard]] bool read_gpio_input(int gpio_num);

    enum class AdcModule : uint8_t { A, B, C, D };

    void inject_adc_result(AdcModule module, int soc_idx, uint16_t value);

    [[nodiscard]] uint16_t read_adc_result(AdcModule module, int soc_idx);

    void clear_adc_results(AdcModule module);

    enum class SpiModule : uint8_t { A, B, C };

    void inject_spi_rx(SpiModule module, uint16_t word);

    void inject_spi_rx(SpiModule module, std::span<const uint16_t> words);

    [[nodiscard]] std::size_t spi_rx_fifo_size(SpiModule module);

    void clear_spi_state(SpiModule module);

    void clear_spi_state();

    enum class EqepModule : uint8_t { Eqep1, Eqep2, Eqep3 };

    void inject_encoder_count(EqepModule module, int32_t count);

    [[nodiscard]] int32_t read_encoder_count(EqepModule module);

    constexpr int LIDAR_FRAME_LEN = 228;

    void inject_lidar_ping(std::span<const float, LIDAR_FRAME_LEN> distances);

    void inject_lidar_pong(std::span<const float, LIDAR_FRAME_LEN> distances);

    void inject_lidar_frame(std::span<const float, LIDAR_FRAME_LEN> distances);

    void clear_lidar_frame();

    void reset_all_stimulus();
}

#endif