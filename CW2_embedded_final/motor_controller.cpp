#include "motor_controller.h"

void motorCtrlTick(){
    motorCtrlT.signal_set(0x1);
}


//Calculate PWM and lead choosing between Yr and Ys
void calculatePWM(float rotations, float yr, float ys, int8_t &l, float &p){
    if(ys > yr){
        //choose Yr
        if(rotations >= 0){
            l = 2;
        }
        else{
            l = -2;
        }
        p = yr;
    }
    else{
        //choose Ys
        if(rotations >= 0){
            if (ys >= 0){
                l = 2;
            }
            else {
                l = -2;
            }
        }
        else{
            if (ys >= 0){
                l = -2;
            }
            else {
                l = 2;
            }
        }
        p = abs(ys);
    }
    if (p > 1.0){
        p = 1.0;
    }
    if (p < 0.0){
        p = 0.0;
    }
}


//Implement speed and rotation controller for the motor
void motorCtrlFn(){
    Ticker motorCtrlTicker;
    motorCtrlTicker.attach_us(&motorCtrlTick,100000);
    int velocityIteration = 0;
    float pwm;
    int oldPosition = 0;
    float Er = 0;
    float ErOld = 0;
    
    while(1){
        motorCtrlT.signal_wait(0x1);
        int diffPosition = position - oldPosition;
        oldPosition = position;
        v = (diffPosition * (1.0/tVel.read()) )/6.0;
        tVel.reset();
        velocityIteration ++;
        if (velocityIteration >= 10){
            //putMessage(MessageType::CURRENT_VELOCITY,(uint64_t)round(v*1000)); //for testing: print current calculated velocity
            velocityIteration=0;
        }
        
        // Rotation control
        receivedRot_mutex.lock();
        int receivedRotPos = round(abs(receivedRot)*6);
        receivedRot_mutex.unlock();
        positionController_mutex.lock();
        Er = receivedRotPos - positionController;
        positionController_mutex.unlock();
        float ErDerivative = (Er - ErOld)*10.0;
        float kpr = 2.9/6000;
        float kdr = 1.5/6000;
        float Yr = kpr*Er + kdr*ErDerivative;
        ErOld = Er;
        
        // Speed control
        float kps = 0.2;
        float kis = 0.05;
        s_mutex.lock();
        float err_vel = s - v;
        s_mutex.unlock();
        float err_vel_integral;
        if(err_vel < 5 && err_vel > -5){
            err_vel_integral = err_vel_integral + err_vel;
        }
        float Ys = kps*err_vel + kis*err_vel_integral;
        
        // Merge Ys and Yr in unique controller   
        receivedRot_mutex.lock();
        float temp=receivedRot;
        receivedRot_mutex.unlock();
        calculatePWM(temp, Yr, Ys, lead, pwm);

        
        positionController_mutex.lock();
        float nRotations = positionController/6.0;
        positionController_mutex.unlock();
        
        // for testing: print number of rotations done when motor stops
        /*if(ErDerivative==0){
            putMessage(MessageType::CURRENT_ROTATIONS,(uint64_t)round(nRotations*1000));
        }*/
                
        //When motor is not moving but it hasn't reached the target position, give it a kick 
        if(v == 0 && Er > 20){
            pwm_mutex.lock();
            MotorPWM.write(1.0);
            pwm_mutex.unlock();
        }
        else if(v == 0 && Er > 3){
            pwm_mutex.lock();
            MotorPWM.write(0.8);
            pwm_mutex.unlock();
        }
        else{
            pwm_mutex.lock();
            MotorPWM.write(pwm);
            pwm_mutex.unlock();
        }
    }
}