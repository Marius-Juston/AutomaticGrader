//
// HW3 grader.
//

#ifndef AUTOMATICGRADER_HW3_H
#define AUTOMATICGRADER_HW3_H


extern "C" {
int temp_main();

void cpu_timer0_isr();

void cpu_timer1_isr();

void cpu_timer2_isr();

void SPIB_isr() __attribute__

((weak));

void SWI_isr(void);

extern uint16_t UARTPrint;

extern int16_t accelx_raw __attribute__((weak));
extern int16_t accely_raw __attribute__((weak));
extern int16_t accelz_raw __attribute__((weak));
extern int16_t gyrox_raw __attribute__((weak));
extern int16_t gyroy_raw __attribute__((weak));
extern int16_t gyroz_raw __attribute__((weak));
extern float accelx_val __attribute__((weak));
extern float accely_val __attribute__((weak));
extern float accelz_val __attribute__((weak));
extern float gyrox_val __attribute__((weak));
extern float gyroy_val __attribute__((weak));
extern float gyroz_val __attribute__((weak));
extern int32_t SPIBCount __attribute__((weak));
}


#endif //AUTOMATICGRADER_HW3_H