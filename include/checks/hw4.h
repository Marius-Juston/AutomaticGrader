//
// HW4 grader.
//

#ifndef AUTOMATICGRADER_HW4_H
#define AUTOMATICGRADER_HW4_H


extern "C" {
int temp_main();

void cpu_timer0_isr();

void cpu_timer1_isr();

void cpu_timer2_isr();

void xint1_isr() __attribute__((weak));
void xint2_isr() __attribute__((weak));

void SWI_isr(void);

extern uint16_t UARTPrint;

extern volatile uint32_t Xint1Count __attribute__((weak));
extern volatile uint32_t Xint2Count __attribute__((weak));

void setEPWM8A_RCServo(float angle) __attribute__((weak));
void setEPWM8B_RCServo(float angle) __attribute__((weak));
}


#endif //AUTOMATICGRADER_HW4_H
