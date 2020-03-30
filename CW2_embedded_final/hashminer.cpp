#include "hashminer.h"

void hashminer(void){
    uint8_t sequence[] = {0x45,0x6D,0x62,0x65,0x64,0x64,0x65,0x64,
                        0x20,0x53,0x79,0x73,0x74,0x65,0x6D,0x73,
                        0x20,0x61,0x72,0x65,0x20,0x66,0x75,0x6E,
                        0x20,0x61,0x6E,0x64,0x20,0x64,0x6F,0x20,
                        0x61,0x77,0x65,0x73,0x6F,0x6D,0x65,0x20,
                        0x74,0x68,0x69,0x6E,0x67,0x73,0x21,0x20,
                        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    uint64_t* key = (uint64_t*)&sequence[48]; //pointer to location within sequence so it can be accessed indipendently
    uint64_t* nonce = (uint64_t*)&sequence[56];
    uint8_t hash[32];    
    Timer t;
    int numberOfHash = 0; //counter for computation rate
    t.start();
    
    while(1){
        if(numberOfHash >= 5000){
            if(1.0-t.read()>0.0) {
                ThisThread::sleep_for(1000-t.read()*1000);
            }
            //putMessage(MessageType::NUMBER_OF_COMPUTATIONS, numberOfHash); //for testing: print the number of computations per second
            numberOfHash = 0;
            t.reset();  
        }
        
        *nonce = *nonce + 1;
        key_mutex.lock();
        *key = newKey;
        key_mutex.unlock();
        SHA256::computeHash(hash, (uint8_t*)sequence, 64);
        numberOfHash++;
        
        
        
        //checks if nonce is successful
        if(hash[0] == 0 && hash[1] == 0){
            putMessage(MessageType::NONCE, *nonce); //print successful nonces
        }
        
    }
}