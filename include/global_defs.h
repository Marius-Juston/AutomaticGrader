//
// Created by Marius on 2/20/2026.
//

#ifndef AUTOMATICGRADER_GLOBAL_DEFS_H
#define AUTOMATICGRADER_GLOBAL_DEFS_H

#include "F2837xD_adc.h"
#include "F2837xD_analogsubsys.h"
#include "F2837xD_cla.h"
#include "F2837xD_clb.h"
#include "F2837xD_clbxbar.h"
#include "F2837xD_cmpss.h"
#include "F2837xD_cputimer.h"
#include "F2837xD_dac.h"
#include "F2837xD_dcsm.h"
#include "F2837xD_dma.h"
#include "F2837xD_ecap.h"
#include "F2837xD_emif.h"
#include "F2837xD_epwm.h"                // Enhanced PWM
#include "F2837xD_epwm_xbar.h"
#include "F2837xD_eqep.h"
#include "F2837xD_flash.h"
#include "F2837xD_gpio.h"                // General Purpose I/O Registers
#include "F2837xD_i2c.h"
#include "F2837xD_input_xbar.h"
#include "F2837xD_ipc.h"
#include "F2837xD_mcbsp.h"
#include "F2837xD_memconfig.h"
#include "F2837xD_nmiintrupt.h"          // NMI Interrupt Registers
#include "F2837xD_output_xbar.h"
#include "F2837xD_piectrl.h"             // PIE Control Registers
#include "F2837xD_pievect.h"
#include "F2837xD_sci.h"
#include "F2837xD_sdfm.h"
#include "F2837xD_spi.h"
#include "F2837xD_sysctrl.h"             // System Control/Power Modes
#include "F2837xD_upp.h"
#include "F2837xD_xbar.h"
#include "F2837xD_xint.h"                // External Interrupts

#include "F2837xD_Pie_defines.h"


extern volatile struct ACCESS_PROTECTION_REGS AccessProtectionRegs;


extern volatile struct ADC_REGS AdcaRegs;


extern volatile struct ADC_REGS AdcbRegs;


extern volatile struct ADC_REGS AdccRegs;


extern volatile struct ADC_REGS AdcdRegs;


extern volatile struct ADC_RESULT_REGS AdcaResultRegs;


extern volatile struct ADC_RESULT_REGS AdcbResultRegs;


extern volatile struct ADC_RESULT_REGS AdccResultRegs;


extern volatile struct ADC_RESULT_REGS AdcdResultRegs;


extern volatile struct ANALOG_SUBSYS_REGS AnalogSubsysRegs;


extern volatile struct CLA_REGS Cla1Regs;


extern volatile struct CLB_DATA_EXCHANGE_REGS Clb1DataExchRegs;


extern volatile struct CLB_DATA_EXCHANGE_REGS Clb2DataExchRegs;


extern volatile struct CLB_DATA_EXCHANGE_REGS Clb3DataExchRegs;


extern volatile struct CLB_DATA_EXCHANGE_REGS Clb4DataExchRegs;


extern volatile struct CLB_LOGIC_CONFIG_REGS Clb1LogicCfgRegs;


extern volatile struct CLB_LOGIC_CONFIG_REGS Clb2LogicCfgRegs;


extern volatile struct CLB_LOGIC_CONFIG_REGS Clb3LogicCfgRegs;


extern volatile struct CLB_LOGIC_CONFIG_REGS Clb4LogicCfgRegs;


extern volatile struct CLB_LOGIC_CONTROL_REGS Clb1LogicCtrlRegs;


extern volatile struct CLB_LOGIC_CONTROL_REGS Clb2LogicCtrlRegs;


extern volatile struct CLB_LOGIC_CONTROL_REGS Clb3LogicCtrlRegs;


extern volatile struct CLB_LOGIC_CONTROL_REGS Clb4LogicCtrlRegs;


extern volatile struct CLB_XBAR_REGS ClbXbarRegs;


extern volatile struct CLK_CFG_REGS ClkCfgRegs;


extern volatile struct CMPSS_REGS Cmpss1Regs;


extern volatile struct CMPSS_REGS Cmpss2Regs;


extern volatile struct CMPSS_REGS Cmpss3Regs;


extern volatile struct CMPSS_REGS Cmpss4Regs;


extern volatile struct CMPSS_REGS Cmpss5Regs;


extern volatile struct CMPSS_REGS Cmpss6Regs;


extern volatile struct CMPSS_REGS Cmpss7Regs;


extern volatile struct CMPSS_REGS Cmpss8Regs;


extern volatile struct CPUTIMER_REGS CpuTimer0Regs;


extern volatile struct CPUTIMER_REGS CpuTimer1Regs;


extern volatile struct CPUTIMER_REGS CpuTimer2Regs;


extern volatile struct CPU_SYS_REGS CpuSysRegs;


extern volatile struct DAC_REGS DacaRegs;


extern volatile struct DAC_REGS DacbRegs;


extern volatile struct DAC_REGS DaccRegs;


extern volatile struct DCSM_COMMON_REGS DcsmCommonRegs;


extern volatile struct DCSM_Z1_REGS DcsmZ1Regs;


extern volatile struct DCSM_Z2_REGS DcsmZ2Regs;


extern volatile struct DEV_CFG_REGS DevCfgRegs;


extern volatile struct DMA_CLA_SRC_SEL_REGS DmaClaSrcSelRegs;


extern volatile struct DMA_REGS DmaRegs;


extern volatile struct ECAP_REGS ECap1Regs;


extern volatile struct ECAP_REGS ECap2Regs;


extern volatile struct ECAP_REGS ECap3Regs;


extern volatile struct ECAP_REGS ECap4Regs;


extern volatile struct ECAP_REGS ECap5Regs;


extern volatile struct ECAP_REGS ECap6Regs;


extern volatile struct EMIF1_CONFIG_REGS Emif1ConfigRegs;


extern volatile struct EMIF2_CONFIG_REGS Emif2ConfigRegs;


extern volatile struct EMIF_REGS Emif1Regs;


extern volatile struct EMIF_REGS Emif2Regs;


extern volatile struct EPWM_REGS EPwm1Regs;


extern volatile struct EPWM_REGS EPwm2Regs;


extern volatile struct EPWM_REGS EPwm3Regs;


extern volatile struct EPWM_REGS EPwm4Regs;


extern volatile struct EPWM_REGS EPwm5Regs;


extern volatile struct EPWM_REGS EPwm6Regs;


extern volatile struct EPWM_REGS EPwm7Regs;


extern volatile struct EPWM_REGS EPwm8Regs;


extern volatile struct EPWM_REGS EPwm9Regs;


extern volatile struct EPWM_REGS EPwm10Regs;


extern volatile struct EPWM_REGS EPwm11Regs;


extern volatile struct EPWM_REGS EPwm12Regs;


extern volatile struct EPWM_XBAR_REGS EPwmXbarRegs;


extern volatile struct EQEP_REGS EQep1Regs;


extern volatile struct EQEP_REGS EQep2Regs;


extern volatile struct EQEP_REGS EQep3Regs;


extern volatile struct FLASH_CTRL_REGS Flash0CtrlRegs;


extern volatile struct FLASH_ECC_REGS Flash0EccRegs;


extern volatile struct FLASH_PUMP_SEMAPHORE_REGS FlashPumpSemaphoreRegs;


extern volatile struct GPIO_CTRL_REGS GpioCtrlRegs;


extern volatile struct GPIO_DATA_REGS GpioDataRegs;


extern volatile struct I2C_REGS I2caRegs;


extern volatile struct I2C_REGS I2cbRegs;


extern volatile struct MEMORY_ERROR_REGS MemoryErrorRegs;


extern volatile struct MEM_CFG_REGS MemCfgRegs;


extern volatile struct McBSP_REGS McbspaRegs;

extern volatile struct McBSP_REGS McbspbRegs;

extern volatile struct NMI_INTRUPT_REGS NmiIntruptRegs;


extern volatile struct PIE_CTRL_REGS PieCtrlRegs;


extern volatile struct PIE_VECT_TABLE PieVectTable;


extern volatile struct ROM_PREFETCH_REGS RomPrefetchRegs;


extern volatile struct SCI_REGS SciaRegs;


extern volatile struct SCI_REGS ScibRegs;


extern volatile struct SCI_REGS ScicRegs;


extern volatile struct SCI_REGS ScidRegs;


extern volatile struct SDFM_REGS Sdfm1Regs;


extern volatile struct SDFM_REGS Sdfm2Regs;


extern volatile struct SPI_REGS SpiaRegs;


extern volatile struct SPI_REGS SpibRegs;

extern volatile struct SPI_REGS SpicRegs;


extern volatile struct WD_REGS WdRegs;


extern volatile struct XINT_REGS XintRegs;

#endif //AUTOMATICGRADER_GLOBAL_DEFS_H