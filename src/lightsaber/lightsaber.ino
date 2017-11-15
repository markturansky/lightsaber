#include "Arduino.h"
#include <DFMiniMp3.h>

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
  Serial.begin(9600);

  pinMode(4, INPUT);

  Serial.println("initializing...");

  mp3.begin();

  uint16_t volume = mp3.getVolume();
  Serial.print("volume ");
  Serial.println(volume);
  mp3.setVolume(10);

  uint16_t count = mp3.getTotalTrackCount();
  Serial.print("files ");
  Serial.println(count);

  Serial.println("starting...");
}

void loop(){

    long now = millis();
    long elapsed = now - lastHummed;
    if (elapsed > wait){
        Serial.print("elapsed is ");
        Serial.println(elapsed);
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

  Serial.print("button is ");
  Serial.println(btn);

  if(btn == HIGH){
    if (!isHumming){
        Serial.println("IGNITE!");
        mp3.playMp3FolderTrack(IGNITE);  // sd:/mp3/0001.mp3
        waitMilliseconds(2500);
        isHumming = 1;
        mp3.playMp3FolderTrack(HUM);  // sd:/mp3/0001.mp3
        lastHummed = millis();
    } else {
        Serial.println("DOWN!");
        mp3.playMp3FolderTrack(POWER_DOWN);  // sd:/mp3/0001.mp3
        waitMilliseconds(2500);
        isHumming = 0;
    }
  }

    Serial.println("bye...");

    mp3.loop();  // sd:/mp3/0001.mp3
    delay(5);
}



