//
// Created by Marius on 2/20/2026.
//
#include "ti_stubs.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>


serialSCIA_t SerialA;
serialSCIB_t SerialB;
serialSCIC_t SerialC;
serialSCID_t SerialD;

extern "C" {
uint16_t init_serialSCIA(serialSCIA_t *s, Uint32 baud) {
    return 0; // Returning a dummy value to satisfy the non-void return type warning
}

uint16_t init_serialSCIB(serialSCIB_t *s, Uint32 baud) {
    return 0;
}

uint16_t init_serialSCIC(serialSCIC_t *s, Uint32 baud) {
    return 0;
}

uint16_t init_serialSCID(serialSCID_t *s, Uint32 baud) {
    return 0;
}

uint16_t serial_sendSCIC(serialSCIC_t *s, char *data, uint16_t len) {
    return 0;
}

uint16_t serial_sendSCID(serialSCID_t *s, char *data, uint16_t len) {
    return 0;
}

char UART_printf_bufferSCIB[BUF_SIZESCIB];

void UART_printfLine(unsigned char line, char *format, ...) {
    va_list ap;
    va_start(ap, format);
    vsprintf(UART_printf_bufferSCIB, format, ap);
    va_end(ap);

    printf(UART_printf_bufferSCIB, format, ap);
}

void TXDINT_data_sent(void) {
}

void TXCINT_data_sent(void) {
}

void TXBINT_data_sent(void) {
}

void TXAINT_data_sent(void) {
}

void RXAINT_recv_ready(void) {
}

void RXBINT_recv_ready(void) {
}

void RXCINT_recv_ready(void) {
}

void RXDINT_recv_ready(void) {
}

void InitCpuTimers(void) {
}

void ConfigCpuTimer(struct CPUTIMER_VARS *Timer, float Freq, float Period) {
}

char serial_printf_bufSCIA[BUF_SIZESCIA];

uint16_t serial_printf(serialSCIA_t *s, char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vsprintf(serial_printf_bufSCIA, fmt, ap);
    va_end(ap);

    return printf(fmt, ap);
}

uint16_t step_execution(void) {
    static uint32_t execution_cycles = 0;

    const uint32_t MAX_CYCLES = 2;

    if (++execution_cycles > MAX_CYCLES) {
        return 0;
    }

    return 1;
}
}

struct GpioSetup gpiosSetup[MAX_GPIO];
struct AdcSetup adcSetup[MAX_ADC];
uint16_t IER = 0;
uint16_t IFR = 0;


volatile struct ACCESS_PROTECTION_REGS AccessProtectionRegs;


volatile struct ADC_REGS AdcaRegs;


volatile struct ADC_REGS AdcbRegs;


volatile struct ADC_REGS AdccRegs;


volatile struct ADC_REGS AdcdRegs;


volatile struct ADC_RESULT_REGS AdcaResultRegs;


volatile struct ADC_RESULT_REGS AdcbResultRegs;


volatile struct ADC_RESULT_REGS AdccResultRegs;


volatile struct ADC_RESULT_REGS AdcdResultRegs;


volatile struct ANALOG_SUBSYS_REGS AnalogSubsysRegs;


volatile struct CLA_REGS Cla1Regs;


volatile struct CLB_DATA_EXCHANGE_REGS Clb1DataExchRegs;


volatile struct CLB_DATA_EXCHANGE_REGS Clb2DataExchRegs;


volatile struct CLB_DATA_EXCHANGE_REGS Clb3DataExchRegs;


volatile struct CLB_DATA_EXCHANGE_REGS Clb4DataExchRegs;


volatile struct CLB_LOGIC_CONFIG_REGS Clb1LogicCfgRegs;


volatile struct CLB_LOGIC_CONFIG_REGS Clb2LogicCfgRegs;


volatile struct CLB_LOGIC_CONFIG_REGS Clb3LogicCfgRegs;


volatile struct CLB_LOGIC_CONFIG_REGS Clb4LogicCfgRegs;


volatile struct CLB_LOGIC_CONTROL_REGS Clb1LogicCtrlRegs;


volatile struct CLB_LOGIC_CONTROL_REGS Clb2LogicCtrlRegs;


volatile struct CLB_LOGIC_CONTROL_REGS Clb3LogicCtrlRegs;


volatile struct CLB_LOGIC_CONTROL_REGS Clb4LogicCtrlRegs;


volatile struct CLB_XBAR_REGS ClbXbarRegs;


volatile struct CLK_CFG_REGS ClkCfgRegs;


volatile struct CMPSS_REGS Cmpss1Regs;


volatile struct CMPSS_REGS Cmpss2Regs;


volatile struct CMPSS_REGS Cmpss3Regs;


volatile struct CMPSS_REGS Cmpss4Regs;


volatile struct CMPSS_REGS Cmpss5Regs;


volatile struct CMPSS_REGS Cmpss6Regs;


volatile struct CMPSS_REGS Cmpss7Regs;


volatile struct CMPSS_REGS Cmpss8Regs;


volatile struct CPUTIMER_REGS CpuTimer0Regs;


volatile struct CPUTIMER_REGS CpuTimer1Regs;


volatile struct CPUTIMER_REGS CpuTimer2Regs;


volatile struct CPU_SYS_REGS CpuSysRegs;


volatile struct DAC_REGS DacaRegs;


volatile struct DAC_REGS DacbRegs;


volatile struct DAC_REGS DaccRegs;


volatile struct DCSM_COMMON_REGS DcsmCommonRegs;


volatile struct DCSM_Z1_REGS DcsmZ1Regs;


volatile struct DCSM_Z2_REGS DcsmZ2Regs;


volatile struct DEV_CFG_REGS DevCfgRegs;


volatile struct DMA_CLA_SRC_SEL_REGS DmaClaSrcSelRegs;


volatile struct DMA_REGS DmaRegs;


volatile struct ECAP_REGS ECap1Regs;


volatile struct ECAP_REGS ECap2Regs;


volatile struct ECAP_REGS ECap3Regs;


volatile struct ECAP_REGS ECap4Regs;


volatile struct ECAP_REGS ECap5Regs;


volatile struct ECAP_REGS ECap6Regs;


volatile struct EMIF1_CONFIG_REGS Emif1ConfigRegs;


volatile struct EMIF2_CONFIG_REGS Emif2ConfigRegs;


volatile struct EMIF_REGS Emif1Regs;


volatile struct EMIF_REGS Emif2Regs;


volatile struct EPWM_REGS EPwm1Regs;


volatile struct EPWM_REGS EPwm2Regs;


volatile struct EPWM_REGS EPwm3Regs;


volatile struct EPWM_REGS EPwm4Regs;


volatile struct EPWM_REGS EPwm5Regs;


volatile struct EPWM_REGS EPwm6Regs;


volatile struct EPWM_REGS EPwm7Regs;


volatile struct EPWM_REGS EPwm8Regs;


volatile struct EPWM_REGS EPwm9Regs;


volatile struct EPWM_REGS EPwm10Regs;


volatile struct EPWM_REGS EPwm11Regs;


volatile struct EPWM_REGS EPwm12Regs;


volatile struct EPWM_XBAR_REGS EPwmXbarRegs;


volatile struct EQEP_REGS EQep1Regs;


volatile struct EQEP_REGS EQep2Regs;


volatile struct EQEP_REGS EQep3Regs;


volatile struct FLASH_CTRL_REGS Flash0CtrlRegs;


volatile struct FLASH_ECC_REGS Flash0EccRegs;


volatile struct FLASH_PUMP_SEMAPHORE_REGS FlashPumpSemaphoreRegs;


volatile struct GPIO_CTRL_REGS GpioCtrlRegs;


volatile struct GPIO_DATA_REGS GpioDataRegs;


volatile struct I2C_REGS I2caRegs;


volatile struct I2C_REGS I2cbRegs;


volatile struct MEMORY_ERROR_REGS MemoryErrorRegs;


volatile struct MEM_CFG_REGS MemCfgRegs;


volatile struct McBSP_REGS McbspaRegs;

volatile struct McBSP_REGS McbspbRegs;

volatile struct NMI_INTRUPT_REGS NmiIntruptRegs;


volatile struct PIE_CTRL_REGS PieCtrlRegs;


volatile struct PIE_VECT_TABLE PieVectTable;


volatile struct ROM_PREFETCH_REGS RomPrefetchRegs;


volatile struct SCI_REGS SciaRegs;


volatile struct SCI_REGS ScibRegs;


volatile struct SCI_REGS ScicRegs;


volatile struct SCI_REGS ScidRegs;


volatile struct SDFM_REGS Sdfm1Regs;


volatile struct SDFM_REGS Sdfm2Regs;


volatile struct SPI_REGS SpiaRegs;


volatile struct SPI_REGS SpibRegs;

volatile struct SPI_REGS SpicRegs;


volatile struct WD_REGS WdRegs;


volatile struct XINT_REGS XintRegs;

volatile struct XINT_REGS Xint2Regs;


struct CPUTIMER_VARS CpuTimer0;
struct CPUTIMER_VARS CpuTimer1;
struct CPUTIMER_VARS CpuTimer2;
