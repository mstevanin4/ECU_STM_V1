EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:STM2_ECU-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 9 16
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L HC-06 U5
U 1 1 59CC11B6
P 6150 4000
F 0 "U5" H 6050 4050 60  0000 C CNN
F 1 "HC-06" H 6250 4150 60  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06_Pitch2.54mm" H 6250 4150 60  0001 C CNN
F 3 "" H 6250 4150 60  0000 C CNN
	1    6150 4000
	1    0    0    -1  
$EndComp
$Comp
L R_Small R64
U 1 1 59CC1247
P 5450 3900
F 0 "R64" H 5480 3920 50  0000 L CNN
F 1 "2K2" H 5480 3860 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 5450 3900 50  0001 C CNN
F 3 "" H 5450 3900 50  0000 C CNN
	1    5450 3900
	0    1    1    0   
$EndComp
$Comp
L R_Small R65
U 1 1 59CC129E
P 5450 4200
F 0 "R65" H 5480 4220 50  0000 L CNN
F 1 "1K" H 5480 4160 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 5450 4200 50  0001 C CNN
F 3 "" H 5450 4200 50  0000 C CNN
	1    5450 4200
	0    1    1    0   
$EndComp
$Comp
L GND #PWR078
U 1 1 59CC12C2
P 5250 3900
F 0 "#PWR078" H 5250 3650 50  0001 C CNN
F 1 "GND" H 5250 3750 50  0000 C CNN
F 2 "" H 5250 3900 50  0000 C CNN
F 3 "" H 5250 3900 50  0000 C CNN
	1    5250 3900
	0    1    1    0   
$EndComp
$Comp
L GND #PWR079
U 1 1 59CC12DD
P 5850 4400
F 0 "#PWR079" H 5850 4150 50  0001 C CNN
F 1 "GND" H 5850 4250 50  0000 C CNN
F 2 "" H 5850 4400 50  0000 C CNN
F 3 "" H 5850 4400 50  0000 C CNN
	1    5850 4400
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR080
U 1 1 59CC12F0
P 5850 4500
F 0 "#PWR080" H 5850 4350 50  0001 C CNN
F 1 "+5V" H 5850 4640 50  0000 C CNN
F 2 "" H 5850 4500 50  0000 C CNN
F 3 "" H 5850 4500 50  0000 C CNN
	1    5850 4500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5250 3900 5350 3900
Wire Wire Line
	5550 3900 5800 3900
Wire Wire Line
	5800 3900 5800 4200
Wire Wire Line
	5550 4200 5950 4200
Connection ~ 5800 4200
Wire Wire Line
	5850 4400 5950 4400
Wire Wire Line
	5850 4500 5950 4500
Text HLabel 5200 4200 0    60   Input ~ 0
TX
Text HLabel 5200 4300 0    60   Input ~ 0
RX
Wire Wire Line
	5200 4200 5350 4200
Wire Wire Line
	5200 4300 5950 4300
$EndSCHEMATC
