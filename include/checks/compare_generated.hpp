#pragma once

#include <string>
#include <type_traits>
#include <cstdint>
#include <sstream>
#include <array>

template<typename T>
bool check_compare(const T &value, const T &expected, const std::string &name);

template<typename T, std::size_t N>
bool check_compare(const T(&arr)[N], const T(&expected)[N], const std::string & name) {
    bool all_zero = true;
    std::stringstream ss;

    for (std::size_t i = 0; i < N; ++i) {
        ss << name << "[" << i << "]";

        all_zero &= check_compare(arr[i], expected[i], ss.str());
        ss.str(std::string());
    }
    return all_zero;
}

template<typename T, std::size_t N>
bool check_compare(const std::array<T, N> & arr, const std::array<T, N> & expected, const std::string & name) {
    bool all_zero = true;
    std::stringstream ss;

    for (std::size_t i = 0; i < N; ++i) {
        ss << name << "[" << i << "]";

        all_zero &= check_compare(arr[i], expected[i], ss.str());
        ss.str(std::string());
    }
    return all_zero;
}

bool check_compare(const AdcSetup &obj, const AdcSetup &expected, const std::string &name);

bool check_compare(const GpioSetup &obj, const GpioSetup &expected, const std::string &name);

bool check_compare(const CPUTIMER_VARS &obj, const CPUTIMER_VARS &expected, const std::string &name);

bool check_compare(const ADCCTL1_BITS &obj, const ADCCTL1_BITS &expected, const std::string &name);

bool check_compare(const ADCCTL1_REG &obj, const ADCCTL1_REG &expected, const std::string &name);

bool check_compare(const ADCCTL2_BITS &obj, const ADCCTL2_BITS &expected, const std::string &name);

bool check_compare(const ADCCTL2_REG &obj, const ADCCTL2_REG &expected, const std::string &name);

bool check_compare(const ADCBURSTCTL_BITS &obj, const ADCBURSTCTL_BITS &expected, const std::string &name);

bool check_compare(const ADCBURSTCTL_REG &obj, const ADCBURSTCTL_REG &expected, const std::string &name);

bool check_compare(const ADCINTFLG_BITS &obj, const ADCINTFLG_BITS &expected, const std::string &name);

bool check_compare(const ADCINTFLG_REG &obj, const ADCINTFLG_REG &expected, const std::string &name);

bool check_compare(const ADCINTFLGCLR_BITS &obj, const ADCINTFLGCLR_BITS &expected, const std::string &name);

bool check_compare(const ADCINTFLGCLR_REG &obj, const ADCINTFLGCLR_REG &expected, const std::string &name);

bool check_compare(const ADCINTOVF_BITS &obj, const ADCINTOVF_BITS &expected, const std::string &name);

bool check_compare(const ADCINTOVF_REG &obj, const ADCINTOVF_REG &expected, const std::string &name);

bool check_compare(const ADCINTOVFCLR_BITS &obj, const ADCINTOVFCLR_BITS &expected, const std::string &name);

bool check_compare(const ADCINTOVFCLR_REG &obj, const ADCINTOVFCLR_REG &expected, const std::string &name);

bool check_compare(const ADCINTSEL1N2_BITS &obj, const ADCINTSEL1N2_BITS &expected, const std::string &name);

bool check_compare(const ADCINTSEL1N2_REG &obj, const ADCINTSEL1N2_REG &expected, const std::string &name);

bool check_compare(const ADCINTSEL3N4_BITS &obj, const ADCINTSEL3N4_BITS &expected, const std::string &name);

bool check_compare(const ADCINTSEL3N4_REG &obj, const ADCINTSEL3N4_REG &expected, const std::string &name);

bool check_compare(const ADCSOCPRICTL_BITS &obj, const ADCSOCPRICTL_BITS &expected, const std::string &name);

bool check_compare(const ADCSOCPRICTL_REG &obj, const ADCSOCPRICTL_REG &expected, const std::string &name);

bool check_compare(const ADCINTSOCSEL1_BITS &obj, const ADCINTSOCSEL1_BITS &expected, const std::string &name);

bool check_compare(const ADCINTSOCSEL1_REG &obj, const ADCINTSOCSEL1_REG &expected, const std::string &name);

bool check_compare(const ADCINTSOCSEL2_BITS &obj, const ADCINTSOCSEL2_BITS &expected, const std::string &name);

bool check_compare(const ADCINTSOCSEL2_REG &obj, const ADCINTSOCSEL2_REG &expected, const std::string &name);

bool check_compare(const ADCSOCFLG1_BITS &obj, const ADCSOCFLG1_BITS &expected, const std::string &name);

bool check_compare(const ADCSOCFLG1_REG &obj, const ADCSOCFLG1_REG &expected, const std::string &name);

bool check_compare(const ADCSOCFRC1_BITS &obj, const ADCSOCFRC1_BITS &expected, const std::string &name);

bool check_compare(const ADCSOCFRC1_REG &obj, const ADCSOCFRC1_REG &expected, const std::string &name);

bool check_compare(const ADCSOCOVF1_BITS &obj, const ADCSOCOVF1_BITS &expected, const std::string &name);

bool check_compare(const ADCSOCOVF1_REG &obj, const ADCSOCOVF1_REG &expected, const std::string &name);

bool check_compare(const ADCSOCOVFCLR1_BITS &obj, const ADCSOCOVFCLR1_BITS &expected, const std::string &name);

bool check_compare(const ADCSOCOVFCLR1_REG &obj, const ADCSOCOVFCLR1_REG &expected, const std::string &name);

bool check_compare(const ADCSOC0CTL_BITS &obj, const ADCSOC0CTL_BITS &expected, const std::string &name);

bool check_compare(const ADCSOC0CTL_REG &obj, const ADCSOC0CTL_REG &expected, const std::string &name);

bool check_compare(const ADCSOC1CTL_BITS &obj, const ADCSOC1CTL_BITS &expected, const std::string &name);

bool check_compare(const ADCSOC1CTL_REG &obj, const ADCSOC1CTL_REG &expected, const std::string &name);

bool check_compare(const ADCSOC2CTL_BITS &obj, const ADCSOC2CTL_BITS &expected, const std::string &name);

bool check_compare(const ADCSOC2CTL_REG &obj, const ADCSOC2CTL_REG &expected, const std::string &name);

bool check_compare(const ADCSOC3CTL_BITS &obj, const ADCSOC3CTL_BITS &expected, const std::string &name);

bool check_compare(const ADCSOC3CTL_REG &obj, const ADCSOC3CTL_REG &expected, const std::string &name);

bool check_compare(const ADCSOC4CTL_BITS &obj, const ADCSOC4CTL_BITS &expected, const std::string &name);

bool check_compare(const ADCSOC4CTL_REG &obj, const ADCSOC4CTL_REG &expected, const std::string &name);

bool check_compare(const ADCSOC5CTL_BITS &obj, const ADCSOC5CTL_BITS &expected, const std::string &name);

bool check_compare(const ADCSOC5CTL_REG &obj, const ADCSOC5CTL_REG &expected, const std::string &name);

bool check_compare(const ADCSOC6CTL_BITS &obj, const ADCSOC6CTL_BITS &expected, const std::string &name);

bool check_compare(const ADCSOC6CTL_REG &obj, const ADCSOC6CTL_REG &expected, const std::string &name);

bool check_compare(const ADCSOC7CTL_BITS &obj, const ADCSOC7CTL_BITS &expected, const std::string &name);

bool check_compare(const ADCSOC7CTL_REG &obj, const ADCSOC7CTL_REG &expected, const std::string &name);

bool check_compare(const ADCSOC8CTL_BITS &obj, const ADCSOC8CTL_BITS &expected, const std::string &name);

bool check_compare(const ADCSOC8CTL_REG &obj, const ADCSOC8CTL_REG &expected, const std::string &name);

bool check_compare(const ADCSOC9CTL_BITS &obj, const ADCSOC9CTL_BITS &expected, const std::string &name);

bool check_compare(const ADCSOC9CTL_REG &obj, const ADCSOC9CTL_REG &expected, const std::string &name);

bool check_compare(const ADCSOC10CTL_BITS &obj, const ADCSOC10CTL_BITS &expected, const std::string &name);

bool check_compare(const ADCSOC10CTL_REG &obj, const ADCSOC10CTL_REG &expected, const std::string &name);

bool check_compare(const ADCSOC11CTL_BITS &obj, const ADCSOC11CTL_BITS &expected, const std::string &name);

bool check_compare(const ADCSOC11CTL_REG &obj, const ADCSOC11CTL_REG &expected, const std::string &name);

bool check_compare(const ADCSOC12CTL_BITS &obj, const ADCSOC12CTL_BITS &expected, const std::string &name);

bool check_compare(const ADCSOC12CTL_REG &obj, const ADCSOC12CTL_REG &expected, const std::string &name);

bool check_compare(const ADCSOC13CTL_BITS &obj, const ADCSOC13CTL_BITS &expected, const std::string &name);

bool check_compare(const ADCSOC13CTL_REG &obj, const ADCSOC13CTL_REG &expected, const std::string &name);

bool check_compare(const ADCSOC14CTL_BITS &obj, const ADCSOC14CTL_BITS &expected, const std::string &name);

bool check_compare(const ADCSOC14CTL_REG &obj, const ADCSOC14CTL_REG &expected, const std::string &name);

bool check_compare(const ADCSOC15CTL_BITS &obj, const ADCSOC15CTL_BITS &expected, const std::string &name);

bool check_compare(const ADCSOC15CTL_REG &obj, const ADCSOC15CTL_REG &expected, const std::string &name);

bool check_compare(const ADCEVTSTAT_BITS &obj, const ADCEVTSTAT_BITS &expected, const std::string &name);

bool check_compare(const ADCEVTSTAT_REG &obj, const ADCEVTSTAT_REG &expected, const std::string &name);

bool check_compare(const ADCEVTCLR_BITS &obj, const ADCEVTCLR_BITS &expected, const std::string &name);

bool check_compare(const ADCEVTCLR_REG &obj, const ADCEVTCLR_REG &expected, const std::string &name);

bool check_compare(const ADCEVTSEL_BITS &obj, const ADCEVTSEL_BITS &expected, const std::string &name);

bool check_compare(const ADCEVTSEL_REG &obj, const ADCEVTSEL_REG &expected, const std::string &name);

bool check_compare(const ADCEVTINTSEL_BITS &obj, const ADCEVTINTSEL_BITS &expected, const std::string &name);

bool check_compare(const ADCEVTINTSEL_REG &obj, const ADCEVTINTSEL_REG &expected, const std::string &name);

bool check_compare(const ADCOSDETECT_BITS &obj, const ADCOSDETECT_BITS &expected, const std::string &name);

bool check_compare(const ADCOSDETECT_REG &obj, const ADCOSDETECT_REG &expected, const std::string &name);

bool check_compare(const ADCCOUNTER_BITS &obj, const ADCCOUNTER_BITS &expected, const std::string &name);

bool check_compare(const ADCCOUNTER_REG &obj, const ADCCOUNTER_REG &expected, const std::string &name);

bool check_compare(const ADCREV_BITS &obj, const ADCREV_BITS &expected, const std::string &name);

bool check_compare(const ADCREV_REG &obj, const ADCREV_REG &expected, const std::string &name);

bool check_compare(const ADCOFFTRIM_BITS &obj, const ADCOFFTRIM_BITS &expected, const std::string &name);

bool check_compare(const ADCOFFTRIM_REG &obj, const ADCOFFTRIM_REG &expected, const std::string &name);

bool check_compare(const ADCPPB1CONFIG_BITS &obj, const ADCPPB1CONFIG_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB1CONFIG_REG &obj, const ADCPPB1CONFIG_REG &expected, const std::string &name);

bool check_compare(const ADCPPB1STAMP_BITS &obj, const ADCPPB1STAMP_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB1STAMP_REG &obj, const ADCPPB1STAMP_REG &expected, const std::string &name);

bool check_compare(const ADCPPB1OFFCAL_BITS &obj, const ADCPPB1OFFCAL_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB1OFFCAL_REG &obj, const ADCPPB1OFFCAL_REG &expected, const std::string &name);

bool check_compare(const ADCPPB1TRIPHI_BITS &obj, const ADCPPB1TRIPHI_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB1TRIPHI_REG &obj, const ADCPPB1TRIPHI_REG &expected, const std::string &name);

bool check_compare(const ADCPPB1TRIPLO_BITS &obj, const ADCPPB1TRIPLO_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB1TRIPLO_REG &obj, const ADCPPB1TRIPLO_REG &expected, const std::string &name);

bool check_compare(const ADCPPB2CONFIG_BITS &obj, const ADCPPB2CONFIG_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB2CONFIG_REG &obj, const ADCPPB2CONFIG_REG &expected, const std::string &name);

bool check_compare(const ADCPPB2STAMP_BITS &obj, const ADCPPB2STAMP_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB2STAMP_REG &obj, const ADCPPB2STAMP_REG &expected, const std::string &name);

bool check_compare(const ADCPPB2OFFCAL_BITS &obj, const ADCPPB2OFFCAL_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB2OFFCAL_REG &obj, const ADCPPB2OFFCAL_REG &expected, const std::string &name);

bool check_compare(const ADCPPB2TRIPHI_BITS &obj, const ADCPPB2TRIPHI_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB2TRIPHI_REG &obj, const ADCPPB2TRIPHI_REG &expected, const std::string &name);

bool check_compare(const ADCPPB2TRIPLO_BITS &obj, const ADCPPB2TRIPLO_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB2TRIPLO_REG &obj, const ADCPPB2TRIPLO_REG &expected, const std::string &name);

bool check_compare(const ADCPPB3CONFIG_BITS &obj, const ADCPPB3CONFIG_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB3CONFIG_REG &obj, const ADCPPB3CONFIG_REG &expected, const std::string &name);

bool check_compare(const ADCPPB3STAMP_BITS &obj, const ADCPPB3STAMP_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB3STAMP_REG &obj, const ADCPPB3STAMP_REG &expected, const std::string &name);

bool check_compare(const ADCPPB3OFFCAL_BITS &obj, const ADCPPB3OFFCAL_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB3OFFCAL_REG &obj, const ADCPPB3OFFCAL_REG &expected, const std::string &name);

bool check_compare(const ADCPPB3TRIPHI_BITS &obj, const ADCPPB3TRIPHI_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB3TRIPHI_REG &obj, const ADCPPB3TRIPHI_REG &expected, const std::string &name);

bool check_compare(const ADCPPB3TRIPLO_BITS &obj, const ADCPPB3TRIPLO_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB3TRIPLO_REG &obj, const ADCPPB3TRIPLO_REG &expected, const std::string &name);

bool check_compare(const ADCPPB4CONFIG_BITS &obj, const ADCPPB4CONFIG_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB4CONFIG_REG &obj, const ADCPPB4CONFIG_REG &expected, const std::string &name);

bool check_compare(const ADCPPB4STAMP_BITS &obj, const ADCPPB4STAMP_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB4STAMP_REG &obj, const ADCPPB4STAMP_REG &expected, const std::string &name);

bool check_compare(const ADCPPB4OFFCAL_BITS &obj, const ADCPPB4OFFCAL_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB4OFFCAL_REG &obj, const ADCPPB4OFFCAL_REG &expected, const std::string &name);

bool check_compare(const ADCPPB4TRIPHI_BITS &obj, const ADCPPB4TRIPHI_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB4TRIPHI_REG &obj, const ADCPPB4TRIPHI_REG &expected, const std::string &name);

bool check_compare(const ADCPPB4TRIPLO_BITS &obj, const ADCPPB4TRIPLO_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB4TRIPLO_REG &obj, const ADCPPB4TRIPLO_REG &expected, const std::string &name);

bool check_compare(const ADC_REGS &obj, const ADC_REGS &expected, const std::string &name);

bool check_compare(const ADCPPB1RESULT_BITS &obj, const ADCPPB1RESULT_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB1RESULT_REG &obj, const ADCPPB1RESULT_REG &expected, const std::string &name);

bool check_compare(const ADCPPB2RESULT_BITS &obj, const ADCPPB2RESULT_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB2RESULT_REG &obj, const ADCPPB2RESULT_REG &expected, const std::string &name);

bool check_compare(const ADCPPB3RESULT_BITS &obj, const ADCPPB3RESULT_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB3RESULT_REG &obj, const ADCPPB3RESULT_REG &expected, const std::string &name);

bool check_compare(const ADCPPB4RESULT_BITS &obj, const ADCPPB4RESULT_BITS &expected, const std::string &name);

bool check_compare(const ADCPPB4RESULT_REG &obj, const ADCPPB4RESULT_REG &expected, const std::string &name);

bool check_compare(const ADC_RESULT_REGS &obj, const ADC_RESULT_REGS &expected, const std::string &name);

bool check_compare(const INTOSC1TRIM_BITS &obj, const INTOSC1TRIM_BITS &expected, const std::string &name);

bool check_compare(const INTOSC1TRIM_REG &obj, const INTOSC1TRIM_REG &expected, const std::string &name);

bool check_compare(const INTOSC2TRIM_BITS &obj, const INTOSC2TRIM_BITS &expected, const std::string &name);

bool check_compare(const INTOSC2TRIM_REG &obj, const INTOSC2TRIM_REG &expected, const std::string &name);

bool check_compare(const TSNSCTL_BITS &obj, const TSNSCTL_BITS &expected, const std::string &name);

bool check_compare(const TSNSCTL_REG &obj, const TSNSCTL_REG &expected, const std::string &name);

bool check_compare(const LOCK_BITS &obj, const LOCK_BITS &expected, const std::string &name);

bool check_compare(const LOCK_REG &obj, const LOCK_REG &expected, const std::string &name);

bool check_compare(const ANAREFTRIMA_BITS &obj, const ANAREFTRIMA_BITS &expected, const std::string &name);

bool check_compare(const ANAREFTRIMA_REG &obj, const ANAREFTRIMA_REG &expected, const std::string &name);

bool check_compare(const ANAREFTRIMB_BITS &obj, const ANAREFTRIMB_BITS &expected, const std::string &name);

bool check_compare(const ANAREFTRIMB_REG &obj, const ANAREFTRIMB_REG &expected, const std::string &name);

bool check_compare(const ANAREFTRIMC_BITS &obj, const ANAREFTRIMC_BITS &expected, const std::string &name);

bool check_compare(const ANAREFTRIMC_REG &obj, const ANAREFTRIMC_REG &expected, const std::string &name);

bool check_compare(const ANAREFTRIMD_BITS &obj, const ANAREFTRIMD_BITS &expected, const std::string &name);

bool check_compare(const ANAREFTRIMD_REG &obj, const ANAREFTRIMD_REG &expected, const std::string &name);

bool check_compare(const ANALOG_SUBSYS_REGS &obj, const ANALOG_SUBSYS_REGS &expected, const std::string &name);

bool check_compare(const MCTL_BITS &obj, const MCTL_BITS &expected, const std::string &name);

bool check_compare(const MCTL_REG &obj, const MCTL_REG &expected, const std::string &name);

bool check_compare(const MIFR_BITS &obj, const MIFR_BITS &expected, const std::string &name);

bool check_compare(const MIFR_REG &obj, const MIFR_REG &expected, const std::string &name);

bool check_compare(const MIOVF_BITS &obj, const MIOVF_BITS &expected, const std::string &name);

bool check_compare(const MIOVF_REG &obj, const MIOVF_REG &expected, const std::string &name);

bool check_compare(const MIFRC_BITS &obj, const MIFRC_BITS &expected, const std::string &name);

bool check_compare(const MIFRC_REG &obj, const MIFRC_REG &expected, const std::string &name);

bool check_compare(const MICLR_BITS &obj, const MICLR_BITS &expected, const std::string &name);

bool check_compare(const MICLR_REG &obj, const MICLR_REG &expected, const std::string &name);

bool check_compare(const MICLROVF_BITS &obj, const MICLROVF_BITS &expected, const std::string &name);

bool check_compare(const MICLROVF_REG &obj, const MICLROVF_REG &expected, const std::string &name);

bool check_compare(const MIER_BITS &obj, const MIER_BITS &expected, const std::string &name);

bool check_compare(const MIER_REG &obj, const MIER_REG &expected, const std::string &name);

bool check_compare(const MIRUN_BITS &obj, const MIRUN_BITS &expected, const std::string &name);

bool check_compare(const MIRUN_REG &obj, const MIRUN_REG &expected, const std::string &name);

bool check_compare(const _MSTF_BITS &obj, const _MSTF_BITS &expected, const std::string &name);

bool check_compare(const _MSTF_REG &obj, const _MSTF_REG &expected, const std::string &name);

bool check_compare(const MR_REG &obj, const MR_REG &expected, const std::string &name);

bool check_compare(const CLA_REGS &obj, const CLA_REGS &expected, const std::string &name);

bool check_compare(const SOFTINTEN_BITS &obj, const SOFTINTEN_BITS &expected, const std::string &name);

bool check_compare(const SOFTINTEN_REG &obj, const SOFTINTEN_REG &expected, const std::string &name);

bool check_compare(const SOFTINTFRC_BITS &obj, const SOFTINTFRC_BITS &expected, const std::string &name);

bool check_compare(const SOFTINTFRC_REG &obj, const SOFTINTFRC_REG &expected, const std::string &name);

bool check_compare(const CLA_SOFTINT_REGS &obj, const CLA_SOFTINT_REGS &expected, const std::string &name);

bool check_compare(const CLB_COUNT_RESET_BITS &obj, const CLB_COUNT_RESET_BITS &expected, const std::string &name);

bool check_compare(const CLB_COUNT_RESET_REG &obj, const CLB_COUNT_RESET_REG &expected, const std::string &name);

bool check_compare(const CLB_COUNT_MODE_1_BITS &obj, const CLB_COUNT_MODE_1_BITS &expected, const std::string &name);

bool check_compare(const CLB_COUNT_MODE_1_REG &obj, const CLB_COUNT_MODE_1_REG &expected, const std::string &name);

bool check_compare(const CLB_COUNT_MODE_0_BITS &obj, const CLB_COUNT_MODE_0_BITS &expected, const std::string &name);

bool check_compare(const CLB_COUNT_MODE_0_REG &obj, const CLB_COUNT_MODE_0_REG &expected, const std::string &name);

bool check_compare(const CLB_COUNT_EVENT_BITS &obj, const CLB_COUNT_EVENT_BITS &expected, const std::string &name);

bool check_compare(const CLB_COUNT_EVENT_REG &obj, const CLB_COUNT_EVENT_REG &expected, const std::string &name);

bool check_compare(const CLB_FSM_EXTRA_IN0_BITS &obj, const CLB_FSM_EXTRA_IN0_BITS &expected, const std::string &name);

bool check_compare(const CLB_FSM_EXTRA_IN0_REG &obj, const CLB_FSM_EXTRA_IN0_REG &expected, const std::string &name);

bool check_compare(const CLB_FSM_EXTERNAL_IN0_BITS &obj, const CLB_FSM_EXTERNAL_IN0_BITS &expected,
                   const std::string &name);

bool check_compare(const CLB_FSM_EXTERNAL_IN0_REG &obj, const CLB_FSM_EXTERNAL_IN0_REG &expected,
                   const std::string &name);

bool check_compare(const CLB_FSM_EXTERNAL_IN1_BITS &obj, const CLB_FSM_EXTERNAL_IN1_BITS &expected,
                   const std::string &name);

bool check_compare(const CLB_FSM_EXTERNAL_IN1_REG &obj, const CLB_FSM_EXTERNAL_IN1_REG &expected,
                   const std::string &name);

bool check_compare(const CLB_FSM_EXTRA_IN1_BITS &obj, const CLB_FSM_EXTRA_IN1_BITS &expected, const std::string &name);

bool check_compare(const CLB_FSM_EXTRA_IN1_REG &obj, const CLB_FSM_EXTRA_IN1_REG &expected, const std::string &name);

bool check_compare(const CLB_LUT4_IN0_BITS &obj, const CLB_LUT4_IN0_BITS &expected, const std::string &name);

bool check_compare(const CLB_LUT4_IN0_REG &obj, const CLB_LUT4_IN0_REG &expected, const std::string &name);

bool check_compare(const CLB_LUT4_IN1_BITS &obj, const CLB_LUT4_IN1_BITS &expected, const std::string &name);

bool check_compare(const CLB_LUT4_IN1_REG &obj, const CLB_LUT4_IN1_REG &expected, const std::string &name);

bool check_compare(const CLB_LUT4_IN2_BITS &obj, const CLB_LUT4_IN2_BITS &expected, const std::string &name);

bool check_compare(const CLB_LUT4_IN2_REG &obj, const CLB_LUT4_IN2_REG &expected, const std::string &name);

bool check_compare(const CLB_LUT4_IN3_BITS &obj, const CLB_LUT4_IN3_BITS &expected, const std::string &name);

bool check_compare(const CLB_LUT4_IN3_REG &obj, const CLB_LUT4_IN3_REG &expected, const std::string &name);

bool check_compare(const CLB_FSM_LUT_FN1_0_BITS &obj, const CLB_FSM_LUT_FN1_0_BITS &expected, const std::string &name);

bool check_compare(const CLB_FSM_LUT_FN1_0_REG &obj, const CLB_FSM_LUT_FN1_0_REG &expected, const std::string &name);

bool check_compare(const CLB_FSM_LUT_FN2_BITS &obj, const CLB_FSM_LUT_FN2_BITS &expected, const std::string &name);

bool check_compare(const CLB_FSM_LUT_FN2_REG &obj, const CLB_FSM_LUT_FN2_REG &expected, const std::string &name);

bool check_compare(const CLB_LUT4_FN1_0_BITS &obj, const CLB_LUT4_FN1_0_BITS &expected, const std::string &name);

bool check_compare(const CLB_LUT4_FN1_0_REG &obj, const CLB_LUT4_FN1_0_REG &expected, const std::string &name);

bool check_compare(const CLB_LUT4_FN2_BITS &obj, const CLB_LUT4_FN2_BITS &expected, const std::string &name);

bool check_compare(const CLB_LUT4_FN2_REG &obj, const CLB_LUT4_FN2_REG &expected, const std::string &name);

bool check_compare(const CLB_FSM_NEXT_STATE_0_BITS &obj, const CLB_FSM_NEXT_STATE_0_BITS &expected,
                   const std::string &name);

bool check_compare(const CLB_FSM_NEXT_STATE_0_REG &obj, const CLB_FSM_NEXT_STATE_0_REG &expected,
                   const std::string &name);

bool check_compare(const CLB_FSM_NEXT_STATE_1_BITS &obj, const CLB_FSM_NEXT_STATE_1_BITS &expected,
                   const std::string &name);

bool check_compare(const CLB_FSM_NEXT_STATE_1_REG &obj, const CLB_FSM_NEXT_STATE_1_REG &expected,
                   const std::string &name);

bool check_compare(const CLB_FSM_NEXT_STATE_2_BITS &obj, const CLB_FSM_NEXT_STATE_2_BITS &expected,
                   const std::string &name);

bool check_compare(const CLB_FSM_NEXT_STATE_2_REG &obj, const CLB_FSM_NEXT_STATE_2_REG &expected,
                   const std::string &name);

bool check_compare(const CLB_MISC_CONTROL_BITS &obj, const CLB_MISC_CONTROL_BITS &expected, const std::string &name);

bool check_compare(const CLB_MISC_CONTROL_REG &obj, const CLB_MISC_CONTROL_REG &expected, const std::string &name);

bool check_compare(const CLB_OUTPUT_LUT_0_BITS &obj, const CLB_OUTPUT_LUT_0_BITS &expected, const std::string &name);

bool check_compare(const CLB_OUTPUT_LUT_0_REG &obj, const CLB_OUTPUT_LUT_0_REG &expected, const std::string &name);

bool check_compare(const CLB_OUTPUT_LUT_1_BITS &obj, const CLB_OUTPUT_LUT_1_BITS &expected, const std::string &name);

bool check_compare(const CLB_OUTPUT_LUT_1_REG &obj, const CLB_OUTPUT_LUT_1_REG &expected, const std::string &name);

bool check_compare(const CLB_OUTPUT_LUT_2_BITS &obj, const CLB_OUTPUT_LUT_2_BITS &expected, const std::string &name);

bool check_compare(const CLB_OUTPUT_LUT_2_REG &obj, const CLB_OUTPUT_LUT_2_REG &expected, const std::string &name);

bool check_compare(const CLB_OUTPUT_LUT_3_BITS &obj, const CLB_OUTPUT_LUT_3_BITS &expected, const std::string &name);

bool check_compare(const CLB_OUTPUT_LUT_3_REG &obj, const CLB_OUTPUT_LUT_3_REG &expected, const std::string &name);

bool check_compare(const CLB_OUTPUT_LUT_4_BITS &obj, const CLB_OUTPUT_LUT_4_BITS &expected, const std::string &name);

bool check_compare(const CLB_OUTPUT_LUT_4_REG &obj, const CLB_OUTPUT_LUT_4_REG &expected, const std::string &name);

bool check_compare(const CLB_OUTPUT_LUT_5_BITS &obj, const CLB_OUTPUT_LUT_5_BITS &expected, const std::string &name);

bool check_compare(const CLB_OUTPUT_LUT_5_REG &obj, const CLB_OUTPUT_LUT_5_REG &expected, const std::string &name);

bool check_compare(const CLB_OUTPUT_LUT_6_BITS &obj, const CLB_OUTPUT_LUT_6_BITS &expected, const std::string &name);

bool check_compare(const CLB_OUTPUT_LUT_6_REG &obj, const CLB_OUTPUT_LUT_6_REG &expected, const std::string &name);

bool check_compare(const CLB_OUTPUT_LUT_7_BITS &obj, const CLB_OUTPUT_LUT_7_BITS &expected, const std::string &name);

bool check_compare(const CLB_OUTPUT_LUT_7_REG &obj, const CLB_OUTPUT_LUT_7_REG &expected, const std::string &name);

bool check_compare(const CLB_HLC_EVENT_SEL_BITS &obj, const CLB_HLC_EVENT_SEL_BITS &expected, const std::string &name);

bool check_compare(const CLB_HLC_EVENT_SEL_REG &obj, const CLB_HLC_EVENT_SEL_REG &expected, const std::string &name);

bool check_compare(const CLB_LOGIC_CONFIG_REGS &obj, const CLB_LOGIC_CONFIG_REGS &expected, const std::string &name);

bool check_compare(const CLB_LOAD_EN_BITS &obj, const CLB_LOAD_EN_BITS &expected, const std::string &name);

bool check_compare(const CLB_LOAD_EN_REG &obj, const CLB_LOAD_EN_REG &expected, const std::string &name);

bool check_compare(const CLB_LOAD_ADDR_BITS &obj, const CLB_LOAD_ADDR_BITS &expected, const std::string &name);

bool check_compare(const CLB_LOAD_ADDR_REG &obj, const CLB_LOAD_ADDR_REG &expected, const std::string &name);

bool check_compare(const CLB_INPUT_FILTER_BITS &obj, const CLB_INPUT_FILTER_BITS &expected, const std::string &name);

bool check_compare(const CLB_INPUT_FILTER_REG &obj, const CLB_INPUT_FILTER_REG &expected, const std::string &name);

bool check_compare(const CLB_IN_MUX_SEL_0_BITS &obj, const CLB_IN_MUX_SEL_0_BITS &expected, const std::string &name);

bool check_compare(const CLB_IN_MUX_SEL_0_REG &obj, const CLB_IN_MUX_SEL_0_REG &expected, const std::string &name);

bool check_compare(const CLB_LCL_MUX_SEL_1_BITS &obj, const CLB_LCL_MUX_SEL_1_BITS &expected, const std::string &name);

bool check_compare(const CLB_LCL_MUX_SEL_1_REG &obj, const CLB_LCL_MUX_SEL_1_REG &expected, const std::string &name);

bool check_compare(const CLB_LCL_MUX_SEL_2_BITS &obj, const CLB_LCL_MUX_SEL_2_BITS &expected, const std::string &name);

bool check_compare(const CLB_LCL_MUX_SEL_2_REG &obj, const CLB_LCL_MUX_SEL_2_REG &expected, const std::string &name);

bool check_compare(const CLB_BUF_PTR_BITS &obj, const CLB_BUF_PTR_BITS &expected, const std::string &name);

bool check_compare(const CLB_BUF_PTR_REG &obj, const CLB_BUF_PTR_REG &expected, const std::string &name);

bool check_compare(const CLB_GP_REG_BITS &obj, const CLB_GP_REG_BITS &expected, const std::string &name);

bool check_compare(const CLB_GP_REG_REG &obj, const CLB_GP_REG_REG &expected, const std::string &name);

bool check_compare(const CLB_GLBL_MUX_SEL_1_BITS &obj, const CLB_GLBL_MUX_SEL_1_BITS &expected,
                   const std::string &name);

bool check_compare(const CLB_GLBL_MUX_SEL_1_REG &obj, const CLB_GLBL_MUX_SEL_1_REG &expected, const std::string &name);

bool check_compare(const CLB_GLBL_MUX_SEL_2_BITS &obj, const CLB_GLBL_MUX_SEL_2_BITS &expected,
                   const std::string &name);

bool check_compare(const CLB_GLBL_MUX_SEL_2_REG &obj, const CLB_GLBL_MUX_SEL_2_REG &expected, const std::string &name);

bool check_compare(const CLB_INTR_TAG_REG_BITS &obj, const CLB_INTR_TAG_REG_BITS &expected, const std::string &name);

bool check_compare(const CLB_INTR_TAG_REG_REG &obj, const CLB_INTR_TAG_REG_REG &expected, const std::string &name);

bool check_compare(const CLB_LOCK_BITS &obj, const CLB_LOCK_BITS &expected, const std::string &name);

bool check_compare(const CLB_LOCK_REG &obj, const CLB_LOCK_REG &expected, const std::string &name);

bool check_compare(const CLB_DBG_OUT_BITS &obj, const CLB_DBG_OUT_BITS &expected, const std::string &name);

bool check_compare(const CLB_DBG_OUT_REG &obj, const CLB_DBG_OUT_REG &expected, const std::string &name);

bool check_compare(const CLB_LOGIC_CONTROL_REGS &obj, const CLB_LOGIC_CONTROL_REGS &expected, const std::string &name);

bool check_compare(const CLB_DATA_EXCHANGE_REGS &obj, const CLB_DATA_EXCHANGE_REGS &expected, const std::string &name);

bool check_compare(const AUXSIG0MUX0TO15CFG_BITS &obj, const AUXSIG0MUX0TO15CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const AUXSIG0MUX0TO15CFG_REG &obj, const AUXSIG0MUX0TO15CFG_REG &expected, const std::string &name);

bool check_compare(const AUXSIG0MUX16TO31CFG_BITS &obj, const AUXSIG0MUX16TO31CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const AUXSIG0MUX16TO31CFG_REG &obj, const AUXSIG0MUX16TO31CFG_REG &expected,
                   const std::string &name);

bool check_compare(const AUXSIG1MUX0TO15CFG_BITS &obj, const AUXSIG1MUX0TO15CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const AUXSIG1MUX0TO15CFG_REG &obj, const AUXSIG1MUX0TO15CFG_REG &expected, const std::string &name);

bool check_compare(const AUXSIG1MUX16TO31CFG_BITS &obj, const AUXSIG1MUX16TO31CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const AUXSIG1MUX16TO31CFG_REG &obj, const AUXSIG1MUX16TO31CFG_REG &expected,
                   const std::string &name);

bool check_compare(const AUXSIG2MUX0TO15CFG_BITS &obj, const AUXSIG2MUX0TO15CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const AUXSIG2MUX0TO15CFG_REG &obj, const AUXSIG2MUX0TO15CFG_REG &expected, const std::string &name);

bool check_compare(const AUXSIG2MUX16TO31CFG_BITS &obj, const AUXSIG2MUX16TO31CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const AUXSIG2MUX16TO31CFG_REG &obj, const AUXSIG2MUX16TO31CFG_REG &expected,
                   const std::string &name);

bool check_compare(const AUXSIG3MUX0TO15CFG_BITS &obj, const AUXSIG3MUX0TO15CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const AUXSIG3MUX0TO15CFG_REG &obj, const AUXSIG3MUX0TO15CFG_REG &expected, const std::string &name);

bool check_compare(const AUXSIG3MUX16TO31CFG_BITS &obj, const AUXSIG3MUX16TO31CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const AUXSIG3MUX16TO31CFG_REG &obj, const AUXSIG3MUX16TO31CFG_REG &expected,
                   const std::string &name);

bool check_compare(const AUXSIG4MUX0TO15CFG_BITS &obj, const AUXSIG4MUX0TO15CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const AUXSIG4MUX0TO15CFG_REG &obj, const AUXSIG4MUX0TO15CFG_REG &expected, const std::string &name);

bool check_compare(const AUXSIG4MUX16TO31CFG_BITS &obj, const AUXSIG4MUX16TO31CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const AUXSIG4MUX16TO31CFG_REG &obj, const AUXSIG4MUX16TO31CFG_REG &expected,
                   const std::string &name);

bool check_compare(const AUXSIG5MUX0TO15CFG_BITS &obj, const AUXSIG5MUX0TO15CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const AUXSIG5MUX0TO15CFG_REG &obj, const AUXSIG5MUX0TO15CFG_REG &expected, const std::string &name);

bool check_compare(const AUXSIG5MUX16TO31CFG_BITS &obj, const AUXSIG5MUX16TO31CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const AUXSIG5MUX16TO31CFG_REG &obj, const AUXSIG5MUX16TO31CFG_REG &expected,
                   const std::string &name);

bool check_compare(const AUXSIG6MUX0TO15CFG_BITS &obj, const AUXSIG6MUX0TO15CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const AUXSIG6MUX0TO15CFG_REG &obj, const AUXSIG6MUX0TO15CFG_REG &expected, const std::string &name);

bool check_compare(const AUXSIG6MUX16TO31CFG_BITS &obj, const AUXSIG6MUX16TO31CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const AUXSIG6MUX16TO31CFG_REG &obj, const AUXSIG6MUX16TO31CFG_REG &expected,
                   const std::string &name);

bool check_compare(const AUXSIG7MUX0TO15CFG_BITS &obj, const AUXSIG7MUX0TO15CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const AUXSIG7MUX0TO15CFG_REG &obj, const AUXSIG7MUX0TO15CFG_REG &expected, const std::string &name);

bool check_compare(const AUXSIG7MUX16TO31CFG_BITS &obj, const AUXSIG7MUX16TO31CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const AUXSIG7MUX16TO31CFG_REG &obj, const AUXSIG7MUX16TO31CFG_REG &expected,
                   const std::string &name);

bool check_compare(const AUXSIG0MUXENABLE_BITS &obj, const AUXSIG0MUXENABLE_BITS &expected, const std::string &name);

bool check_compare(const AUXSIG0MUXENABLE_REG &obj, const AUXSIG0MUXENABLE_REG &expected, const std::string &name);

bool check_compare(const AUXSIG1MUXENABLE_BITS &obj, const AUXSIG1MUXENABLE_BITS &expected, const std::string &name);

bool check_compare(const AUXSIG1MUXENABLE_REG &obj, const AUXSIG1MUXENABLE_REG &expected, const std::string &name);

bool check_compare(const AUXSIG2MUXENABLE_BITS &obj, const AUXSIG2MUXENABLE_BITS &expected, const std::string &name);

bool check_compare(const AUXSIG2MUXENABLE_REG &obj, const AUXSIG2MUXENABLE_REG &expected, const std::string &name);

bool check_compare(const AUXSIG3MUXENABLE_BITS &obj, const AUXSIG3MUXENABLE_BITS &expected, const std::string &name);

bool check_compare(const AUXSIG3MUXENABLE_REG &obj, const AUXSIG3MUXENABLE_REG &expected, const std::string &name);

bool check_compare(const AUXSIG4MUXENABLE_BITS &obj, const AUXSIG4MUXENABLE_BITS &expected, const std::string &name);

bool check_compare(const AUXSIG4MUXENABLE_REG &obj, const AUXSIG4MUXENABLE_REG &expected, const std::string &name);

bool check_compare(const AUXSIG5MUXENABLE_BITS &obj, const AUXSIG5MUXENABLE_BITS &expected, const std::string &name);

bool check_compare(const AUXSIG5MUXENABLE_REG &obj, const AUXSIG5MUXENABLE_REG &expected, const std::string &name);

bool check_compare(const AUXSIG6MUXENABLE_BITS &obj, const AUXSIG6MUXENABLE_BITS &expected, const std::string &name);

bool check_compare(const AUXSIG6MUXENABLE_REG &obj, const AUXSIG6MUXENABLE_REG &expected, const std::string &name);

bool check_compare(const AUXSIG7MUXENABLE_BITS &obj, const AUXSIG7MUXENABLE_BITS &expected, const std::string &name);

bool check_compare(const AUXSIG7MUXENABLE_REG &obj, const AUXSIG7MUXENABLE_REG &expected, const std::string &name);

bool check_compare(const AUXSIGOUTINV_BITS &obj, const AUXSIGOUTINV_BITS &expected, const std::string &name);

bool check_compare(const AUXSIGOUTINV_REG &obj, const AUXSIGOUTINV_REG &expected, const std::string &name);

bool check_compare(const AUXSIGLOCK_BITS &obj, const AUXSIGLOCK_BITS &expected, const std::string &name);

bool check_compare(const AUXSIGLOCK_REG &obj, const AUXSIGLOCK_REG &expected, const std::string &name);

bool check_compare(const CLB_XBAR_REGS &obj, const CLB_XBAR_REGS &expected, const std::string &name);

bool check_compare(const COMPCTL_BITS &obj, const COMPCTL_BITS &expected, const std::string &name);

bool check_compare(const COMPCTL_REG &obj, const COMPCTL_REG &expected, const std::string &name);

bool check_compare(const COMPHYSCTL_BITS &obj, const COMPHYSCTL_BITS &expected, const std::string &name);

bool check_compare(const COMPHYSCTL_REG &obj, const COMPHYSCTL_REG &expected, const std::string &name);

bool check_compare(const COMPSTS_BITS &obj, const COMPSTS_BITS &expected, const std::string &name);

bool check_compare(const COMPSTS_REG &obj, const COMPSTS_REG &expected, const std::string &name);

bool check_compare(const COMPSTSCLR_BITS &obj, const COMPSTSCLR_BITS &expected, const std::string &name);

bool check_compare(const COMPSTSCLR_REG &obj, const COMPSTSCLR_REG &expected, const std::string &name);

bool check_compare(const COMPDACCTL_BITS &obj, const COMPDACCTL_BITS &expected, const std::string &name);

bool check_compare(const COMPDACCTL_REG &obj, const COMPDACCTL_REG &expected, const std::string &name);

bool check_compare(const DACHVALS_BITS &obj, const DACHVALS_BITS &expected, const std::string &name);

bool check_compare(const DACHVALS_REG &obj, const DACHVALS_REG &expected, const std::string &name);

bool check_compare(const DACHVALA_BITS &obj, const DACHVALA_BITS &expected, const std::string &name);

bool check_compare(const DACHVALA_REG &obj, const DACHVALA_REG &expected, const std::string &name);

bool check_compare(const DACLVALS_BITS &obj, const DACLVALS_BITS &expected, const std::string &name);

bool check_compare(const DACLVALS_REG &obj, const DACLVALS_REG &expected, const std::string &name);

bool check_compare(const DACLVALA_BITS &obj, const DACLVALA_BITS &expected, const std::string &name);

bool check_compare(const DACLVALA_REG &obj, const DACLVALA_REG &expected, const std::string &name);

bool check_compare(const RAMPDLYA_BITS &obj, const RAMPDLYA_BITS &expected, const std::string &name);

bool check_compare(const RAMPDLYA_REG &obj, const RAMPDLYA_REG &expected, const std::string &name);

bool check_compare(const RAMPDLYS_BITS &obj, const RAMPDLYS_BITS &expected, const std::string &name);

bool check_compare(const RAMPDLYS_REG &obj, const RAMPDLYS_REG &expected, const std::string &name);

bool check_compare(const CTRIPLFILCTL_BITS &obj, const CTRIPLFILCTL_BITS &expected, const std::string &name);

bool check_compare(const CTRIPLFILCTL_REG &obj, const CTRIPLFILCTL_REG &expected, const std::string &name);

bool check_compare(const CTRIPLFILCLKCTL_BITS &obj, const CTRIPLFILCLKCTL_BITS &expected, const std::string &name);

bool check_compare(const CTRIPLFILCLKCTL_REG &obj, const CTRIPLFILCLKCTL_REG &expected, const std::string &name);

bool check_compare(const CTRIPHFILCTL_BITS &obj, const CTRIPHFILCTL_BITS &expected, const std::string &name);

bool check_compare(const CTRIPHFILCTL_REG &obj, const CTRIPHFILCTL_REG &expected, const std::string &name);

bool check_compare(const CTRIPHFILCLKCTL_BITS &obj, const CTRIPHFILCLKCTL_BITS &expected, const std::string &name);

bool check_compare(const CTRIPHFILCLKCTL_REG &obj, const CTRIPHFILCLKCTL_REG &expected, const std::string &name);

bool check_compare(const COMPLOCK_BITS &obj, const COMPLOCK_BITS &expected, const std::string &name);

bool check_compare(const COMPLOCK_REG &obj, const COMPLOCK_REG &expected, const std::string &name);

bool check_compare(const CMPSS_REGS &obj, const CMPSS_REGS &expected, const std::string &name);

bool check_compare(const TIM_BITS &obj, const TIM_BITS &expected, const std::string &name);

bool check_compare(const TIM_REG &obj, const TIM_REG &expected, const std::string &name);

bool check_compare(const PRD_BITS &obj, const PRD_BITS &expected, const std::string &name);

bool check_compare(const PRD_REG &obj, const PRD_REG &expected, const std::string &name);

bool check_compare(const TCR_BITS &obj, const TCR_BITS &expected, const std::string &name);

bool check_compare(const TCR_REG &obj, const TCR_REG &expected, const std::string &name);

bool check_compare(const TPR_BITS &obj, const TPR_BITS &expected, const std::string &name);

bool check_compare(const TPR_REG &obj, const TPR_REG &expected, const std::string &name);

bool check_compare(const TPRH_BITS &obj, const TPRH_BITS &expected, const std::string &name);

bool check_compare(const TPRH_REG &obj, const TPRH_REG &expected, const std::string &name);

bool check_compare(const CPUTIMER_REGS &obj, const CPUTIMER_REGS &expected, const std::string &name);

bool check_compare(const DACREV_BITS &obj, const DACREV_BITS &expected, const std::string &name);

bool check_compare(const DACREV_REG &obj, const DACREV_REG &expected, const std::string &name);

bool check_compare(const DACCTL_BITS &obj, const DACCTL_BITS &expected, const std::string &name);

bool check_compare(const DACCTL_REG &obj, const DACCTL_REG &expected, const std::string &name);

bool check_compare(const DACVALA_BITS &obj, const DACVALA_BITS &expected, const std::string &name);

bool check_compare(const DACVALA_REG &obj, const DACVALA_REG &expected, const std::string &name);

bool check_compare(const DACVALS_BITS &obj, const DACVALS_BITS &expected, const std::string &name);

bool check_compare(const DACVALS_REG &obj, const DACVALS_REG &expected, const std::string &name);

bool check_compare(const DACOUTEN_BITS &obj, const DACOUTEN_BITS &expected, const std::string &name);

bool check_compare(const DACOUTEN_REG &obj, const DACOUTEN_REG &expected, const std::string &name);

bool check_compare(const DACLOCK_BITS &obj, const DACLOCK_BITS &expected, const std::string &name);

bool check_compare(const DACLOCK_REG &obj, const DACLOCK_REG &expected, const std::string &name);

bool check_compare(const DACTRIM_BITS &obj, const DACTRIM_BITS &expected, const std::string &name);

bool check_compare(const DACTRIM_REG &obj, const DACTRIM_REG &expected, const std::string &name);

bool check_compare(const DAC_REGS &obj, const DAC_REGS &expected, const std::string &name);

bool check_compare(const Z1_LINKPOINTER_BITS &obj, const Z1_LINKPOINTER_BITS &expected, const std::string &name);

bool check_compare(const Z1_LINKPOINTER_REG &obj, const Z1_LINKPOINTER_REG &expected, const std::string &name);

bool check_compare(const Z1_OTPSECLOCK_BITS &obj, const Z1_OTPSECLOCK_BITS &expected, const std::string &name);

bool check_compare(const Z1_OTPSECLOCK_REG &obj, const Z1_OTPSECLOCK_REG &expected, const std::string &name);

bool check_compare(const Z1_BOOTCTRL_BITS &obj, const Z1_BOOTCTRL_BITS &expected, const std::string &name);

bool check_compare(const Z1_BOOTCTRL_REG &obj, const Z1_BOOTCTRL_REG &expected, const std::string &name);

bool check_compare(const Z1_CR_BITS &obj, const Z1_CR_BITS &expected, const std::string &name);

bool check_compare(const Z1_CR_REG &obj, const Z1_CR_REG &expected, const std::string &name);

bool check_compare(const Z1_GRABSECTR_BITS &obj, const Z1_GRABSECTR_BITS &expected, const std::string &name);

bool check_compare(const Z1_GRABSECTR_REG &obj, const Z1_GRABSECTR_REG &expected, const std::string &name);

bool check_compare(const Z1_GRABRAMR_BITS &obj, const Z1_GRABRAMR_BITS &expected, const std::string &name);

bool check_compare(const Z1_GRABRAMR_REG &obj, const Z1_GRABRAMR_REG &expected, const std::string &name);

bool check_compare(const Z1_EXEONLYSECTR_BITS &obj, const Z1_EXEONLYSECTR_BITS &expected, const std::string &name);

bool check_compare(const Z1_EXEONLYSECTR_REG &obj, const Z1_EXEONLYSECTR_REG &expected, const std::string &name);

bool check_compare(const Z1_EXEONLYRAMR_BITS &obj, const Z1_EXEONLYRAMR_BITS &expected, const std::string &name);

bool check_compare(const Z1_EXEONLYRAMR_REG &obj, const Z1_EXEONLYRAMR_REG &expected, const std::string &name);

bool check_compare(const DCSM_Z1_REGS &obj, const DCSM_Z1_REGS &expected, const std::string &name);

bool check_compare(const Z2_LINKPOINTER_BITS &obj, const Z2_LINKPOINTER_BITS &expected, const std::string &name);

bool check_compare(const Z2_LINKPOINTER_REG &obj, const Z2_LINKPOINTER_REG &expected, const std::string &name);

bool check_compare(const Z2_OTPSECLOCK_BITS &obj, const Z2_OTPSECLOCK_BITS &expected, const std::string &name);

bool check_compare(const Z2_OTPSECLOCK_REG &obj, const Z2_OTPSECLOCK_REG &expected, const std::string &name);

bool check_compare(const Z2_BOOTCTRL_BITS &obj, const Z2_BOOTCTRL_BITS &expected, const std::string &name);

bool check_compare(const Z2_BOOTCTRL_REG &obj, const Z2_BOOTCTRL_REG &expected, const std::string &name);

bool check_compare(const Z2_CR_BITS &obj, const Z2_CR_BITS &expected, const std::string &name);

bool check_compare(const Z2_CR_REG &obj, const Z2_CR_REG &expected, const std::string &name);

bool check_compare(const Z2_GRABSECTR_BITS &obj, const Z2_GRABSECTR_BITS &expected, const std::string &name);

bool check_compare(const Z2_GRABSECTR_REG &obj, const Z2_GRABSECTR_REG &expected, const std::string &name);

bool check_compare(const Z2_GRABRAMR_BITS &obj, const Z2_GRABRAMR_BITS &expected, const std::string &name);

bool check_compare(const Z2_GRABRAMR_REG &obj, const Z2_GRABRAMR_REG &expected, const std::string &name);

bool check_compare(const Z2_EXEONLYSECTR_BITS &obj, const Z2_EXEONLYSECTR_BITS &expected, const std::string &name);

bool check_compare(const Z2_EXEONLYSECTR_REG &obj, const Z2_EXEONLYSECTR_REG &expected, const std::string &name);

bool check_compare(const Z2_EXEONLYRAMR_BITS &obj, const Z2_EXEONLYRAMR_BITS &expected, const std::string &name);

bool check_compare(const Z2_EXEONLYRAMR_REG &obj, const Z2_EXEONLYRAMR_REG &expected, const std::string &name);

bool check_compare(const DCSM_Z2_REGS &obj, const DCSM_Z2_REGS &expected, const std::string &name);

bool check_compare(const FLSEM_BITS &obj, const FLSEM_BITS &expected, const std::string &name);

bool check_compare(const FLSEM_REG &obj, const FLSEM_REG &expected, const std::string &name);

bool check_compare(const SECTSTAT_BITS &obj, const SECTSTAT_BITS &expected, const std::string &name);

bool check_compare(const SECTSTAT_REG &obj, const SECTSTAT_REG &expected, const std::string &name);

bool check_compare(const RAMSTAT_BITS &obj, const RAMSTAT_BITS &expected, const std::string &name);

bool check_compare(const RAMSTAT_REG &obj, const RAMSTAT_REG &expected, const std::string &name);

bool check_compare(const DCSM_COMMON_REGS &obj, const DCSM_COMMON_REGS &expected, const std::string &name);

bool check_compare(const MODE_BITS &obj, const MODE_BITS &expected, const std::string &name);

bool check_compare(const MODE_REG &obj, const MODE_REG &expected, const std::string &name);

bool check_compare(const CONTROL_BITS &obj, const CONTROL_BITS &expected, const std::string &name);

bool check_compare(const CONTROL_REG &obj, const CONTROL_REG &expected, const std::string &name);

bool check_compare(const BURST_SIZE_BITS &obj, const BURST_SIZE_BITS &expected, const std::string &name);

bool check_compare(const BURST_SIZE_REG &obj, const BURST_SIZE_REG &expected, const std::string &name);

bool check_compare(const BURST_COUNT_BITS &obj, const BURST_COUNT_BITS &expected, const std::string &name);

bool check_compare(const BURST_COUNT_REG &obj, const BURST_COUNT_REG &expected, const std::string &name);

bool check_compare(const CH_REGS &obj, const CH_REGS &expected, const std::string &name);

bool check_compare(const DMACTRL_BITS &obj, const DMACTRL_BITS &expected, const std::string &name);

bool check_compare(const DMACTRL_REG &obj, const DMACTRL_REG &expected, const std::string &name);

bool check_compare(const DEBUGCTRL_BITS &obj, const DEBUGCTRL_BITS &expected, const std::string &name);

bool check_compare(const DEBUGCTRL_REG &obj, const DEBUGCTRL_REG &expected, const std::string &name);

bool check_compare(const PRIORITYCTRL1_BITS &obj, const PRIORITYCTRL1_BITS &expected, const std::string &name);

bool check_compare(const PRIORITYCTRL1_REG &obj, const PRIORITYCTRL1_REG &expected, const std::string &name);

bool check_compare(const PRIORITYSTAT_BITS &obj, const PRIORITYSTAT_BITS &expected, const std::string &name);

bool check_compare(const PRIORITYSTAT_REG &obj, const PRIORITYSTAT_REG &expected, const std::string &name);

bool check_compare(const DMA_REGS &obj, const DMA_REGS &expected, const std::string &name);

bool check_compare(const ECCTL1_BITS &obj, const ECCTL1_BITS &expected, const std::string &name);

bool check_compare(const ECCTL1_REG &obj, const ECCTL1_REG &expected, const std::string &name);

bool check_compare(const ECCTL2_BITS &obj, const ECCTL2_BITS &expected, const std::string &name);

bool check_compare(const ECCTL2_REG &obj, const ECCTL2_REG &expected, const std::string &name);

bool check_compare(const ECEINT_BITS &obj, const ECEINT_BITS &expected, const std::string &name);

bool check_compare(const ECEINT_REG &obj, const ECEINT_REG &expected, const std::string &name);

bool check_compare(const ECFLG_BITS &obj, const ECFLG_BITS &expected, const std::string &name);

bool check_compare(const ECFLG_REG &obj, const ECFLG_REG &expected, const std::string &name);

bool check_compare(const ECCLR_BITS &obj, const ECCLR_BITS &expected, const std::string &name);

bool check_compare(const ECCLR_REG &obj, const ECCLR_REG &expected, const std::string &name);

bool check_compare(const ECFRC_BITS &obj, const ECFRC_BITS &expected, const std::string &name);

bool check_compare(const ECFRC_REG &obj, const ECFRC_REG &expected, const std::string &name);

bool check_compare(const ECAP_REGS &obj, const ECAP_REGS &expected, const std::string &name);

bool check_compare(const RCSR_BITS &obj, const RCSR_BITS &expected, const std::string &name);

bool check_compare(const RCSR_REG &obj, const RCSR_REG &expected, const std::string &name);

bool check_compare(const ASYNC_WCCR_BITS &obj, const ASYNC_WCCR_BITS &expected, const std::string &name);

bool check_compare(const ASYNC_WCCR_REG &obj, const ASYNC_WCCR_REG &expected, const std::string &name);

bool check_compare(const SDRAM_CR_BITS &obj, const SDRAM_CR_BITS &expected, const std::string &name);

bool check_compare(const SDRAM_CR_REG &obj, const SDRAM_CR_REG &expected, const std::string &name);

bool check_compare(const SDRAM_RCR_BITS &obj, const SDRAM_RCR_BITS &expected, const std::string &name);

bool check_compare(const SDRAM_RCR_REG &obj, const SDRAM_RCR_REG &expected, const std::string &name);

bool check_compare(const ASYNC_CS2_CR_BITS &obj, const ASYNC_CS2_CR_BITS &expected, const std::string &name);

bool check_compare(const ASYNC_CS2_CR_REG &obj, const ASYNC_CS2_CR_REG &expected, const std::string &name);

bool check_compare(const ASYNC_CS3_CR_BITS &obj, const ASYNC_CS3_CR_BITS &expected, const std::string &name);

bool check_compare(const ASYNC_CS3_CR_REG &obj, const ASYNC_CS3_CR_REG &expected, const std::string &name);

bool check_compare(const ASYNC_CS4_CR_BITS &obj, const ASYNC_CS4_CR_BITS &expected, const std::string &name);

bool check_compare(const ASYNC_CS4_CR_REG &obj, const ASYNC_CS4_CR_REG &expected, const std::string &name);

bool check_compare(const SDRAM_TR_BITS &obj, const SDRAM_TR_BITS &expected, const std::string &name);

bool check_compare(const SDRAM_TR_REG &obj, const SDRAM_TR_REG &expected, const std::string &name);

bool check_compare(const SDR_EXT_TMNG_BITS &obj, const SDR_EXT_TMNG_BITS &expected, const std::string &name);

bool check_compare(const SDR_EXT_TMNG_REG &obj, const SDR_EXT_TMNG_REG &expected, const std::string &name);

bool check_compare(const INT_RAW_BITS &obj, const INT_RAW_BITS &expected, const std::string &name);

bool check_compare(const INT_RAW_REG &obj, const INT_RAW_REG &expected, const std::string &name);

bool check_compare(const INT_MSK_BITS &obj, const INT_MSK_BITS &expected, const std::string &name);

bool check_compare(const INT_MSK_REG &obj, const INT_MSK_REG &expected, const std::string &name);

bool check_compare(const INT_MSK_SET_BITS &obj, const INT_MSK_SET_BITS &expected, const std::string &name);

bool check_compare(const INT_MSK_SET_REG &obj, const INT_MSK_SET_REG &expected, const std::string &name);

bool check_compare(const INT_MSK_CLR_BITS &obj, const INT_MSK_CLR_BITS &expected, const std::string &name);

bool check_compare(const INT_MSK_CLR_REG &obj, const INT_MSK_CLR_REG &expected, const std::string &name);

bool check_compare(const EMIF_REGS &obj, const EMIF_REGS &expected, const std::string &name);

bool check_compare(const TBCTL_BITS &obj, const TBCTL_BITS &expected, const std::string &name);

bool check_compare(const TBCTL_REG &obj, const TBCTL_REG &expected, const std::string &name);

bool check_compare(const TBCTL2_BITS &obj, const TBCTL2_BITS &expected, const std::string &name);

bool check_compare(const TBCTL2_REG &obj, const TBCTL2_REG &expected, const std::string &name);

bool check_compare(const TBSTS_BITS &obj, const TBSTS_BITS &expected, const std::string &name);

bool check_compare(const TBSTS_REG &obj, const TBSTS_REG &expected, const std::string &name);

bool check_compare(const CMPCTL_BITS &obj, const CMPCTL_BITS &expected, const std::string &name);

bool check_compare(const CMPCTL_REG &obj, const CMPCTL_REG &expected, const std::string &name);

bool check_compare(const CMPCTL2_BITS &obj, const CMPCTL2_BITS &expected, const std::string &name);

bool check_compare(const CMPCTL2_REG &obj, const CMPCTL2_REG &expected, const std::string &name);

bool check_compare(const DBCTL_BITS &obj, const DBCTL_BITS &expected, const std::string &name);

bool check_compare(const DBCTL_REG &obj, const DBCTL_REG &expected, const std::string &name);

bool check_compare(const DBCTL2_BITS &obj, const DBCTL2_BITS &expected, const std::string &name);

bool check_compare(const DBCTL2_REG &obj, const DBCTL2_REG &expected, const std::string &name);

bool check_compare(const AQCTL_BITS &obj, const AQCTL_BITS &expected, const std::string &name);

bool check_compare(const AQCTL_REG &obj, const AQCTL_REG &expected, const std::string &name);

bool check_compare(const AQTSRCSEL_BITS &obj, const AQTSRCSEL_BITS &expected, const std::string &name);

bool check_compare(const AQTSRCSEL_REG &obj, const AQTSRCSEL_REG &expected, const std::string &name);

bool check_compare(const PCCTL_BITS &obj, const PCCTL_BITS &expected, const std::string &name);

bool check_compare(const PCCTL_REG &obj, const PCCTL_REG &expected, const std::string &name);

bool check_compare(const VCAPCTL_BITS &obj, const VCAPCTL_BITS &expected, const std::string &name);

bool check_compare(const VCAPCTL_REG &obj, const VCAPCTL_REG &expected, const std::string &name);

bool check_compare(const VCNTCFG_BITS &obj, const VCNTCFG_BITS &expected, const std::string &name);

bool check_compare(const VCNTCFG_REG &obj, const VCNTCFG_REG &expected, const std::string &name);

bool check_compare(const HRCNFG_BITS &obj, const HRCNFG_BITS &expected, const std::string &name);

bool check_compare(const HRCNFG_REG &obj, const HRCNFG_REG &expected, const std::string &name);

bool check_compare(const HRPWR_BITS &obj, const HRPWR_BITS &expected, const std::string &name);

bool check_compare(const HRPWR_REG &obj, const HRPWR_REG &expected, const std::string &name);

bool check_compare(const HRMSTEP_BITS &obj, const HRMSTEP_BITS &expected, const std::string &name);

bool check_compare(const HRMSTEP_REG &obj, const HRMSTEP_REG &expected, const std::string &name);

bool check_compare(const HRCNFG2_BITS &obj, const HRCNFG2_BITS &expected, const std::string &name);

bool check_compare(const HRCNFG2_REG &obj, const HRCNFG2_REG &expected, const std::string &name);

bool check_compare(const HRPCTL_BITS &obj, const HRPCTL_BITS &expected, const std::string &name);

bool check_compare(const HRPCTL_REG &obj, const HRPCTL_REG &expected, const std::string &name);

bool check_compare(const TRREM_BITS &obj, const TRREM_BITS &expected, const std::string &name);

bool check_compare(const TRREM_REG &obj, const TRREM_REG &expected, const std::string &name);

bool check_compare(const GLDCTL_BITS &obj, const GLDCTL_BITS &expected, const std::string &name);

bool check_compare(const GLDCTL_REG &obj, const GLDCTL_REG &expected, const std::string &name);

bool check_compare(const GLDCFG_BITS &obj, const GLDCFG_BITS &expected, const std::string &name);

bool check_compare(const GLDCFG_REG &obj, const GLDCFG_REG &expected, const std::string &name);

bool check_compare(const EPWMXLINK_BITS &obj, const EPWMXLINK_BITS &expected, const std::string &name);

bool check_compare(const EPWMXLINK_REG &obj, const EPWMXLINK_REG &expected, const std::string &name);

bool check_compare(const AQCTLA_BITS &obj, const AQCTLA_BITS &expected, const std::string &name);

bool check_compare(const AQCTLA_REG &obj, const AQCTLA_REG &expected, const std::string &name);

bool check_compare(const AQCTLA2_BITS &obj, const AQCTLA2_BITS &expected, const std::string &name);

bool check_compare(const AQCTLA2_REG &obj, const AQCTLA2_REG &expected, const std::string &name);

bool check_compare(const AQCTLB_BITS &obj, const AQCTLB_BITS &expected, const std::string &name);

bool check_compare(const AQCTLB_REG &obj, const AQCTLB_REG &expected, const std::string &name);

bool check_compare(const AQCTLB2_BITS &obj, const AQCTLB2_BITS &expected, const std::string &name);

bool check_compare(const AQCTLB2_REG &obj, const AQCTLB2_REG &expected, const std::string &name);

bool check_compare(const AQSFRC_BITS &obj, const AQSFRC_BITS &expected, const std::string &name);

bool check_compare(const AQSFRC_REG &obj, const AQSFRC_REG &expected, const std::string &name);

bool check_compare(const AQCSFRC_BITS &obj, const AQCSFRC_BITS &expected, const std::string &name);

bool check_compare(const AQCSFRC_REG &obj, const AQCSFRC_REG &expected, const std::string &name);

bool check_compare(const DBREDHR_BITS &obj, const DBREDHR_BITS &expected, const std::string &name);

bool check_compare(const DBREDHR_REG &obj, const DBREDHR_REG &expected, const std::string &name);

bool check_compare(const DBRED_BITS &obj, const DBRED_BITS &expected, const std::string &name);

bool check_compare(const DBRED_REG &obj, const DBRED_REG &expected, const std::string &name);

bool check_compare(const DBFEDHR_BITS &obj, const DBFEDHR_BITS &expected, const std::string &name);

bool check_compare(const DBFEDHR_REG &obj, const DBFEDHR_REG &expected, const std::string &name);

bool check_compare(const DBFED_BITS &obj, const DBFED_BITS &expected, const std::string &name);

bool check_compare(const DBFED_REG &obj, const DBFED_REG &expected, const std::string &name);

bool check_compare(const TBPHS_BITS &obj, const TBPHS_BITS &expected, const std::string &name);

bool check_compare(const TBPHS_REG &obj, const TBPHS_REG &expected, const std::string &name);

bool check_compare(const CMPA_BITS &obj, const CMPA_BITS &expected, const std::string &name);

bool check_compare(const CMPA_REG &obj, const CMPA_REG &expected, const std::string &name);

bool check_compare(const CMPB_BITS &obj, const CMPB_BITS &expected, const std::string &name);

bool check_compare(const CMPB_REG &obj, const CMPB_REG &expected, const std::string &name);

bool check_compare(const GLDCTL2_BITS &obj, const GLDCTL2_BITS &expected, const std::string &name);

bool check_compare(const GLDCTL2_REG &obj, const GLDCTL2_REG &expected, const std::string &name);

bool check_compare(const TZSEL_BITS &obj, const TZSEL_BITS &expected, const std::string &name);

bool check_compare(const TZSEL_REG &obj, const TZSEL_REG &expected, const std::string &name);

bool check_compare(const TZDCSEL_BITS &obj, const TZDCSEL_BITS &expected, const std::string &name);

bool check_compare(const TZDCSEL_REG &obj, const TZDCSEL_REG &expected, const std::string &name);

bool check_compare(const TZCTL_BITS &obj, const TZCTL_BITS &expected, const std::string &name);

bool check_compare(const TZCTL_REG &obj, const TZCTL_REG &expected, const std::string &name);

bool check_compare(const TZCTL2_BITS &obj, const TZCTL2_BITS &expected, const std::string &name);

bool check_compare(const TZCTL2_REG &obj, const TZCTL2_REG &expected, const std::string &name);

bool check_compare(const TZCTLDCA_BITS &obj, const TZCTLDCA_BITS &expected, const std::string &name);

bool check_compare(const TZCTLDCA_REG &obj, const TZCTLDCA_REG &expected, const std::string &name);

bool check_compare(const TZCTLDCB_BITS &obj, const TZCTLDCB_BITS &expected, const std::string &name);

bool check_compare(const TZCTLDCB_REG &obj, const TZCTLDCB_REG &expected, const std::string &name);

bool check_compare(const TZEINT_BITS &obj, const TZEINT_BITS &expected, const std::string &name);

bool check_compare(const TZEINT_REG &obj, const TZEINT_REG &expected, const std::string &name);

bool check_compare(const TZFLG_BITS &obj, const TZFLG_BITS &expected, const std::string &name);

bool check_compare(const TZFLG_REG &obj, const TZFLG_REG &expected, const std::string &name);

bool check_compare(const TZCBCFLG_BITS &obj, const TZCBCFLG_BITS &expected, const std::string &name);

bool check_compare(const TZCBCFLG_REG &obj, const TZCBCFLG_REG &expected, const std::string &name);

bool check_compare(const TZOSTFLG_BITS &obj, const TZOSTFLG_BITS &expected, const std::string &name);

bool check_compare(const TZOSTFLG_REG &obj, const TZOSTFLG_REG &expected, const std::string &name);

bool check_compare(const TZCLR_BITS &obj, const TZCLR_BITS &expected, const std::string &name);

bool check_compare(const TZCLR_REG &obj, const TZCLR_REG &expected, const std::string &name);

bool check_compare(const TZCBCCLR_BITS &obj, const TZCBCCLR_BITS &expected, const std::string &name);

bool check_compare(const TZCBCCLR_REG &obj, const TZCBCCLR_REG &expected, const std::string &name);

bool check_compare(const TZOSTCLR_BITS &obj, const TZOSTCLR_BITS &expected, const std::string &name);

bool check_compare(const TZOSTCLR_REG &obj, const TZOSTCLR_REG &expected, const std::string &name);

bool check_compare(const TZFRC_BITS &obj, const TZFRC_BITS &expected, const std::string &name);

bool check_compare(const TZFRC_REG &obj, const TZFRC_REG &expected, const std::string &name);

bool check_compare(const ETSEL_BITS &obj, const ETSEL_BITS &expected, const std::string &name);

bool check_compare(const ETSEL_REG &obj, const ETSEL_REG &expected, const std::string &name);

bool check_compare(const ETPS_BITS &obj, const ETPS_BITS &expected, const std::string &name);

bool check_compare(const ETPS_REG &obj, const ETPS_REG &expected, const std::string &name);

bool check_compare(const ETFLG_BITS &obj, const ETFLG_BITS &expected, const std::string &name);

bool check_compare(const ETFLG_REG &obj, const ETFLG_REG &expected, const std::string &name);

bool check_compare(const ETCLR_BITS &obj, const ETCLR_BITS &expected, const std::string &name);

bool check_compare(const ETCLR_REG &obj, const ETCLR_REG &expected, const std::string &name);

bool check_compare(const ETFRC_BITS &obj, const ETFRC_BITS &expected, const std::string &name);

bool check_compare(const ETFRC_REG &obj, const ETFRC_REG &expected, const std::string &name);

bool check_compare(const ETINTPS_BITS &obj, const ETINTPS_BITS &expected, const std::string &name);

bool check_compare(const ETINTPS_REG &obj, const ETINTPS_REG &expected, const std::string &name);

bool check_compare(const ETSOCPS_BITS &obj, const ETSOCPS_BITS &expected, const std::string &name);

bool check_compare(const ETSOCPS_REG &obj, const ETSOCPS_REG &expected, const std::string &name);

bool check_compare(const ETCNTINITCTL_BITS &obj, const ETCNTINITCTL_BITS &expected, const std::string &name);

bool check_compare(const ETCNTINITCTL_REG &obj, const ETCNTINITCTL_REG &expected, const std::string &name);

bool check_compare(const ETCNTINIT_BITS &obj, const ETCNTINIT_BITS &expected, const std::string &name);

bool check_compare(const ETCNTINIT_REG &obj, const ETCNTINIT_REG &expected, const std::string &name);

bool check_compare(const DCTRIPSEL_BITS &obj, const DCTRIPSEL_BITS &expected, const std::string &name);

bool check_compare(const DCTRIPSEL_REG &obj, const DCTRIPSEL_REG &expected, const std::string &name);

bool check_compare(const DCACTL_BITS &obj, const DCACTL_BITS &expected, const std::string &name);

bool check_compare(const DCACTL_REG &obj, const DCACTL_REG &expected, const std::string &name);

bool check_compare(const DCBCTL_BITS &obj, const DCBCTL_BITS &expected, const std::string &name);

bool check_compare(const DCBCTL_REG &obj, const DCBCTL_REG &expected, const std::string &name);

bool check_compare(const DCFCTL_BITS &obj, const DCFCTL_BITS &expected, const std::string &name);

bool check_compare(const DCFCTL_REG &obj, const DCFCTL_REG &expected, const std::string &name);

bool check_compare(const DCCAPCTL_BITS &obj, const DCCAPCTL_BITS &expected, const std::string &name);

bool check_compare(const DCCAPCTL_REG &obj, const DCCAPCTL_REG &expected, const std::string &name);

bool check_compare(const DCAHTRIPSEL_BITS &obj, const DCAHTRIPSEL_BITS &expected, const std::string &name);

bool check_compare(const DCAHTRIPSEL_REG &obj, const DCAHTRIPSEL_REG &expected, const std::string &name);

bool check_compare(const DCALTRIPSEL_BITS &obj, const DCALTRIPSEL_BITS &expected, const std::string &name);

bool check_compare(const DCALTRIPSEL_REG &obj, const DCALTRIPSEL_REG &expected, const std::string &name);

bool check_compare(const DCBHTRIPSEL_BITS &obj, const DCBHTRIPSEL_BITS &expected, const std::string &name);

bool check_compare(const DCBHTRIPSEL_REG &obj, const DCBHTRIPSEL_REG &expected, const std::string &name);

bool check_compare(const DCBLTRIPSEL_BITS &obj, const DCBLTRIPSEL_BITS &expected, const std::string &name);

bool check_compare(const DCBLTRIPSEL_REG &obj, const DCBLTRIPSEL_REG &expected, const std::string &name);

bool check_compare(const EPWM_REGS &obj, const EPWM_REGS &expected, const std::string &name);

bool check_compare(const TRIP4MUX0TO15CFG_BITS &obj, const TRIP4MUX0TO15CFG_BITS &expected, const std::string &name);

bool check_compare(const TRIP4MUX0TO15CFG_REG &obj, const TRIP4MUX0TO15CFG_REG &expected, const std::string &name);

bool check_compare(const TRIP4MUX16TO31CFG_BITS &obj, const TRIP4MUX16TO31CFG_BITS &expected, const std::string &name);

bool check_compare(const TRIP4MUX16TO31CFG_REG &obj, const TRIP4MUX16TO31CFG_REG &expected, const std::string &name);

bool check_compare(const TRIP5MUX0TO15CFG_BITS &obj, const TRIP5MUX0TO15CFG_BITS &expected, const std::string &name);

bool check_compare(const TRIP5MUX0TO15CFG_REG &obj, const TRIP5MUX0TO15CFG_REG &expected, const std::string &name);

bool check_compare(const TRIP5MUX16TO31CFG_BITS &obj, const TRIP5MUX16TO31CFG_BITS &expected, const std::string &name);

bool check_compare(const TRIP5MUX16TO31CFG_REG &obj, const TRIP5MUX16TO31CFG_REG &expected, const std::string &name);

bool check_compare(const TRIP7MUX0TO15CFG_BITS &obj, const TRIP7MUX0TO15CFG_BITS &expected, const std::string &name);

bool check_compare(const TRIP7MUX0TO15CFG_REG &obj, const TRIP7MUX0TO15CFG_REG &expected, const std::string &name);

bool check_compare(const TRIP7MUX16TO31CFG_BITS &obj, const TRIP7MUX16TO31CFG_BITS &expected, const std::string &name);

bool check_compare(const TRIP7MUX16TO31CFG_REG &obj, const TRIP7MUX16TO31CFG_REG &expected, const std::string &name);

bool check_compare(const TRIP8MUX0TO15CFG_BITS &obj, const TRIP8MUX0TO15CFG_BITS &expected, const std::string &name);

bool check_compare(const TRIP8MUX0TO15CFG_REG &obj, const TRIP8MUX0TO15CFG_REG &expected, const std::string &name);

bool check_compare(const TRIP8MUX16TO31CFG_BITS &obj, const TRIP8MUX16TO31CFG_BITS &expected, const std::string &name);

bool check_compare(const TRIP8MUX16TO31CFG_REG &obj, const TRIP8MUX16TO31CFG_REG &expected, const std::string &name);

bool check_compare(const TRIP9MUX0TO15CFG_BITS &obj, const TRIP9MUX0TO15CFG_BITS &expected, const std::string &name);

bool check_compare(const TRIP9MUX0TO15CFG_REG &obj, const TRIP9MUX0TO15CFG_REG &expected, const std::string &name);

bool check_compare(const TRIP9MUX16TO31CFG_BITS &obj, const TRIP9MUX16TO31CFG_BITS &expected, const std::string &name);

bool check_compare(const TRIP9MUX16TO31CFG_REG &obj, const TRIP9MUX16TO31CFG_REG &expected, const std::string &name);

bool check_compare(const TRIP10MUX0TO15CFG_BITS &obj, const TRIP10MUX0TO15CFG_BITS &expected, const std::string &name);

bool check_compare(const TRIP10MUX0TO15CFG_REG &obj, const TRIP10MUX0TO15CFG_REG &expected, const std::string &name);

bool check_compare(const TRIP10MUX16TO31CFG_BITS &obj, const TRIP10MUX16TO31CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const TRIP10MUX16TO31CFG_REG &obj, const TRIP10MUX16TO31CFG_REG &expected, const std::string &name);

bool check_compare(const TRIP11MUX0TO15CFG_BITS &obj, const TRIP11MUX0TO15CFG_BITS &expected, const std::string &name);

bool check_compare(const TRIP11MUX0TO15CFG_REG &obj, const TRIP11MUX0TO15CFG_REG &expected, const std::string &name);

bool check_compare(const TRIP11MUX16TO31CFG_BITS &obj, const TRIP11MUX16TO31CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const TRIP11MUX16TO31CFG_REG &obj, const TRIP11MUX16TO31CFG_REG &expected, const std::string &name);

bool check_compare(const TRIP12MUX0TO15CFG_BITS &obj, const TRIP12MUX0TO15CFG_BITS &expected, const std::string &name);

bool check_compare(const TRIP12MUX0TO15CFG_REG &obj, const TRIP12MUX0TO15CFG_REG &expected, const std::string &name);

bool check_compare(const TRIP12MUX16TO31CFG_BITS &obj, const TRIP12MUX16TO31CFG_BITS &expected,
                   const std::string &name);

bool check_compare(const TRIP12MUX16TO31CFG_REG &obj, const TRIP12MUX16TO31CFG_REG &expected, const std::string &name);

bool check_compare(const TRIP4MUXENABLE_BITS &obj, const TRIP4MUXENABLE_BITS &expected, const std::string &name);

bool check_compare(const TRIP4MUXENABLE_REG &obj, const TRIP4MUXENABLE_REG &expected, const std::string &name);

bool check_compare(const TRIP5MUXENABLE_BITS &obj, const TRIP5MUXENABLE_BITS &expected, const std::string &name);

bool check_compare(const TRIP5MUXENABLE_REG &obj, const TRIP5MUXENABLE_REG &expected, const std::string &name);

bool check_compare(const TRIP7MUXENABLE_BITS &obj, const TRIP7MUXENABLE_BITS &expected, const std::string &name);

bool check_compare(const TRIP7MUXENABLE_REG &obj, const TRIP7MUXENABLE_REG &expected, const std::string &name);

bool check_compare(const TRIP8MUXENABLE_BITS &obj, const TRIP8MUXENABLE_BITS &expected, const std::string &name);

bool check_compare(const TRIP8MUXENABLE_REG &obj, const TRIP8MUXENABLE_REG &expected, const std::string &name);

bool check_compare(const TRIP9MUXENABLE_BITS &obj, const TRIP9MUXENABLE_BITS &expected, const std::string &name);

bool check_compare(const TRIP9MUXENABLE_REG &obj, const TRIP9MUXENABLE_REG &expected, const std::string &name);

bool check_compare(const TRIP10MUXENABLE_BITS &obj, const TRIP10MUXENABLE_BITS &expected, const std::string &name);

bool check_compare(const TRIP10MUXENABLE_REG &obj, const TRIP10MUXENABLE_REG &expected, const std::string &name);

bool check_compare(const TRIP11MUXENABLE_BITS &obj, const TRIP11MUXENABLE_BITS &expected, const std::string &name);

bool check_compare(const TRIP11MUXENABLE_REG &obj, const TRIP11MUXENABLE_REG &expected, const std::string &name);

bool check_compare(const TRIP12MUXENABLE_BITS &obj, const TRIP12MUXENABLE_BITS &expected, const std::string &name);

bool check_compare(const TRIP12MUXENABLE_REG &obj, const TRIP12MUXENABLE_REG &expected, const std::string &name);

bool check_compare(const TRIPOUTINV_BITS &obj, const TRIPOUTINV_BITS &expected, const std::string &name);

bool check_compare(const TRIPOUTINV_REG &obj, const TRIPOUTINV_REG &expected, const std::string &name);

bool check_compare(const TRIPLOCK_BITS &obj, const TRIPLOCK_BITS &expected, const std::string &name);

bool check_compare(const TRIPLOCK_REG &obj, const TRIPLOCK_REG &expected, const std::string &name);

bool check_compare(const EPWM_XBAR_REGS &obj, const EPWM_XBAR_REGS &expected, const std::string &name);

bool check_compare(const QDECCTL_BITS &obj, const QDECCTL_BITS &expected, const std::string &name);

bool check_compare(const QDECCTL_REG &obj, const QDECCTL_REG &expected, const std::string &name);

bool check_compare(const QEPCTL_BITS &obj, const QEPCTL_BITS &expected, const std::string &name);

bool check_compare(const QEPCTL_REG &obj, const QEPCTL_REG &expected, const std::string &name);

bool check_compare(const QCAPCTL_BITS &obj, const QCAPCTL_BITS &expected, const std::string &name);

bool check_compare(const QCAPCTL_REG &obj, const QCAPCTL_REG &expected, const std::string &name);

bool check_compare(const QPOSCTL_BITS &obj, const QPOSCTL_BITS &expected, const std::string &name);

bool check_compare(const QPOSCTL_REG &obj, const QPOSCTL_REG &expected, const std::string &name);

bool check_compare(const QEINT_BITS &obj, const QEINT_BITS &expected, const std::string &name);

bool check_compare(const QEINT_REG &obj, const QEINT_REG &expected, const std::string &name);

bool check_compare(const QFLG_BITS &obj, const QFLG_BITS &expected, const std::string &name);

bool check_compare(const QFLG_REG &obj, const QFLG_REG &expected, const std::string &name);

bool check_compare(const QCLR_BITS &obj, const QCLR_BITS &expected, const std::string &name);

bool check_compare(const QCLR_REG &obj, const QCLR_REG &expected, const std::string &name);

bool check_compare(const QFRC_BITS &obj, const QFRC_BITS &expected, const std::string &name);

bool check_compare(const QFRC_REG &obj, const QFRC_REG &expected, const std::string &name);

bool check_compare(const QEPSTS_BITS &obj, const QEPSTS_BITS &expected, const std::string &name);

bool check_compare(const QEPSTS_REG &obj, const QEPSTS_REG &expected, const std::string &name);

bool check_compare(const EQEP_REGS &obj, const EQEP_REGS &expected, const std::string &name);

bool check_compare(const FRDCNTL_BITS &obj, const FRDCNTL_BITS &expected, const std::string &name);

bool check_compare(const FRDCNTL_REG &obj, const FRDCNTL_REG &expected, const std::string &name);

bool check_compare(const FBAC_BITS &obj, const FBAC_BITS &expected, const std::string &name);

bool check_compare(const FBAC_REG &obj, const FBAC_REG &expected, const std::string &name);

bool check_compare(const FBFALLBACK_BITS &obj, const FBFALLBACK_BITS &expected, const std::string &name);

bool check_compare(const FBFALLBACK_REG &obj, const FBFALLBACK_REG &expected, const std::string &name);

bool check_compare(const FBPRDY_BITS &obj, const FBPRDY_BITS &expected, const std::string &name);

bool check_compare(const FBPRDY_REG &obj, const FBPRDY_REG &expected, const std::string &name);

bool check_compare(const FPAC1_BITS &obj, const FPAC1_BITS &expected, const std::string &name);

bool check_compare(const FPAC1_REG &obj, const FPAC1_REG &expected, const std::string &name);

bool check_compare(const FMSTAT_BITS &obj, const FMSTAT_BITS &expected, const std::string &name);

bool check_compare(const FMSTAT_REG &obj, const FMSTAT_REG &expected, const std::string &name);

bool check_compare(const FRD_INTF_CTRL_BITS &obj, const FRD_INTF_CTRL_BITS &expected, const std::string &name);

bool check_compare(const FRD_INTF_CTRL_REG &obj, const FRD_INTF_CTRL_REG &expected, const std::string &name);

bool check_compare(const FLASH_CTRL_REGS &obj, const FLASH_CTRL_REGS &expected, const std::string &name);

bool check_compare(const ECC_ENABLE_BITS &obj, const ECC_ENABLE_BITS &expected, const std::string &name);

bool check_compare(const ECC_ENABLE_REG &obj, const ECC_ENABLE_REG &expected, const std::string &name);

bool check_compare(const ERR_STATUS_BITS &obj, const ERR_STATUS_BITS &expected, const std::string &name);

bool check_compare(const ERR_STATUS_REG &obj, const ERR_STATUS_REG &expected, const std::string &name);

bool check_compare(const ERR_POS_BITS &obj, const ERR_POS_BITS &expected, const std::string &name);

bool check_compare(const ERR_POS_REG &obj, const ERR_POS_REG &expected, const std::string &name);

bool check_compare(const ERR_STATUS_CLR_BITS &obj, const ERR_STATUS_CLR_BITS &expected, const std::string &name);

bool check_compare(const ERR_STATUS_CLR_REG &obj, const ERR_STATUS_CLR_REG &expected, const std::string &name);

bool check_compare(const ERR_CNT_BITS &obj, const ERR_CNT_BITS &expected, const std::string &name);

bool check_compare(const ERR_CNT_REG &obj, const ERR_CNT_REG &expected, const std::string &name);

bool check_compare(const ERR_THRESHOLD_BITS &obj, const ERR_THRESHOLD_BITS &expected, const std::string &name);

bool check_compare(const ERR_THRESHOLD_REG &obj, const ERR_THRESHOLD_REG &expected, const std::string &name);

bool check_compare(const ERR_INTFLG_BITS &obj, const ERR_INTFLG_BITS &expected, const std::string &name);

bool check_compare(const ERR_INTFLG_REG &obj, const ERR_INTFLG_REG &expected, const std::string &name);

bool check_compare(const ERR_INTCLR_BITS &obj, const ERR_INTCLR_BITS &expected, const std::string &name);

bool check_compare(const ERR_INTCLR_REG &obj, const ERR_INTCLR_REG &expected, const std::string &name);

bool check_compare(const FADDR_TEST_BITS &obj, const FADDR_TEST_BITS &expected, const std::string &name);

bool check_compare(const FADDR_TEST_REG &obj, const FADDR_TEST_REG &expected, const std::string &name);

bool check_compare(const FECC_TEST_BITS &obj, const FECC_TEST_BITS &expected, const std::string &name);

bool check_compare(const FECC_TEST_REG &obj, const FECC_TEST_REG &expected, const std::string &name);

bool check_compare(const FECC_CTRL_BITS &obj, const FECC_CTRL_BITS &expected, const std::string &name);

bool check_compare(const FECC_CTRL_REG &obj, const FECC_CTRL_REG &expected, const std::string &name);

bool check_compare(const FECC_STATUS_BITS &obj, const FECC_STATUS_BITS &expected, const std::string &name);

bool check_compare(const FECC_STATUS_REG &obj, const FECC_STATUS_REG &expected, const std::string &name);

bool check_compare(const FLASH_ECC_REGS &obj, const FLASH_ECC_REGS &expected, const std::string &name);

bool check_compare(const PUMPREQUEST_BITS &obj, const PUMPREQUEST_BITS &expected, const std::string &name);

bool check_compare(const PUMPREQUEST_REG &obj, const PUMPREQUEST_REG &expected, const std::string &name);

bool check_compare(const FLASH_PUMP_SEMAPHORE_REGS &obj, const FLASH_PUMP_SEMAPHORE_REGS &expected,
                   const std::string &name);

bool check_compare(const GPACTRL_BITS &obj, const GPACTRL_BITS &expected, const std::string &name);

bool check_compare(const GPACTRL_REG &obj, const GPACTRL_REG &expected, const std::string &name);

bool check_compare(const GPAQSEL1_BITS &obj, const GPAQSEL1_BITS &expected, const std::string &name);

bool check_compare(const GPAQSEL1_REG &obj, const GPAQSEL1_REG &expected, const std::string &name);

bool check_compare(const GPAQSEL2_BITS &obj, const GPAQSEL2_BITS &expected, const std::string &name);

bool check_compare(const GPAQSEL2_REG &obj, const GPAQSEL2_REG &expected, const std::string &name);

bool check_compare(const GPAMUX1_BITS &obj, const GPAMUX1_BITS &expected, const std::string &name);

bool check_compare(const GPAMUX1_REG &obj, const GPAMUX1_REG &expected, const std::string &name);

bool check_compare(const GPAMUX2_BITS &obj, const GPAMUX2_BITS &expected, const std::string &name);

bool check_compare(const GPAMUX2_REG &obj, const GPAMUX2_REG &expected, const std::string &name);

bool check_compare(const GPADIR_BITS &obj, const GPADIR_BITS &expected, const std::string &name);

bool check_compare(const GPADIR_REG &obj, const GPADIR_REG &expected, const std::string &name);

bool check_compare(const GPAPUD_BITS &obj, const GPAPUD_BITS &expected, const std::string &name);

bool check_compare(const GPAPUD_REG &obj, const GPAPUD_REG &expected, const std::string &name);

bool check_compare(const GPAINV_BITS &obj, const GPAINV_BITS &expected, const std::string &name);

bool check_compare(const GPAINV_REG &obj, const GPAINV_REG &expected, const std::string &name);

bool check_compare(const GPAODR_BITS &obj, const GPAODR_BITS &expected, const std::string &name);

bool check_compare(const GPAODR_REG &obj, const GPAODR_REG &expected, const std::string &name);

bool check_compare(const GPAGMUX1_BITS &obj, const GPAGMUX1_BITS &expected, const std::string &name);

bool check_compare(const GPAGMUX1_REG &obj, const GPAGMUX1_REG &expected, const std::string &name);

bool check_compare(const GPAGMUX2_BITS &obj, const GPAGMUX2_BITS &expected, const std::string &name);

bool check_compare(const GPAGMUX2_REG &obj, const GPAGMUX2_REG &expected, const std::string &name);

bool check_compare(const GPACSEL1_BITS &obj, const GPACSEL1_BITS &expected, const std::string &name);

bool check_compare(const GPACSEL1_REG &obj, const GPACSEL1_REG &expected, const std::string &name);

bool check_compare(const GPACSEL2_BITS &obj, const GPACSEL2_BITS &expected, const std::string &name);

bool check_compare(const GPACSEL2_REG &obj, const GPACSEL2_REG &expected, const std::string &name);

bool check_compare(const GPACSEL3_BITS &obj, const GPACSEL3_BITS &expected, const std::string &name);

bool check_compare(const GPACSEL3_REG &obj, const GPACSEL3_REG &expected, const std::string &name);

bool check_compare(const GPACSEL4_BITS &obj, const GPACSEL4_BITS &expected, const std::string &name);

bool check_compare(const GPACSEL4_REG &obj, const GPACSEL4_REG &expected, const std::string &name);

bool check_compare(const GPALOCK_BITS &obj, const GPALOCK_BITS &expected, const std::string &name);

bool check_compare(const GPALOCK_REG &obj, const GPALOCK_REG &expected, const std::string &name);

bool check_compare(const GPACR_BITS &obj, const GPACR_BITS &expected, const std::string &name);

bool check_compare(const GPACR_REG &obj, const GPACR_REG &expected, const std::string &name);

bool check_compare(const GPBCTRL_BITS &obj, const GPBCTRL_BITS &expected, const std::string &name);

bool check_compare(const GPBCTRL_REG &obj, const GPBCTRL_REG &expected, const std::string &name);

bool check_compare(const GPBQSEL1_BITS &obj, const GPBQSEL1_BITS &expected, const std::string &name);

bool check_compare(const GPBQSEL1_REG &obj, const GPBQSEL1_REG &expected, const std::string &name);

bool check_compare(const GPBQSEL2_BITS &obj, const GPBQSEL2_BITS &expected, const std::string &name);

bool check_compare(const GPBQSEL2_REG &obj, const GPBQSEL2_REG &expected, const std::string &name);

bool check_compare(const GPBMUX1_BITS &obj, const GPBMUX1_BITS &expected, const std::string &name);

bool check_compare(const GPBMUX1_REG &obj, const GPBMUX1_REG &expected, const std::string &name);

bool check_compare(const GPBMUX2_BITS &obj, const GPBMUX2_BITS &expected, const std::string &name);

bool check_compare(const GPBMUX2_REG &obj, const GPBMUX2_REG &expected, const std::string &name);

bool check_compare(const GPBDIR_BITS &obj, const GPBDIR_BITS &expected, const std::string &name);

bool check_compare(const GPBDIR_REG &obj, const GPBDIR_REG &expected, const std::string &name);

bool check_compare(const GPBPUD_BITS &obj, const GPBPUD_BITS &expected, const std::string &name);

bool check_compare(const GPBPUD_REG &obj, const GPBPUD_REG &expected, const std::string &name);

bool check_compare(const GPBINV_BITS &obj, const GPBINV_BITS &expected, const std::string &name);

bool check_compare(const GPBINV_REG &obj, const GPBINV_REG &expected, const std::string &name);

bool check_compare(const GPBODR_BITS &obj, const GPBODR_BITS &expected, const std::string &name);

bool check_compare(const GPBODR_REG &obj, const GPBODR_REG &expected, const std::string &name);

bool check_compare(const GPBAMSEL_BITS &obj, const GPBAMSEL_BITS &expected, const std::string &name);

bool check_compare(const GPBAMSEL_REG &obj, const GPBAMSEL_REG &expected, const std::string &name);

bool check_compare(const GPBGMUX1_BITS &obj, const GPBGMUX1_BITS &expected, const std::string &name);

bool check_compare(const GPBGMUX1_REG &obj, const GPBGMUX1_REG &expected, const std::string &name);

bool check_compare(const GPBGMUX2_BITS &obj, const GPBGMUX2_BITS &expected, const std::string &name);

bool check_compare(const GPBGMUX2_REG &obj, const GPBGMUX2_REG &expected, const std::string &name);

bool check_compare(const GPBCSEL1_BITS &obj, const GPBCSEL1_BITS &expected, const std::string &name);

bool check_compare(const GPBCSEL1_REG &obj, const GPBCSEL1_REG &expected, const std::string &name);

bool check_compare(const GPBCSEL2_BITS &obj, const GPBCSEL2_BITS &expected, const std::string &name);

bool check_compare(const GPBCSEL2_REG &obj, const GPBCSEL2_REG &expected, const std::string &name);

bool check_compare(const GPBCSEL3_BITS &obj, const GPBCSEL3_BITS &expected, const std::string &name);

bool check_compare(const GPBCSEL3_REG &obj, const GPBCSEL3_REG &expected, const std::string &name);

bool check_compare(const GPBCSEL4_BITS &obj, const GPBCSEL4_BITS &expected, const std::string &name);

bool check_compare(const GPBCSEL4_REG &obj, const GPBCSEL4_REG &expected, const std::string &name);

bool check_compare(const GPBLOCK_BITS &obj, const GPBLOCK_BITS &expected, const std::string &name);

bool check_compare(const GPBLOCK_REG &obj, const GPBLOCK_REG &expected, const std::string &name);

bool check_compare(const GPBCR_BITS &obj, const GPBCR_BITS &expected, const std::string &name);

bool check_compare(const GPBCR_REG &obj, const GPBCR_REG &expected, const std::string &name);

bool check_compare(const GPCCTRL_BITS &obj, const GPCCTRL_BITS &expected, const std::string &name);

bool check_compare(const GPCCTRL_REG &obj, const GPCCTRL_REG &expected, const std::string &name);

bool check_compare(const GPCQSEL1_BITS &obj, const GPCQSEL1_BITS &expected, const std::string &name);

bool check_compare(const GPCQSEL1_REG &obj, const GPCQSEL1_REG &expected, const std::string &name);

bool check_compare(const GPCQSEL2_BITS &obj, const GPCQSEL2_BITS &expected, const std::string &name);

bool check_compare(const GPCQSEL2_REG &obj, const GPCQSEL2_REG &expected, const std::string &name);

bool check_compare(const GPCMUX1_BITS &obj, const GPCMUX1_BITS &expected, const std::string &name);

bool check_compare(const GPCMUX1_REG &obj, const GPCMUX1_REG &expected, const std::string &name);

bool check_compare(const GPCMUX2_BITS &obj, const GPCMUX2_BITS &expected, const std::string &name);

bool check_compare(const GPCMUX2_REG &obj, const GPCMUX2_REG &expected, const std::string &name);

bool check_compare(const GPCDIR_BITS &obj, const GPCDIR_BITS &expected, const std::string &name);

bool check_compare(const GPCDIR_REG &obj, const GPCDIR_REG &expected, const std::string &name);

bool check_compare(const GPCPUD_BITS &obj, const GPCPUD_BITS &expected, const std::string &name);

bool check_compare(const GPCPUD_REG &obj, const GPCPUD_REG &expected, const std::string &name);

bool check_compare(const GPCINV_BITS &obj, const GPCINV_BITS &expected, const std::string &name);

bool check_compare(const GPCINV_REG &obj, const GPCINV_REG &expected, const std::string &name);

bool check_compare(const GPCODR_BITS &obj, const GPCODR_BITS &expected, const std::string &name);

bool check_compare(const GPCODR_REG &obj, const GPCODR_REG &expected, const std::string &name);

bool check_compare(const GPCGMUX1_BITS &obj, const GPCGMUX1_BITS &expected, const std::string &name);

bool check_compare(const GPCGMUX1_REG &obj, const GPCGMUX1_REG &expected, const std::string &name);

bool check_compare(const GPCGMUX2_BITS &obj, const GPCGMUX2_BITS &expected, const std::string &name);

bool check_compare(const GPCGMUX2_REG &obj, const GPCGMUX2_REG &expected, const std::string &name);

bool check_compare(const GPCCSEL1_BITS &obj, const GPCCSEL1_BITS &expected, const std::string &name);

bool check_compare(const GPCCSEL1_REG &obj, const GPCCSEL1_REG &expected, const std::string &name);

bool check_compare(const GPCCSEL2_BITS &obj, const GPCCSEL2_BITS &expected, const std::string &name);

bool check_compare(const GPCCSEL2_REG &obj, const GPCCSEL2_REG &expected, const std::string &name);

bool check_compare(const GPCCSEL3_BITS &obj, const GPCCSEL3_BITS &expected, const std::string &name);

bool check_compare(const GPCCSEL3_REG &obj, const GPCCSEL3_REG &expected, const std::string &name);

bool check_compare(const GPCCSEL4_BITS &obj, const GPCCSEL4_BITS &expected, const std::string &name);

bool check_compare(const GPCCSEL4_REG &obj, const GPCCSEL4_REG &expected, const std::string &name);

bool check_compare(const GPCLOCK_BITS &obj, const GPCLOCK_BITS &expected, const std::string &name);

bool check_compare(const GPCLOCK_REG &obj, const GPCLOCK_REG &expected, const std::string &name);

bool check_compare(const GPCCR_BITS &obj, const GPCCR_BITS &expected, const std::string &name);

bool check_compare(const GPCCR_REG &obj, const GPCCR_REG &expected, const std::string &name);

bool check_compare(const GPDCTRL_BITS &obj, const GPDCTRL_BITS &expected, const std::string &name);

bool check_compare(const GPDCTRL_REG &obj, const GPDCTRL_REG &expected, const std::string &name);

bool check_compare(const GPDQSEL1_BITS &obj, const GPDQSEL1_BITS &expected, const std::string &name);

bool check_compare(const GPDQSEL1_REG &obj, const GPDQSEL1_REG &expected, const std::string &name);

bool check_compare(const GPDQSEL2_BITS &obj, const GPDQSEL2_BITS &expected, const std::string &name);

bool check_compare(const GPDQSEL2_REG &obj, const GPDQSEL2_REG &expected, const std::string &name);

bool check_compare(const GPDMUX1_BITS &obj, const GPDMUX1_BITS &expected, const std::string &name);

bool check_compare(const GPDMUX1_REG &obj, const GPDMUX1_REG &expected, const std::string &name);

bool check_compare(const GPDMUX2_BITS &obj, const GPDMUX2_BITS &expected, const std::string &name);

bool check_compare(const GPDMUX2_REG &obj, const GPDMUX2_REG &expected, const std::string &name);

bool check_compare(const GPDDIR_BITS &obj, const GPDDIR_BITS &expected, const std::string &name);

bool check_compare(const GPDDIR_REG &obj, const GPDDIR_REG &expected, const std::string &name);

bool check_compare(const GPDPUD_BITS &obj, const GPDPUD_BITS &expected, const std::string &name);

bool check_compare(const GPDPUD_REG &obj, const GPDPUD_REG &expected, const std::string &name);

bool check_compare(const GPDINV_BITS &obj, const GPDINV_BITS &expected, const std::string &name);

bool check_compare(const GPDINV_REG &obj, const GPDINV_REG &expected, const std::string &name);

bool check_compare(const GPDODR_BITS &obj, const GPDODR_BITS &expected, const std::string &name);

bool check_compare(const GPDODR_REG &obj, const GPDODR_REG &expected, const std::string &name);

bool check_compare(const GPDGMUX1_BITS &obj, const GPDGMUX1_BITS &expected, const std::string &name);

bool check_compare(const GPDGMUX1_REG &obj, const GPDGMUX1_REG &expected, const std::string &name);

bool check_compare(const GPDGMUX2_BITS &obj, const GPDGMUX2_BITS &expected, const std::string &name);

bool check_compare(const GPDGMUX2_REG &obj, const GPDGMUX2_REG &expected, const std::string &name);

bool check_compare(const GPDCSEL1_BITS &obj, const GPDCSEL1_BITS &expected, const std::string &name);

bool check_compare(const GPDCSEL1_REG &obj, const GPDCSEL1_REG &expected, const std::string &name);

bool check_compare(const GPDCSEL2_BITS &obj, const GPDCSEL2_BITS &expected, const std::string &name);

bool check_compare(const GPDCSEL2_REG &obj, const GPDCSEL2_REG &expected, const std::string &name);

bool check_compare(const GPDCSEL3_BITS &obj, const GPDCSEL3_BITS &expected, const std::string &name);

bool check_compare(const GPDCSEL3_REG &obj, const GPDCSEL3_REG &expected, const std::string &name);

bool check_compare(const GPDCSEL4_BITS &obj, const GPDCSEL4_BITS &expected, const std::string &name);

bool check_compare(const GPDCSEL4_REG &obj, const GPDCSEL4_REG &expected, const std::string &name);

bool check_compare(const GPDLOCK_BITS &obj, const GPDLOCK_BITS &expected, const std::string &name);

bool check_compare(const GPDLOCK_REG &obj, const GPDLOCK_REG &expected, const std::string &name);

bool check_compare(const GPDCR_BITS &obj, const GPDCR_BITS &expected, const std::string &name);

bool check_compare(const GPDCR_REG &obj, const GPDCR_REG &expected, const std::string &name);

bool check_compare(const GPECTRL_BITS &obj, const GPECTRL_BITS &expected, const std::string &name);

bool check_compare(const GPECTRL_REG &obj, const GPECTRL_REG &expected, const std::string &name);

bool check_compare(const GPEQSEL1_BITS &obj, const GPEQSEL1_BITS &expected, const std::string &name);

bool check_compare(const GPEQSEL1_REG &obj, const GPEQSEL1_REG &expected, const std::string &name);

bool check_compare(const GPEQSEL2_BITS &obj, const GPEQSEL2_BITS &expected, const std::string &name);

bool check_compare(const GPEQSEL2_REG &obj, const GPEQSEL2_REG &expected, const std::string &name);

bool check_compare(const GPEMUX1_BITS &obj, const GPEMUX1_BITS &expected, const std::string &name);

bool check_compare(const GPEMUX1_REG &obj, const GPEMUX1_REG &expected, const std::string &name);

bool check_compare(const GPEMUX2_BITS &obj, const GPEMUX2_BITS &expected, const std::string &name);

bool check_compare(const GPEMUX2_REG &obj, const GPEMUX2_REG &expected, const std::string &name);

bool check_compare(const GPEDIR_BITS &obj, const GPEDIR_BITS &expected, const std::string &name);

bool check_compare(const GPEDIR_REG &obj, const GPEDIR_REG &expected, const std::string &name);

bool check_compare(const GPEPUD_BITS &obj, const GPEPUD_BITS &expected, const std::string &name);

bool check_compare(const GPEPUD_REG &obj, const GPEPUD_REG &expected, const std::string &name);

bool check_compare(const GPEINV_BITS &obj, const GPEINV_BITS &expected, const std::string &name);

bool check_compare(const GPEINV_REG &obj, const GPEINV_REG &expected, const std::string &name);

bool check_compare(const GPEODR_BITS &obj, const GPEODR_BITS &expected, const std::string &name);

bool check_compare(const GPEODR_REG &obj, const GPEODR_REG &expected, const std::string &name);

bool check_compare(const GPEGMUX1_BITS &obj, const GPEGMUX1_BITS &expected, const std::string &name);

bool check_compare(const GPEGMUX1_REG &obj, const GPEGMUX1_REG &expected, const std::string &name);

bool check_compare(const GPEGMUX2_BITS &obj, const GPEGMUX2_BITS &expected, const std::string &name);

bool check_compare(const GPEGMUX2_REG &obj, const GPEGMUX2_REG &expected, const std::string &name);

bool check_compare(const GPECSEL1_BITS &obj, const GPECSEL1_BITS &expected, const std::string &name);

bool check_compare(const GPECSEL1_REG &obj, const GPECSEL1_REG &expected, const std::string &name);

bool check_compare(const GPECSEL2_BITS &obj, const GPECSEL2_BITS &expected, const std::string &name);

bool check_compare(const GPECSEL2_REG &obj, const GPECSEL2_REG &expected, const std::string &name);

bool check_compare(const GPECSEL3_BITS &obj, const GPECSEL3_BITS &expected, const std::string &name);

bool check_compare(const GPECSEL3_REG &obj, const GPECSEL3_REG &expected, const std::string &name);

bool check_compare(const GPECSEL4_BITS &obj, const GPECSEL4_BITS &expected, const std::string &name);

bool check_compare(const GPECSEL4_REG &obj, const GPECSEL4_REG &expected, const std::string &name);

bool check_compare(const GPELOCK_BITS &obj, const GPELOCK_BITS &expected, const std::string &name);

bool check_compare(const GPELOCK_REG &obj, const GPELOCK_REG &expected, const std::string &name);

bool check_compare(const GPECR_BITS &obj, const GPECR_BITS &expected, const std::string &name);

bool check_compare(const GPECR_REG &obj, const GPECR_REG &expected, const std::string &name);

bool check_compare(const GPFCTRL_BITS &obj, const GPFCTRL_BITS &expected, const std::string &name);

bool check_compare(const GPFCTRL_REG &obj, const GPFCTRL_REG &expected, const std::string &name);

bool check_compare(const GPFQSEL1_BITS &obj, const GPFQSEL1_BITS &expected, const std::string &name);

bool check_compare(const GPFQSEL1_REG &obj, const GPFQSEL1_REG &expected, const std::string &name);

bool check_compare(const GPFMUX1_BITS &obj, const GPFMUX1_BITS &expected, const std::string &name);

bool check_compare(const GPFMUX1_REG &obj, const GPFMUX1_REG &expected, const std::string &name);

bool check_compare(const GPFDIR_BITS &obj, const GPFDIR_BITS &expected, const std::string &name);

bool check_compare(const GPFDIR_REG &obj, const GPFDIR_REG &expected, const std::string &name);

bool check_compare(const GPFPUD_BITS &obj, const GPFPUD_BITS &expected, const std::string &name);

bool check_compare(const GPFPUD_REG &obj, const GPFPUD_REG &expected, const std::string &name);

bool check_compare(const GPFINV_BITS &obj, const GPFINV_BITS &expected, const std::string &name);

bool check_compare(const GPFINV_REG &obj, const GPFINV_REG &expected, const std::string &name);

bool check_compare(const GPFODR_BITS &obj, const GPFODR_BITS &expected, const std::string &name);

bool check_compare(const GPFODR_REG &obj, const GPFODR_REG &expected, const std::string &name);

bool check_compare(const GPFGMUX1_BITS &obj, const GPFGMUX1_BITS &expected, const std::string &name);

bool check_compare(const GPFGMUX1_REG &obj, const GPFGMUX1_REG &expected, const std::string &name);

bool check_compare(const GPFCSEL1_BITS &obj, const GPFCSEL1_BITS &expected, const std::string &name);

bool check_compare(const GPFCSEL1_REG &obj, const GPFCSEL1_REG &expected, const std::string &name);

bool check_compare(const GPFCSEL2_BITS &obj, const GPFCSEL2_BITS &expected, const std::string &name);

bool check_compare(const GPFCSEL2_REG &obj, const GPFCSEL2_REG &expected, const std::string &name);

bool check_compare(const GPFLOCK_BITS &obj, const GPFLOCK_BITS &expected, const std::string &name);

bool check_compare(const GPFLOCK_REG &obj, const GPFLOCK_REG &expected, const std::string &name);

bool check_compare(const GPFCR_BITS &obj, const GPFCR_BITS &expected, const std::string &name);

bool check_compare(const GPFCR_REG &obj, const GPFCR_REG &expected, const std::string &name);

bool check_compare(const GPIO_CTRL_REGS &obj, const GPIO_CTRL_REGS &expected, const std::string &name);

bool check_compare(const GPADAT_BITS &obj, const GPADAT_BITS &expected, const std::string &name);

bool check_compare(const GPADAT_REG &obj, const GPADAT_REG &expected, const std::string &name);

bool check_compare(const GPASET_BITS &obj, const GPASET_BITS &expected, const std::string &name);

bool check_compare(const GPASET_REG &obj, const GPASET_REG &expected, const std::string &name);

bool check_compare(const GPACLEAR_BITS &obj, const GPACLEAR_BITS &expected, const std::string &name);

bool check_compare(const GPACLEAR_REG &obj, const GPACLEAR_REG &expected, const std::string &name);

bool check_compare(const GPATOGGLE_BITS &obj, const GPATOGGLE_BITS &expected, const std::string &name);

bool check_compare(const GPATOGGLE_REG &obj, const GPATOGGLE_REG &expected, const std::string &name);

bool check_compare(const GPBDAT_BITS &obj, const GPBDAT_BITS &expected, const std::string &name);

bool check_compare(const GPBDAT_REG &obj, const GPBDAT_REG &expected, const std::string &name);

bool check_compare(const GPBSET_BITS &obj, const GPBSET_BITS &expected, const std::string &name);

bool check_compare(const GPBSET_REG &obj, const GPBSET_REG &expected, const std::string &name);

bool check_compare(const GPBCLEAR_BITS &obj, const GPBCLEAR_BITS &expected, const std::string &name);

bool check_compare(const GPBCLEAR_REG &obj, const GPBCLEAR_REG &expected, const std::string &name);

bool check_compare(const GPBTOGGLE_BITS &obj, const GPBTOGGLE_BITS &expected, const std::string &name);

bool check_compare(const GPBTOGGLE_REG &obj, const GPBTOGGLE_REG &expected, const std::string &name);

bool check_compare(const GPCDAT_BITS &obj, const GPCDAT_BITS &expected, const std::string &name);

bool check_compare(const GPCDAT_REG &obj, const GPCDAT_REG &expected, const std::string &name);

bool check_compare(const GPCSET_BITS &obj, const GPCSET_BITS &expected, const std::string &name);

bool check_compare(const GPCSET_REG &obj, const GPCSET_REG &expected, const std::string &name);

bool check_compare(const GPCCLEAR_BITS &obj, const GPCCLEAR_BITS &expected, const std::string &name);

bool check_compare(const GPCCLEAR_REG &obj, const GPCCLEAR_REG &expected, const std::string &name);

bool check_compare(const GPCTOGGLE_BITS &obj, const GPCTOGGLE_BITS &expected, const std::string &name);

bool check_compare(const GPCTOGGLE_REG &obj, const GPCTOGGLE_REG &expected, const std::string &name);

bool check_compare(const GPDDAT_BITS &obj, const GPDDAT_BITS &expected, const std::string &name);

bool check_compare(const GPDDAT_REG &obj, const GPDDAT_REG &expected, const std::string &name);

bool check_compare(const GPDSET_BITS &obj, const GPDSET_BITS &expected, const std::string &name);

bool check_compare(const GPDSET_REG &obj, const GPDSET_REG &expected, const std::string &name);

bool check_compare(const GPDCLEAR_BITS &obj, const GPDCLEAR_BITS &expected, const std::string &name);

bool check_compare(const GPDCLEAR_REG &obj, const GPDCLEAR_REG &expected, const std::string &name);

bool check_compare(const GPDTOGGLE_BITS &obj, const GPDTOGGLE_BITS &expected, const std::string &name);

bool check_compare(const GPDTOGGLE_REG &obj, const GPDTOGGLE_REG &expected, const std::string &name);

bool check_compare(const GPEDAT_BITS &obj, const GPEDAT_BITS &expected, const std::string &name);

bool check_compare(const GPEDAT_REG &obj, const GPEDAT_REG &expected, const std::string &name);

bool check_compare(const GPESET_BITS &obj, const GPESET_BITS &expected, const std::string &name);

bool check_compare(const GPESET_REG &obj, const GPESET_REG &expected, const std::string &name);

bool check_compare(const GPECLEAR_BITS &obj, const GPECLEAR_BITS &expected, const std::string &name);

bool check_compare(const GPECLEAR_REG &obj, const GPECLEAR_REG &expected, const std::string &name);

bool check_compare(const GPETOGGLE_BITS &obj, const GPETOGGLE_BITS &expected, const std::string &name);

bool check_compare(const GPETOGGLE_REG &obj, const GPETOGGLE_REG &expected, const std::string &name);

bool check_compare(const GPFDAT_BITS &obj, const GPFDAT_BITS &expected, const std::string &name);

bool check_compare(const GPFDAT_REG &obj, const GPFDAT_REG &expected, const std::string &name);

bool check_compare(const GPFSET_BITS &obj, const GPFSET_BITS &expected, const std::string &name);

bool check_compare(const GPFSET_REG &obj, const GPFSET_REG &expected, const std::string &name);

bool check_compare(const GPFCLEAR_BITS &obj, const GPFCLEAR_BITS &expected, const std::string &name);

bool check_compare(const GPFCLEAR_REG &obj, const GPFCLEAR_REG &expected, const std::string &name);

bool check_compare(const GPFTOGGLE_BITS &obj, const GPFTOGGLE_BITS &expected, const std::string &name);

bool check_compare(const GPFTOGGLE_REG &obj, const GPFTOGGLE_REG &expected, const std::string &name);

bool check_compare(const GPIO_DATA_REGS &obj, const GPIO_DATA_REGS &expected, const std::string &name);

bool check_compare(const I2COAR_BITS &obj, const I2COAR_BITS &expected, const std::string &name);

bool check_compare(const I2COAR_REG &obj, const I2COAR_REG &expected, const std::string &name);

bool check_compare(const I2CIER_BITS &obj, const I2CIER_BITS &expected, const std::string &name);

bool check_compare(const I2CIER_REG &obj, const I2CIER_REG &expected, const std::string &name);

bool check_compare(const I2CSTR_BITS &obj, const I2CSTR_BITS &expected, const std::string &name);

bool check_compare(const I2CSTR_REG &obj, const I2CSTR_REG &expected, const std::string &name);

bool check_compare(const I2CDRR_BITS &obj, const I2CDRR_BITS &expected, const std::string &name);

bool check_compare(const I2CDRR_REG &obj, const I2CDRR_REG &expected, const std::string &name);

bool check_compare(const I2CSAR_BITS &obj, const I2CSAR_BITS &expected, const std::string &name);

bool check_compare(const I2CSAR_REG &obj, const I2CSAR_REG &expected, const std::string &name);

bool check_compare(const I2CDXR_BITS &obj, const I2CDXR_BITS &expected, const std::string &name);

bool check_compare(const I2CDXR_REG &obj, const I2CDXR_REG &expected, const std::string &name);

bool check_compare(const I2CMDR_BITS &obj, const I2CMDR_BITS &expected, const std::string &name);

bool check_compare(const I2CMDR_REG &obj, const I2CMDR_REG &expected, const std::string &name);

bool check_compare(const I2CISRC_BITS &obj, const I2CISRC_BITS &expected, const std::string &name);

bool check_compare(const I2CISRC_REG &obj, const I2CISRC_REG &expected, const std::string &name);

bool check_compare(const I2CEMDR_BITS &obj, const I2CEMDR_BITS &expected, const std::string &name);

bool check_compare(const I2CEMDR_REG &obj, const I2CEMDR_REG &expected, const std::string &name);

bool check_compare(const I2CPSC_BITS &obj, const I2CPSC_BITS &expected, const std::string &name);

bool check_compare(const I2CPSC_REG &obj, const I2CPSC_REG &expected, const std::string &name);

bool check_compare(const I2CFFTX_BITS &obj, const I2CFFTX_BITS &expected, const std::string &name);

bool check_compare(const I2CFFTX_REG &obj, const I2CFFTX_REG &expected, const std::string &name);

bool check_compare(const I2CFFRX_BITS &obj, const I2CFFRX_BITS &expected, const std::string &name);

bool check_compare(const I2CFFRX_REG &obj, const I2CFFRX_REG &expected, const std::string &name);

bool check_compare(const I2C_REGS &obj, const I2C_REGS &expected, const std::string &name);

bool check_compare(const DRR2_BITS &obj, const DRR2_BITS &expected, const std::string &name);

bool check_compare(const DRR2_REG &obj, const DRR2_REG &expected, const std::string &name);

bool check_compare(const DRR1_BITS &obj, const DRR1_BITS &expected, const std::string &name);

bool check_compare(const DRR1_REG &obj, const DRR1_REG &expected, const std::string &name);

bool check_compare(const DXR2_BITS &obj, const DXR2_BITS &expected, const std::string &name);

bool check_compare(const DXR2_REG &obj, const DXR2_REG &expected, const std::string &name);

bool check_compare(const DXR1_BITS &obj, const DXR1_BITS &expected, const std::string &name);

bool check_compare(const DXR1_REG &obj, const DXR1_REG &expected, const std::string &name);

bool check_compare(const SPCR2_BITS &obj, const SPCR2_BITS &expected, const std::string &name);

bool check_compare(const SPCR2_REG &obj, const SPCR2_REG &expected, const std::string &name);

bool check_compare(const SPCR1_BITS &obj, const SPCR1_BITS &expected, const std::string &name);

bool check_compare(const SPCR1_REG &obj, const SPCR1_REG &expected, const std::string &name);

bool check_compare(const RCR2_BITS &obj, const RCR2_BITS &expected, const std::string &name);

bool check_compare(const RCR2_REG &obj, const RCR2_REG &expected, const std::string &name);

bool check_compare(const RCR1_BITS &obj, const RCR1_BITS &expected, const std::string &name);

bool check_compare(const RCR1_REG &obj, const RCR1_REG &expected, const std::string &name);

bool check_compare(const XCR2_BITS &obj, const XCR2_BITS &expected, const std::string &name);

bool check_compare(const XCR2_REG &obj, const XCR2_REG &expected, const std::string &name);

bool check_compare(const XCR1_BITS &obj, const XCR1_BITS &expected, const std::string &name);

bool check_compare(const XCR1_REG &obj, const XCR1_REG &expected, const std::string &name);

bool check_compare(const SRGR2_BITS &obj, const SRGR2_BITS &expected, const std::string &name);

bool check_compare(const SRGR2_REG &obj, const SRGR2_REG &expected, const std::string &name);

bool check_compare(const SRGR1_BITS &obj, const SRGR1_BITS &expected, const std::string &name);

bool check_compare(const SRGR1_REG &obj, const SRGR1_REG &expected, const std::string &name);

bool check_compare(const MCR2_BITS &obj, const MCR2_BITS &expected, const std::string &name);

bool check_compare(const MCR2_REG &obj, const MCR2_REG &expected, const std::string &name);

bool check_compare(const MCR1_BITS &obj, const MCR1_BITS &expected, const std::string &name);

bool check_compare(const MCR1_REG &obj, const MCR1_REG &expected, const std::string &name);

bool check_compare(const PCR_BITS &obj, const PCR_BITS &expected, const std::string &name);

bool check_compare(const PCR_REG &obj, const PCR_REG &expected, const std::string &name);

bool check_compare(const MFFINT_BITS &obj, const MFFINT_BITS &expected, const std::string &name);

bool check_compare(const MFFINT_REG &obj, const MFFINT_REG &expected, const std::string &name);

bool check_compare(const McBSP_REGS &obj, const McBSP_REGS &expected, const std::string &name);

bool check_compare(const DxLOCK_BITS &obj, const DxLOCK_BITS &expected, const std::string &name);

bool check_compare(const DxLOCK_REG &obj, const DxLOCK_REG &expected, const std::string &name);

bool check_compare(const DxCOMMIT_BITS &obj, const DxCOMMIT_BITS &expected, const std::string &name);

bool check_compare(const DxCOMMIT_REG &obj, const DxCOMMIT_REG &expected, const std::string &name);

bool check_compare(const DxACCPROT0_BITS &obj, const DxACCPROT0_BITS &expected, const std::string &name);

bool check_compare(const DxACCPROT0_REG &obj, const DxACCPROT0_REG &expected, const std::string &name);

bool check_compare(const DxTEST_BITS &obj, const DxTEST_BITS &expected, const std::string &name);

bool check_compare(const DxTEST_REG &obj, const DxTEST_REG &expected, const std::string &name);

bool check_compare(const DxINIT_BITS &obj, const DxINIT_BITS &expected, const std::string &name);

bool check_compare(const DxINIT_REG &obj, const DxINIT_REG &expected, const std::string &name);

bool check_compare(const DxINITDONE_BITS &obj, const DxINITDONE_BITS &expected, const std::string &name);

bool check_compare(const DxINITDONE_REG &obj, const DxINITDONE_REG &expected, const std::string &name);

bool check_compare(const LSxLOCK_BITS &obj, const LSxLOCK_BITS &expected, const std::string &name);

bool check_compare(const LSxLOCK_REG &obj, const LSxLOCK_REG &expected, const std::string &name);

bool check_compare(const LSxCOMMIT_BITS &obj, const LSxCOMMIT_BITS &expected, const std::string &name);

bool check_compare(const LSxCOMMIT_REG &obj, const LSxCOMMIT_REG &expected, const std::string &name);

bool check_compare(const LSxMSEL_BITS &obj, const LSxMSEL_BITS &expected, const std::string &name);

bool check_compare(const LSxMSEL_REG &obj, const LSxMSEL_REG &expected, const std::string &name);

bool check_compare(const LSxCLAPGM_BITS &obj, const LSxCLAPGM_BITS &expected, const std::string &name);

bool check_compare(const LSxCLAPGM_REG &obj, const LSxCLAPGM_REG &expected, const std::string &name);

bool check_compare(const LSxACCPROT0_BITS &obj, const LSxACCPROT0_BITS &expected, const std::string &name);

bool check_compare(const LSxACCPROT0_REG &obj, const LSxACCPROT0_REG &expected, const std::string &name);

bool check_compare(const LSxACCPROT1_BITS &obj, const LSxACCPROT1_BITS &expected, const std::string &name);

bool check_compare(const LSxACCPROT1_REG &obj, const LSxACCPROT1_REG &expected, const std::string &name);

bool check_compare(const LSxTEST_BITS &obj, const LSxTEST_BITS &expected, const std::string &name);

bool check_compare(const LSxTEST_REG &obj, const LSxTEST_REG &expected, const std::string &name);

bool check_compare(const LSxINIT_BITS &obj, const LSxINIT_BITS &expected, const std::string &name);

bool check_compare(const LSxINIT_REG &obj, const LSxINIT_REG &expected, const std::string &name);

bool check_compare(const LSxINITDONE_BITS &obj, const LSxINITDONE_BITS &expected, const std::string &name);

bool check_compare(const LSxINITDONE_REG &obj, const LSxINITDONE_REG &expected, const std::string &name);

bool check_compare(const GSxLOCK_BITS &obj, const GSxLOCK_BITS &expected, const std::string &name);

bool check_compare(const GSxLOCK_REG &obj, const GSxLOCK_REG &expected, const std::string &name);

bool check_compare(const GSxCOMMIT_BITS &obj, const GSxCOMMIT_BITS &expected, const std::string &name);

bool check_compare(const GSxCOMMIT_REG &obj, const GSxCOMMIT_REG &expected, const std::string &name);

bool check_compare(const GSxMSEL_BITS &obj, const GSxMSEL_BITS &expected, const std::string &name);

bool check_compare(const GSxMSEL_REG &obj, const GSxMSEL_REG &expected, const std::string &name);

bool check_compare(const GSxACCPROT0_BITS &obj, const GSxACCPROT0_BITS &expected, const std::string &name);

bool check_compare(const GSxACCPROT0_REG &obj, const GSxACCPROT0_REG &expected, const std::string &name);

bool check_compare(const GSxACCPROT1_BITS &obj, const GSxACCPROT1_BITS &expected, const std::string &name);

bool check_compare(const GSxACCPROT1_REG &obj, const GSxACCPROT1_REG &expected, const std::string &name);

bool check_compare(const GSxACCPROT2_BITS &obj, const GSxACCPROT2_BITS &expected, const std::string &name);

bool check_compare(const GSxACCPROT2_REG &obj, const GSxACCPROT2_REG &expected, const std::string &name);

bool check_compare(const GSxACCPROT3_BITS &obj, const GSxACCPROT3_BITS &expected, const std::string &name);

bool check_compare(const GSxACCPROT3_REG &obj, const GSxACCPROT3_REG &expected, const std::string &name);

bool check_compare(const GSxTEST_BITS &obj, const GSxTEST_BITS &expected, const std::string &name);

bool check_compare(const GSxTEST_REG &obj, const GSxTEST_REG &expected, const std::string &name);

bool check_compare(const GSxINIT_BITS &obj, const GSxINIT_BITS &expected, const std::string &name);

bool check_compare(const GSxINIT_REG &obj, const GSxINIT_REG &expected, const std::string &name);

bool check_compare(const GSxINITDONE_BITS &obj, const GSxINITDONE_BITS &expected, const std::string &name);

bool check_compare(const GSxINITDONE_REG &obj, const GSxINITDONE_REG &expected, const std::string &name);

bool check_compare(const MSGxTEST_BITS &obj, const MSGxTEST_BITS &expected, const std::string &name);

bool check_compare(const MSGxTEST_REG &obj, const MSGxTEST_REG &expected, const std::string &name);

bool check_compare(const MSGxINIT_BITS &obj, const MSGxINIT_BITS &expected, const std::string &name);

bool check_compare(const MSGxINIT_REG &obj, const MSGxINIT_REG &expected, const std::string &name);

bool check_compare(const MSGxINITDONE_BITS &obj, const MSGxINITDONE_BITS &expected, const std::string &name);

bool check_compare(const MSGxINITDONE_REG &obj, const MSGxINITDONE_REG &expected, const std::string &name);

bool check_compare(const MEM_CFG_REGS &obj, const MEM_CFG_REGS &expected, const std::string &name);

bool check_compare(const EMIF1LOCK_BITS &obj, const EMIF1LOCK_BITS &expected, const std::string &name);

bool check_compare(const EMIF1LOCK_REG &obj, const EMIF1LOCK_REG &expected, const std::string &name);

bool check_compare(const EMIF1COMMIT_BITS &obj, const EMIF1COMMIT_BITS &expected, const std::string &name);

bool check_compare(const EMIF1COMMIT_REG &obj, const EMIF1COMMIT_REG &expected, const std::string &name);

bool check_compare(const EMIF1MSEL_BITS &obj, const EMIF1MSEL_BITS &expected, const std::string &name);

bool check_compare(const EMIF1MSEL_REG &obj, const EMIF1MSEL_REG &expected, const std::string &name);

bool check_compare(const EMIF1ACCPROT0_BITS &obj, const EMIF1ACCPROT0_BITS &expected, const std::string &name);

bool check_compare(const EMIF1ACCPROT0_REG &obj, const EMIF1ACCPROT0_REG &expected, const std::string &name);

bool check_compare(const EMIF1_CONFIG_REGS &obj, const EMIF1_CONFIG_REGS &expected, const std::string &name);

bool check_compare(const EMIF2LOCK_BITS &obj, const EMIF2LOCK_BITS &expected, const std::string &name);

bool check_compare(const EMIF2LOCK_REG &obj, const EMIF2LOCK_REG &expected, const std::string &name);

bool check_compare(const EMIF2COMMIT_BITS &obj, const EMIF2COMMIT_BITS &expected, const std::string &name);

bool check_compare(const EMIF2COMMIT_REG &obj, const EMIF2COMMIT_REG &expected, const std::string &name);

bool check_compare(const EMIF2ACCPROT0_BITS &obj, const EMIF2ACCPROT0_BITS &expected, const std::string &name);

bool check_compare(const EMIF2ACCPROT0_REG &obj, const EMIF2ACCPROT0_REG &expected, const std::string &name);

bool check_compare(const EMIF2_CONFIG_REGS &obj, const EMIF2_CONFIG_REGS &expected, const std::string &name);

bool check_compare(const NMAVFLG_BITS &obj, const NMAVFLG_BITS &expected, const std::string &name);

bool check_compare(const NMAVFLG_REG &obj, const NMAVFLG_REG &expected, const std::string &name);

bool check_compare(const NMAVSET_BITS &obj, const NMAVSET_BITS &expected, const std::string &name);

bool check_compare(const NMAVSET_REG &obj, const NMAVSET_REG &expected, const std::string &name);

bool check_compare(const NMAVCLR_BITS &obj, const NMAVCLR_BITS &expected, const std::string &name);

bool check_compare(const NMAVCLR_REG &obj, const NMAVCLR_REG &expected, const std::string &name);

bool check_compare(const NMAVINTEN_BITS &obj, const NMAVINTEN_BITS &expected, const std::string &name);

bool check_compare(const NMAVINTEN_REG &obj, const NMAVINTEN_REG &expected, const std::string &name);

bool check_compare(const MAVFLG_BITS &obj, const MAVFLG_BITS &expected, const std::string &name);

bool check_compare(const MAVFLG_REG &obj, const MAVFLG_REG &expected, const std::string &name);

bool check_compare(const MAVSET_BITS &obj, const MAVSET_BITS &expected, const std::string &name);

bool check_compare(const MAVSET_REG &obj, const MAVSET_REG &expected, const std::string &name);

bool check_compare(const MAVCLR_BITS &obj, const MAVCLR_BITS &expected, const std::string &name);

bool check_compare(const MAVCLR_REG &obj, const MAVCLR_REG &expected, const std::string &name);

bool check_compare(const MAVINTEN_BITS &obj, const MAVINTEN_BITS &expected, const std::string &name);

bool check_compare(const MAVINTEN_REG &obj, const MAVINTEN_REG &expected, const std::string &name);

bool check_compare(const ACCESS_PROTECTION_REGS &obj, const ACCESS_PROTECTION_REGS &expected, const std::string &name);

bool check_compare(const UCERRFLG_BITS &obj, const UCERRFLG_BITS &expected, const std::string &name);

bool check_compare(const UCERRFLG_REG &obj, const UCERRFLG_REG &expected, const std::string &name);

bool check_compare(const UCERRSET_BITS &obj, const UCERRSET_BITS &expected, const std::string &name);

bool check_compare(const UCERRSET_REG &obj, const UCERRSET_REG &expected, const std::string &name);

bool check_compare(const UCERRCLR_BITS &obj, const UCERRCLR_BITS &expected, const std::string &name);

bool check_compare(const UCERRCLR_REG &obj, const UCERRCLR_REG &expected, const std::string &name);

bool check_compare(const CERRFLG_BITS &obj, const CERRFLG_BITS &expected, const std::string &name);

bool check_compare(const CERRFLG_REG &obj, const CERRFLG_REG &expected, const std::string &name);

bool check_compare(const CERRSET_BITS &obj, const CERRSET_BITS &expected, const std::string &name);

bool check_compare(const CERRSET_REG &obj, const CERRSET_REG &expected, const std::string &name);

bool check_compare(const CERRCLR_BITS &obj, const CERRCLR_BITS &expected, const std::string &name);

bool check_compare(const CERRCLR_REG &obj, const CERRCLR_REG &expected, const std::string &name);

bool check_compare(const CEINTFLG_BITS &obj, const CEINTFLG_BITS &expected, const std::string &name);

bool check_compare(const CEINTFLG_REG &obj, const CEINTFLG_REG &expected, const std::string &name);

bool check_compare(const CEINTCLR_BITS &obj, const CEINTCLR_BITS &expected, const std::string &name);

bool check_compare(const CEINTCLR_REG &obj, const CEINTCLR_REG &expected, const std::string &name);

bool check_compare(const CEINTSET_BITS &obj, const CEINTSET_BITS &expected, const std::string &name);

bool check_compare(const CEINTSET_REG &obj, const CEINTSET_REG &expected, const std::string &name);

bool check_compare(const CEINTEN_BITS &obj, const CEINTEN_BITS &expected, const std::string &name);

bool check_compare(const CEINTEN_REG &obj, const CEINTEN_REG &expected, const std::string &name);

bool check_compare(const MEMORY_ERROR_REGS &obj, const MEMORY_ERROR_REGS &expected, const std::string &name);

bool check_compare(const ROMWAITSTATE_BITS &obj, const ROMWAITSTATE_BITS &expected, const std::string &name);

bool check_compare(const ROMWAITSTATE_REG &obj, const ROMWAITSTATE_REG &expected, const std::string &name);

bool check_compare(const ROM_WAIT_STATE_REGS &obj, const ROM_WAIT_STATE_REGS &expected, const std::string &name);

bool check_compare(const ROMPREFETCH_BITS &obj, const ROMPREFETCH_BITS &expected, const std::string &name);

bool check_compare(const ROMPREFETCH_REG &obj, const ROMPREFETCH_REG &expected, const std::string &name);

bool check_compare(const ROM_PREFETCH_REGS &obj, const ROM_PREFETCH_REGS &expected, const std::string &name);

bool check_compare(const NMICFG_BITS &obj, const NMICFG_BITS &expected, const std::string &name);

bool check_compare(const NMICFG_REG &obj, const NMICFG_REG &expected, const std::string &name);

bool check_compare(const NMIFLG_BITS &obj, const NMIFLG_BITS &expected, const std::string &name);

bool check_compare(const NMIFLG_REG &obj, const NMIFLG_REG &expected, const std::string &name);

bool check_compare(const NMIFLGCLR_BITS &obj, const NMIFLGCLR_BITS &expected, const std::string &name);

bool check_compare(const NMIFLGCLR_REG &obj, const NMIFLGCLR_REG &expected, const std::string &name);

bool check_compare(const NMIFLGFRC_BITS &obj, const NMIFLGFRC_BITS &expected, const std::string &name);

bool check_compare(const NMIFLGFRC_REG &obj, const NMIFLGFRC_REG &expected, const std::string &name);

bool check_compare(const NMISHDFLG_BITS &obj, const NMISHDFLG_BITS &expected, const std::string &name);

bool check_compare(const NMISHDFLG_REG &obj, const NMISHDFLG_REG &expected, const std::string &name);

bool check_compare(const NMI_INTRUPT_REGS &obj, const NMI_INTRUPT_REGS &expected, const std::string &name);

bool check_compare(const PIECTRL_BITS &obj, const PIECTRL_BITS &expected, const std::string &name);

bool check_compare(const PIECTRL_REG &obj, const PIECTRL_REG &expected, const std::string &name);

bool check_compare(const PIEACK_BITS &obj, const PIEACK_BITS &expected, const std::string &name);

bool check_compare(const PIEACK_REG &obj, const PIEACK_REG &expected, const std::string &name);

bool check_compare(const PIEIER1_BITS &obj, const PIEIER1_BITS &expected, const std::string &name);

bool check_compare(const PIEIER1_REG &obj, const PIEIER1_REG &expected, const std::string &name);

bool check_compare(const PIEIFR1_BITS &obj, const PIEIFR1_BITS &expected, const std::string &name);

bool check_compare(const PIEIFR1_REG &obj, const PIEIFR1_REG &expected, const std::string &name);

bool check_compare(const PIEIER2_BITS &obj, const PIEIER2_BITS &expected, const std::string &name);

bool check_compare(const PIEIER2_REG &obj, const PIEIER2_REG &expected, const std::string &name);

bool check_compare(const PIEIFR2_BITS &obj, const PIEIFR2_BITS &expected, const std::string &name);

bool check_compare(const PIEIFR2_REG &obj, const PIEIFR2_REG &expected, const std::string &name);

bool check_compare(const PIEIER3_BITS &obj, const PIEIER3_BITS &expected, const std::string &name);

bool check_compare(const PIEIER3_REG &obj, const PIEIER3_REG &expected, const std::string &name);

bool check_compare(const PIEIFR3_BITS &obj, const PIEIFR3_BITS &expected, const std::string &name);

bool check_compare(const PIEIFR3_REG &obj, const PIEIFR3_REG &expected, const std::string &name);

bool check_compare(const PIEIER4_BITS &obj, const PIEIER4_BITS &expected, const std::string &name);

bool check_compare(const PIEIER4_REG &obj, const PIEIER4_REG &expected, const std::string &name);

bool check_compare(const PIEIFR4_BITS &obj, const PIEIFR4_BITS &expected, const std::string &name);

bool check_compare(const PIEIFR4_REG &obj, const PIEIFR4_REG &expected, const std::string &name);

bool check_compare(const PIEIER5_BITS &obj, const PIEIER5_BITS &expected, const std::string &name);

bool check_compare(const PIEIER5_REG &obj, const PIEIER5_REG &expected, const std::string &name);

bool check_compare(const PIEIFR5_BITS &obj, const PIEIFR5_BITS &expected, const std::string &name);

bool check_compare(const PIEIFR5_REG &obj, const PIEIFR5_REG &expected, const std::string &name);

bool check_compare(const PIEIER6_BITS &obj, const PIEIER6_BITS &expected, const std::string &name);

bool check_compare(const PIEIER6_REG &obj, const PIEIER6_REG &expected, const std::string &name);

bool check_compare(const PIEIFR6_BITS &obj, const PIEIFR6_BITS &expected, const std::string &name);

bool check_compare(const PIEIFR6_REG &obj, const PIEIFR6_REG &expected, const std::string &name);

bool check_compare(const PIEIER7_BITS &obj, const PIEIER7_BITS &expected, const std::string &name);

bool check_compare(const PIEIER7_REG &obj, const PIEIER7_REG &expected, const std::string &name);

bool check_compare(const PIEIFR7_BITS &obj, const PIEIFR7_BITS &expected, const std::string &name);

bool check_compare(const PIEIFR7_REG &obj, const PIEIFR7_REG &expected, const std::string &name);

bool check_compare(const PIEIER8_BITS &obj, const PIEIER8_BITS &expected, const std::string &name);

bool check_compare(const PIEIER8_REG &obj, const PIEIER8_REG &expected, const std::string &name);

bool check_compare(const PIEIFR8_BITS &obj, const PIEIFR8_BITS &expected, const std::string &name);

bool check_compare(const PIEIFR8_REG &obj, const PIEIFR8_REG &expected, const std::string &name);

bool check_compare(const PIEIER9_BITS &obj, const PIEIER9_BITS &expected, const std::string &name);

bool check_compare(const PIEIER9_REG &obj, const PIEIER9_REG &expected, const std::string &name);

bool check_compare(const PIEIFR9_BITS &obj, const PIEIFR9_BITS &expected, const std::string &name);

bool check_compare(const PIEIFR9_REG &obj, const PIEIFR9_REG &expected, const std::string &name);

bool check_compare(const PIEIER10_BITS &obj, const PIEIER10_BITS &expected, const std::string &name);

bool check_compare(const PIEIER10_REG &obj, const PIEIER10_REG &expected, const std::string &name);

bool check_compare(const PIEIFR10_BITS &obj, const PIEIFR10_BITS &expected, const std::string &name);

bool check_compare(const PIEIFR10_REG &obj, const PIEIFR10_REG &expected, const std::string &name);

bool check_compare(const PIEIER11_BITS &obj, const PIEIER11_BITS &expected, const std::string &name);

bool check_compare(const PIEIER11_REG &obj, const PIEIER11_REG &expected, const std::string &name);

bool check_compare(const PIEIFR11_BITS &obj, const PIEIFR11_BITS &expected, const std::string &name);

bool check_compare(const PIEIFR11_REG &obj, const PIEIFR11_REG &expected, const std::string &name);

bool check_compare(const PIEIER12_BITS &obj, const PIEIER12_BITS &expected, const std::string &name);

bool check_compare(const PIEIER12_REG &obj, const PIEIER12_REG &expected, const std::string &name);

bool check_compare(const PIEIFR12_BITS &obj, const PIEIFR12_BITS &expected, const std::string &name);

bool check_compare(const PIEIFR12_REG &obj, const PIEIFR12_REG &expected, const std::string &name);

bool check_compare(const PIE_CTRL_REGS &obj, const PIE_CTRL_REGS &expected, const std::string &name);

bool check_compare(const PIE_VECT_TABLE &obj, const PIE_VECT_TABLE &expected, const std::string &name);

bool check_compare(const SCICCR_BITS &obj, const SCICCR_BITS &expected, const std::string &name);

bool check_compare(const SCICCR_REG &obj, const SCICCR_REG &expected, const std::string &name);

bool check_compare(const SCICTL1_BITS &obj, const SCICTL1_BITS &expected, const std::string &name);

bool check_compare(const SCICTL1_REG &obj, const SCICTL1_REG &expected, const std::string &name);

bool check_compare(const SCIHBAUD_BITS &obj, const SCIHBAUD_BITS &expected, const std::string &name);

bool check_compare(const SCIHBAUD_REG &obj, const SCIHBAUD_REG &expected, const std::string &name);

bool check_compare(const SCILBAUD_BITS &obj, const SCILBAUD_BITS &expected, const std::string &name);

bool check_compare(const SCILBAUD_REG &obj, const SCILBAUD_REG &expected, const std::string &name);

bool check_compare(const SCICTL2_BITS &obj, const SCICTL2_BITS &expected, const std::string &name);

bool check_compare(const SCICTL2_REG &obj, const SCICTL2_REG &expected, const std::string &name);

bool check_compare(const SCIRXST_BITS &obj, const SCIRXST_BITS &expected, const std::string &name);

bool check_compare(const SCIRXST_REG &obj, const SCIRXST_REG &expected, const std::string &name);

bool check_compare(const SCIRXEMU_BITS &obj, const SCIRXEMU_BITS &expected, const std::string &name);

bool check_compare(const SCIRXEMU_REG &obj, const SCIRXEMU_REG &expected, const std::string &name);

bool check_compare(const SCIRXBUF_BITS &obj, const SCIRXBUF_BITS &expected, const std::string &name);

bool check_compare(const SCIRXBUF_REG &obj, const SCIRXBUF_REG &expected, const std::string &name);

bool check_compare(const SCITXBUF_BITS &obj, const SCITXBUF_BITS &expected, const std::string &name);

bool check_compare(const SCITXBUF_REG &obj, const SCITXBUF_REG &expected, const std::string &name);

bool check_compare(const SCIFFTX_BITS &obj, const SCIFFTX_BITS &expected, const std::string &name);

bool check_compare(const SCIFFTX_REG &obj, const SCIFFTX_REG &expected, const std::string &name);

bool check_compare(const SCIFFRX_BITS &obj, const SCIFFRX_BITS &expected, const std::string &name);

bool check_compare(const SCIFFRX_REG &obj, const SCIFFRX_REG &expected, const std::string &name);

bool check_compare(const SCIFFCT_BITS &obj, const SCIFFCT_BITS &expected, const std::string &name);

bool check_compare(const SCIFFCT_REG &obj, const SCIFFCT_REG &expected, const std::string &name);

bool check_compare(const SCIPRI_BITS &obj, const SCIPRI_BITS &expected, const std::string &name);

bool check_compare(const SCIPRI_REG &obj, const SCIPRI_REG &expected, const std::string &name);

bool check_compare(const SCI_REGS &obj, const SCI_REGS &expected, const std::string &name);

bool check_compare(const SDIFLG_BITS &obj, const SDIFLG_BITS &expected, const std::string &name);

bool check_compare(const SDIFLG_REG &obj, const SDIFLG_REG &expected, const std::string &name);

bool check_compare(const SDIFLGCLR_BITS &obj, const SDIFLGCLR_BITS &expected, const std::string &name);

bool check_compare(const SDIFLGCLR_REG &obj, const SDIFLGCLR_REG &expected, const std::string &name);

bool check_compare(const SDCTL_BITS &obj, const SDCTL_BITS &expected, const std::string &name);

bool check_compare(const SDCTL_REG &obj, const SDCTL_REG &expected, const std::string &name);

bool check_compare(const SDMFILEN_BITS &obj, const SDMFILEN_BITS &expected, const std::string &name);

bool check_compare(const SDMFILEN_REG &obj, const SDMFILEN_REG &expected, const std::string &name);

bool check_compare(const SDCTLPARM1_BITS &obj, const SDCTLPARM1_BITS &expected, const std::string &name);

bool check_compare(const SDCTLPARM1_REG &obj, const SDCTLPARM1_REG &expected, const std::string &name);

bool check_compare(const SDDFPARM1_BITS &obj, const SDDFPARM1_BITS &expected, const std::string &name);

bool check_compare(const SDDFPARM1_REG &obj, const SDDFPARM1_REG &expected, const std::string &name);

bool check_compare(const SDDPARM1_BITS &obj, const SDDPARM1_BITS &expected, const std::string &name);

bool check_compare(const SDDPARM1_REG &obj, const SDDPARM1_REG &expected, const std::string &name);

bool check_compare(const SDCMPH1_BITS &obj, const SDCMPH1_BITS &expected, const std::string &name);

bool check_compare(const SDCMPH1_REG &obj, const SDCMPH1_REG &expected, const std::string &name);

bool check_compare(const SDCMPL1_BITS &obj, const SDCMPL1_BITS &expected, const std::string &name);

bool check_compare(const SDCMPL1_REG &obj, const SDCMPL1_REG &expected, const std::string &name);

bool check_compare(const SDCPARM1_BITS &obj, const SDCPARM1_BITS &expected, const std::string &name);

bool check_compare(const SDCPARM1_REG &obj, const SDCPARM1_REG &expected, const std::string &name);

bool check_compare(const SDDATA1_BITS &obj, const SDDATA1_BITS &expected, const std::string &name);

bool check_compare(const SDDATA1_REG &obj, const SDDATA1_REG &expected, const std::string &name);

bool check_compare(const SDCTLPARM2_BITS &obj, const SDCTLPARM2_BITS &expected, const std::string &name);

bool check_compare(const SDCTLPARM2_REG &obj, const SDCTLPARM2_REG &expected, const std::string &name);

bool check_compare(const SDDFPARM2_BITS &obj, const SDDFPARM2_BITS &expected, const std::string &name);

bool check_compare(const SDDFPARM2_REG &obj, const SDDFPARM2_REG &expected, const std::string &name);

bool check_compare(const SDDPARM2_BITS &obj, const SDDPARM2_BITS &expected, const std::string &name);

bool check_compare(const SDDPARM2_REG &obj, const SDDPARM2_REG &expected, const std::string &name);

bool check_compare(const SDCMPH2_BITS &obj, const SDCMPH2_BITS &expected, const std::string &name);

bool check_compare(const SDCMPH2_REG &obj, const SDCMPH2_REG &expected, const std::string &name);

bool check_compare(const SDCMPL2_BITS &obj, const SDCMPL2_BITS &expected, const std::string &name);

bool check_compare(const SDCMPL2_REG &obj, const SDCMPL2_REG &expected, const std::string &name);

bool check_compare(const SDCPARM2_BITS &obj, const SDCPARM2_BITS &expected, const std::string &name);

bool check_compare(const SDCPARM2_REG &obj, const SDCPARM2_REG &expected, const std::string &name);

bool check_compare(const SDDATA2_BITS &obj, const SDDATA2_BITS &expected, const std::string &name);

bool check_compare(const SDDATA2_REG &obj, const SDDATA2_REG &expected, const std::string &name);

bool check_compare(const SDCTLPARM3_BITS &obj, const SDCTLPARM3_BITS &expected, const std::string &name);

bool check_compare(const SDCTLPARM3_REG &obj, const SDCTLPARM3_REG &expected, const std::string &name);

bool check_compare(const SDDFPARM3_BITS &obj, const SDDFPARM3_BITS &expected, const std::string &name);

bool check_compare(const SDDFPARM3_REG &obj, const SDDFPARM3_REG &expected, const std::string &name);

bool check_compare(const SDDPARM3_BITS &obj, const SDDPARM3_BITS &expected, const std::string &name);

bool check_compare(const SDDPARM3_REG &obj, const SDDPARM3_REG &expected, const std::string &name);

bool check_compare(const SDCMPH3_BITS &obj, const SDCMPH3_BITS &expected, const std::string &name);

bool check_compare(const SDCMPH3_REG &obj, const SDCMPH3_REG &expected, const std::string &name);

bool check_compare(const SDCMPL3_BITS &obj, const SDCMPL3_BITS &expected, const std::string &name);

bool check_compare(const SDCMPL3_REG &obj, const SDCMPL3_REG &expected, const std::string &name);

bool check_compare(const SDCPARM3_BITS &obj, const SDCPARM3_BITS &expected, const std::string &name);

bool check_compare(const SDCPARM3_REG &obj, const SDCPARM3_REG &expected, const std::string &name);

bool check_compare(const SDDATA3_BITS &obj, const SDDATA3_BITS &expected, const std::string &name);

bool check_compare(const SDDATA3_REG &obj, const SDDATA3_REG &expected, const std::string &name);

bool check_compare(const SDCTLPARM4_BITS &obj, const SDCTLPARM4_BITS &expected, const std::string &name);

bool check_compare(const SDCTLPARM4_REG &obj, const SDCTLPARM4_REG &expected, const std::string &name);

bool check_compare(const SDDFPARM4_BITS &obj, const SDDFPARM4_BITS &expected, const std::string &name);

bool check_compare(const SDDFPARM4_REG &obj, const SDDFPARM4_REG &expected, const std::string &name);

bool check_compare(const SDDPARM4_BITS &obj, const SDDPARM4_BITS &expected, const std::string &name);

bool check_compare(const SDDPARM4_REG &obj, const SDDPARM4_REG &expected, const std::string &name);

bool check_compare(const SDCMPH4_BITS &obj, const SDCMPH4_BITS &expected, const std::string &name);

bool check_compare(const SDCMPH4_REG &obj, const SDCMPH4_REG &expected, const std::string &name);

bool check_compare(const SDCMPL4_BITS &obj, const SDCMPL4_BITS &expected, const std::string &name);

bool check_compare(const SDCMPL4_REG &obj, const SDCMPL4_REG &expected, const std::string &name);

bool check_compare(const SDCPARM4_BITS &obj, const SDCPARM4_BITS &expected, const std::string &name);

bool check_compare(const SDCPARM4_REG &obj, const SDCPARM4_REG &expected, const std::string &name);

bool check_compare(const SDDATA4_BITS &obj, const SDDATA4_BITS &expected, const std::string &name);

bool check_compare(const SDDATA4_REG &obj, const SDDATA4_REG &expected, const std::string &name);

bool check_compare(const SDFM_REGS &obj, const SDFM_REGS &expected, const std::string &name);

bool check_compare(const SPICCR_BITS &obj, const SPICCR_BITS &expected, const std::string &name);

bool check_compare(const SPICCR_REG &obj, const SPICCR_REG &expected, const std::string &name);

bool check_compare(const SPICTL_BITS &obj, const SPICTL_BITS &expected, const std::string &name);

bool check_compare(const SPICTL_REG &obj, const SPICTL_REG &expected, const std::string &name);

bool check_compare(const SPISTS_BITS &obj, const SPISTS_BITS &expected, const std::string &name);

bool check_compare(const SPISTS_REG &obj, const SPISTS_REG &expected, const std::string &name);

bool check_compare(const SPIBRR_BITS &obj, const SPIBRR_BITS &expected, const std::string &name);

bool check_compare(const SPIBRR_REG &obj, const SPIBRR_REG &expected, const std::string &name);

bool check_compare(const SPIFFTX_BITS &obj, const SPIFFTX_BITS &expected, const std::string &name);

bool check_compare(const SPIFFTX_REG &obj, const SPIFFTX_REG &expected, const std::string &name);

bool check_compare(const SPIFFRX_BITS &obj, const SPIFFRX_BITS &expected, const std::string &name);

bool check_compare(const SPIFFRX_REG &obj, const SPIFFRX_REG &expected, const std::string &name);

bool check_compare(const SPIFFCT_BITS &obj, const SPIFFCT_BITS &expected, const std::string &name);

bool check_compare(const SPIFFCT_REG &obj, const SPIFFCT_REG &expected, const std::string &name);

bool check_compare(const SPIPRI_BITS &obj, const SPIPRI_BITS &expected, const std::string &name);

bool check_compare(const SPIPRI_REG &obj, const SPIPRI_REG &expected, const std::string &name);

bool check_compare(const SPI_REGS &obj, const SPI_REGS &expected, const std::string &name);

bool check_compare(const DEVCFGLOCK1_BITS &obj, const DEVCFGLOCK1_BITS &expected, const std::string &name);

bool check_compare(const DEVCFGLOCK1_REG &obj, const DEVCFGLOCK1_REG &expected, const std::string &name);

bool check_compare(const PARTIDL_BITS &obj, const PARTIDL_BITS &expected, const std::string &name);

bool check_compare(const PARTIDL_REG &obj, const PARTIDL_REG &expected, const std::string &name);

bool check_compare(const PARTIDH_BITS &obj, const PARTIDH_BITS &expected, const std::string &name);

bool check_compare(const PARTIDH_REG &obj, const PARTIDH_REG &expected, const std::string &name);

bool check_compare(const DC0_BITS &obj, const DC0_BITS &expected, const std::string &name);

bool check_compare(const DC0_REG &obj, const DC0_REG &expected, const std::string &name);

bool check_compare(const DC1_BITS &obj, const DC1_BITS &expected, const std::string &name);

bool check_compare(const DC1_REG &obj, const DC1_REG &expected, const std::string &name);

bool check_compare(const DC2_BITS &obj, const DC2_BITS &expected, const std::string &name);

bool check_compare(const DC2_REG &obj, const DC2_REG &expected, const std::string &name);

bool check_compare(const DC3_BITS &obj, const DC3_BITS &expected, const std::string &name);

bool check_compare(const DC3_REG &obj, const DC3_REG &expected, const std::string &name);

bool check_compare(const DC4_BITS &obj, const DC4_BITS &expected, const std::string &name);

bool check_compare(const DC4_REG &obj, const DC4_REG &expected, const std::string &name);

bool check_compare(const DC5_BITS &obj, const DC5_BITS &expected, const std::string &name);

bool check_compare(const DC5_REG &obj, const DC5_REG &expected, const std::string &name);

bool check_compare(const DC6_BITS &obj, const DC6_BITS &expected, const std::string &name);

bool check_compare(const DC6_REG &obj, const DC6_REG &expected, const std::string &name);

bool check_compare(const DC7_BITS &obj, const DC7_BITS &expected, const std::string &name);

bool check_compare(const DC7_REG &obj, const DC7_REG &expected, const std::string &name);

bool check_compare(const DC8_BITS &obj, const DC8_BITS &expected, const std::string &name);

bool check_compare(const DC8_REG &obj, const DC8_REG &expected, const std::string &name);

bool check_compare(const DC9_BITS &obj, const DC9_BITS &expected, const std::string &name);

bool check_compare(const DC9_REG &obj, const DC9_REG &expected, const std::string &name);

bool check_compare(const DC10_BITS &obj, const DC10_BITS &expected, const std::string &name);

bool check_compare(const DC10_REG &obj, const DC10_REG &expected, const std::string &name);

bool check_compare(const DC11_BITS &obj, const DC11_BITS &expected, const std::string &name);

bool check_compare(const DC11_REG &obj, const DC11_REG &expected, const std::string &name);

bool check_compare(const DC12_BITS &obj, const DC12_BITS &expected, const std::string &name);

bool check_compare(const DC12_REG &obj, const DC12_REG &expected, const std::string &name);

bool check_compare(const DC13_BITS &obj, const DC13_BITS &expected, const std::string &name);

bool check_compare(const DC13_REG &obj, const DC13_REG &expected, const std::string &name);

bool check_compare(const DC14_BITS &obj, const DC14_BITS &expected, const std::string &name);

bool check_compare(const DC14_REG &obj, const DC14_REG &expected, const std::string &name);

bool check_compare(const DC15_BITS &obj, const DC15_BITS &expected, const std::string &name);

bool check_compare(const DC15_REG &obj, const DC15_REG &expected, const std::string &name);

bool check_compare(const DC17_BITS &obj, const DC17_BITS &expected, const std::string &name);

bool check_compare(const DC17_REG &obj, const DC17_REG &expected, const std::string &name);

bool check_compare(const DC18_BITS &obj, const DC18_BITS &expected, const std::string &name);

bool check_compare(const DC18_REG &obj, const DC18_REG &expected, const std::string &name);

bool check_compare(const DC19_BITS &obj, const DC19_BITS &expected, const std::string &name);

bool check_compare(const DC19_REG &obj, const DC19_REG &expected, const std::string &name);

bool check_compare(const DC20_BITS &obj, const DC20_BITS &expected, const std::string &name);

bool check_compare(const DC20_REG &obj, const DC20_REG &expected, const std::string &name);

bool check_compare(const PERCNF1_BITS &obj, const PERCNF1_BITS &expected, const std::string &name);

bool check_compare(const PERCNF1_REG &obj, const PERCNF1_REG &expected, const std::string &name);

bool check_compare(const FUSEERR_BITS &obj, const FUSEERR_BITS &expected, const std::string &name);

bool check_compare(const FUSEERR_REG &obj, const FUSEERR_REG &expected, const std::string &name);

bool check_compare(const SOFTPRES0_BITS &obj, const SOFTPRES0_BITS &expected, const std::string &name);

bool check_compare(const SOFTPRES0_REG &obj, const SOFTPRES0_REG &expected, const std::string &name);

bool check_compare(const SOFTPRES1_BITS &obj, const SOFTPRES1_BITS &expected, const std::string &name);

bool check_compare(const SOFTPRES1_REG &obj, const SOFTPRES1_REG &expected, const std::string &name);

bool check_compare(const SOFTPRES2_BITS &obj, const SOFTPRES2_BITS &expected, const std::string &name);

bool check_compare(const SOFTPRES2_REG &obj, const SOFTPRES2_REG &expected, const std::string &name);

bool check_compare(const SOFTPRES3_BITS &obj, const SOFTPRES3_BITS &expected, const std::string &name);

bool check_compare(const SOFTPRES3_REG &obj, const SOFTPRES3_REG &expected, const std::string &name);

bool check_compare(const SOFTPRES4_BITS &obj, const SOFTPRES4_BITS &expected, const std::string &name);

bool check_compare(const SOFTPRES4_REG &obj, const SOFTPRES4_REG &expected, const std::string &name);

bool check_compare(const SOFTPRES6_BITS &obj, const SOFTPRES6_BITS &expected, const std::string &name);

bool check_compare(const SOFTPRES6_REG &obj, const SOFTPRES6_REG &expected, const std::string &name);

bool check_compare(const SOFTPRES7_BITS &obj, const SOFTPRES7_BITS &expected, const std::string &name);

bool check_compare(const SOFTPRES7_REG &obj, const SOFTPRES7_REG &expected, const std::string &name);

bool check_compare(const SOFTPRES8_BITS &obj, const SOFTPRES8_BITS &expected, const std::string &name);

bool check_compare(const SOFTPRES8_REG &obj, const SOFTPRES8_REG &expected, const std::string &name);

bool check_compare(const SOFTPRES9_BITS &obj, const SOFTPRES9_BITS &expected, const std::string &name);

bool check_compare(const SOFTPRES9_REG &obj, const SOFTPRES9_REG &expected, const std::string &name);

bool check_compare(const SOFTPRES11_BITS &obj, const SOFTPRES11_BITS &expected, const std::string &name);

bool check_compare(const SOFTPRES11_REG &obj, const SOFTPRES11_REG &expected, const std::string &name);

bool check_compare(const SOFTPRES13_BITS &obj, const SOFTPRES13_BITS &expected, const std::string &name);

bool check_compare(const SOFTPRES13_REG &obj, const SOFTPRES13_REG &expected, const std::string &name);

bool check_compare(const SOFTPRES14_BITS &obj, const SOFTPRES14_BITS &expected, const std::string &name);

bool check_compare(const SOFTPRES14_REG &obj, const SOFTPRES14_REG &expected, const std::string &name);

bool check_compare(const SOFTPRES16_BITS &obj, const SOFTPRES16_BITS &expected, const std::string &name);

bool check_compare(const SOFTPRES16_REG &obj, const SOFTPRES16_REG &expected, const std::string &name);

bool check_compare(const CPUSEL0_BITS &obj, const CPUSEL0_BITS &expected, const std::string &name);

bool check_compare(const CPUSEL0_REG &obj, const CPUSEL0_REG &expected, const std::string &name);

bool check_compare(const CPUSEL1_BITS &obj, const CPUSEL1_BITS &expected, const std::string &name);

bool check_compare(const CPUSEL1_REG &obj, const CPUSEL1_REG &expected, const std::string &name);

bool check_compare(const CPUSEL2_BITS &obj, const CPUSEL2_BITS &expected, const std::string &name);

bool check_compare(const CPUSEL2_REG &obj, const CPUSEL2_REG &expected, const std::string &name);

bool check_compare(const CPUSEL4_BITS &obj, const CPUSEL4_BITS &expected, const std::string &name);

bool check_compare(const CPUSEL4_REG &obj, const CPUSEL4_REG &expected, const std::string &name);

bool check_compare(const CPUSEL5_BITS &obj, const CPUSEL5_BITS &expected, const std::string &name);

bool check_compare(const CPUSEL5_REG &obj, const CPUSEL5_REG &expected, const std::string &name);

bool check_compare(const CPUSEL6_BITS &obj, const CPUSEL6_BITS &expected, const std::string &name);

bool check_compare(const CPUSEL6_REG &obj, const CPUSEL6_REG &expected, const std::string &name);

bool check_compare(const CPUSEL7_BITS &obj, const CPUSEL7_BITS &expected, const std::string &name);

bool check_compare(const CPUSEL7_REG &obj, const CPUSEL7_REG &expected, const std::string &name);

bool check_compare(const CPUSEL8_BITS &obj, const CPUSEL8_BITS &expected, const std::string &name);

bool check_compare(const CPUSEL8_REG &obj, const CPUSEL8_REG &expected, const std::string &name);

bool check_compare(const CPUSEL9_BITS &obj, const CPUSEL9_BITS &expected, const std::string &name);

bool check_compare(const CPUSEL9_REG &obj, const CPUSEL9_REG &expected, const std::string &name);

bool check_compare(const CPUSEL11_BITS &obj, const CPUSEL11_BITS &expected, const std::string &name);

bool check_compare(const CPUSEL11_REG &obj, const CPUSEL11_REG &expected, const std::string &name);

bool check_compare(const CPUSEL12_BITS &obj, const CPUSEL12_BITS &expected, const std::string &name);

bool check_compare(const CPUSEL12_REG &obj, const CPUSEL12_REG &expected, const std::string &name);

bool check_compare(const CPUSEL14_BITS &obj, const CPUSEL14_BITS &expected, const std::string &name);

bool check_compare(const CPUSEL14_REG &obj, const CPUSEL14_REG &expected, const std::string &name);

bool check_compare(const CPU2RESCTL_BITS &obj, const CPU2RESCTL_BITS &expected, const std::string &name);

bool check_compare(const CPU2RESCTL_REG &obj, const CPU2RESCTL_REG &expected, const std::string &name);

bool check_compare(const RSTSTAT_BITS &obj, const RSTSTAT_BITS &expected, const std::string &name);

bool check_compare(const RSTSTAT_REG &obj, const RSTSTAT_REG &expected, const std::string &name);

bool check_compare(const LPMSTAT_BITS &obj, const LPMSTAT_BITS &expected, const std::string &name);

bool check_compare(const LPMSTAT_REG &obj, const LPMSTAT_REG &expected, const std::string &name);

bool check_compare(const SYSDBGCTL_BITS &obj, const SYSDBGCTL_BITS &expected, const std::string &name);

bool check_compare(const SYSDBGCTL_REG &obj, const SYSDBGCTL_REG &expected, const std::string &name);

bool check_compare(const DEV_CFG_REGS &obj, const DEV_CFG_REGS &expected, const std::string &name);

bool check_compare(const CLKSEM_BITS &obj, const CLKSEM_BITS &expected, const std::string &name);

bool check_compare(const CLKSEM_REG &obj, const CLKSEM_REG &expected, const std::string &name);

bool check_compare(const CLKCFGLOCK1_BITS &obj, const CLKCFGLOCK1_BITS &expected, const std::string &name);

bool check_compare(const CLKCFGLOCK1_REG &obj, const CLKCFGLOCK1_REG &expected, const std::string &name);

bool check_compare(const CLKSRCCTL1_BITS &obj, const CLKSRCCTL1_BITS &expected, const std::string &name);

bool check_compare(const CLKSRCCTL1_REG &obj, const CLKSRCCTL1_REG &expected, const std::string &name);

bool check_compare(const CLKSRCCTL2_BITS &obj, const CLKSRCCTL2_BITS &expected, const std::string &name);

bool check_compare(const CLKSRCCTL2_REG &obj, const CLKSRCCTL2_REG &expected, const std::string &name);

bool check_compare(const CLKSRCCTL3_BITS &obj, const CLKSRCCTL3_BITS &expected, const std::string &name);

bool check_compare(const CLKSRCCTL3_REG &obj, const CLKSRCCTL3_REG &expected, const std::string &name);

bool check_compare(const SYSPLLCTL1_BITS &obj, const SYSPLLCTL1_BITS &expected, const std::string &name);

bool check_compare(const SYSPLLCTL1_REG &obj, const SYSPLLCTL1_REG &expected, const std::string &name);

bool check_compare(const SYSPLLMULT_BITS &obj, const SYSPLLMULT_BITS &expected, const std::string &name);

bool check_compare(const SYSPLLMULT_REG &obj, const SYSPLLMULT_REG &expected, const std::string &name);

bool check_compare(const SYSPLLSTS_BITS &obj, const SYSPLLSTS_BITS &expected, const std::string &name);

bool check_compare(const SYSPLLSTS_REG &obj, const SYSPLLSTS_REG &expected, const std::string &name);

bool check_compare(const AUXPLLCTL1_BITS &obj, const AUXPLLCTL1_BITS &expected, const std::string &name);

bool check_compare(const AUXPLLCTL1_REG &obj, const AUXPLLCTL1_REG &expected, const std::string &name);

bool check_compare(const AUXPLLMULT_BITS &obj, const AUXPLLMULT_BITS &expected, const std::string &name);

bool check_compare(const AUXPLLMULT_REG &obj, const AUXPLLMULT_REG &expected, const std::string &name);

bool check_compare(const AUXPLLSTS_BITS &obj, const AUXPLLSTS_BITS &expected, const std::string &name);

bool check_compare(const AUXPLLSTS_REG &obj, const AUXPLLSTS_REG &expected, const std::string &name);

bool check_compare(const SYSCLKDIVSEL_BITS &obj, const SYSCLKDIVSEL_BITS &expected, const std::string &name);

bool check_compare(const SYSCLKDIVSEL_REG &obj, const SYSCLKDIVSEL_REG &expected, const std::string &name);

bool check_compare(const AUXCLKDIVSEL_BITS &obj, const AUXCLKDIVSEL_BITS &expected, const std::string &name);

bool check_compare(const AUXCLKDIVSEL_REG &obj, const AUXCLKDIVSEL_REG &expected, const std::string &name);

bool check_compare(const PERCLKDIVSEL_BITS &obj, const PERCLKDIVSEL_BITS &expected, const std::string &name);

bool check_compare(const PERCLKDIVSEL_REG &obj, const PERCLKDIVSEL_REG &expected, const std::string &name);

bool check_compare(const XCLKOUTDIVSEL_BITS &obj, const XCLKOUTDIVSEL_BITS &expected, const std::string &name);

bool check_compare(const XCLKOUTDIVSEL_REG &obj, const XCLKOUTDIVSEL_REG &expected, const std::string &name);

bool check_compare(const LOSPCP_BITS &obj, const LOSPCP_BITS &expected, const std::string &name);

bool check_compare(const LOSPCP_REG &obj, const LOSPCP_REG &expected, const std::string &name);

bool check_compare(const MCDCR_BITS &obj, const MCDCR_BITS &expected, const std::string &name);

bool check_compare(const MCDCR_REG &obj, const MCDCR_REG &expected, const std::string &name);

bool check_compare(const X1CNT_BITS &obj, const X1CNT_BITS &expected, const std::string &name);

bool check_compare(const X1CNT_REG &obj, const X1CNT_REG &expected, const std::string &name);

bool check_compare(const CLK_CFG_REGS &obj, const CLK_CFG_REGS &expected, const std::string &name);

bool check_compare(const CPUSYSLOCK1_BITS &obj, const CPUSYSLOCK1_BITS &expected, const std::string &name);

bool check_compare(const CPUSYSLOCK1_REG &obj, const CPUSYSLOCK1_REG &expected, const std::string &name);

bool check_compare(const IORESTOREADDR_BITS &obj, const IORESTOREADDR_BITS &expected, const std::string &name);

bool check_compare(const IORESTOREADDR_REG &obj, const IORESTOREADDR_REG &expected, const std::string &name);

bool check_compare(const PIEVERRADDR_BITS &obj, const PIEVERRADDR_BITS &expected, const std::string &name);

bool check_compare(const PIEVERRADDR_REG &obj, const PIEVERRADDR_REG &expected, const std::string &name);

bool check_compare(const PCLKCR0_BITS &obj, const PCLKCR0_BITS &expected, const std::string &name);

bool check_compare(const PCLKCR0_REG &obj, const PCLKCR0_REG &expected, const std::string &name);

bool check_compare(const PCLKCR1_BITS &obj, const PCLKCR1_BITS &expected, const std::string &name);

bool check_compare(const PCLKCR1_REG &obj, const PCLKCR1_REG &expected, const std::string &name);

bool check_compare(const PCLKCR2_BITS &obj, const PCLKCR2_BITS &expected, const std::string &name);

bool check_compare(const PCLKCR2_REG &obj, const PCLKCR2_REG &expected, const std::string &name);

bool check_compare(const PCLKCR3_BITS &obj, const PCLKCR3_BITS &expected, const std::string &name);

bool check_compare(const PCLKCR3_REG &obj, const PCLKCR3_REG &expected, const std::string &name);

bool check_compare(const PCLKCR4_BITS &obj, const PCLKCR4_BITS &expected, const std::string &name);

bool check_compare(const PCLKCR4_REG &obj, const PCLKCR4_REG &expected, const std::string &name);

bool check_compare(const PCLKCR6_BITS &obj, const PCLKCR6_BITS &expected, const std::string &name);

bool check_compare(const PCLKCR6_REG &obj, const PCLKCR6_REG &expected, const std::string &name);

bool check_compare(const PCLKCR7_BITS &obj, const PCLKCR7_BITS &expected, const std::string &name);

bool check_compare(const PCLKCR7_REG &obj, const PCLKCR7_REG &expected, const std::string &name);

bool check_compare(const PCLKCR8_BITS &obj, const PCLKCR8_BITS &expected, const std::string &name);

bool check_compare(const PCLKCR8_REG &obj, const PCLKCR8_REG &expected, const std::string &name);

bool check_compare(const PCLKCR9_BITS &obj, const PCLKCR9_BITS &expected, const std::string &name);

bool check_compare(const PCLKCR9_REG &obj, const PCLKCR9_REG &expected, const std::string &name);

bool check_compare(const PCLKCR10_BITS &obj, const PCLKCR10_BITS &expected, const std::string &name);

bool check_compare(const PCLKCR10_REG &obj, const PCLKCR10_REG &expected, const std::string &name);

bool check_compare(const PCLKCR11_BITS &obj, const PCLKCR11_BITS &expected, const std::string &name);

bool check_compare(const PCLKCR11_REG &obj, const PCLKCR11_REG &expected, const std::string &name);

bool check_compare(const PCLKCR12_BITS &obj, const PCLKCR12_BITS &expected, const std::string &name);

bool check_compare(const PCLKCR12_REG &obj, const PCLKCR12_REG &expected, const std::string &name);

bool check_compare(const PCLKCR13_BITS &obj, const PCLKCR13_BITS &expected, const std::string &name);

bool check_compare(const PCLKCR13_REG &obj, const PCLKCR13_REG &expected, const std::string &name);

bool check_compare(const PCLKCR14_BITS &obj, const PCLKCR14_BITS &expected, const std::string &name);

bool check_compare(const PCLKCR14_REG &obj, const PCLKCR14_REG &expected, const std::string &name);

bool check_compare(const PCLKCR16_BITS &obj, const PCLKCR16_BITS &expected, const std::string &name);

bool check_compare(const PCLKCR16_REG &obj, const PCLKCR16_REG &expected, const std::string &name);

bool check_compare(const SECMSEL_BITS &obj, const SECMSEL_BITS &expected, const std::string &name);

bool check_compare(const SECMSEL_REG &obj, const SECMSEL_REG &expected, const std::string &name);

bool check_compare(const LPMCR_BITS &obj, const LPMCR_BITS &expected, const std::string &name);

bool check_compare(const LPMCR_REG &obj, const LPMCR_REG &expected, const std::string &name);

bool check_compare(const GPIOLPMSEL0_BITS &obj, const GPIOLPMSEL0_BITS &expected, const std::string &name);

bool check_compare(const GPIOLPMSEL0_REG &obj, const GPIOLPMSEL0_REG &expected, const std::string &name);

bool check_compare(const GPIOLPMSEL1_BITS &obj, const GPIOLPMSEL1_BITS &expected, const std::string &name);

bool check_compare(const GPIOLPMSEL1_REG &obj, const GPIOLPMSEL1_REG &expected, const std::string &name);

bool check_compare(const TMR2CLKCTL_BITS &obj, const TMR2CLKCTL_BITS &expected, const std::string &name);

bool check_compare(const TMR2CLKCTL_REG &obj, const TMR2CLKCTL_REG &expected, const std::string &name);

bool check_compare(const RESC_BITS &obj, const RESC_BITS &expected, const std::string &name);

bool check_compare(const RESC_REG &obj, const RESC_REG &expected, const std::string &name);

bool check_compare(const CPU_SYS_REGS &obj, const CPU_SYS_REGS &expected, const std::string &name);

bool check_compare(const SCSR_BITS &obj, const SCSR_BITS &expected, const std::string &name);

bool check_compare(const SCSR_REG &obj, const SCSR_REG &expected, const std::string &name);

bool check_compare(const WDCNTR_BITS &obj, const WDCNTR_BITS &expected, const std::string &name);

bool check_compare(const WDCNTR_REG &obj, const WDCNTR_REG &expected, const std::string &name);

bool check_compare(const WDKEY_BITS &obj, const WDKEY_BITS &expected, const std::string &name);

bool check_compare(const WDKEY_REG &obj, const WDKEY_REG &expected, const std::string &name);

bool check_compare(const WDCR_BITS &obj, const WDCR_BITS &expected, const std::string &name);

bool check_compare(const WDCR_REG &obj, const WDCR_REG &expected, const std::string &name);

bool check_compare(const WDWCR_BITS &obj, const WDWCR_BITS &expected, const std::string &name);

bool check_compare(const WDWCR_REG &obj, const WDWCR_REG &expected, const std::string &name);

bool check_compare(const WD_REGS &obj, const WD_REGS &expected, const std::string &name);

bool check_compare(const CLA1TASKSRCSELLOCK_BITS &obj, const CLA1TASKSRCSELLOCK_BITS &expected,
                   const std::string &name);

bool check_compare(const CLA1TASKSRCSELLOCK_REG &obj, const CLA1TASKSRCSELLOCK_REG &expected, const std::string &name);

bool check_compare(const DMACHSRCSELLOCK_BITS &obj, const DMACHSRCSELLOCK_BITS &expected, const std::string &name);

bool check_compare(const DMACHSRCSELLOCK_REG &obj, const DMACHSRCSELLOCK_REG &expected, const std::string &name);

bool check_compare(const CLA1TASKSRCSEL1_BITS &obj, const CLA1TASKSRCSEL1_BITS &expected, const std::string &name);

bool check_compare(const CLA1TASKSRCSEL1_REG &obj, const CLA1TASKSRCSEL1_REG &expected, const std::string &name);

bool check_compare(const CLA1TASKSRCSEL2_BITS &obj, const CLA1TASKSRCSEL2_BITS &expected, const std::string &name);

bool check_compare(const CLA1TASKSRCSEL2_REG &obj, const CLA1TASKSRCSEL2_REG &expected, const std::string &name);

bool check_compare(const DMACHSRCSEL1_BITS &obj, const DMACHSRCSEL1_BITS &expected, const std::string &name);

bool check_compare(const DMACHSRCSEL1_REG &obj, const DMACHSRCSEL1_REG &expected, const std::string &name);

bool check_compare(const DMACHSRCSEL2_BITS &obj, const DMACHSRCSEL2_BITS &expected, const std::string &name);

bool check_compare(const DMACHSRCSEL2_REG &obj, const DMACHSRCSEL2_REG &expected, const std::string &name);

bool check_compare(const DMA_CLA_SRC_SEL_REGS &obj, const DMA_CLA_SRC_SEL_REGS &expected, const std::string &name);

bool check_compare(const SYNCSELECT_BITS &obj, const SYNCSELECT_BITS &expected, const std::string &name);

bool check_compare(const SYNCSELECT_REG &obj, const SYNCSELECT_REG &expected, const std::string &name);

bool check_compare(const ADCSOCOUTSELECT_BITS &obj, const ADCSOCOUTSELECT_BITS &expected, const std::string &name);

bool check_compare(const ADCSOCOUTSELECT_REG &obj, const ADCSOCOUTSELECT_REG &expected, const std::string &name);

bool check_compare(const SYNCSOCLOCK_BITS &obj, const SYNCSOCLOCK_BITS &expected, const std::string &name);

bool check_compare(const SYNCSOCLOCK_REG &obj, const SYNCSOCLOCK_REG &expected, const std::string &name);

bool check_compare(const SYNC_SOC_REGS &obj, const SYNC_SOC_REGS &expected, const std::string &name);

bool check_compare(const XINT1CR_BITS &obj, const XINT1CR_BITS &expected, const std::string &name);

bool check_compare(const XINT1CR_REG &obj, const XINT1CR_REG &expected, const std::string &name);

bool check_compare(const XINT2CR_BITS &obj, const XINT2CR_BITS &expected, const std::string &name);

bool check_compare(const XINT2CR_REG &obj, const XINT2CR_REG &expected, const std::string &name);

bool check_compare(const XINT3CR_BITS &obj, const XINT3CR_BITS &expected, const std::string &name);

bool check_compare(const XINT3CR_REG &obj, const XINT3CR_REG &expected, const std::string &name);

bool check_compare(const XINT4CR_BITS &obj, const XINT4CR_BITS &expected, const std::string &name);

bool check_compare(const XINT4CR_REG &obj, const XINT4CR_REG &expected, const std::string &name);

bool check_compare(const XINT5CR_BITS &obj, const XINT5CR_BITS &expected, const std::string &name);

bool check_compare(const XINT5CR_REG &obj, const XINT5CR_REG &expected, const std::string &name);

bool check_compare(const XINT_REGS &obj, const XINT_REGS &expected, const std::string &name);