#include "Arduino.h"
#include <DFMiniMp3.h>
#include <math.h>

class Mp3Notify
{
public:
  static void OnError(uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }

  static void OnPlayFinished(uint16_t globalTrack)
  {
    Serial.println();
    Serial.print("Play finished for #");
    Serial.println(globalTrack);
  }

  static void OnCardOnline(uint16_t code)
  {
    Serial.println();
    Serial.print("Card online ");
    Serial.println(code);
  }

  static void OnCardInserted(uint16_t code)
  {
    Serial.println();
    Serial.print("Card inserted ");
    Serial.println(code);
  }

  static void OnCardRemoved(uint16_t code)
  {
    Serial.println();
    Serial.print("Card removed ");
    Serial.println(code);
  }
};

int IGNITE = 1;
int HUM = 2;
int CRASH = 3;
int POWER_DOWN= 4;
int SWING_SHORT = 5;
int SWING_MEDIUM = 6;
int SWING_MEDIUM_DEEPER_PITCH = 7;

int SEGMENTS[] = {3, 5, 6, 9, 10, 11};
int SEGMENT_COUNT = 6;

// instance a DFMiniMp3 object,
DFMiniMp3<HardwareSerial, Mp3Notify> mp3(Serial);

void waitMilliseconds(uint16_t msWait){
  uint32_t start = millis();

  while ((millis() - start) < msWait){
    // calling mp3.loop() periodically allows for notifications
    // to be handled without interrupts
    mp3.loop();
    delay(1);
  }
}

int isHumming = 0;
long lastHummed = 0;
long wait = 5000;

void setup(){
    for(int i = 0; i < SEGMENT_COUNT; i++){
        pinMode(i, OUTPUT);
    }
    pinMode(4, INPUT);

    Serial.println("initializing...");

    mp3.begin();

    uint16_t volume = mp3.getVolume();
    Serial.print("volume ");
    Serial.println(volume);
    mp3.setVolume(60);

    uint16_t count = mp3.getTotalTrackCount();
    Serial.print("files ");
    Serial.println(count);

    Serial.println("starting...");
}

void loop(){

    long now = millis();
    long elapsed = now - lastHummed;
    if (elapsed > wait){
        if(isHumming){
            Serial.println("is humming!");
            Serial.println("huummmmmmmmmmmmm   playing hum.");
            mp3.playMp3FolderTrack(HUM);
            lastHummed = now;
            Serial.print("last hummed is ");
            Serial.println(lastHummed);
        }
    }

  int btn = digitalRead(4);

  if(btn == LOW){
    if (!isHumming){
        Serial.println("IGNITE!");
        ignite();
    } else {
        Serial.println("DOWN!");
        powerdown();
    }
  }

    mp3.loop();  // sd:/mp3/0001.mp3
    delay(5);
}

void play(int track){
    // sd:/mp3/0001.mp3 where number is 'track'
    mp3.playMp3FolderTrack(track);
}

void ignite(){
    play(IGNITE);

    // ignite is 1800 millis long
    // 6 segments means each are 300ms apart
    int brightness[] = { 0, -300, -600, -900 -1200, -1500 };

    for(int i = 0; i < 1800; i++){
        for(int s = 0; s < SEGMENT_COUNT; s++){
            int b = brightness[s];
            if (b < 0){
                b = 0;
            }
            int f = round(b / 7);

            digitalWrite(SEGMENTS[s], f);
        }
        for(int s = 0; s < SEGMENT_COUNT; s++){
           brightness[s]++;
        }
        delay(1);
    }

    waitMilliseconds(2500);
    isHumming = 1;
    play(HUM);
    lastHummed = millis();
}

void powerdown(){
    play(POWER_DOWN);
    isHumming = 0;
    for(int s = 0; s < SEGMENT_COUNT; s++){
        digitalWrite(SEGMENTS[s], 0);
    }
    waitMilliseconds(2500);

    // ignite is 1800 millis long
    // 6 segments means each are 300ms apart
    int brightness[] = { 1500, 1200, 900, 600, 300, 0};

//    for(int i = 0; i < 1800; i++){
//        for(int s = 0; s < SEGMENT_COUNT; s++){
//            int b = brightness[s];
//            if (b < 0){
//                b = 0;
//            }
//            int f = round(b / 7);
//
//            digitalWrite(SEGMENTS[s], f);
//        }
//        for(int s = 0; s < SEGMENT_COUNT; s++){
//           brightness[s]++;
//        }
//        delay(1);
//    }
//
//    waitMilliseconds(2500);
//    isHumming = 1;
//    play(HUM);
//    lastHummed = millis();
}

