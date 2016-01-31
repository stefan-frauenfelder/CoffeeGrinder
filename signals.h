
#ifndef signals_h
#define signals_h

#include "qp_port.h"

/*--------------------------------------------------------------------------*/

using namespace QP;


enum GrinderSignals { // signals for the Grinder application
    TIMEOUT_SIG = Q_USER_SIG,
    BUTTON_DOWN_SIG,
    SCALE_CALIB_SIG,
    SCALE_CALIB_DONE_SIG,
    SCALE_START_SIG,
    SCALE_SAMPLING_DONE_SIG,
    ENCODER_INC_SIG,
    ENCODER_DEC_SIG
};

#endif  /* signals_h */
