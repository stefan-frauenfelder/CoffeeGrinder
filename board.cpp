

#include "includes.h"


// button with led
#define BUTTON_LED_PIN 5
#define BUTTON_SWITCH_PIN 7

// encoder port
#define ENCODER_CTL DDRD //encoder port control
#define ENCODER_WR PORTD //encoder port write
#define ENCODER_RD PIND //encoder port read
#define ENCODER_A 2
#define ENCODER_B 3
Encoder myEnc(ENCODER_A, ENCODER_B);


// scale
#define SCALE_DOUT  4
#define SCALE_CLK   6
#define SCALE_CALIBRATION_FACTOR -444190.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
HX711 scale(SCALE_DOUT, SCALE_CLK);


// OLED
//#define SPI_MOSI 16
//#define SPI_MISO 14
//#define SPI_CLK  15
#define OLED_DC     8
#define OLED_CS    10
#define OLED_RESET  9
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);



void postEncoderEvent(int direction) {

    if (direction == 1) {
        // post event
        GrinderEvent *pEvt = Q_NEW(GrinderEvent, ENCODER_INC_SIG);
        AO_Grinder->POST(pEvt, 2);
        Serial1.println("ENCODER_INC_SIG posted.");
    }
    if (direction == -1) {
        // post event
        GrinderEvent *pEvt = Q_NEW(GrinderEvent, ENCODER_DEC_SIG);
        AO_Grinder->POST(pEvt, 2);
        Serial1.println("ENCODER_DEC_SIG posted.");
    }
}

ISR(INT0_vect) { 
    // call the static update function
    int direction = Encoder::update(myEnc.interruptArgs[0]);
    if (direction) {
        // post event
        postEncoderEvent(direction);
    }
}

ISR(INT1_vect) { 
    // call the static update function
    int direction = Encoder::update(myEnc.interruptArgs[1]);
    if (direction) {
        // post event
        postEncoderEvent(direction);
    }
}




ISR(TIMER0_COMPA_vect) {
    QF::TICK((void *)0); // process all armed time events
}

void BSP_ledOff(void) {
    digitalWrite(BUTTON_LED_PIN, LOW);    // turn the LED off by making the voltage LOW
}

void BSP_ledOn(void) {
    digitalWrite(BUTTON_LED_PIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    //Serial1.println(myEnc.read());
}


void BSP_displayDose(float dose) {

    // clear area
    //display.clearDisplay();
    display.fillRect(0,0,128,32,0);

    display.setCursor(20,20);
    if (dose>0) {
        display.print(" ");
    }
    if (abs(dose)<1000) {
        display.print(" ");
        if (abs(dose)<100) {
            display.print(" ");
            if (abs(dose)<10) {
                display.print(" ");
            }
        }
    }
    display.print(dose,1);
    display.print("g");
    display.display();
}

void BSP_displayPosition(float position) {

    // clear area
    display.fillRect(0,32,128,32,0);

    display.setCursor(20,60);
    if (position>0) {
        display.print(" ");
    }
    if (abs(position)<1000) {
        display.print(" ");
        if (abs(position)<100) {
            display.print(" ");
            if (abs(position)<10) {
                display.print(" ");
            }
        }
    }
    display.print(position,1);
    display.print("g");
    display.display();
}



float BSP_scaleSample(void) {
    // get the weight from the scale
    float gramms = scale.get_units(1)*1000;
    // display weight
    BSP_displayPosition(gramms);
    // post an event to signal measurement is done
    GrinderEvent *pEvt = Q_NEW(GrinderEvent, SCALE_SAMPLING_DONE_SIG);
    AO_Scale->POST(pEvt, 2); // invoke the POST macro doing post_()
    return gramms;
}


void BSP_init(void) {

    Serial1.begin(115200U); // set the highest stanard baud rate of 115200 bps
    Serial1.println("Starting.");

    // button pins
    pinMode(BUTTON_LED_PIN, OUTPUT);    // button LED
    pinMode(BUTTON_SWITCH_PIN, INPUT);     // button direction
    digitalWrite(BUTTON_SWITCH_PIN, HIGH); // button: turn on pullup resistors


    //ENCODER_CTL &= ~(( 1<<ENCODER_A )|( 1<<ENCODER_B )); //inputs
    //ENCODER_WR |= (( 1<<ENCODER_A )|( 1<<ENCODER_B )); //turn on pullups

    // configure external interrupt 1 (ArduinoPin2, PD1, Encoder)
    EICRA |= (1<<ISC00)|(0<<ISC01); // external int ctrl, any edge detection for interrupt INT0
    EIMSK |= (1<<INT0);             // external int mask, activates interrupt 0

    EICRA |= (1<<ISC10)|(0<<ISC11); // external int ctrl, any edge detection for interrupt INT1
    EIMSK |= (1<<INT1);             // external int mask, activates interrupt 1

    // configure external interrupt 6 (Pin7)
    EICRB |= (0<<ISC60)|(1<<ISC61); // external int ctrl, falling edge detection for interrupt INT6
    EIMSK |= (1<<INT6);             // external int mask, activates interrupt 6

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

