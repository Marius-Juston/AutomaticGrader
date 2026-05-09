//
// Created by Marius on 2/18/2026.
//

#ifndef AUTOMATICGRADER_TI_STUBS_H
#define AUTOMATICGRADER_TI_STUBS_H

#include <stdint.h>
#include <stdbool.h>

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

#define MAX_GPIO (160 + 1) // The system is indexed starting 1
#define MAX_ADC (3 + 1)

extern struct GpioSetup gpiosSetup[MAX_GPIO];
extern struct AdcSetup adcSetup[MAX_ADC];

extern uint16_t IER;
extern uint16_t IFR;

// HW4: GPIO -> XINT binding map. xintPinMap[N] = gpio bound to XINTN (1..5).
extern uint16_t xintPinMap[8];

#ifdef __cplusplus
extern "C" {
#endif
void GPIO_SetupXINT1Gpio(Uint16 gpioNumber);

void GPIO_SetupXINT2Gpio(Uint16 gpioNumber);

void GPIO_SetupXINT3Gpio(Uint16 gpioNumber);

void GPIO_SetupXINT4Gpio(Uint16 gpioNumber);

void GPIO_SetupXINT5Gpio(Uint16 gpioNumber);
#ifdef __cplusplus
}
#endif

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

#ifdef __cplusplus
}
#endif

#define DELAY_US(x)         ((void)0)

#define NUM_GPIO_BUCKETS 6
#define NUM_GPIO_PER_BUCKETS 32

bool updateGPIOState();

bool updateGPIOSet();

bool updateGPIOClear();

bool updateGPIOToggle();

// ---- Cooperative main-loop driver -----------------------------------------
// Under AUTO_GRADER, the build patches the student's `while (1)` IDLE loop
// into the GRADER_MAIN_LOOP macro: a for-loop bounded by an iteration count
// that the harness sets before re-entering temp_main(). The harness owns
// control flow over the main-loop body. Init code is re-run on each entry
// but is idempotent at register level; checks that depend on running
// counters (e.g. CpuTimer*.InterruptCount) save/restore them around the
// step. See src/checks/main_loop_driver.cpp.
#ifdef AUTO_GRADER
#ifdef __cplusplus
extern "C" {

#endif
extern int grader_main_loop_iterations(void);
#ifdef __cplusplus
}
#endif
// Guard injected at the top of main() by tools/patch_student_source.py.
// First call to temp_main() runs init normally; every subsequent call jumps
// straight to the GRADER_MAIN_LOOP label, skipping init (and any busy-waits
// the student's init may contain — e.g. setupSpib() in HW3 reference).
#define GRADER_MAIN_INIT_GUARD \
    static int _grader_inited; \
    if (_grader_inited) goto _grader_loop_start; \
    _grader_inited = 1
#define GRADER_MAIN_LOOP \
    for (int _grader_i = 0; _grader_i < grader_main_loop_iterations(); ++_grader_i)
#endif

#endif //AUTOMATICGRADER_TI_STUBS_H