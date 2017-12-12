#include "Arduino.h"
#include <DFMiniMp3.h>
#include <math.h>
#include<Wire.h>

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,GyX,GyY,GyZ;
int16_t last_AcX,last_AcY,last_AcZ,last_GyX,last_GyY,last_GyZ;

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

int isIgnited = 0;
long lastSound = 0;
long lastHum = 0;
long wait = 1000;
long humWait = 10000;

void setup(){
    for(int i = 0; i < SEGMENT_COUNT; i++){
        pinMode(i, OUTPUT);
    }
    pinMode(4, INPUT);
    mp3.begin();
    mp3.setVolume(40);

  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);   
  Serial.begin(9600); 
}

int readGyro(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
 
    
  bool isMoving = 0;
  int threshold = 1750;
  if(abs(last_AcX - AcX) > threshold ||  abs(last_AcY - AcY) > threshold || abs(last_AcZ - AcZ) > threshold ){

    Serial.print("x = ");
    Serial.print(AcX);
    Serial.print(" ");
    Serial.print(last_AcX);
    Serial.print(" - ");
  
    Serial.print("y = ");
    Serial.print(AcY);
    Serial.print(" ");
    Serial.print(last_AcY);
    Serial.print(" - ");
  
    Serial.print("z = ");
    Serial.print(AcZ);
    Serial.print(" ");
    Serial.print(last_AcZ);
    Serial.println("");  
    
    last_AcX = AcX;
      last_AcY = AcY;
      last_AcZ = AcZ;
      last_GyX = GyX;
      last_GyY = GyY;
      last_GyZ = GyZ;    
      isMoving = 1;    
    }  

  return isMoving;
}

void loop(){
  bool isMoving = readGyro();
  long now = millis();    
  
  if (isMoving && isIgnited){
    if(now % 2 == 0){
      play(SWING_MEDIUM);      
    } else if(now % 3 == 0){ 
      play(SWING_SHORT);  
    } else {
      play(SWING_MEDIUM_DEEPER_PITCH);  
    }    
  }

  if(isIgnited){
    play(HUM);
  }
  
  int btn = digitalRead(4);

  if(btn == LOW){
    if (!isIgnited){
        Serial.println("IGNITE!");
        ignite();
    } else {
        Serial.println("DOWN!");
        powerdown();
    }
  }

    mp3.loop();
    delay(5);
}

void play(int track){
  long now = millis();
  long elapsed = now - lastSound;  
  long humElapsed = now - lastHum;

  if (track == IGNITE || track == POWER_DOWN){
    mp3.playMp3FolderTrack(track);
    lastSound = now;    
  } else if(track == HUM){
    if(humElapsed > humWait){
        mp3.playMp3FolderTrack(track);
        lastHum = now;
        lastSound = now;
    }
  } else {
    if (elapsed > wait){
      if(isIgnited){
        mp3.playMp3FolderTrack(track);
        lastSound = now;
      }
    }    
  }
}

void ignite(){
    play(IGNITE);
    isIgnited = 1;

    // ignite is 1800 millis long
    // 6 segments means each are 300ms apart
    int brightness[] = { 0, -50, -100, -150, -200, -250 };

    for(int i = 0; i < 1800; i++){
        for(int s = 0; s < SEGMENT_COUNT; s++){
            int b = brightness[s];
            if (b < 0){
                b = 0;  
            }
            if (b > 250){
                b = 250;
            }
            analogWrite(SEGMENTS[s], b);
        }
        for(int s = 0; s < SEGMENT_COUNT; s++){
           brightness[s]++;
        }
        delay(1);
    }   
}

void powerdown(){
    play(POWER_DOWN);
    isIgnited = 0;

    // ignite is 1800 millis long
    // 6 segments means each are 300ms apart
    int brightness[] = { 1000, 800, 700, 600, 500, 400};
    int intialStartLoop = brightness[0];
    for(int i = intialStartLoop; i >= 0; i--){
        for(int s = 0; s < SEGMENT_COUNT; s++){
            int b = brightness[s];
             if (b < 0){
                b = 0;  
            }
            if (b > 250){
                b = 250;
            }

            analogWrite(SEGMENTS[s], b);
        }
        for(int s = 0; s < SEGMENT_COUNT; s++){
           brightness[s]--;
        }
        delay(1);
    }
}

