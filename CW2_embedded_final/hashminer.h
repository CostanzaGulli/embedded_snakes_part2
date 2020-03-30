#ifndef HASHMINER_H
#define HASHMINER_H
#include "messager.h"
#include "mbed.h"
#include "Crypto.h"

extern Mutex key_mutex;
extern uint64_t newKey;

void hashminer(void);

#endif