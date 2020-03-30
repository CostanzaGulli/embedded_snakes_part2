///////////////
// LIBRARIES //
///////////////
#include "mbed.h"
#include "Crypto.h"
#include <Mutex.h>
#include <string>
#include <math.h>
#include <vector>
#include "messager.h"
#include "melody.h"
#include "hashminer.h"
#include "motor_controller.h"
#include "decoder.h"

//////////
// PINS //
//////////
//Photointerrupter input pins
#define I1pin D3
#define I2pin D6
#define I3pin D5

//Incremental encoder input pins
#define CHApin   D12
#define CHBpin   D11

//Motor Drive output pins   //Mask in output byte
#define L1Lpin D1           //0x01
#define L1Hpin A3           //0x02
#define L2Lpin D0           //0x04
#define L2Hpin A6           //0x08
#define L3Lpin D10          //0x10
#define L3Hpin D2           //0x20

#define PWMpin D9

//Motor current sense
#define MCSPpin   A1
#define MCSNpin   A0

//Test outputs
#define TP0pin D4
#define TP1pin D13
#define TP2pin A2

//Status LED
DigitalOut led1(LED1);

//Photointerrupter inputs
InterruptIn I1(I1pin);
InterruptIn I2(I2pin);
InterruptIn I3(I3pin);

//Motor Drive outputs
DigitalOut L1L(L1Lpin);
DigitalOut L1H(L1Hpin);
DigitalOut L2L(L2Lpin);
DigitalOut L2H(L2Hpin);
DigitalOut L3L(L3Lpin);
DigitalOut L3H(L3Hpin);

DigitalOut TP1(TP1pin);
PwmOut MotorPWM(PWMpin);
PwmOut *pMotor = &MotorPWM;

///////////////
// VARIABLES //
///////////////
//Mapping from sequential drive states to motor phase outputs
/*
State   L1  L2  L3
0       H   -   L
1       -   H   L
2       L   H   -
3       L   -   H
4       -   L   H
5       H   L   -
6       -   -   -
7       -   -   -
*/
//Drive state to output table
const int8_t driveTable[] = {0x12,0x18,0x09,0x21,0x24,0x06,0x00,0x00};

//Mapping from interrupter inputs to sequential rotor states. 0x00 and 0x07 are not valid
const int8_t stateMap[] = {0x07,0x05,0x03,0x04,0x01,0x00,0x02,0x07};  
//const int8_t stateMap[] = {0x07,0x01,0x03,0x02,0x05,0x00,0x04,0x07}; //Alternative if phase order of input or drive is reversed

//Phase lead to make motor spin
int8_t lead = 0;  //2 for forwards, -2 for backwards. Set to 0 at beginning so motor doesn't spin

int8_t orState = 0;
int8_t intState = 0;
int8_t intStateOld = 0;
uint64_t position = 0;
int positionController = 0;

float s = 100;//set initial maximum velocity to 100
float v; //current velocity
float receivedRot = 0;//set initial target rotations to 0

std::vector<std::pair<std::string,int>> new_melody;

uint64_t newKey;//setting the bitcoin key

RawSerial pc(SERIAL_TX, SERIAL_RX);

Mail<uint8_t, 8> inCharQ;

/////////////
// THREADS //
/////////////
Thread messagerThread(osPriorityNormal,1024);
Thread messagerStringThread(osPriorityNormal,1024);
Thread decoderThread(osPriorityAboveNormal,1024);
Thread motorCtrlT(osPriorityNormal,1024);
Thread melodyThread(osPriorityBelowNormal,1024);
Thread hashminerThread(osPriorityNormal,1024);

///////////
// MUTEX //
///////////
Mutex key_mutex;
Mutex melody_mutex;
Mutex pwm_mutex;
Mutex positionController_mutex;
Mutex s_mutex;
Mutex receivedRot_mutex;

///////////
// TIMER //
///////////
Timer tVel;//timer to calculate velocity


///////////////
// FUNCTIONS //
///////////////

//Set a given drive state
void motorOut(int8_t driveState){
    //Lookup the output byte from the drive state.
    int8_t driveOut = driveTable[driveState & 0x07];
     
    //Turn off first
    if (~driveOut & 0x01) L1L = 0;
    if (~driveOut & 0x02) L1H = 1;
    if (~driveOut & 0x04) L2L = 0;
    if (~driveOut & 0x08) L2H = 1;
    if (~driveOut & 0x10) L3L = 0;
    if (~driveOut & 0x20) L3H = 1;
   
    //Then turn on
    if (driveOut & 0x01) L1L = 1;
    if (driveOut & 0x02) L1H = 0;
    if (driveOut & 0x04) L2L = 1;
    if (driveOut & 0x08) L2H = 0;
    if (driveOut & 0x10) L3L = 1;
    if (driveOut & 0x20) L3H = 0;
}
    
//Convert photointerrupter inputs to a rotor state
inline int8_t readRotorState(){
    return stateMap[I1 + 2*I2 + 4*I3];
}

//Synchronisation routine    
int8_t motorHome(){
    //Put the motor in drive state 0 and wait for it to stabilise
    motorOut(0);
    wait(2.0);
   
    //Get the rotor state
    return readRotorState();
}

//Interrupt service routine to move motor
void photoISR(){
    position++;
    positionController++; 
       
    intState = readRotorState();
    int diff = intState - intStateOld;

    if (intState != intStateOld) {
        motorOut((intState-orState+lead+6)%6);
        intStateOld = intState;
    }     
}

// Main
int main(){
    const int32_t PWM_PRD = 2000;            
    pwm_mutex.lock();
    MotorPWM.period_us(PWM_PRD);
    MotorPWM.pulsewidth_us(PWM_PRD);
    pwm_mutex.unlock();
    
    orState = motorHome();
    //putMessage(MessageType::ROTOR_ORIGIN,orState); //for testing: print rotor origin
    
    messagerThread.start(callback(messager));
    messagerStringThread.start(callback(messagerString));
    decoderThread.start(callback(decoder));
    motorCtrlT.start(callback(motorCtrlFn));
    melodyThread.start(callback(melody));
    hashminerThread.start(callback(hashminer));
    
    I1.rise(&photoISR);
    I1.fall(&photoISR);
    I2.rise(&photoISR);
    I2.fall(&photoISR);
    I3.rise(&photoISR);
    I3.fall(&photoISR);    
    tVel.start();
    
    return 0;
}