//
// HW2 grader — auto-generated
//

#ifndef AUTOMATICGRADER_HW2_H
#define AUTOMATICGRADER_HW2_H


extern "C" {
int temp_main();

void cpu_timer0_isr();

void cpu_timer1_isr();

void cpu_timer2_isr();

void ADCA_ISR() __attribute__

((weak));

void SWI_isr(void);

extern uint16_t UARTPrint;

extern int16_t Adca4result __attribute__((weak));
extern int16_t JoystickXresult __attribute__((weak));
extern int16_t JoystickYresult __attribute__((weak));
extern float Adca4voltage __attribute__((weak));
extern float JoystickXvoltage __attribute__((weak));
extern float JoystickYvoltage __attribute__((weak));
extern int32_t AdcaInterruptCount __attribute__((weak));

void setLightLEDs(int16_t val) __attribute__

((weak));

void setJoystickLEDs(float y, float x) __attribute__

((weak));
}


#endif //AUTOMATICGRADER_HW2_H