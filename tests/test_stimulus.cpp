// GTest coverage for include/checks/stimulus.hpp.

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <limits>
#include <random>

#include "checks/stimulus.hpp"
#include "lidar.h"
#include "ti_stubs.h"

using namespace grader;

extern datapts ladar_data[228];

namespace {
    class StimulusTest : public ::testing::Test {
    protected:
        void TearDown() override { reset_all_stimulus(); }
    };

    // ---------- buttons ----------

    TEST_F(StimulusTest, PressButton_DrivesPinLow) {
        // Arrange
        GpioDataRegs.GPADAT.all = 0xFFFFFFFFu;
        // Act
        press_button(4);
        // Assert
        EXPECT_FALSE(button_state(4));
    }

    TEST_F(StimulusTest, ReleaseButton_DrivesPinHigh) {
        // Arrange
        GpioDataRegs.GPADAT.all = 0;
        // Act
        release_button(4);
        // Assert
        EXPECT_TRUE(button_state(4));
    }

    TEST_F(StimulusTest, PressButton_DoesNotAffectOtherPins) {
        // Arrange: equivalence partition — independence of pin manipulations.
        GpioDataRegs.GPADAT.all = 0;
        release_button(7);
        // Act
        press_button(4);
        // Assert
        EXPECT_FALSE(button_state(4));
        EXPECT_TRUE(button_state(7));
    }

    TEST_F(StimulusTest, SetGpioInput_HighThenLow_RoundTrips) {
        // Arrange
        // (clean state via TearDown of prior test)
        // Act
        set_gpio_input(34, true);
        const bool high = read_gpio_input(34);
        set_gpio_input(34, false);
        const bool low = read_gpio_input(34);
        // Assert (round-trip invariant — single logical act)
        EXPECT_TRUE(high);
        EXPECT_FALSE(low);
    }

    TEST_F(StimulusTest, SetGpioInput_BankBBoundary) {
        // Arrange: GPIO 32 is first pin of bank B.
        // Act
        set_gpio_input(32, true);
        // Assert
        EXPECT_TRUE(read_gpio_input(32));
    }

    TEST_F(StimulusTest, SetGpioInput_GpioZero_BankALowBoundary) {
        // Arrange: GPIO 0 — bank A low boundary.
        // Act
        set_gpio_input(0, true);
        // Assert
        EXPECT_TRUE(read_gpio_input(0));
    }

    TEST_F(StimulusTest, SetGpioInput_Gpio31_BankAHighBoundary) {
        // Arrange: GPIO 31 — last pin of bank A.
        // Act
        set_gpio_input(31, true);
        // Assert
        EXPECT_TRUE(read_gpio_input(31));
    }

    TEST_F(StimulusTest, SetGpioInput_Gpio63_BankBHighBoundary) {
        // Arrange: GPIO 63 — last pin of bank B.
        // Act
        set_gpio_input(63, true);
        // Assert
        EXPECT_TRUE(read_gpio_input(63));
    }

    // ---------- ADC ----------

    TEST_F(StimulusTest, InjectAdcResult_RoundTrip_AllModules) {
        // Arrange
        // Act
        for (auto mod: {AdcModule::A, AdcModule::B, AdcModule::C, AdcModule::D}) {
            clear_adc_results(mod);
            inject_adc_result(mod, 0, 4095);
        }
        // Assert
        for (auto mod: {AdcModule::A, AdcModule::B, AdcModule::C, AdcModule::D}) {
            EXPECT_EQ(read_adc_result(mod, 0), 4095);
        }
    }

    TEST_F(StimulusTest, InjectAdcResult_ZeroBoundary) {
        // Arrange
        // Act
        inject_adc_result(AdcModule::A, 0, 0);
        // Assert
        EXPECT_EQ(read_adc_result(AdcModule::A, 0), 0);
    }

    TEST_F(StimulusTest, InjectAdcResult_TwelveBitMaxBoundary) {
        // Arrange
        // Act
        inject_adc_result(AdcModule::A, 0, 4095); // 12-bit max
        // Assert
        EXPECT_EQ(read_adc_result(AdcModule::A, 0), 4095);
    }

    TEST_F(StimulusTest, InjectAdcResult_OutOfRangeSoc_DoesNotCrashAndDoesNotLeak) {
        // Arrange
        clear_adc_results(AdcModule::A);
        // Act: invalid soc indices must be ignored, not UB.
        inject_adc_result(AdcModule::A, 99, 1);
        inject_adc_result(AdcModule::A, -1, 1);
        // Assert: no in-range slot should have been written.
        for (int soc = 0; soc < 16; ++soc) {
            EXPECT_EQ(read_adc_result(AdcModule::A, soc), 0) << "soc=" << soc;
        }
    }

    TEST_F(StimulusTest, ClearAdcResults_ZeroesAllSocs) {
        // Arrange
        inject_adc_result(AdcModule::A, 3, 1234);
        // Act
        clear_adc_results(AdcModule::A);
        // Assert
        EXPECT_EQ(read_adc_result(AdcModule::A, 3), 0);
    }

    TEST_F(StimulusTest, InjectAdcResult_CrossModuleIsolation) {
        // Arrange: clean all four modules.
        for (auto m: {AdcModule::A, AdcModule::B, AdcModule::C, AdcModule::D}) {
            clear_adc_results(m);
        }
        // Act: inject only into module A.
        inject_adc_result(AdcModule::A, 5, 0xABC);
        // Assert: B/C/D remain untouched.
        EXPECT_EQ(read_adc_result(AdcModule::A, 5), 0xABC);
        EXPECT_EQ(read_adc_result(AdcModule::B, 5), 0);
        EXPECT_EQ(read_adc_result(AdcModule::C, 5), 0);
        EXPECT_EQ(read_adc_result(AdcModule::D, 5), 0);
    }

    TEST_F(StimulusTest, InjectAdcResult_RandomFuzz_RoundTripInvariant) {
        // Arrange: deterministic seed, round-trip property.
        std::mt19937 rng(0xC0FFEE);
        for (auto m: {AdcModule::A, AdcModule::B, AdcModule::C, AdcModule::D}) {
            clear_adc_results(m);
        }
        // Act + Assert: inject 200 random tuples, confirm readback matches.
        constexpr AdcModule mods[] = {AdcModule::A, AdcModule::B, AdcModule::C, AdcModule::D};
        for (int i = 0; i < 200; ++i) {
            const auto mod = mods[rng() % 4];
            const int soc = static_cast<int>(rng() % 16);
            const auto value = static_cast<uint16_t>(rng() & 0x0FFFu); // 12-bit ADC
            inject_adc_result(mod, soc, value);
            EXPECT_EQ(read_adc_result(mod, soc), value)
                    << "iter=" << i << " soc=" << soc;
        }
    }

    // ---------- SPI ----------

    TEST_F(StimulusTest, InjectSpiRx_SingleWord_ReflectsInBuf) {
        // Arrange
        clear_spi_state(SpiModule::B);
        // Act
        inject_spi_rx(SpiModule::B, uint16_t{0xBEEF});
        // Assert
        EXPECT_EQ(SpibRegs.SPIRXBUF, 0xBEEFu);
        EXPECT_EQ(spi_rx_fifo_size(SpiModule::B), 1u);
    }

    TEST_F(StimulusTest, InjectSpiRx_SpanOfWords_LeavesLatestInBuf) {
        // Arrange
        clear_spi_state(SpiModule::B);
        constexpr uint16_t words[] = {0x1, 0x2, 0x3};
        // Act
        inject_spi_rx(SpiModule::B, std::span<const uint16_t>(words, 3));
        // Assert
        EXPECT_EQ(SpibRegs.SPIRXBUF, 0x3u);
        EXPECT_EQ(spi_rx_fifo_size(SpiModule::B), 3u);
    }

    TEST_F(StimulusTest, ClearSpiState_PerModule_ResetsFifoSize) {
        // Arrange
        inject_spi_rx(SpiModule::B, uint16_t{0x1});
        // Act
        clear_spi_state(SpiModule::B);
        // Assert
        EXPECT_EQ(spi_rx_fifo_size(SpiModule::B), 0u);
    }

    TEST_F(StimulusTest, ClearSpiState_All_ResetsAllModules) {
        // Arrange
        inject_spi_rx(SpiModule::A, uint16_t{0x1});
        inject_spi_rx(SpiModule::B, uint16_t{0x2});
        // Act
        clear_spi_state();
        // Assert
        EXPECT_EQ(spi_rx_fifo_size(SpiModule::A), 0u);
        EXPECT_EQ(spi_rx_fifo_size(SpiModule::B), 0u);
    }

    TEST_F(StimulusTest, InjectSpiRx_CrossModuleIsolation) {
        // Arrange: clean A, B, C.
        clear_spi_state();
        // Act: inject only into A.
        inject_spi_rx(SpiModule::A, uint16_t{0xAAAA});
        // Assert: B and C remain empty.
        EXPECT_EQ(spi_rx_fifo_size(SpiModule::A), 1u);
        EXPECT_EQ(spi_rx_fifo_size(SpiModule::B), 0u);
        EXPECT_EQ(spi_rx_fifo_size(SpiModule::C), 0u);
    }

    TEST_F(StimulusTest, InjectSpiRx_AtCapacity_RXFFSTEqualsCapacity) {
        // Arrange: SPI FIFO capacity is 16 (per src/checks/stimulus.cpp).
        clear_spi_state(SpiModule::B);
        std::array<uint16_t, 16> words{};
        for (size_t i = 0; i < words.size(); ++i) words[i] = static_cast<uint16_t>(i + 1);
        // Act
        inject_spi_rx(SpiModule::B, std::span<const uint16_t>(words.data(), words.size()));
        // Assert
        EXPECT_EQ(spi_rx_fifo_size(SpiModule::B), 16u);
    }

    // ---------- encoders ----------

    TEST_F(StimulusTest, InjectEncoderCount_RoundTrip) {
        // Arrange
        // Act
        inject_encoder_count(EqepModule::Eqep1, 12345);
        // Assert
        EXPECT_EQ(read_encoder_count(EqepModule::Eqep1), 12345);
    }

    TEST_F(StimulusTest, InjectEncoderCount_NegativeValue_RoundTrip) {
        // Arrange: signed wraparound boundary.
        // Act
        inject_encoder_count(EqepModule::Eqep2, -1);
        // Assert
        EXPECT_EQ(read_encoder_count(EqepModule::Eqep2), -1);
    }

    TEST_F(StimulusTest, InjectEncoderCount_Int32Max_RoundTrip) {
        // Arrange: BVA — INT32_MAX.
        // Act
        inject_encoder_count(EqepModule::Eqep1, std::numeric_limits<int32_t>::max());
        // Assert
        EXPECT_EQ(read_encoder_count(EqepModule::Eqep1), std::numeric_limits<int32_t>::max());
    }

    TEST_F(StimulusTest, InjectEncoderCount_Int32Min_RoundTrip) {
        // Arrange: BVA — INT32_MIN.
        // Act
        inject_encoder_count(EqepModule::Eqep3, std::numeric_limits<int32_t>::min());
        // Assert
        EXPECT_EQ(read_encoder_count(EqepModule::Eqep3), std::numeric_limits<int32_t>::min());
    }

    TEST_F(StimulusTest, InjectEncoderCount_AllModulesIndependent) {
        // Arrange
        // Act
        inject_encoder_count(EqepModule::Eqep1, 10);
        inject_encoder_count(EqepModule::Eqep2, 20);
        inject_encoder_count(EqepModule::Eqep3, 30);
        // Assert
        EXPECT_EQ(read_encoder_count(EqepModule::Eqep1), 10);
        EXPECT_EQ(read_encoder_count(EqepModule::Eqep2), 20);
        EXPECT_EQ(read_encoder_count(EqepModule::Eqep3), 30);
    }

    // ---------- lidar ----------

    TEST_F(StimulusTest, InjectLidarPing_PopulatesPingBuffer) {
        // Arrange
        clear_lidar_frame();
        std::array<float, LIDAR_FRAME_LEN> dist{};
        for (size_t i = 0; i < dist.size(); ++i) dist[i] = static_cast<float>(i) * 0.5f;
        // Act
        inject_lidar_ping(std::span<const float, LIDAR_FRAME_LEN>(dist));
        // Assert: ping populated, pong untouched.
        for (size_t i = 0; i < dist.size(); ++i) {
            EXPECT_FLOAT_EQ(ladar_data[i].distance_ping, static_cast<float>(i) * 0.5f);
            EXPECT_FLOAT_EQ(ladar_data[i].distance_pong, 0.0f);
        }
    }

    TEST_F(StimulusTest, InjectLidarPong_PopulatesPongBuffer) {
        // Arrange
        clear_lidar_frame();
        std::array<float, LIDAR_FRAME_LEN> dist{};
        for (size_t i = 0; i < dist.size(); ++i) dist[i] = static_cast<float>(i) + 1.0f;
        // Act
        inject_lidar_pong(std::span<const float, LIDAR_FRAME_LEN>(dist));
        // Assert: pong populated, ping untouched.
        for (size_t i = 0; i < dist.size(); ++i) {
            EXPECT_FLOAT_EQ(ladar_data[i].distance_pong, static_cast<float>(i) + 1.0f);
            EXPECT_FLOAT_EQ(ladar_data[i].distance_ping, 0.0f);
        }
    }

    TEST_F(StimulusTest, InjectLidarFrame_PopulatesBothBuffers) {
        // Arrange
        clear_lidar_frame();
        std::array<float, LIDAR_FRAME_LEN> dist{};
        for (size_t i = 0; i < dist.size(); ++i) dist[i] = static_cast<float>(i) * 0.25f;
        // Act
        inject_lidar_frame(std::span<const float, LIDAR_FRAME_LEN>(dist));
        // Assert: both ping and pong populated identically.
        for (size_t i = 0; i < dist.size(); ++i) {
            EXPECT_FLOAT_EQ(ladar_data[i].distance_ping, static_cast<float>(i) * 0.25f);
            EXPECT_FLOAT_EQ(ladar_data[i].distance_pong, static_cast<float>(i) * 0.25f);
        }
    }

    TEST_F(StimulusTest, ClearLidarFrame_ZeroesBothBuffers) {
        // Arrange: populate first.
        std::array<float, LIDAR_FRAME_LEN> dist{};
        for (size_t i = 0; i < dist.size(); ++i) dist[i] = 7.5f;
        inject_lidar_frame(std::span<const float, LIDAR_FRAME_LEN>(dist));
        // Act
        clear_lidar_frame();
        // Assert
        for (size_t i = 0; i < dist.size(); ++i) {
            EXPECT_FLOAT_EQ(ladar_data[i].distance_ping, 0.0f);
            EXPECT_FLOAT_EQ(ladar_data[i].distance_pong, 0.0f);
        }
    }

    TEST_F(StimulusTest, ResetAllStimulus_ClearsLidarAndSpiAndAdc) {
        // Arrange
        press_button(4);
        inject_adc_result(AdcModule::A, 0, 1234);
        inject_spi_rx(SpiModule::B, uint16_t{0xABCD});
        inject_encoder_count(EqepModule::Eqep1, 7);
        std::array<float, LIDAR_FRAME_LEN> dist{};
        for (auto &x: dist) x = 3.14f;
        inject_lidar_frame(std::span<const float, LIDAR_FRAME_LEN>(dist));
        // Act
        reset_all_stimulus();
        // Assert
        EXPECT_EQ(read_adc_result(AdcModule::A, 0), 0);
        EXPECT_EQ(spi_rx_fifo_size(SpiModule::B), 0u);
        EXPECT_FLOAT_EQ(ladar_data[0].distance_ping, 0.0f);
        EXPECT_FLOAT_EQ(ladar_data[227].distance_pong, 0.0f);
    }
} // namespace