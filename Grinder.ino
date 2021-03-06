//****************************************************************************
// Model: Grinder.qm
// File:  ./Grinder.ino
//
// This code has been generated by QM tool (see state-machine.com/qm).
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//****************************************************************************
//${.::Grinder.ino} ..........................................................

#include "includes.h"

Q_DEFINE_THIS_FILE

 // this blathering on is in case you want to make the class a static variable in a function
extern "C" {
  __extension__ typedef int __guard __attribute__((mode (__DI__)));
  int __cxa_guard_acquire(__guard *g) { return !*(char *)(g); };
  void __cxa_guard_release (__guard *g) { *(char *)g = 1; };
  void __cxa_guard_abort (__guard *) { };
} // end extern "C"

using namespace QP;

// ===== definitions =====

//............................................................................
void QF::onStartup(void) {
    // configure 8bit Timer0
    TCCR0A = (1 << WGM01) | (0 << WGM00); // timer/counter control, CTC mode, no PWM
    TCCR0B = (1 << CS02 ) | (0 << CS01) | (1 << CS00); // timer/counter control, 1/1024 prescaler, internal clock source
    //ASSR  &= ~(1 << AS2);
    TIMSK0 = (1 << OCIE0A);         // timer interrupt mask, enable TIMER2 compare Interrupt
    TCNT0  = 0;                     // timer/counter 0, initialize couter to 0
    OCR0A  = (F_CPU / BSP_TICKS_PER_SEC / 1024) - 1; // output compare register, init with value to compare to
}
//............................................................................
void QF::onCleanup(void) {
}
//............................................................................
void QF::onIdle() {
    QF_INT_ENABLE(); // re-enable interrupts
}
//............................................................................
void Q_onAssert(char const Q_ROM * const file, int line) {
    QF_INT_DISABLE(); // disable all interrupts
    Serial1.println("Assert hit!");
    BSP_ledOff(); // User LED permanently ON
    asm volatile ("jmp 0x0000"); // perform a software reset of the Arduino
}


//............................................................................
void setup() {
    BSP_init(); // initialize the BSP

    QF::init(); // initialize the framework and the underlying RT kernel

    static QEvt const *grinder_queueSto[10]; // alocate event queue buffer
    static QEvt const *scale_queueSto[10]; // alocate event queue buffer

    static QF_MPOOL_EL(GrinderEvent) l_smlPoolSto[20]; //storage for small epool

    QF::poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0])); // initialize event pools...

    DOSE = 7.0;

    AO_Grinder->start(1, grinder_queueSto, Q_DIM(grinder_queueSto), (void *)0, 0U); // start the grinder active object
    AO_Scale->start(2, scale_queueSto, Q_DIM(scale_queueSto), (void *)0, 0U);        // start the scale active object

}

