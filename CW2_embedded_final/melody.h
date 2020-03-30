#ifndef MELODY_H
#define MELODY_H

#include <string>
#include <map>
#include <vector>
#include "mbed.h"

extern Mutex melody_mutex;
extern Mutex pwm_mutex;

extern PwmOut *pMotor;
extern PwmOut MotorPWM;

extern std::vector<std::pair<std::string,int>> new_melody;

extern std::map<std::string,float> frequency_map;

void melody(void);

#endif