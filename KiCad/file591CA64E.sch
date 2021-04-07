EESchema Schematic File Version 2
LIBS:ARDF 2 Band MiniTx-rescue
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
LIBS:special
LIBS:ARDF 2 Band MiniTx-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 5
Title "USB-to-UART Interface"
Date "2018-04-07"
Rev "X.2.9"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_01X06 P2003
U 1 1 59115E12
P 6100 3550
F 0 "P2003" H 6100 3900 50  0000 C CNN
F 1 "CONN_01X06" V 6200 3550 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06" H 6100 3550 50  0001 C CNN
F 3 "" H 6100 3550 50  0000 C CNN
	1    6100 3550
	1    0    0    1   
$EndComp
$Comp
L GND_L9 #PWR075
U 1 1 59115E9B
P 5800 3900
F 0 "#PWR075" H 5800 3650 50  0001 C CNN
F 1 "GND_L9" H 5800 3750 50  0000 C CNN
F 2 "" H 5800 3900 50  0000 C CNN
F 3 "" H 5800 3900 50  0000 C CNN
	1    5800 3900
	1    0    0    -1  
$EndComp
NoConn ~ 5900 3700
$Comp
L C_Small C2010
U 1 1 59116016
P 5000 3300
F 0 "C2010" V 5100 3250 50  0000 L CNN
F 1 "100nF" V 4900 3200 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5000 3300 50  0001 C CNN
F 3 "" H 5000 3300 50  0000 C CNN
	1    5000 3300
	0    -1   -1   0   
$EndComp
Text GLabel 4850 3300 0    39   Output ~ 0
~RESET~
$Comp
L GND_L9 #PWR076
U 1 1 59129F20
P 800 7400
F 0 "#PWR076" H 800 7150 50  0001 C CNN
F 1 "GND_L9" H 800 7250 50  0000 C CNN
F 2 "" H 800 7400 50  0000 C CNN
F 3 "" H 800 7400 50  0000 C CNN
	1    800  7400
	1    0    0    -1  
$EndComp
Text Notes 6250 2900 2    79   ~ 16
FTDI Cable Connector
Text Notes 7050 7000 0    118  ~ 24
PCB: Digital Interface Board (p. 2/3)
$Comp
L GND_L8 #PWR088
U 1 1 591801FB
P 1100 7400
F 0 "#PWR088" H 1100 7150 50  0001 C CNN
F 1 "GND_L8" H 1100 7250 50  0000 C CNN
F 2 "" H 1100 7400 50  0000 C CNN
F 3 "" H 1100 7400 50  0000 C CNN
	1    1100 7400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5800 3900 5800 3800
Wire Wire Line
	5800 3800 5900 3800
Wire Wire Line
	5900 3300 5100 3300
Wire Wire Line
	4900 3300 4850 3300
Wire Wire Line
	5900 3600 5750 3600
Wire Wire Line
	800  7400 800  7300
Wire Wire Line
	800  7300 1100 7300
Wire Wire Line
	1100 7300 1100 7400
Text GLabel 5750 3400 0    39   Input ~ 0
DI_TXD0
Text GLabel 4600 3500 0    39   Output ~ 0
PROCESSOR_RXD0_AND_FTDI
Wire Wire Line
	5900 3400 5750 3400
Text GLabel 5750 3600 0    39   Output ~ 0
FTDI_CABLE_+5V
$Comp
L Jumper_NC_Small JP?
U 1 1 5BCB50B3
P 5100 3500
F 0 "JP?" H 5100 3580 50  0000 C CNN
F 1 "Jumper_NC_Small" H 5110 3440 50  0001 C CNN
F 2 "" H 5100 3500 50  0000 C CNN
F 3 "" H 5100 3500 50  0000 C CNN
	1    5100 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5900 3500 5200 3500
Wire Wire Line
	5000 3500 4600 3500
$EndSCHEMATC
