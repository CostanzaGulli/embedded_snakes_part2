#include <melody.h>

std::map<std::string,float> frequency_map={
    {"C^",493.99},
    {"C",261.626},
    {"C#",277.183},
    {"D^",277.183},
    {"D",293.665} ,
    {"D#",311.127},
    {"E^",311.127},
    {"E",329.628} ,
    {"E#",349.228},
    {"F^",349.228},
    {"F",349.228} ,
    {"F#",369.994},
    {"G^",369.994},
    {"G",391.995} ,
    {"G#",415.305},
    {"A^",415.305},
    {"A",440},
    {"A#",466.164},
    {"B^",466.164},
    {"B",493.99},
    {"B#",261.626}
};

void melody(void){
    float tone_frequency = frequency_map["A"]; //Tone is found from map 
    int tone_duration = 1000; // Can range from 0 to 8 
    int i = 0;
    float period = 1;
    std::vector<std::pair<std::string,int>> current_melody;
    
    while(1){
        melody_mutex.lock();
        current_melody = new_melody;
        melody_mutex.unlock();
        if(current_melody.size() > 0){
            tone_frequency = frequency_map[current_melody[i].first];
            tone_duration = current_melody[i].second*125;        
        }
        if(++i >= current_melody.size())i = 0;//Cyclic loop
        if(tone_frequency == 0.0f)tone_frequency = 1.0f;//Prevents 
        float tone_period = 1.0/tone_frequency;
        pwm_mutex.lock();
        MotorPWM.period(tone_period);
        pwm_mutex.unlock();
        ThisThread::sleep_for(tone_duration);
    }
}