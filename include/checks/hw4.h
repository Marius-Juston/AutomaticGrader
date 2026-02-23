//
// Created by marius on 2/20/26.
//

#ifndef AUTOMATICGRADER_HW1_H
#define AUTOMATICGRADER_HW1_H


extern "C" {
int temp_main();

void cpu_timer0_isr();

void cpu_timer1_isr();

void cpu_timer2_isr();

//
// // Resolves to null if it does not exist
float saturate(float, float) __attribute__

((weak));
}


int check_initialization();

int check_timer0();

int check_timer1();

int check_timer2();

#endif //AUTOMATICGRADER_HW1_H
