//
// Created by Marius on 2/18/2026.
//

#ifndef AUTOMATICGRADER_TI_STUBS_H
#define AUTOMATICGRADER_TI_STUBS_H

#include <stdint.h>

#define interrupt
#define __interrupt
#define __cregister

#define asm(x)         ((void)0)

#define EALLOW
#define EDIS
#define DINT

// typedef unsigned short uint16_t;
// typedef short int16_t;
//
// typedef unsigned int uint32_t;
// typedef int int32_t;


// It is not used as a variable for it to ma

#define Uint16 uint16_t
#define Uint8 uint16_t
#define Uint32 uint32_t

#define uint8_t uint16_t
#define int8_t  int16_t
#define int8 int16_t
#define int16 int16_t
#define int32 int32_t

#define DATA_SECTION(x, y)

// #define GPIO_INPUT 1
// #define GPIO_PULLUP 1
// #define GPIO_PUSHPULL 1
// #define GPIO_OUTPUT 1
// #define GPIO_INPUT 1
// #define GPIO_MUX_CPU1 1

#define EINT
#define ERTM


static inline void InitSysCtrl() {
}

static inline void InitGpio() {
}

static inline void InitPieCtrl() {
}

static inline void InitPieVectTable() {
}


struct GpioSetup {
    Uint16 cpu;
    Uint16 muxPosition;
    Uint16 output;
    Uint16 flags;
};

struct AdcSetup {
    Uint16 resolution;
    Uint16 signalmode;
};

#define MAX_GPIO 160
#define MAX_ADC 3

extern struct GpioSetup gpiosSetup[MAX_GPIO];
extern struct AdcSetup adcSetup[MAX_ADC];

extern uint16_t IER;
extern uint16_t IFR;

static inline void GPIO_SetupPinOptions(Uint16 gpioNumber, Uint16 output, Uint16 flags) {
    if (gpioNumber >= MAX_GPIO) {
        return;
    }

    gpiosSetup[gpioNumber].flags = flags;
    gpiosSetup[gpioNumber].output = output;
    gpiosSetup[gpioNumber].flags = flags;
}

static inline void GPIO_SetupPinMux(Uint16 gpioNumber, Uint16 cpu, Uint16 muxPosition) {
    if (gpioNumber >= MAX_GPIO) {
        return;
    }

    gpiosSetup[gpioNumber].muxPosition = muxPosition;
    gpiosSetup[gpioNumber].cpu = cpu;
}

static inline void AdcSetMode(Uint16 adc, Uint16 resolution, Uint16 signalmode) {
    if (adc >= MAX_ADC) {
        return;
    }
    adcSetup[adc].resolution = resolution;
    adcSetup[adc].signalmode = signalmode;
}

#define M_INT1  0x0001
#define M_INT2  0x0002
#define M_INT3  0x0004
#define M_INT4  0x0008
#define M_INT5  0x0010
#define M_INT6  0x0020
#define M_INT7  0x0040
#define M_INT8  0x0080
#define M_INT9  0x0100
#define M_INT10 0x0200
#define M_INT11 0x0400
#define M_INT12 0x0800
#define M_INT13 0x1000
#define M_INT14 0x2000
#define M_DLOG  0x4000
#define M_RTOS  0x8000

#ifdef __cplusplus
extern "C" {
#endif
#include "global_defs.h"
#include "timer.h"
#include "F2837xD_Adc_defines.h"
#include "F28379dSerial.h"
#include "F2837xD_Gpio_defines.h"

uint16_t step_execution(void);
#ifdef __cplusplus
}
#endif

#define DELAY_US(x)         ((void)0)

#ifdef while
#undef while
#endif

#define while(condition) while( (condition) && step_execution() )


#endif //AUTOMATICGRADER_TI_STUBS_H