#ifndef COMMAND_DECODER_H
#define COMMAND_DECODER_H
#include "messager.h"
#include "mbed.h"
#include "Crypto.h"
#include <string>
#include <vector>

extern Mutex melody_mutex;
extern Mutex key_mutex;
extern Mutex s_mutex;
extern Mutex receivedRot_mutex;
extern Mutex positionController_mutex;

extern uint64_t newKey;
extern float s;
extern float receivedRot;
extern int positionController;
extern int8_t lead;
extern float v;
extern std::vector<std::pair<std::string,int>> new_melody;

extern Mail<uint8_t, 8> inCharQ;
extern RawSerial pc;

void serialISR();
void decoder();
extern void photoISR();

#endif