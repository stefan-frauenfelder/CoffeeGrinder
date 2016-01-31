//****************************************************************************
// Model: Grinder.qm
// File:  ./Scale.h
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
//${.::Scale.h} ..............................................................
#ifndef Scale_h
#define Scale_h

#include "includes.h"

extern QActive * const AO_Scale;


//${AOs::Scale} ..............................................................
class Scale : public QP::QMActive {
public:
    QTimeEvt timeEvt;
    float weight;

public:
    Scale()
    : QMActive(Q_STATE_CAST(&Scale::initial)),
      timeEvt(TIMEOUT_SIG)
    {}

protected:
    static QP::QState initial(Scale * const me, QP::QEvt const * const e);
    static QP::QState idle  (Scale * const me, QP::QEvt const * const e);
    static QP::QState idle_e(Scale * const me);
    static QP::QMState const idle_s;
    static QP::QState sampling  (Scale * const me, QP::QEvt const * const e);
    static QP::QState sampling_e(Scale * const me);
    static QP::QMState const sampling_s;
    static QP::QState calibration  (Scale * const me, QP::QEvt const * const e);
    static QP::QMState const calibration_s;
    static QP::QState measuring  (Scale * const me, QP::QEvt const * const e);
    static QP::QState measuring_e(Scale * const me);
    static QP::QMState const measuring_s;
};


#endif // Scale_h
