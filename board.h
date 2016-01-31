
#ifndef board_h
#define board_h

#define BSP_TICKS_PER_SEC 100

const int ONESEC_TOUT           =  1 * BSP_TICKS_PER_SEC;   // one second
const int TWOSECS_TOUT          =  2 * BSP_TICKS_PER_SEC;   // two seconds
const int THREESECS_TOUT        =  3 * BSP_TICKS_PER_SEC;   // three seconds

const int SCALE_MEASUREMENT_RATE    =  BSP_TICKS_PER_SEC / 20; 
const float GRINDER_DOSE_STEP       =  0.1; 

/*--------------------------------------------------------------------------*/

//namespace QP{

//ISR(INT6_vect);
void BSP_init(void);
void BSP_ledOn(void);
void BSP_ledOff(void);

void BSP_displayPosition(float position); 
void BSP_displayDose(float dose); 

float BSP_scaleSample(void);

//} // namespace QP

#endif                                                             /* board_h */
