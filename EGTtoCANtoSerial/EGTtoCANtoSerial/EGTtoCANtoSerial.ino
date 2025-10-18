// -------------------------------------------------------------
// CANtest for Teensy 3.6 dual CAN bus
// by Collin Kidder, Based on CANTest by Pawelsky (based on CANtest by teachop)
//
// Both buses are left at default 250k speed and the second bus sends frames to the first
// to do this properly you should have the two buses linked together. This sketch
// also assumes that you need to set enable pins active. Comment out if not using
// enable pins or set them to your correct pins.
//
// This sketch tests both buses as well as interrupt driven Rx and Tx. There are only
// two Tx buffers by default so sending 5 at a time forces the interrupt driven system
// to buffer the final three and send them via interrupts. All the while all Rx frames
// are internally saved to a software buffer by the interrupt handler.
//

#include <FlexCAN.h>


static CAN_message_t msg;
static uint8_t hex[17] = "0123456789abcdef";
byte serCmd=0;
byte speeduinoInputChannel = 0;
int16_t egtCyl1 = 0;
int16_t egtCyl2 = 0;
int16_t egtCyl3 = 0;
int16_t egtCyl4 = 0;
int16_t egtCyl5 = 0;
int16_t egtCyl6 = 0;
int16_t egtCyl7 = 0;
int16_t egtCyl8 = 0;

// -------------------------------------------------------------
void setup(void)
{
  delay(1000);
  Serial.println(F("Hello Teensy 3.2  CAN Test."));
  Serial2.begin(115200);

  Can0.begin(500000);  
//  Can1.begin();


  msg.ext = 0;
  msg.id = 0x100;
  msg.len = 8;
  msg.buf[0] = 10;
  msg.buf[1] = 20;
  msg.buf[2] = 0;
  msg.buf[3] = 100;
  msg.buf[4] = 128;
  msg.buf[5] = 64;
  msg.buf[6] = 32;
  msg.buf[7] = 16;
}


// -------------------------------------------------------------
void loop(void)
{
  CAN_message_t inMsg;
  while (Can0.available()) 
  {
    Can0.read(inMsg);
    if(inMsg.id == 1552)
    {
      egtCyl1 = (inMsg.buf[1] << 8) + inMsg.buf[0];
      egtCyl2 = (inMsg.buf[3] << 8) + inMsg.buf[2];
      egtCyl3 = (inMsg.buf[5] << 8) + inMsg.buf[4];
      egtCyl4 = (inMsg.buf[7] << 8) + inMsg.buf[6];
    }
    else if(inMsg.id == 1553)
    {
      egtCyl5 = (inMsg.buf[1] << 8) + inMsg.buf[0];
      egtCyl6 = (inMsg.buf[3] << 8) + inMsg.buf[2];
      egtCyl7 = (inMsg.buf[5] << 8) + inMsg.buf[4];
      egtCyl8 = (inMsg.buf[7] << 8) + inMsg.buf[6];
    }
    /*Serial.print("EGTCyl1: ");
    Serial.println(egtCyl1);
    Serial.print("EGTCyl2: ");
    Serial.println(egtCyl2);
    Serial.print("EGTCyl3: ");
    Serial.println(egtCyl3);
    Serial.print("EGTCyl4: ");
    Serial.println(egtCyl4);
    Serial.print("EGTCyl5: ");
    Serial.println(egtCyl5);
    Serial.print("EGTCyl6: ");
    Serial.println(egtCyl6);*/  
  }
  while(Serial2.available() >= 4)
  {
    serCmd = Serial2.read();
    if(serCmd == 82)
    {
      speeduinoInputChannel = Serial2.read();
      Serial2.read(); // consums CAN Address
      Serial2.read(); // consums CAN Address
      Serial2.write('G');
      Serial2.write(1);
      Serial2.write(speeduinoInputChannel);
      switch(speeduinoInputChannel)
      {
        case 0:
          Serial2.write(lowByte(egtCyl1));
          Serial2.write(highByte(egtCyl1));
          break;
        case 1:
          Serial2.write(lowByte(egtCyl2));
          Serial2.write(highByte(egtCyl2));
          break;
        case 2:
          Serial2.write(lowByte(egtCyl3));
          Serial2.write(highByte(egtCyl3));
          break;
        case 3:
          Serial2.write(lowByte(egtCyl4));
          Serial2.write(highByte(egtCyl4));
          break;
        case 4:
          Serial2.write(lowByte(egtCyl5));
          Serial2.write(highByte(egtCyl5));
          break;
        case 5:
          Serial2.write(lowByte(egtCyl6));
          Serial2.write(highByte(egtCyl6));
          break;
        default:
          Serial2.write(0);
          Serial2.write(0);
          break;
      }
      Serial2.write(0);
      Serial2.write(0);
      Serial2.write(0);
      Serial2.write(0);
      Serial2.write(0);
      Serial2.write(0);
    }
  }
}
