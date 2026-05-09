//
// HW5 grader.
//

#ifndef AUTOMATICGRADER_HW5_H
#define AUTOMATICGRADER_HW5_H


extern "C" {
int temp_main();

void cpu_timer0_isr();

void cpu_timer1_isr();

void cpu_timer2_isr();

void ADCA_isr() __attribute__

((weak));

void DMA_isr() __attribute__

((weak));

void SWI_isr(void);

extern uint16_t UARTPrint;

extern int16_t DMAcount __attribute__((weak));
extern uint16_t pingFFT __attribute__((weak));
extern uint16_t pongFFT __attribute__((weak));
extern uint16_t iPingPong __attribute__((weak));
extern uint16_t pingpongFFT __attribute__((weak));

extern int32_t AdcaInterruptCount __attribute__((weak));

void setEPWM8A_RCServo(float angle) __attribute__

((weak));

void setEPWM8B_RCServo(float angle) __attribute__

((weak));
}


#endif //AUTOMATICGRADER_HW5_H