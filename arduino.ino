#include <SD.h>
#include <SPI.h>
#include <SoftwareSerial.h>

#include "TMRpcm.h"

SoftwareSerial SUART(2, 3);

TMRpcm tmrpcm;
int incomingByte = 0;

char* audio[] = {
  "aaaaaaa0.wav",
  "aaaaaaa1.wav",
  "aaaaaaa2.wav",
  "aaaaaaa3.wav",
  "aaaaaaa4.wav",
  "aaaaaaa5.wav",
  "aaaaaaa6.wav",
  "aaaaaaa7.wav",
  "aaaaaaa8.wav",
  "aaaaaaa9.wav",
  "aaaaaa10.wav",
  "aaaaaa11.wav",
  "aaaaaa12.wav",
  "aaaaaa13.wav",
  "aaaaaa14.wav",
  "aaaaaa15.wav",
  "aaaaaa16.wav",
  "aaaaaa17.wav",
  "aaaaaa18.wav",
  "aaaaaa19.wav",
  "aaaaaa20.wav",
  "aaaaaa21.wav",
  "aaaaaa22.wav",
  "aaaaaa23.wav",
  "aaaaaa24.wav",
  "aaaaaa25.wav"
};

void setup() {
  Serial.begin(115200);
  SUART.begin(9600);

  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  tmrpcm.speakerPin = 9;
  tmrpcm.setVolume(6);
  // tmrpcm.play("hehehe.wav");
  // tmrpcm.play("ivanewe.wav");
  // tmrpcm.play("deargod.wav");  
}

void loop() {
  if (SUART.available() > 0) {
    String a = SUART.readStringUntil('\n');
    int i = a.toInt();
    // int readed = Serial.read();
    // if (readed != '\n' && readed != 13) {
    //   int i = readed - 48;
    Serial.print("Playing: ");
    Serial.println(i);
    tmrpcm.stopPlayback();
    tmrpcm.play(audio[i]);
    // }
  }
}