#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H
#include "messager.h"
#include "mbed.h"
#include "Crypto.h"

extern Mutex pwm_mutex;
extern Mutex positionController_mutex;
extern Mutex s_mutex;
extern Mutex receivedRot_mutex;

extern Thread motorCtrlT;

extern float v;
extern uint64_t position;
extern Timer tVel;
extern float receivedRot;
extern int positionController;
extern float s;
extern int8_t lead;

extern PwmOut MotorPWM;
extern PwmOut *pMotor;


void motorCtrlTick();
void calculatePWM(float rotations, float yr, float ys, int8_t &l, float &p);
void motorCtrlFn();

#endif