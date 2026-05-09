// GTest coverage for include/checks/stimulus.hpp.

#include <gtest/gtest.h>

#include <array>

#include "checks/stimulus.hpp"
#include "ti_stubs.h"

using namespace grader;

namespace {
    class StimulusTest : public ::testing::Test {
    protected:
        void TearDown() override { reset_all_stimulus(); }
    };

    // ---------- buttons ----------

    TEST_F(StimulusTest, PressButton_DrivesPinLow) {
        GpioDataRegs.GPADAT.all = 0xFFFFFFFFu;
        press_button(4);
        EXPECT_FALSE(button_state(4));
    }

    TEST_F(StimulusTest, ReleaseButton_DrivesPinHigh) {
        GpioDataRegs.GPADAT.all = 0;
        release_button(4);
        EXPECT_TRUE(button_state(4));
    }

    TEST_F(StimulusTest, PressButton_DoesNotAffectOtherPins) {
        // Equivalence partition: independence of pin manipulations.
        GpioDataRegs.GPADAT.all = 0;
        release_button(7);
        press_button(4);
        EXPECT_FALSE(button_state(4));
        EXPECT_TRUE(button_state(7));
    }

    TEST_F(StimulusTest, SetGpioInput_HighThenLow_RoundTrips) {
        set_gpio_input(34, true);
        EXPECT_TRUE(read_gpio_input(34));
        set_gpio_input(34, false);
        EXPECT_FALSE(read_gpio_input(34));
    }

    TEST_F(StimulusTest, SetGpioInput_BankBBoundary) {
        // Boundary: GPIO 32 is first pin of bank B.
        set_gpio_input(32, true);
        EXPECT_TRUE(read_gpio_input(32));
    }

    // ---------- ADC ----------

    TEST_F(StimulusTest, InjectAdcResult_RoundTrip_AllModules) {
        for (auto mod: {AdcModule::A, AdcModule::B, AdcModule::C, AdcModule::D}) {
            clear_adc_results(mod);
            inject_adc_result(mod, 0, 4095);
            EXPECT_EQ(read_adc_result(mod, 0), 4095);
        }
    }

    TEST_F(StimulusTest, InjectAdcResult_BoundaryValues) {
        inject_adc_result(AdcModule::A, 0, 0);
        EXPECT_EQ(read_adc_result(AdcModule::A, 0), 0);
        inject_adc_result(AdcModule::A, 0, 4095); // 12-bit max
        EXPECT_EQ(read_adc_result(AdcModule::A, 0), 4095);
    }

    TEST_F(StimulusTest, InjectAdcResult_OutOfRangeSoc_DoesNotCrash) {
        // Boundary: invalid soc indices must be ignored, not UB.
        inject_adc_result(AdcModule::A, 99, 1);
        inject_adc_result(AdcModule::A, -1, 1);
        SUCCEED();
    }

    TEST_F(StimulusTest, ClearAdcResults_ZeroesAllSocs) {
        inject_adc_result(AdcModule::A, 3, 1234);
        clear_adc_results(AdcModule::A);
        EXPECT_EQ(read_adc_result(AdcModule::A, 3), 0);
    }

    // ---------- SPI ----------

    TEST_F(StimulusTest, InjectSpiRx_SingleWord_ReflectsInBuf) {
        clear_spi_state(SpiModule::B);
        inject_spi_rx(SpiModule::B, uint16_t{0xBEEF});
        EXPECT_EQ(SpibRegs.SPIRXBUF, 0xBEEFu);
        EXPECT_EQ(spi_rx_fifo_size(SpiModule::B), 1u);
    }

    TEST_F(StimulusTest, InjectSpiRx_SpanOfWords_LeavesLatestInBuf) {
        clear_spi_state(SpiModule::B);
        constexpr uint16_t words[] = {0x1, 0x2, 0x3};
        inject_spi_rx(SpiModule::B, std::span<const uint16_t>(words, 3));
        EXPECT_EQ(SpibRegs.SPIRXBUF, 0x3u);
        EXPECT_EQ(spi_rx_fifo_size(SpiModule::B), 3u);
    }

    TEST_F(StimulusTest, ClearSpiState_PerModule_ResetsFifoSize) {
        inject_spi_rx(SpiModule::B, uint16_t{0x1});
        clear_spi_state(SpiModule::B);
        EXPECT_EQ(spi_rx_fifo_size(SpiModule::B), 0u);
    }

    TEST_F(StimulusTest, ClearSpiState_All_ResetsAllModules) {
        inject_spi_rx(SpiModule::A, uint16_t{0x1});
        inject_spi_rx(SpiModule::B, uint16_t{0x2});
        clear_spi_state();
        EXPECT_EQ(spi_rx_fifo_size(SpiModule::A), 0u);
        EXPECT_EQ(spi_rx_fifo_size(SpiModule::B), 0u);
    }

    // ---------- encoders ----------

    TEST_F(StimulusTest, InjectEncoderCount_RoundTrip) {
        inject_encoder_count(EqepModule::Eqep1, 12345);
        EXPECT_EQ(read_encoder_count(EqepModule::Eqep1), 12345);
    }

    TEST_F(StimulusTest, InjectEncoderCount_NegativeValue_RoundTrip) {
        // Boundary: signed wraparound.
        inject_encoder_count(EqepModule::Eqep2, -1);
        EXPECT_EQ(read_encoder_count(EqepModule::Eqep2), -1);
    }

    TEST_F(StimulusTest, InjectEncoderCount_AllModulesIndependent) {
        inject_encoder_count(EqepModule::Eqep1, 10);
        inject_encoder_count(EqepModule::Eqep2, 20);
        inject_encoder_count(EqepModule::Eqep3, 30);
        EXPECT_EQ(read_encoder_count(EqepModule::Eqep1), 10);
        EXPECT_EQ(read_encoder_count(EqepModule::Eqep2), 20);
        EXPECT_EQ(read_encoder_count(EqepModule::Eqep3), 30);
    }

    // ---------- lidar ----------

    TEST_F(StimulusTest, InjectLidarFrame_DoesNotCrash) {
        std::array<float, LIDAR_FRAME_LEN> dist{};
        for (size_t i = 0; i < dist.size(); ++i) dist[i] = static_cast<float>(i);
        inject_lidar_frame(std::span<const float, LIDAR_FRAME_LEN>(dist));
        SUCCEED();
    }

    TEST_F(StimulusTest, ClearLidarFrame_DoesNotCrash) {
        clear_lidar_frame();
        SUCCEED();
    }

    TEST_F(StimulusTest, ResetAllStimulus_DoesNotCrash) {
        press_button(4);
        inject_adc_result(AdcModule::A, 0, 1234);
        inject_spi_rx(SpiModule::B, uint16_t{0xABCD});
        inject_encoder_count(EqepModule::Eqep1, 7);
        reset_all_stimulus();
        SUCCEED();
    }
} // namespace