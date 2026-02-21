//
// Created by marius on 2/20/26.
//

#include "checks/state_checker.h"
#include "checks/generated.hpp"

void HardwareStateValidator::populate_map() {
    tracker_["AccessProtectionRegs"] = [] {
        return check_zero(const_cast<const ACCESS_PROTECTION_REGS &>(AccessProtectionRegs), "AccessProtectionRegs");
    };

    tracker_["AdcaRegs"] = [] {
        return check_zero(const_cast<const ADC_REGS &>(AdcaRegs), "AdcaRegs");
    };

    tracker_["AdcbRegs"] = [] {
        return check_zero(const_cast<const ADC_REGS &>(AdcbRegs), "AdcbRegs");
    };

    tracker_["AdccRegs"] = [] {
        return check_zero(const_cast<const ADC_REGS &>(AdccRegs), "AdccRegs");
    };

    tracker_["AdcdRegs"] = [] {
        return check_zero(const_cast<const ADC_REGS &>(AdcdRegs), "AdcdRegs");
    };

    tracker_["AdcaResultRegs"] = [] {
        return check_zero(const_cast<const ADC_RESULT_REGS &>(AdcaResultRegs), "AdcaResultRegs");
    };

    tracker_["AdcbResultRegs"] = [] {
        return check_zero(const_cast<const ADC_RESULT_REGS &>(AdcbResultRegs), "AdcbResultRegs");
    };

    tracker_["AdccResultRegs"] = [] {
        return check_zero(const_cast<const ADC_RESULT_REGS &>(AdccResultRegs), "AdccResultRegs");
    };

    tracker_["AdcdResultRegs"] = [] {
        return check_zero(const_cast<const ADC_RESULT_REGS &>(AdcdResultRegs), "AdcdResultRegs");
    };

    tracker_["AnalogSubsysRegs"] = [] {
        return check_zero(const_cast<const ANALOG_SUBSYS_REGS &>(AnalogSubsysRegs), "AnalogSubsysRegs");
    };

    tracker_["Cla1Regs"] = [] {
        return check_zero(const_cast<const CLA_REGS &>(Cla1Regs), "Cla1Regs");
    };

    tracker_["Clb1DataExchRegs"] = [] {
        return check_zero(const_cast<const CLB_DATA_EXCHANGE_REGS &>(Clb1DataExchRegs), "Clb1DataExchRegs");
    };

    tracker_["Clb2DataExchRegs"] = [] {
        return check_zero(const_cast<const CLB_DATA_EXCHANGE_REGS &>(Clb2DataExchRegs), "Clb2DataExchRegs");
    };

    tracker_["Clb3DataExchRegs"] = [] {
        return check_zero(const_cast<const CLB_DATA_EXCHANGE_REGS &>(Clb3DataExchRegs), "Clb3DataExchRegs");
    };

    tracker_["Clb4DataExchRegs"] = [] {
        return check_zero(const_cast<const CLB_DATA_EXCHANGE_REGS &>(Clb4DataExchRegs), "Clb4DataExchRegs");
    };

    tracker_["Clb1LogicCfgRegs"] = [] {
        return check_zero(const_cast<const CLB_LOGIC_CONFIG_REGS &>(Clb1LogicCfgRegs), "Clb1LogicCfgRegs");
    };

    tracker_["Clb2LogicCfgRegs"] = [] {
        return check_zero(const_cast<const CLB_LOGIC_CONFIG_REGS &>(Clb2LogicCfgRegs), "Clb2LogicCfgRegs");
    };

    tracker_["Clb3LogicCfgRegs"] = [] {
        return check_zero(const_cast<const CLB_LOGIC_CONFIG_REGS &>(Clb3LogicCfgRegs), "Clb3LogicCfgRegs");
    };

    tracker_["Clb4LogicCfgRegs"] = [] {
        return check_zero(const_cast<const CLB_LOGIC_CONFIG_REGS &>(Clb4LogicCfgRegs), "Clb4LogicCfgRegs");
    };

    tracker_["Clb1LogicCtrlRegs"] = [] {
        return check_zero(const_cast<const CLB_LOGIC_CONTROL_REGS &>(Clb1LogicCtrlRegs), "Clb1LogicCtrlRegs");
    };

    tracker_["Clb2LogicCtrlRegs"] = [] {
        return check_zero(const_cast<const CLB_LOGIC_CONTROL_REGS &>(Clb2LogicCtrlRegs), "Clb2LogicCtrlRegs");
    };

    tracker_["Clb3LogicCtrlRegs"] = [] {
        return check_zero(const_cast<const CLB_LOGIC_CONTROL_REGS &>(Clb3LogicCtrlRegs), "Clb3LogicCtrlRegs");
    };

    tracker_["Clb4LogicCtrlRegs"] = [] {
        return check_zero(const_cast<const CLB_LOGIC_CONTROL_REGS &>(Clb4LogicCtrlRegs), "Clb4LogicCtrlRegs");
    };

    tracker_["ClbXbarRegs"] = [] {
        return check_zero(const_cast<const CLB_XBAR_REGS &>(ClbXbarRegs), "ClbXbarRegs");
    };

    tracker_["ClkCfgRegs"] = [] {
        return check_zero(const_cast<const CLK_CFG_REGS &>(ClkCfgRegs), "ClkCfgRegs");
    };

    tracker_["Cmpss1Regs"] = [] {
        return check_zero(const_cast<const CMPSS_REGS &>(Cmpss1Regs), "Cmpss1Regs");
    };

    tracker_["Cmpss2Regs"] = [] {
        return check_zero(const_cast<const CMPSS_REGS &>(Cmpss2Regs), "Cmpss2Regs");
    };

    tracker_["Cmpss3Regs"] = [] {
        return check_zero(const_cast<const CMPSS_REGS &>(Cmpss3Regs), "Cmpss3Regs");
    };

    tracker_["Cmpss4Regs"] = [] {
        return check_zero(const_cast<const CMPSS_REGS &>(Cmpss4Regs), "Cmpss4Regs");
    };

    tracker_["Cmpss5Regs"] = [] {
        return check_zero(const_cast<const CMPSS_REGS &>(Cmpss5Regs), "Cmpss5Regs");
    };

    tracker_["Cmpss6Regs"] = [] {
        return check_zero(const_cast<const CMPSS_REGS &>(Cmpss6Regs), "Cmpss6Regs");
    };

    tracker_["Cmpss7Regs"] = [] {
        return check_zero(const_cast<const CMPSS_REGS &>(Cmpss7Regs), "Cmpss7Regs");
    };

    tracker_["Cmpss8Regs"] = [] {
        return check_zero(const_cast<const CMPSS_REGS &>(Cmpss8Regs), "Cmpss8Regs");
    };

    tracker_["CpuTimer0Regs"] = [] {
        return check_zero(const_cast<const CPUTIMER_REGS &>(CpuTimer0Regs), "CpuTimer0Regs");
    };

    tracker_["CpuTimer1Regs"] = [] {
        return check_zero(const_cast<const CPUTIMER_REGS &>(CpuTimer1Regs), "CpuTimer1Regs");
    };

    tracker_["CpuTimer2Regs"] = [] {
        return check_zero(const_cast<const CPUTIMER_REGS &>(CpuTimer2Regs), "CpuTimer2Regs");
    };

    tracker_["CpuSysRegs"] = [] {
        return check_zero(const_cast<const CPU_SYS_REGS &>(CpuSysRegs), "CpuSysRegs");
    };

    tracker_["DacaRegs"] = [] {
        return check_zero(const_cast<const DAC_REGS &>(DacaRegs), "DacaRegs");
    };

    tracker_["DacbRegs"] = [] {
        return check_zero(const_cast<const DAC_REGS &>(DacbRegs), "DacbRegs");
    };

    tracker_["DaccRegs"] = [] {
        return check_zero(const_cast<const DAC_REGS &>(DaccRegs), "DaccRegs");
    };

    tracker_["DcsmCommonRegs"] = [] {
        return check_zero(const_cast<const DCSM_COMMON_REGS &>(DcsmCommonRegs), "DcsmCommonRegs");
    };

    tracker_["DcsmZ1Regs"] = [] {
        return check_zero(const_cast<const DCSM_Z1_REGS &>(DcsmZ1Regs), "DcsmZ1Regs");
    };

    tracker_["DcsmZ2Regs"] = [] {
        return check_zero(const_cast<const DCSM_Z2_REGS &>(DcsmZ2Regs), "DcsmZ2Regs");
    };

    tracker_["DevCfgRegs"] = [] {
        return check_zero(const_cast<const DEV_CFG_REGS &>(DevCfgRegs), "DevCfgRegs");
    };

    tracker_["DmaClaSrcSelRegs"] = [] {
        return check_zero(const_cast<const DMA_CLA_SRC_SEL_REGS &>(DmaClaSrcSelRegs), "DmaClaSrcSelRegs");
    };

    tracker_["DmaRegs"] = [] {
        return check_zero(const_cast<const DMA_REGS &>(DmaRegs), "DmaRegs");
    };

    tracker_["ECap1Regs"] = [] {
        return check_zero(const_cast<const ECAP_REGS &>(ECap1Regs), "ECap1Regs");
    };

    tracker_["ECap2Regs"] = [] {
        return check_zero(const_cast<const ECAP_REGS &>(ECap2Regs), "ECap2Regs");
    };

    tracker_["ECap3Regs"] = [] {
        return check_zero(const_cast<const ECAP_REGS &>(ECap3Regs), "ECap3Regs");
    };

    tracker_["ECap4Regs"] = [] {
        return check_zero(const_cast<const ECAP_REGS &>(ECap4Regs), "ECap4Regs");
    };

    tracker_["ECap5Regs"] = [] {
        return check_zero(const_cast<const ECAP_REGS &>(ECap5Regs), "ECap5Regs");
    };

    tracker_["ECap6Regs"] = [] {
        return check_zero(const_cast<const ECAP_REGS &>(ECap6Regs), "ECap6Regs");
    };

    tracker_["Emif1ConfigRegs"] = [] {
        return check_zero(const_cast<const EMIF1_CONFIG_REGS &>(Emif1ConfigRegs), "Emif1ConfigRegs");
    };

    tracker_["Emif2ConfigRegs"] = [] {
        return check_zero(const_cast<const EMIF2_CONFIG_REGS &>(Emif2ConfigRegs), "Emif2ConfigRegs");
    };

    tracker_["Emif1Regs"] = [] {
        return check_zero(const_cast<const EMIF_REGS &>(Emif1Regs), "Emif1Regs");
    };

    tracker_["Emif2Regs"] = [] {
        return check_zero(const_cast<const EMIF_REGS &>(Emif2Regs), "Emif2Regs");
    };

    tracker_["EPwm1Regs"] = [] {
        return check_zero(const_cast<const EPWM_REGS &>(EPwm1Regs), "EPwm1Regs");
    };

    tracker_["EPwm2Regs"] = [] {
        return check_zero(const_cast<const EPWM_REGS &>(EPwm2Regs), "EPwm2Regs");
    };

    tracker_["EPwm3Regs"] = [] {
        return check_zero(const_cast<const EPWM_REGS &>(EPwm3Regs), "EPwm3Regs");
    };

    tracker_["EPwm4Regs"] = [] {
        return check_zero(const_cast<const EPWM_REGS &>(EPwm4Regs), "EPwm4Regs");
    };

    tracker_["EPwm5Regs"] = [] {
        return check_zero(const_cast<const EPWM_REGS &>(EPwm5Regs), "EPwm5Regs");
    };

    tracker_["EPwm6Regs"] = [] {
        return check_zero(const_cast<const EPWM_REGS &>(EPwm6Regs), "EPwm6Regs");
    };

    tracker_["EPwm7Regs"] = [] {
        return check_zero(const_cast<const EPWM_REGS &>(EPwm7Regs), "EPwm7Regs");
    };

    tracker_["EPwm8Regs"] = [] {
        return check_zero(const_cast<const EPWM_REGS &>(EPwm8Regs), "EPwm8Regs");
    };

    tracker_["EPwm9Regs"] = [] {
        return check_zero(const_cast<const EPWM_REGS &>(EPwm9Regs), "EPwm9Regs");
    };

    tracker_["EPwm10Regs"] = [] {
        return check_zero(const_cast<const EPWM_REGS &>(EPwm10Regs), "EPwm10Regs");
    };

    tracker_["EPwm11Regs"] = [] {
        return check_zero(const_cast<const EPWM_REGS &>(EPwm11Regs), "EPwm11Regs");
    };

    tracker_["EPwm12Regs"] = [] {
        return check_zero(const_cast<const EPWM_REGS &>(EPwm12Regs), "EPwm12Regs");
    };

    tracker_["EPwmXbarRegs"] = [] {
        return check_zero(const_cast<const EPWM_XBAR_REGS &>(EPwmXbarRegs), "EPwmXbarRegs");
    };

    tracker_["EQep1Regs"] = [] {
        return check_zero(const_cast<const EQEP_REGS &>(EQep1Regs), "EQep1Regs");
    };

    tracker_["EQep2Regs"] = [] {
        return check_zero(const_cast<const EQEP_REGS &>(EQep2Regs), "EQep2Regs");
    };

    tracker_["EQep3Regs"] = [] {
        return check_zero(const_cast<const EQEP_REGS &>(EQep3Regs), "EQep3Regs");
    };

    tracker_["Flash0CtrlRegs"] = [] {
        return check_zero(const_cast<const FLASH_CTRL_REGS &>(Flash0CtrlRegs), "Flash0CtrlRegs");
    };

    tracker_["Flash0EccRegs"] = [] {
        return check_zero(const_cast<const FLASH_ECC_REGS &>(Flash0EccRegs), "Flash0EccRegs");
    };

    tracker_["FlashPumpSemaphoreRegs"] = [] {
        return check_zero(const_cast<const FLASH_PUMP_SEMAPHORE_REGS &>(FlashPumpSemaphoreRegs),
                          "FlashPumpSemaphoreRegs");
    };

    tracker_["GpioCtrlRegs"] = [] {
        return check_zero(const_cast<const GPIO_CTRL_REGS &>(GpioCtrlRegs), "GpioCtrlRegs");
    };

    tracker_["GpioDataRegs"] = [] {
        return check_zero(const_cast<const GPIO_DATA_REGS &>(GpioDataRegs), "GpioDataRegs");
    };

    tracker_["I2caRegs"] = [] {
        return check_zero(const_cast<const I2C_REGS &>(I2caRegs), "I2caRegs");
    };

    tracker_["I2cbRegs"] = [] {
        return check_zero(const_cast<const I2C_REGS &>(I2cbRegs), "I2cbRegs");
    };

    tracker_["MemoryErrorRegs"] = [] {
        return check_zero(const_cast<const MEMORY_ERROR_REGS &>(MemoryErrorRegs), "MemoryErrorRegs");
    };

    tracker_["MemCfgRegs"] = [] {
        return check_zero(const_cast<const MEM_CFG_REGS &>(MemCfgRegs), "MemCfgRegs");
    };

    tracker_["McbspaRegs"] = [] {
        return check_zero(const_cast<const McBSP_REGS &>(McbspaRegs), "McbspaRegs");
    };

    tracker_["McbspbRegs"] = [] {
        return check_zero(const_cast<const McBSP_REGS &>(McbspbRegs), "McbspbRegs");
    };

    tracker_["NmiIntruptRegs"] = [] {
        return check_zero(const_cast<const NMI_INTRUPT_REGS &>(NmiIntruptRegs), "NmiIntruptRegs");
    };

    tracker_["PieCtrlRegs"] = [] {
        return check_zero(const_cast<const PIE_CTRL_REGS &>(PieCtrlRegs), "PieCtrlRegs");
    };

    tracker_["PieVectTable"] = [] {
        return check_zero(const_cast<const PIE_VECT_TABLE &>(PieVectTable), "PieVectTable");
    };

    tracker_["RomPrefetchRegs"] = [] {
        return check_zero(const_cast<const ROM_PREFETCH_REGS &>(RomPrefetchRegs), "RomPrefetchRegs");
    };

    tracker_["SciaRegs"] = [] {
        return check_zero(const_cast<const SCI_REGS &>(SciaRegs), "SciaRegs");
    };

    tracker_["ScibRegs"] = [] {
        return check_zero(const_cast<const SCI_REGS &>(ScibRegs), "ScibRegs");
    };

    tracker_["ScicRegs"] = [] {
        return check_zero(const_cast<const SCI_REGS &>(ScicRegs), "ScicRegs");
    };

    tracker_["ScidRegs"] = [] {
        return check_zero(const_cast<const SCI_REGS &>(ScidRegs), "ScidRegs");
    };

    tracker_["Sdfm1Regs"] = [] {
        return check_zero(const_cast<const SDFM_REGS &>(Sdfm1Regs), "Sdfm1Regs");
    };

    tracker_["Sdfm2Regs"] = [] {
        return check_zero(const_cast<const SDFM_REGS &>(Sdfm2Regs), "Sdfm2Regs");
    };

    tracker_["SpiaRegs"] = [] {
        return check_zero(const_cast<const SPI_REGS &>(SpiaRegs), "SpiaRegs");
    };

    tracker_["SpibRegs"] = [] {
        return check_zero(const_cast<const SPI_REGS &>(SpibRegs), "SpibRegs");
    };

    tracker_["SpicRegs"] = [] {
        return check_zero(const_cast<const SPI_REGS &>(SpicRegs), "SpicRegs");
    };

    tracker_["WdRegs"] = [] {
        return check_zero(const_cast<const WD_REGS &>(WdRegs), "WdRegs");
    };

    tracker_["XintRegs"] = [] {
        return check_zero(const_cast<const XINT_REGS &>(XintRegs), "XintRegs");
    };

    tracker_["GpioSetup"] = [] {
        return check_zero(gpiosSetup, "GpioSetup");
    };

    tracker_["AdcSetup"] = [] {
        return check_zero(adcSetup, "AdcSetup");
    };
}
