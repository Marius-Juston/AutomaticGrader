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
    Timer->CPUFreqInMHz = Freq;
    Timer->PeriodInUSec = Period;

    Timer->InterruptCount = 0;
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


struct ACCESS_PROTECTION_REGS AccessProtectionRegs;


struct ADC_REGS AdcaRegs;


struct ADC_REGS AdcbRegs;


struct ADC_REGS AdccRegs;


struct ADC_REGS AdcdRegs;


struct ADC_RESULT_REGS AdcaResultRegs;


struct ADC_RESULT_REGS AdcbResultRegs;


struct ADC_RESULT_REGS AdccResultRegs;


struct ADC_RESULT_REGS AdcdResultRegs;


struct ANALOG_SUBSYS_REGS AnalogSubsysRegs;


struct CLA_REGS Cla1Regs;


struct CLB_DATA_EXCHANGE_REGS Clb1DataExchRegs;


struct CLB_DATA_EXCHANGE_REGS Clb2DataExchRegs;


struct CLB_DATA_EXCHANGE_REGS Clb3DataExchRegs;


struct CLB_DATA_EXCHANGE_REGS Clb4DataExchRegs;


struct CLB_LOGIC_CONFIG_REGS Clb1LogicCfgRegs;


struct CLB_LOGIC_CONFIG_REGS Clb2LogicCfgRegs;


struct CLB_LOGIC_CONFIG_REGS Clb3LogicCfgRegs;


struct CLB_LOGIC_CONFIG_REGS Clb4LogicCfgRegs;


struct CLB_LOGIC_CONTROL_REGS Clb1LogicCtrlRegs;


struct CLB_LOGIC_CONTROL_REGS Clb2LogicCtrlRegs;


struct CLB_LOGIC_CONTROL_REGS Clb3LogicCtrlRegs;


struct CLB_LOGIC_CONTROL_REGS Clb4LogicCtrlRegs;


struct CLB_XBAR_REGS ClbXbarRegs;


struct CLK_CFG_REGS ClkCfgRegs;


struct CMPSS_REGS Cmpss1Regs;


struct CMPSS_REGS Cmpss2Regs;


struct CMPSS_REGS Cmpss3Regs;


struct CMPSS_REGS Cmpss4Regs;


struct CMPSS_REGS Cmpss5Regs;


struct CMPSS_REGS Cmpss6Regs;


struct CMPSS_REGS Cmpss7Regs;


struct CMPSS_REGS Cmpss8Regs;


struct CPUTIMER_REGS CpuTimer0Regs;


struct CPUTIMER_REGS CpuTimer1Regs;


struct CPUTIMER_REGS CpuTimer2Regs;


struct CPU_SYS_REGS CpuSysRegs;


struct DAC_REGS DacaRegs;


struct DAC_REGS DacbRegs;


struct DAC_REGS DaccRegs;


struct DCSM_COMMON_REGS DcsmCommonRegs;


struct DCSM_Z1_REGS DcsmZ1Regs;


struct DCSM_Z2_REGS DcsmZ2Regs;


struct DEV_CFG_REGS DevCfgRegs;


struct DMA_CLA_SRC_SEL_REGS DmaClaSrcSelRegs;


struct DMA_REGS DmaRegs;


struct ECAP_REGS ECap1Regs;


struct ECAP_REGS ECap2Regs;


struct ECAP_REGS ECap3Regs;


struct ECAP_REGS ECap4Regs;


struct ECAP_REGS ECap5Regs;


struct ECAP_REGS ECap6Regs;


struct EMIF1_CONFIG_REGS Emif1ConfigRegs;


struct EMIF2_CONFIG_REGS Emif2ConfigRegs;


struct EMIF_REGS Emif1Regs;


struct EMIF_REGS Emif2Regs;


struct EPWM_REGS EPwm1Regs;


struct EPWM_REGS EPwm2Regs;


struct EPWM_REGS EPwm3Regs;


struct EPWM_REGS EPwm4Regs;


struct EPWM_REGS EPwm5Regs;


struct EPWM_REGS EPwm6Regs;


struct EPWM_REGS EPwm7Regs;


struct EPWM_REGS EPwm8Regs;


struct EPWM_REGS EPwm9Regs;


struct EPWM_REGS EPwm10Regs;


struct EPWM_REGS EPwm11Regs;


struct EPWM_REGS EPwm12Regs;


struct EPWM_XBAR_REGS EPwmXbarRegs;


struct EQEP_REGS EQep1Regs;


struct EQEP_REGS EQep2Regs;


struct EQEP_REGS EQep3Regs;


struct FLASH_CTRL_REGS Flash0CtrlRegs;


struct FLASH_ECC_REGS Flash0EccRegs;


struct FLASH_PUMP_SEMAPHORE_REGS FlashPumpSemaphoreRegs;


struct GPIO_CTRL_REGS GpioCtrlRegs;


struct GPIO_DATA_REGS GpioDataRegs;


struct I2C_REGS I2caRegs;


struct I2C_REGS I2cbRegs;


struct MEMORY_ERROR_REGS MemoryErrorRegs;


struct MEM_CFG_REGS MemCfgRegs;


struct McBSP_REGS McbspaRegs;

struct McBSP_REGS McbspbRegs;

struct NMI_INTRUPT_REGS NmiIntruptRegs;


struct PIE_CTRL_REGS PieCtrlRegs;


struct PIE_VECT_TABLE PieVectTable;


struct ROM_PREFETCH_REGS RomPrefetchRegs;


struct SCI_REGS SciaRegs;


struct SCI_REGS ScibRegs;


struct SCI_REGS ScicRegs;


struct SCI_REGS ScidRegs;


struct SDFM_REGS Sdfm1Regs;


struct SDFM_REGS Sdfm2Regs;


struct SPI_REGS SpiaRegs;


struct SPI_REGS SpibRegs;

struct SPI_REGS SpicRegs;


struct WD_REGS WdRegs;


struct XINT_REGS XintRegs;

struct XINT_REGS Xint2Regs;


struct CPUTIMER_VARS CpuTimer0;
struct CPUTIMER_VARS CpuTimer1;
struct CPUTIMER_VARS CpuTimer2;