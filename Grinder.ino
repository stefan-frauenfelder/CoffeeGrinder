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
#include "qp_port.h"
#include "Arduino.h"  // always include in your sketch
#include "board.h"

#include <SPI.h>
#include <Encoder.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Adafruit_SSD1306.h>
#include "HX711.h"

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

#define BSP_TICKS_PER_SEC 100

// button
#define BUTTON_LED_PIN 5
#define BUTTON_SWITCH_PIN 7

// scale
#define SCALE_DOUT  4
#define SCALE_CLK   6
#define SCALE_CALIBRATION_FACTOR -444190.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
HX711 scale(SCALE_DOUT, SCALE_CLK);

// OLED
//#define OLED_MOSI   9
//#define OLED_CLK   10
//#define OLED_DC    14
//#define OLED_CS    15
//#define OLED_RESET 16
//Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Uncomment this block to use hardware SPI

//#define SPI_MOSI 16
//#define SPI_MISO 14
//#define SPI_CLK  15

#define OLED_DC     8
#define OLED_CS    10
#define OLED_RESET  9
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// encoder
//Encoder myEnc(2, 4);



enum GrinderSignals { // signals for the Grinder application
    TIMEOUT_SIG = Q_USER_SIG,
    BUTTON_DOWN_SIG
};

//............................................................................
void BSP_init(void) {

    Serial1.begin(115200U); // set the highest stanard baud rate of 115200 bps
    Serial1.println("Starting.");

    pinMode(BUTTON_LED_PIN, OUTPUT);    // button LED
    pinMode(BUTTON_SWITCH_PIN, INPUT);     // button direction
    digitalWrite(BUTTON_SWITCH_PIN, HIGH); // button: turn on pullup resistors

    // configure external interrupt 6 (Pin7)
    EICRB |= (0<<ISC60)|(1<<ISC61); // external int ctrl, falling edge detection for interrupt 6
    EIMSK |= (1<<INT6);             // external int mask, activates the interrupt

    scale.set_scale(SCALE_CALIBRATION_FACTOR); //This value is obtained by using the SparkFun_HX711_Calibration sketch
    scale.tare();  //Assuming there is no weight on the scale at start up, reset the scale to 0

    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    display.begin(SSD1306_SWITCHCAPVCC);

    display.clearDisplay();

    // text display
    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(0,0);
    display.println("Ennel power!");

    display.setFont(&FreeMonoBold9pt7b);
    display.display();

}
//............................................................................
void BSP_ledOff(void) {
    digitalWrite(BUTTON_LED_PIN, LOW);    // turn the LED off by making the voltage LOW
}
//............................................................................
void BSP_ledOn(void) {
    digitalWrite(BUTTON_LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
}
//............................................................................
ISR(TIMER0_COMPA_vect) {
    QF::TICK((void *)0); // process all armed time events
}


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

//============================================================================
// generate code for the Blinky application...
//${Events::GrinderEvent} ....................................................
class GrinderEvent : public QP::QEvt {};


//${AOs::Motor} ..............................................................
class Motor : public QP::QMActive {
public:
    QTimeEvt m_timeEvt;

public:
    Motor()
    : QMActive(Q_STATE_CAST(&Motor::initial)),
      m_timeEvt(TIMEOUT_SIG)
    {}

protected:
    static QP::QState initial(Motor * const me, QP::QEvt const * const e);
    static QP::QState off  (Motor * const me, QP::QEvt const * const e);
    static QP::QState off_e(Motor * const me);
    static QP::QMState const off_s;
    static QP::QState on  (Motor * const me, QP::QEvt const * const e);
    static QP::QState on_e(Motor * const me);
    static QP::QMState const on_s;
};

//${AOs::Motor} ..............................................................

//${AOs::Motor::SM} ..........................................................
QP::QState Motor::initial(Motor * const me, QP::QEvt const * const e) {
    static struct {
        QP::QMState const *target;
        QP::QActionHandler act[2];
    } const tatbl_ = { // transition-action table
        &off_s,
        {
            Q_ACTION_CAST(&off_e), // entry
            Q_ACTION_CAST(0)  // zero terminator
        }
    };
    // ${AOs::Motor::SM::initial}
    me->m_timeEvt.postEvery(me, BSP_TICKS_PER_SEC/2);
    return QM_TRAN_INIT(&tatbl_);
}
//${AOs::Motor::SM::off} .....................................................
QP::QMState const Motor::off_s = {
    static_cast<QP::QMState const *>(0), // superstate (top)
    Q_STATE_CAST(&off),
    Q_ACTION_CAST(&off_e),
    Q_ACTION_CAST(0), // no exit action
    Q_ACTION_CAST(0)  // no intitial tran.
};
// ${AOs::Motor::SM::off}
QP::QState Motor::off_e(Motor * const me) {
    BSP_ledOff();
    (void)me; // avoid compiler warning in case 'me' is not used
    return QM_ENTRY(&off_s);
}
// ${AOs::Motor::SM::off}
QP::QState Motor::off(Motor * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${AOs::Motor::SM::off::TIMEOUT}
        case TIMEOUT_SIG: {
            static struct {
                QP::QMState const *target;
                QP::QActionHandler act[2];
            } const tatbl_ = { // transition-action table
                &on_s,
                {
                    Q_ACTION_CAST(&on_e), // entry
                    Q_ACTION_CAST(0)  // zero terminator
                }
            };
            status_ = QM_TRAN(&tatbl_);
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}
//${AOs::Motor::SM::on} ......................................................
QP::QMState const Motor::on_s = {
    static_cast<QP::QMState const *>(0), // superstate (top)
    Q_STATE_CAST(&on),
    Q_ACTION_CAST(&on_e),
    Q_ACTION_CAST(0), // no exit action
    Q_ACTION_CAST(0)  // no intitial tran.
};
// ${AOs::Motor::SM::on}
QP::QState Motor::on_e(Motor * const me) {
    BSP_ledOn();
    (void)me; // avoid compiler warning in case 'me' is not used
    return QM_ENTRY(&on_s);
}
// ${AOs::Motor::SM::on}
QP::QState Motor::on(Motor * const me, QP::QEvt const * const e) {
    QP::QState status_;
    switch (e->sig) {
        // ${AOs::Motor::SM::on::TIMEOUT, BUTTON_DOWN}
        case TIMEOUT_SIG: // intentionally fall through
        case BUTTON_DOWN_SIG: {
            static struct {
                QP::QMState const *target;
                QP::QActionHandler act[2];
            } const tatbl_ = { // transition-action table
                &off_s,
                {
                    Q_ACTION_CAST(&off_e), // entry
                    Q_ACTION_CAST(0)  // zero terminator
                }
            };
            status_ = QM_TRAN(&tatbl_);
            break;
        }
        default: {
            status_ = QM_SUPER();
            break;
        }
    }
    return status_;
}


//............................................................................
static Motor l_motor;                           // instantiate the Motor AO
QActive *AO_Motor = &l_motor;     // initialize the global pointer to motor

//............................................................................
void setup() {
    BSP_init(); // initialize the BSP

    QF::init(); // initialize the framework and the underlying RT kernel

    static QEvt const *motor_queueSto[10]; // alocate event queue buffer

    static QF_MPOOL_EL(GrinderEvent) l_smlPoolSto[4]; //storage for small epool

    QF::poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0])); // initialize event pools...

    AO_Motor->start(1, motor_queueSto, Q_DIM(motor_queueSto),
                     (void *)0, 0U);        // start the motor active object
}

ISR(INT6_vect) {
    Serial1.println("ISR hit.");
    // new event
    GrinderEvent *pEvt = Q_NEW(GrinderEvent, BUTTON_DOWN_SIG);
    AO_Motor->POST(pEvt, 2); // invoke the POST macro doing post_()
}


// Do not define the loop() function, because this function is already defined in the QP port to Arduino


