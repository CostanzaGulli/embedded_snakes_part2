#include "decoder.h"

void serialISR(){
    uint8_t* newChar = inCharQ.alloc();
    *newChar = pc.getc();
    inCharQ.put(newChar);
}

//Decoding commands
void decoder(){
    std::string serialIn;
    pc.attach(&serialISR);
    
    while(1){
        osEvent newEvent = inCharQ.get();
        uint8_t* newChar = (uint8_t*)newEvent.value.p;
        serialIn += (*newChar);
        //putMessageString(MessageTypeString::CHAR,serialIn); //for testing: print received inputs from serial port
        inCharQ.free(newChar);
        if(serialIn[serialIn.length()-1] == '\r'){
            switch(serialIn[0]){
                case 'K':{
                    std::string receivedKey_temp1 = serialIn.substr(1,8);
                    std::string receivedKey_temp0 = serialIn.substr(9,8);
                    uint32_t receivedKey1;
                    uint32_t receivedKey0;
                    std::sscanf(receivedKey_temp1.c_str(), "%x", &receivedKey1);
                    std::sscanf(receivedKey_temp0.c_str(), "%x", &receivedKey0);
                    uint32_t newKey1 = receivedKey1;
                    uint32_t newKey0 = receivedKey0;
                    key_mutex.lock();
                    newKey = (pow(16,8))*newKey1;
                    newKey = newKey + newKey0;
                    //putMessage(MessageType::NEW_KEY,newKey); //for testing: print received key 
                    key_mutex.unlock();
                    break;
                }
                case 'V':{
                    std::string receivedVel_temp = serialIn.substr(1,serialIn.length()-1);
                    s_mutex.lock();
                    std::sscanf(receivedVel_temp.c_str(), "%f", &s);
                    //putMessage(MessageType::RECEIVED_VELOCITY,(uint64_t)round(s*1000)); //for testing: print received velocity
                    s_mutex.unlock();
                    break;
                }
                case 'R':{
                    std::string receivedRot_temp = serialIn.substr(1,serialIn.length()-1);
                    receivedRot_mutex.lock();
                    std::sscanf(receivedRot_temp.c_str(), "%f", &receivedRot);
                    if(receivedRot >= 0){
                        //putMessage(MessageType::RECEIVED_POSITIONpositive,(uint64_t)round(receivedRot*1000)); //for testing: print received (positive) rotations
                    }
                    else{
                        //putMessage(MessageType::RECEIVED_POSITIONnegative,(uint64_t)round(abs(receivedRot*1000))); //for testing: print received (negative) rotations
                    }
                    receivedRot_mutex.unlock();
                    positionController_mutex.lock();
                    positionController = 0;
                    positionController_mutex.unlock();
                    // chosing between clockwise and anticlockwise
                    receivedRot_mutex.lock();
                    if(receivedRot < 0){
                        lead = -2;
                    }
                    else{
                        lead = 2;
                    }
                    if(receivedRot != 0 && v == 0){
                        photoISR();
                    }
                    receivedRot_mutex.unlock();
                    break;
                }
                case 'T':{
                    //putMessageString(MessageTypeString::MELODY,serialIn); //for testing: print received melody
                    std::string melody_string = serialIn.substr(1,serialIn.length()-1);
                    std::vector<std::pair<std::string,int>> local_melody;
                    std::string tone;
                    int period;
                    for(int i = 0; i < melody_string.length()-1; i += 2){
                        if (melody_string[i+1] == '#' || melody_string[i+1] == '^'){
                            tone = melody_string.substr(i,2);
                            period = melody_string[i+1] - '0';
                            local_melody.push_back(make_pair(tone,period));
                            i++;
                        }
                        else{
                            tone = melody_string[i];
                            period = melody_string[i+1] - '0';
                            local_melody.push_back(make_pair(tone,period));
                        }
                    }
                    melody_mutex.lock();
                    new_melody = local_melody;
                    melody_mutex.unlock();
                    break;
                }
                default:                    
                    break; 
            }
            serialIn.clear();
        }
    }
}