/*
 Example using the SparkFun HX711 breakout board with a scale
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 19th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 This example demonstrates basic scale output. See the calibration sketch to get the calibration_factor for your
 specific load cell setup.

 This example code uses bogde's excellent library: https://github.com/bogde/HX711
 bogde's library is released under a GNU GENERAL PUBLIC LICENSE

 The HX711 does one thing well: read load cells. The breakout board is compatible with any wheat-stone bridge
 based load cell which should allow a user to measure everything from a few grams to tens of tons.
 Arduino pin 2 -> HX711 CLK
 3 -> DAT
 5V -> VCC
 GND -> GND

 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.

*/

#include "HX711.h"

#define calibration_factor -66700.0 //This value is obtained using the SparkFun_HX711_Calibration sketch

#define DOUT1  54
#define CLK1  55
long staticLoad = 0;
bool rdy = 0;
long loadOnTbCell_long;
int loadOnTbCell_int;
float loadOnTbCell_flt;

HX711 scale2(DOUT1, CLK1);

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 scale demo");

  scale2.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  //scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
  //staticLoad = scale.read_average(4);
  Serial.println("Readings:");
}

void loop() {
  Serial.print("Ready: ");
  rdy = scale2.is_ready();
  Serial.print(rdy);
  if (rdy)
  {
    loadOnTbCell_flt = scale2.get_units();
    loadOnTbCell_int = (int)map(loadOnTbCell_long, 0,4294967295, 32767, 0);
    Serial.print("Reading (float): ");
    Serial.print(loadOnTbCell_flt, 3); //scale.get_units() returns a float
    Serial.print("Kg");
    loadOnTbCell_int = (int) (loadOnTbCell_flt * 100);
    Serial.print("\nReading (int): ");
    Serial.print(loadOnTbCell_int, DEC); //scale.get_units() returns a float
    Serial.print("[Kg/100]");
//    loadOnTbCell_int = (int)loadOnTbCell_long;
//    Serial.print("Reading (int): ");
//    Serial.print(loadOnTbCell_int, 1); //scale.get_units() returns a float
//    Serial.print("  Reading good(int): ");
//    Serial.print(loadOnTbCell_long >> 16, 1); //scale.get_units() returns a float
  }
  Serial.println();
  delay(500);
}

