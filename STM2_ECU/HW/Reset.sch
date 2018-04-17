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
Sheet 12 16
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
L +5V #PWR087
U 1 1 59DB810B
P 5650 3950
F 0 "#PWR087" H 5650 3800 50  0001 C CNN
F 1 "+5V" H 5650 4090 50  0000 C CNN
F 2 "" H 5650 3950 50  0000 C CNN
F 3 "" H 5650 3950 50  0000 C CNN
	1    5650 3950
	1    0    0    -1  
$EndComp
$Comp
L C_Small C33
U 1 1 59DB811F
P 5650 4550
F 0 "C33" H 5660 4620 50  0000 L CNN
F 1 "4nF7" H 5660 4470 50  0000 L CNN
F 2 "Capacitors_SMD:C_0603_HandSoldering" H 5650 4550 50  0001 C CNN
F 3 "" H 5650 4550 50  0000 C CNN
	1    5650 4550
	1    0    0    -1  
$EndComp
$Comp
L R_Small R69
U 1 1 59DB8166
P 5650 4150
F 0 "R69" H 5680 4170 50  0000 L CNN
F 1 "1K" H 5680 4110 50  0000 L CNN
F 2 "Resistors_SMD:R_0603_HandSoldering" H 5650 4150 50  0001 C CNN
F 3 "" H 5650 4150 50  0000 C CNN
	1    5650 4150
	1    0    0    -1  
$EndComp
$Comp
L D_Schottky_Small D8
U 1 1 59DB818F
P 6150 4150
F 0 "D8" H 6100 4230 50  0000 L CNN
F 1 "B0540WS-7" H 5870 4070 50  0000 L CNN
F 2 "Diodes_SMD:D_SOD-323_HandSoldering" V 6150 4150 50  0001 C CNN
F 3 "" V 6150 4150 50  0000 C CNN
	1    6150 4150
	0    1    1    0   
$EndComp
Text HLabel 6350 4350 2    60   Input ~ 0
RESET_CPU
$Comp
L GND #PWR088
U 1 1 59DB81F2
P 5650 4750
F 0 "#PWR088" H 5650 4500 50  0001 C CNN
F 1 "GND" H 5650 4600 50  0000 C CNN
F 2 "" H 5650 4750 50  0000 C CNN
F 3 "" H 5650 4750 50  0000 C CNN
	1    5650 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5650 3950 5650 4050
Wire Wire Line
	5650 4250 5650 4450
Wire Wire Line
	5650 4650 5650 4750
Wire Wire Line
	6350 4350 5650 4350
Connection ~ 5650 4350
Wire Wire Line
	6150 4250 6150 4350
Connection ~ 6150 4350
Wire Wire Line
	6150 4050 6150 4000
Wire Wire Line
	6150 4000 5650 4000
Connection ~ 5650 4000
$EndSCHEMATC
