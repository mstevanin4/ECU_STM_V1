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
Sheet 13 16
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
L Seeeduino_CAN-BUS_Module_MCP2551_and_MCP2515 U7
U 1 1 59DFFA64
P 5150 3700
F 0 "U7" H 4950 3700 60  0000 C CNN
F 1 "Seeeduino_CAN-BUS_Module_MCP2551_and_MCP2515" H 5750 3800 60  0000 C CNN
F 2 "ECU_STM:Seeeduino_CAN_Module" H 5150 3700 60  0001 C CNN
F 3 "" H 5150 3700 60  0000 C CNN
	1    5150 3700
	1    0    0    -1  
$EndComp
$Comp
L MCP1703A-3302E/DB U8
U 1 1 59DFFC0D
P 5600 4500
F 0 "U8" H 4700 4700 60  0000 C CNN
F 1 "MCP1703A-3302E/DB" H 5150 4800 60  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-223-3_TabPin2" H 5150 4800 60  0001 C CNN
F 3 "" H 5150 4800 60  0000 C CNN
	1    5600 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 3850 6950 3850
Wire Wire Line
	6950 3850 6950 4550
Wire Wire Line
	6950 4550 6100 4550
$Comp
L GND #PWR089
U 1 1 59DFFD21
P 5600 5100
F 0 "#PWR089" H 5600 4850 50  0001 C CNN
F 1 "GND" H 5600 4950 50  0000 C CNN
F 2 "" H 5600 5100 50  0000 C CNN
F 3 "" H 5600 5100 50  0000 C CNN
	1    5600 5100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR090
U 1 1 59DFFD39
P 7100 3750
F 0 "#PWR090" H 7100 3500 50  0001 C CNN
F 1 "GND" H 7100 3600 50  0000 C CNN
F 2 "" H 7100 3750 50  0000 C CNN
F 3 "" H 7100 3750 50  0000 C CNN
	1    7100 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5550 4950 5550 5000
Wire Wire Line
	5550 5000 5650 5000
Wire Wire Line
	5600 5000 5600 5100
Wire Wire Line
	5650 5000 5650 4950
Connection ~ 5600 5000
Wire Wire Line
	6200 3750 7100 3750
$Comp
L +5V #PWR091
U 1 1 59DFFD69
P 4900 4550
F 0 "#PWR091" H 4900 4400 50  0001 C CNN
F 1 "+5V" H 4900 4690 50  0000 C CNN
F 2 "" H 4900 4550 50  0000 C CNN
F 3 "" H 4900 4550 50  0000 C CNN
	1    4900 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 4550 4900 4550
Text HLabel 6250 3950 2    60   Input ~ 0
TX_CAN
Text HLabel 6250 4050 2    60   Input ~ 0
RX_CAN
Wire Wire Line
	6200 3950 6250 3950
Wire Wire Line
	6200 4050 6250 4050
Text HLabel 4850 3850 0    60   Input ~ 0
CANL
Wire Wire Line
	4850 3850 4950 3850
Text HLabel 4850 3950 0    60   Input ~ 0
CANH
Wire Wire Line
	4850 3950 4950 3950
$EndSCHEMATC
