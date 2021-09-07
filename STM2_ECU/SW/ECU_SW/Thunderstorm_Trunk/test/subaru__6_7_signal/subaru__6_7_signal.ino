#define PIN_CRK 13
#define PIN_CAM 12

float revPerd = 0.0;
float engSpd = 6000.0;
int incr = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PIN_CAM, OUTPUT);
  digitalWrite(PIN_CAM, HIGH);

  
  pinMode(PIN_CRK, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(engSpd > 8000)
    incr = -10;
  else if(engSpd < 500)
    incr = 10;
  engSpd = engSpd + incr;
  revPerd = (1000000.0 / engSpd) * 60.0;


      // 1 CAM pulse
  delayMicroseconds((uint16_t)(revPerd * 15.0/360.0) - 100);
  digitalWrite(PIN_CAM, LOW);
  delayMicroseconds(100);
  digitalWrite(PIN_CAM, HIGH);
  
  //crk cyl 1  97BTDC => 83+10 = 93
  delayMicroseconds((uint16_t)(revPerd * 78.0/360.0) - 100);
  digitalWrite(PIN_CRK, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_CRK, LOW);
  //crk cyl 1 65BTDC => 97-65= 32
  delayMicroseconds((uint16_t)(revPerd * 32.0/360.0) - 100);
  digitalWrite(PIN_CRK, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_CRK, LOW);
  //crk cyl 1 10BTDC => 65-10= 55
  delayMicroseconds((uint16_t)(revPerd * 55.0/360.0) - 100);
  digitalWrite(PIN_CRK, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_CRK, LOW);

  // 3 CAM pulse
  delayMicroseconds((uint16_t)(revPerd * 15.0/360.0) - 100);
  digitalWrite(PIN_CAM, LOW);
  delayMicroseconds(100);
  digitalWrite(PIN_CAM, HIGH);
  delayMicroseconds((uint16_t)(revPerd * 5.0/360.0) - 100);
  digitalWrite(PIN_CAM, LOW);
  delayMicroseconds(100);
  digitalWrite(PIN_CAM, HIGH);
  delayMicroseconds((uint16_t)(revPerd * 5.0/360.0) - 100);
  digitalWrite(PIN_CAM, LOW);
  delayMicroseconds(100);
  digitalWrite(PIN_CAM, HIGH);

  //crk cyl 3 97BTDC => 83+10 = 93
  delayMicroseconds((uint16_t)(revPerd * 68.0/360.0) - 100);
  digitalWrite(PIN_CRK, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_CRK, LOW);
  //crk cyl 3 65BTDC => 97-65= 32
  delayMicroseconds((uint16_t)(revPerd * 32.0/360.0) - 100);
  digitalWrite(PIN_CRK, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_CRK, LOW);
  //crk cyl 3 10BTDC => 65-10= 55
  delayMicroseconds((uint16_t)(revPerd * 55.0/360.0) - 100);
  digitalWrite(PIN_CRK, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_CRK, LOW);

    // 1 CAM pulse
  delayMicroseconds((uint16_t)(revPerd * 15.0/360.0) - 100);
  digitalWrite(PIN_CAM, LOW);
  delayMicroseconds(100);
  digitalWrite(PIN_CAM, HIGH);

  //crk cyl 2  97BTDC => 83+10 = 93
  delayMicroseconds((uint16_t)(revPerd * 78.0/360.0) - 100);
  digitalWrite(PIN_CRK, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_CRK, LOW);
  //crk cyl 2 65BTDC => 97-65= 32
  delayMicroseconds((uint16_t)(revPerd * 32.0/360.0) - 100);
  digitalWrite(PIN_CRK, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_CRK, LOW);
  //crk cyl 2 10BTDC => 65-10= 55
  delayMicroseconds((uint16_t)(revPerd * 55.0/360.0) - 100);
  digitalWrite(PIN_CRK, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_CRK, LOW);

    // 2 CAM pulse
  delayMicroseconds((uint16_t)(revPerd * 15.0/360.0) - 100);
  digitalWrite(PIN_CAM, LOW);
  delayMicroseconds(100);
  digitalWrite(PIN_CAM, HIGH);
  delayMicroseconds((uint16_t)(revPerd * 5.0/360.0) - 100);
  digitalWrite(PIN_CAM, LOW);
  delayMicroseconds(100);
  digitalWrite(PIN_CAM, HIGH);

  //crk cyl 4 97BTDC => 83+10 = 93
  delayMicroseconds((uint16_t)(revPerd * 73.0/360.0) - 100);
  digitalWrite(PIN_CRK, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_CRK, LOW);
  //crk cyl 4 65BTDC => 97-65= 32
  delayMicroseconds((uint16_t)(revPerd * 32.0/360.0) - 100);
  digitalWrite(PIN_CRK, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_CRK, LOW);
  //crk cyl 4 10BTDC => 65-10= 55
  delayMicroseconds((uint16_t)(revPerd * 55.0/360.0) - 100);
  digitalWrite(PIN_CRK, HIGH);
  delayMicroseconds(100);
  digitalWrite(PIN_CRK, LOW);


//  Serial.print("revPerd: ");
//  Serial.println(revPerd, 6);
//  Serial.print((uint16_t)(revPerd * 107.0/360.0));
  

}
