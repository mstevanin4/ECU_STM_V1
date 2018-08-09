void getTbSpd1();

uint8_t TbTooth1 = 0;
uint8_t TbNrOfTooth1 = 36;
byte pinTbSpd1 = 18;
unsigned long curTbTime1;
unsigned long oldcurTbTime1;
unsigned long TiSinceLstTooth1;
unsigned long TbSpd1;

void setup() 
{
  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(pinTbSpd1), getTbSpd1, RISING);
} 

void loop()
{
  Serial.print("Tooth: ");
  Serial.print(TbTooth1, DEC);
  Serial.print("\ncurTime: ");
  Serial.print(curTbTime1, DEC);
  Serial.print("\ndelta T: ");
  Serial.print(TiSinceLstTooth1, DEC);
  Serial.print("\nTbSpd: ");
  Serial.print(TbSpd1, DEC);
  Serial.println();
}

void getTbSpd1()
{
  oldcurTbTime1 = curTbTime1;
  curTbTime1 = micros();
  TiSinceLstTooth1 = curTbTime1 - oldcurTbTime1;
  TbSpd1 = TiSinceLstTooth1 * TbNrOfTooth1;
  //TbSpd1 = TbSpd1/60;
  TbSpd1 = 60000000/TbSpd1;
  TbTooth1++;
  
}
