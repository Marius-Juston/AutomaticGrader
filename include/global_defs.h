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


extern struct ACCESS_PROTECTION_REGS AccessProtectionRegs;


extern struct ADC_REGS AdcaRegs;


extern struct ADC_REGS AdcbRegs;


extern struct ADC_REGS AdccRegs;


extern struct ADC_REGS AdcdRegs;


extern struct ADC_RESULT_REGS AdcaResultRegs;


extern struct ADC_RESULT_REGS AdcbResultRegs;


extern struct ADC_RESULT_REGS AdccResultRegs;


extern struct ADC_RESULT_REGS AdcdResultRegs;


extern struct ANALOG_SUBSYS_REGS AnalogSubsysRegs;


extern struct CLA_REGS Cla1Regs;


extern struct CLB_DATA_EXCHANGE_REGS Clb1DataExchRegs;


extern struct CLB_DATA_EXCHANGE_REGS Clb2DataExchRegs;


extern struct CLB_DATA_EXCHANGE_REGS Clb3DataExchRegs;


extern struct CLB_DATA_EXCHANGE_REGS Clb4DataExchRegs;


extern struct CLB_LOGIC_CONFIG_REGS Clb1LogicCfgRegs;


extern struct CLB_LOGIC_CONFIG_REGS Clb2LogicCfgRegs;


extern struct CLB_LOGIC_CONFIG_REGS Clb3LogicCfgRegs;


extern struct CLB_LOGIC_CONFIG_REGS Clb4LogicCfgRegs;


extern struct CLB_LOGIC_CONTROL_REGS Clb1LogicCtrlRegs;


extern struct CLB_LOGIC_CONTROL_REGS Clb2LogicCtrlRegs;


extern struct CLB_LOGIC_CONTROL_REGS Clb3LogicCtrlRegs;


extern struct CLB_LOGIC_CONTROL_REGS Clb4LogicCtrlRegs;


extern struct CLB_XBAR_REGS ClbXbarRegs;


extern struct CLK_CFG_REGS ClkCfgRegs;


extern struct CMPSS_REGS Cmpss1Regs;


extern struct CMPSS_REGS Cmpss2Regs;


extern struct CMPSS_REGS Cmpss3Regs;


extern struct CMPSS_REGS Cmpss4Regs;


extern struct CMPSS_REGS Cmpss5Regs;


extern struct CMPSS_REGS Cmpss6Regs;


extern struct CMPSS_REGS Cmpss7Regs;


extern struct CMPSS_REGS Cmpss8Regs;


extern struct CPUTIMER_REGS CpuTimer0Regs;


extern struct CPUTIMER_REGS CpuTimer1Regs;


extern struct CPUTIMER_REGS CpuTimer2Regs;


extern struct CPU_SYS_REGS CpuSysRegs;


extern struct DAC_REGS DacaRegs;


extern struct DAC_REGS DacbRegs;


extern struct DAC_REGS DaccRegs;


extern struct DCSM_COMMON_REGS DcsmCommonRegs;


extern struct DCSM_Z1_REGS DcsmZ1Regs;


extern struct DCSM_Z2_REGS DcsmZ2Regs;


extern struct DEV_CFG_REGS DevCfgRegs;


extern struct DMA_CLA_SRC_SEL_REGS DmaClaSrcSelRegs;


extern struct DMA_REGS DmaRegs;


extern struct ECAP_REGS ECap1Regs;


extern struct ECAP_REGS ECap2Regs;


extern struct ECAP_REGS ECap3Regs;


extern struct ECAP_REGS ECap4Regs;


extern struct ECAP_REGS ECap5Regs;


extern struct ECAP_REGS ECap6Regs;


extern struct EMIF1_CONFIG_REGS Emif1ConfigRegs;


extern struct EMIF2_CONFIG_REGS Emif2ConfigRegs;


extern struct EMIF_REGS Emif1Regs;


extern struct EMIF_REGS Emif2Regs;


extern struct EPWM_REGS EPwm1Regs;


extern struct EPWM_REGS EPwm2Regs;


extern struct EPWM_REGS EPwm3Regs;


extern struct EPWM_REGS EPwm4Regs;


extern struct EPWM_REGS EPwm5Regs;


extern struct EPWM_REGS EPwm6Regs;


extern struct EPWM_REGS EPwm7Regs;


extern struct EPWM_REGS EPwm8Regs;


extern struct EPWM_REGS EPwm9Regs;


extern struct EPWM_REGS EPwm10Regs;


extern struct EPWM_REGS EPwm11Regs;


extern struct EPWM_REGS EPwm12Regs;


extern struct EPWM_XBAR_REGS EPwmXbarRegs;


extern struct EQEP_REGS EQep1Regs;


extern struct EQEP_REGS EQep2Regs;


extern struct EQEP_REGS EQep3Regs;


extern struct FLASH_CTRL_REGS Flash0CtrlRegs;


extern struct FLASH_ECC_REGS Flash0EccRegs;


extern struct FLASH_PUMP_SEMAPHORE_REGS FlashPumpSemaphoreRegs;


extern struct GPIO_CTRL_REGS GpioCtrlRegs;


extern struct GPIO_DATA_REGS GpioDataRegs;


extern struct I2C_REGS I2caRegs;


extern struct I2C_REGS I2cbRegs;


extern struct MEMORY_ERROR_REGS MemoryErrorRegs;


extern struct MEM_CFG_REGS MemCfgRegs;


extern struct McBSP_REGS McbspaRegs;

extern struct McBSP_REGS McbspbRegs;

extern struct NMI_INTRUPT_REGS NmiIntruptRegs;


extern struct PIE_CTRL_REGS PieCtrlRegs;


extern struct PIE_VECT_TABLE PieVectTable;


extern struct ROM_PREFETCH_REGS RomPrefetchRegs;


extern struct SCI_REGS SciaRegs;


extern struct SCI_REGS ScibRegs;


extern struct SCI_REGS ScicRegs;


extern struct SCI_REGS ScidRegs;


extern struct SDFM_REGS Sdfm1Regs;


extern struct SDFM_REGS Sdfm2Regs;


extern struct SPI_REGS SpiaRegs;


extern struct SPI_REGS SpibRegs;

extern struct SPI_REGS SpicRegs;


extern struct WD_REGS WdRegs;


extern struct XINT_REGS XintRegs;

#endif //AUTOMATICGRADER_GLOBAL_DEFS_H