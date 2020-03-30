#include "messager.h"

Mail<mail_t, 16> mail_box;
Mail<mail_tString, 16> mail_boxString;

void putMessage(uint8_t messageType,uint64_t data){
    mail_t *mail = mail_box.alloc();
    mail->type = messageType;
    mail->data = data;
    mail_box.put(mail);
}

void messager(void){
    
    while(true){
        osEvent evt = mail_box.get();
        if(evt.status == osEventMail){
            mail_t *mail = (mail_t*)evt.value.p;
            switch (mail->type){
                case MessageType::HELLO_WORLD:
                    printf("Hello world! The program is starting now: \n\r");
                    break;
                case MessageType::NONCE:
                    printf("N%016llx \n\r", mail->data);
                    break;
                case MessageType::LEAD:
                    printf("Lead is: %d\n\r", mail->data);
                    break;
                case MessageType::NEW_KEY:
                    printf("New bitcoin key: %#016llx \n\r", mail->data);
                    break;
                case MessageType::ROTOR_ORIGIN:
                    printf("Rotor origin: %d \n\r", mail->data);
                    break;
                case MessageType::NUMBER_OF_COMPUTATIONS:
                    printf("Number of compuations per second: %llu \n\r", mail->data);
                    break;
                case MessageType::CURRENT_PWM:
                    printf("PWM is:%f\n\r", (float)(mail->data)/1000);
                    break;                   
                case MessageType::CURRENT_VELOCITY:
                    printf("Current velocity: %f \n\r", (mail->data)/1000.0);
                    break;
                case MessageType::RECEIVED_VELOCITY:
                    printf("Received velocity: %f \n\r", (float)(mail->data)/1000);
                    break;  
                case MessageType::CURRENT_POSITION:
                    printf("Current position: %llu \n\r", mail->data);
                    break;                              
                case MessageType::RECEIVED_POSITIONpositive:
                    printf("Received number of rotations: %f \n\r", (mail->data)/1000.0);
                    break;
                case MessageType::RECEIVED_POSITIONnegative:
                    printf("Received number of rotations: -%f \n\r", (mail->data)/1000.0);
                    break;
                case MessageType::CURRENT_ROTATIONS:
                    printf("Rotations: %f\n\r", (float)(mail->data)/1000);
                    break; 
                default:
                    printf("Unknown command:%d \n\r",mail->type);
                    break;              
            }      
            mail_box.free(mail);
        }
    }
}

void putMessageString(uint8_t messageTypeString, std::string data){
    mail_tString *mail = mail_boxString.alloc();
    mail->type = messageTypeString;
    mail->data = data;
    mail_boxString.put(mail);
}

void messagerString(void){
    
    while(true){
        osEvent evt = mail_boxString.get();
        if(evt.status == osEventMail){
            mail_tString *mail = (mail_tString*)evt.value.p;
            switch (mail->type){
                case MessageTypeString::CHAR:
                    printf("%s\n\r", (mail->data).c_str());
                    break;
                case MessageTypeString::MELODY:
                    printf("The received melody is %s\n\r", (mail->data).c_str());
                    break;
                default:
                    printf("Unknown command:%d \n\r", mail->type);
                    break;              
            }      
            mail_boxString.free(mail);
        }
    }
}