#include <SoftwareSerial.h>

SoftwareSerial hc06(10,11);

void setup(){

  Serial.begin(9600);
  Serial.println("ENTER AT Commands:");
  hc06.begin(19200);
  
}

void loop(){

  if (hc06.available()){
    Serial.write(hc06.read());
  }

  if (Serial.available()){
    hc06.write(Serial.read());
  }  

}

