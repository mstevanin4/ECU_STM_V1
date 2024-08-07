#include <SPI.h>
#include <mcp2515.h>


struct can_frame canMsg1;
MCP2515 mcp2515(10);


void setup() {
  canMsg1.can_id  = 0x201;
  canMsg1.can_dlc = 8;
  canMsg1.data[0] = 0x0F;
  canMsg1.data[1] = 0x0A;
  canMsg1.data[2] = 0xFF;
  canMsg1.data[3] = 0xFF;
  canMsg1.data[4] = 0x27;
  canMsg1.data[5] = 0x10;
  canMsg1.data[6] = 0x00;
  canMsg1.data[7] = 0xFF;
  
  while (!Serial);
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  
  Serial.println("Example: Write to CAN");
}

void loop() {
  mcp2515.sendMessage(&canMsg1);

  Serial.println("Messages sent");
  
  delay(100);
}
