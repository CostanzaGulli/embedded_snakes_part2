 
#ifndef MESSAGER_H_
#define MESSAGER_H_

#include "rtos.h"
#include <string>

extern void putMessage(uint8_t messageType,uint64_t data);
void messager(void);

struct MessageType{
    enum Enum : uint8_t{
        HELLO_WORLD         =   0,
        NONCE               =   1,        
        LEAD                =   4,
        SET_KEY             =   5,
        RECEIVED_KEY_LOWER  =   6,
        RECEIVED_KEY_HIGHER =   7,
        NEW_KEY             =   8,
        ROTOR_ORIGIN        =   9,
        NUMBER_OF_COMPUTATIONS= 10,
        CURRENT_PWM         =   19,
        CURRENT_VELOCITY    =   20,
        NEXT_VELOCITY       =   21,
        RECEIVED_VELOCITY   =   22,
        CURRENT_POSITION    =   23,
        NEXT_POSITION       =   24,
        RECEIVED_POSITIONpositive   =   25,
        RECEIVED_POSITIONnegative = 26,
        CURRENT_ROTATIONS = 27,   
    };    
};

typedef struct {
  uint8_t    type; /* AD result of measured current */
  uint64_t    data;    /* A counter value               */
} mail_t;

extern void putMessageString(uint8_t messageTypeString, std::string data);
void messagerString(void);

struct MessageTypeString{
    enum Enum : uint8_t{
        CHAR         =   0,
        MELODY       =   1,           
    };    
};

typedef struct {
    uint8_t    type;
    std::string    data;
} mail_tString;

#endif