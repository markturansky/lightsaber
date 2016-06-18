
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

int IGNITE = 1;
int HUM = 2;
int CRASH = 3;
int POWER_DOWN= 4;
int SWING_SHORT = 5;
int SWING_MEDIUM = 6;
int SWING_MEDIUM_DEEPER_PITCH = 7;

//SoftwareSerial mySerial(30,31); // RX, TX

void setup () {
    Serial.begin (9600);

//    Serial.println("holy cow!");
    mp3_set_serial (Serial);      //set Serial for DFPlayer-mini mp3 module
    delay(1);                     // delay 1ms to set volume
    mp3_set_volume (15);          // value 0~30
//    Serial.println("holy moly!");
}

void loop () {

//        Serial.println("begining !");
    mp3_play (1);
    delay (1000);
    mp3_play (2);
    delay (1000);
    mp3_play (3);
    delay (1000);
    mp3_play (4);
    delay (1000);
    mp3_play (5);
    delay (1000);
    mp3_play (6);
    delay (1000);
    mp3_play (7);
    delay (1000);

//    Serial.println("end!");
}
