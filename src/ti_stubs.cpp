//
// Created by Marius on 2/20/2026.
//
#include "ti_stubs.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <array>

#include <spdlog/spdlog.h>


serialSCIA_t SerialA;
serialSCIB_t SerialB;
serialSCIC_t SerialC;
serialSCID_t SerialD;


inline void rtrim(char *buffer, uint32_t &length) {
    if (!buffer || length == 0) {
        return;
    }

    while (length > 0 && std::isspace(static_cast<unsigned char>(buffer[length - 1]))) {
        buffer[length--] = '\0'; // ordering of -- matters here
    }

    buffer[length] = '\0';
}

extern "C" {
char UART_printf_bufferSCIB[BUF_SIZESCIB];
char serial_printf_bufSCIA[BUF_SIZESCIA];


void clearBuffers() {
    for (char &i: serial_printf_bufSCIA) {
        i = 0;
    }
    for (char &i: UART_printf_bufferSCIB) {
        i = 0;
    }
}

uint16_t init_serialSCIA(serialSCIA_t *s, Uint32 baud) {
    clearBuffers();

    return 0; // Returning a dummy value to satisfy the non-void return type warning
}

uint16_t init_serialSCIB(serialSCIB_t *s, Uint32 baud) {
    clearBuffers();
    return 0;
}

uint16_t init_serialSCIC(serialSCIC_t *s, Uint32 baud) {
    clearBuffers();
    return 0;
}

uint16_t init_serialSCID(serialSCID_t *s, Uint32 baud) {
    clearBuffers();
    return 0;
}

uint16_t serial_sendSCIC(serialSCIC_t *s, char *data, uint16_t len) {
    clearBuffers();
    return 0;
}

uint16_t serial_sendSCID(serialSCID_t *s, char *data, uint16_t len) {
    clearBuffers();
    return 0;
}


void UART_printfLine(unsigned char line, char *fmt, ...) {
    clearBuffers();
    va_list ap;

    va_start(ap, fmt);
    const uint32_t len = vsnprintf(UART_printf_bufferSCIB, BUF_SIZESCIB, fmt, ap);
    va_end(ap);

    if (len <= 0) {
        return;
    }

    uint32_t write_len = (len < BUF_SIZESCIA) ? len : (BUF_SIZESCIA - 1);

    rtrim(serial_printf_bufSCIA, write_len);

    spdlog::info("UART: Line {:d}: {}", line, serial_printf_bufSCIA);
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


uint16_t serial_printf(serialSCIA_t *s, char *fmt, ...) {
    clearBuffers();
    va_list ap;

    va_start(ap, fmt);
    const uint32_t len = vsnprintf(serial_printf_bufSCIA, BUF_SIZESCIA, fmt, ap);
    va_end(ap);

    if (len <= 0) {
        return 0;
    }

    uint32_t write_len = (len < BUF_SIZESCIA) ? len : (BUF_SIZESCIA - 1);

    rtrim(serial_printf_bufSCIA, write_len);

    spdlog::info("serial: {}", serial_printf_bufSCIA);

    return len;
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

struct CPUTIMER_VARS CpuTimer0;
struct CPUTIMER_VARS CpuTimer1;
struct CPUTIMER_VARS CpuTimer2;


constexpr std::array<Uint32 *, NUM_GPIO_BUCKETS> gpioDataRegister = {
    &GpioDataRegs.GPADAT.all,
    &GpioDataRegs.GPBDAT.all,
    &GpioDataRegs.GPCDAT.all,
    &GpioDataRegs.GPDDAT.all,
    &GpioDataRegs.GPEDAT.all,
    &GpioDataRegs.GPFDAT.all,
};

constexpr std::array<Uint32 *, NUM_GPIO_BUCKETS> gpioSetRegister = {
    &GpioDataRegs.GPASET.all,
    &GpioDataRegs.GPBSET.all,
    &GpioDataRegs.GPCSET.all,
    &GpioDataRegs.GPDSET.all,
    &GpioDataRegs.GPESET.all,
    &GpioDataRegs.GPFSET.all,
};

constexpr std::array<Uint32 *, NUM_GPIO_BUCKETS> gpioClearRegister = {
    &GpioDataRegs.GPACLEAR.all,
    &GpioDataRegs.GPBCLEAR.all,
    &GpioDataRegs.GPCCLEAR.all,
    &GpioDataRegs.GPDCLEAR.all,
    &GpioDataRegs.GPECLEAR.all,
    &GpioDataRegs.GPFCLEAR.all,
};

constexpr std::array<Uint32 *, NUM_GPIO_BUCKETS> gpioToggleRegister = {
    &GpioDataRegs.GPATOGGLE.all,
    &GpioDataRegs.GPATOGGLE.all,
    &GpioDataRegs.GPATOGGLE.all,
    &GpioDataRegs.GPATOGGLE.all,
    &GpioDataRegs.GPATOGGLE.all,
    &GpioDataRegs.GPATOGGLE.all,
};

bool updateGPIOSet() {
    bool changed = false;

    for (size_t i = 0; i < gpioSetRegister.size(); ++i) {
        Uint32 *gpio = gpioSetRegister[i];

        // A bit has been set
        if (*gpio) {
            // Set the specific bits
            *gpioDataRegister[i] |= *gpio;
            changed = true;
        }

        // clear the bit-field
        *gpio = 0;
    }

    return changed;
}

bool updateGPIOClear() {
    bool changed = false;

    for (size_t i = 0; i < gpioClearRegister.size(); ++i) {
        Uint32 *gpio = gpioClearRegister[i];

        // A bit has been set
        if (*gpio) {
            // Clear the specific bits
            *gpioDataRegister[i] &= ~(*gpio);
            changed = true;
        }

        // clear the bit-field
        *gpio = 0;
    }

    return changed;
}

bool updateGPIOToggle() {
    bool changed = false;

    for (size_t i = 0; i < gpioToggleRegister.size(); ++i) {
        Uint32 *gpio = gpioToggleRegister[i];

        // A bit has been set
        if (*gpio) {
            // Set the specific bits
            *gpioDataRegister[i] ^= *gpio;
            changed = true;
        }

        // clear the bit-field
        *gpio = 0;
    }

    return changed;
}

bool updateGPIOState() {
    bool changed = false;

    changed |= updateGPIOSet();
    changed |= updateGPIOClear();
    changed |= updateGPIOToggle();

    return changed;
}
