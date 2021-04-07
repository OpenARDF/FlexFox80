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
Sheet 4 4
Title ""
Date "2018-04-07"
Rev "X.2.9"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L GND_L8 #PWR093
U 1 1 592F6B93
P 7150 2950
F 0 "#PWR093" H 7150 2700 50  0001 C CNN
F 1 "GND_L8" H 7150 2800 50  0001 C CNN
F 2 "" H 7150 2950 50  0000 C CNN
F 3 "" H 7150 2950 50  0000 C CNN
	1    7150 2950
	0    -1   -1   0   
$EndComp
Text GLabel 3350 3200 2    39   Input ~ 0
PROCESSOR_RXD0_AND_FTDI
Text GLabel 2350 3950 0    39   Output ~ 0
DI_RXD0
Wire Wire Line
	5850 3850 4950 3850
Wire Wire Line
	5100 4050 4950 4050
Text Notes 5900 1450 0    100  ~ 20
WiFi LAN Module
$Comp
L HUZZAH_ESP8266_Breakout MOD3001
U 1 1 5934A1F8
P 6500 3400
F 0 "MOD3001" H 6500 3400 61  0000 C CNB
F 1 "HUZZAH_ESP8266_Breakout" H 6500 4000 61  0000 C CNB
F 2 "Oddities:Adafruit_HUZZAH_ESP8266_Breakout" H 6500 3400 50  0001 C CNN
F 3 "" H 6500 3400 50  0000 C CNN
	1    6500 3400
	-1   0    0    1   
$EndComp
$Comp
L GND_L8 #PWR094
U 1 1 5934A2DC
P 5850 2950
F 0 "#PWR094" H 5850 2700 50  0001 C CNN
F 1 "GND_L8" H 5850 2800 50  0000 C CNN
F 2 "" H 5850 2950 50  0000 C CNN
F 3 "" H 5850 2950 50  0000 C CNN
	1    5850 2950
	0    1    -1   0   
$EndComp
$Comp
L GND_L8 #PWR095
U 1 1 5934A30B
P 6200 2700
F 0 "#PWR095" H 6200 2450 50  0001 C CNN
F 1 "GND_L8" H 6200 2550 50  0000 C CNN
F 2 "" H 6200 2700 50  0000 C CNN
F 3 "" H 6200 2700 50  0000 C CNN
	1    6200 2700
	0    1    -1   0   
$EndComp
$Comp
L CONN_01X06 P3001
U 1 1 5934A32A
P 6500 1750
F 0 "P3001" H 6500 2100 50  0000 C CNN
F 1 "Programming" V 6600 1750 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06" H 6500 1750 50  0001 C CNN
F 3 "" H 6500 1750 50  0000 C CNN
	1    6500 1750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6250 1950 6250 2750
Wire Wire Line
	6350 1950 6350 2750
Wire Wire Line
	6450 1950 6450 2450
Wire Wire Line
	6550 1950 6550 2750
Wire Wire Line
	6650 1950 6650 2750
Wire Wire Line
	6750 1950 6750 2750
Wire Wire Line
	6200 2700 6250 2700
Connection ~ 6250 2700
Text GLabel 7300 3850 2    39   Input ~ 0
~WIFI_RESET~
Wire Wire Line
	7300 3850 7150 3850
Wire Wire Line
	7150 3050 7750 3050
$Comp
L C_Small C3006
U 1 1 5937008D
P 7850 3050
F 0 "C3006" V 8000 2950 50  0000 L CNN
F 1 "10uF" V 7750 2950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 7850 3050 50  0001 C CNN
F 3 "" H 7850 3050 50  0000 C CNN
	1    7850 3050
	0    -1   -1   0   
$EndComp
$Comp
L GND_L8 #PWR096
U 1 1 593700CF
P 7950 3050
F 0 "#PWR096" H 7950 2800 50  0001 C CNN
F 1 "GND_L8" H 7950 2900 50  0000 C CNN
F 2 "" H 7950 3050 50  0000 C CNN
F 3 "" H 7950 3050 50  0000 C CNN
	1    7950 3050
	0    -1   -1   0   
$EndComp
Connection ~ 7650 3050
$Comp
L MIC5219 U3004
U 1 1 593705DA
P 8000 4600
F 0 "U3004" H 8000 4900 61  0000 C CNB
F 1 "MIC5219-3.3BM5" H 8000 4800 61  0000 C CNB
F 2 "TO_SOT_Packages_SMD:SOT-23-5_Handsolder" H 8000 4700 50  0001 C CIN
F 3 "" H 8000 4600 50  0000 C CNN
	1    8000 4600
	1    0    0    -1  
$EndComp
$Comp
L C_Small C3005
U 1 1 5937253B
P 7200 4650
F 0 "C3005" H 7210 4720 50  0000 L CNN
F 1 "2.2uF" H 7210 4570 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 7200 4650 50  0001 C CNN
F 3 "" H 7200 4650 50  0000 C CNN
	1    7200 4650
	1    0    0    -1  
$EndComp
$Comp
L R R3004
U 1 1 593725AE
P 7800 5000
F 0 "R3004" V 7880 5000 50  0000 C CNN
F 1 "10k" V 7800 5000 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 7730 5000 50  0001 C CNN
F 3 "" H 7800 5000 50  0000 C CNN
	1    7800 5000
	0    1    1    0   
$EndComp
$Comp
L GND_L8 #PWR097
U 1 1 5937261D
P 8000 5050
F 0 "#PWR097" H 8000 4800 50  0001 C CNN
F 1 "GND_L8" H 8000 4900 50  0000 C CNN
F 2 "" H 8000 5050 50  0000 C CNN
F 3 "" H 8000 5050 50  0000 C CNN
	1    8000 5050
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 4900 8000 5050
Wire Wire Line
	7950 5000 8000 5000
Connection ~ 8000 5000
Text GLabel 7400 5000 0    39   Input ~ 0
WIFI_ENABLE
Wire Wire Line
	7400 5000 7650 5000
Wire Wire Line
	7600 4700 7550 4700
Wire Wire Line
	7550 4700 7550 5000
Connection ~ 7550 5000
$Comp
L GND_L8 #PWR098
U 1 1 5937275D
P 7200 4750
F 0 "#PWR098" H 7200 4500 50  0001 C CNN
F 1 "GND_L8" H 7200 4600 50  0000 C CNN
F 2 "" H 7200 4750 50  0000 C CNN
F 3 "" H 7200 4750 50  0000 C CNN
	1    7200 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	7450 4350 7450 4550
Wire Wire Line
	7450 4550 7600 4550
Wire Wire Line
	7200 4550 7200 4450
Connection ~ 7200 4450
$Comp
L C_Small C3008
U 1 1 593731B3
P 8700 4700
F 0 "C3008" H 8710 4770 50  0000 L CNN
F 1 "2.2uF" H 8710 4620 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 8700 4700 50  0001 C CNN
F 3 "" H 8700 4700 50  0000 C CNN
	1    8700 4700
	1    0    0    -1  
$EndComp
$Comp
L C_Small C3007
U 1 1 5937320B
P 8500 4850
F 0 "C3007" H 8150 4850 50  0000 L CNN
F 1 "470pF" H 8200 4750 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 8500 4850 50  0001 C CNN
F 3 "" H 8500 4850 50  0000 C CNN
	1    8500 4850
	1    0    0    -1  
$EndComp
$Comp
L GND_L8 #PWR099
U 1 1 59373275
P 8500 4950
F 0 "#PWR099" H 8500 4700 50  0001 C CNN
F 1 "GND_L8" H 8500 4800 50  0000 C CNN
F 2 "" H 8500 4950 50  0000 C CNN
F 3 "" H 8500 4950 50  0000 C CNN
	1    8500 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	8400 4700 8500 4700
Wire Wire Line
	8500 4700 8500 4750
Wire Wire Line
	8700 3350 8700 4600
Wire Wire Line
	8700 4550 8400 4550
$Comp
L GND_L8 #PWR0100
U 1 1 59373340
P 8700 4800
F 0 "#PWR0100" H 8700 4550 50  0001 C CNN
F 1 "GND_L8" H 8700 4650 50  0000 C CNN
F 2 "" H 8700 4800 50  0000 C CNN
F 3 "" H 8700 4800 50  0000 C CNN
	1    8700 4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 3350 8700 3350
Connection ~ 8700 4550
Wire Wire Line
	7650 3350 7650 3050
NoConn ~ 7150 3150
NoConn ~ 7150 3650
NoConn ~ 5850 3150
NoConn ~ 5850 3050
$Comp
L TEST_1P W3001
U 1 1 5937356F
P 5850 3250
F 0 "W3001" V 5850 3520 50  0000 C CNN
F 1 "TEST_1P" H 5850 3450 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 6050 3250 50  0001 C CNN
F 3 "" H 6050 3250 50  0000 C CNN
	1    5850 3250
	0    -1   -1   0   
$EndComp
$Comp
L TEST_1P W3002
U 1 1 59373749
P 5850 3450
F 0 "W3002" V 5850 3720 50  0000 C CNN
F 1 "TEST_1P" H 5850 3650 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 6050 3450 50  0001 C CNN
F 3 "" H 6050 3450 50  0000 C CNN
	1    5850 3450
	0    -1   -1   0   
$EndComp
$Comp
L TEST_1P W3003
U 1 1 59373790
P 5850 3550
F 0 "W3003" V 5850 3820 50  0000 C CNN
F 1 "TEST_1P" H 5850 3750 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 6050 3550 50  0001 C CNN
F 3 "" H 6050 3550 50  0000 C CNN
	1    5850 3550
	0    -1   -1   0   
$EndComp
$Comp
L TEST_1P W3004
U 1 1 593737DA
P 5850 3650
F 0 "W3004" V 5850 3920 50  0000 C CNN
F 1 "TEST_1P" H 5850 3850 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 6050 3650 50  0001 C CNN
F 3 "" H 6050 3650 50  0000 C CNN
	1    5850 3650
	0    -1   -1   0   
$EndComp
$Comp
L TEST_1P W3005
U 1 1 59373903
P 7150 3250
F 0 "W3005" V 7150 3520 50  0000 C CNN
F 1 "TEST_1P" H 7150 3450 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 7350 3250 50  0001 C CNN
F 3 "" H 7350 3250 50  0000 C CNN
	1    7150 3250
	0    1    -1   0   
$EndComp
$Comp
L TEST_1P W3006
U 1 1 5937396E
P 7150 3350
F 0 "W3006" V 7150 3620 50  0000 C CNN
F 1 "TEST_1P" H 7150 3550 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 7350 3350 50  0001 C CNN
F 3 "" H 7350 3350 50  0000 C CNN
	1    7150 3350
	0    1    -1   0   
$EndComp
$Comp
L TEST_1P W3007
U 1 1 59373A17
P 7150 3550
F 0 "W3007" V 7150 3820 50  0000 C CNN
F 1 "TEST_1P" H 7150 3750 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 7350 3550 50  0001 C CNN
F 3 "" H 7350 3550 50  0000 C CNN
	1    7150 3550
	0    1    -1   0   
$EndComp
$Comp
L TEST_1P W3008
U 1 1 59373A72
P 7150 3750
F 0 "W3008" V 7150 4020 50  0000 C CNN
F 1 "TEST_1P" H 7150 3950 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 7350 3750 50  0001 C CNN
F 3 "" H 7350 3750 50  0000 C CNN
	1    7150 3750
	0    1    -1   0   
$EndComp
Text GLabel 5700 3750 0    39   Input ~ 0
DI_TXD0
Wire Wire Line
	5700 3750 5850 3750
Connection ~ 6450 2300
$Comp
L GND_L8 #PWR0111
U 1 1 59385A08
P 4550 4200
F 0 "#PWR0111" H 4550 3950 50  0001 C CNN
F 1 "GND_L8" H 4550 4050 50  0000 C CNN
F 2 "" H 4550 4200 50  0000 C CNN
F 3 "" H 4550 4200 50  0000 C CNN
	1    4550 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4550 4200 4550 4150
$Comp
L C_Small C3004
U 1 1 59385B03
P 4400 3650
F 0 "C3004" H 4410 3720 50  0000 L CNN
F 1 "100nF" H 4410 3570 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4400 3650 50  0001 C CNN
F 3 "" H 4400 3650 50  0000 C CNN
	1    4400 3650
	0    -1   -1   0   
$EndComp
$Comp
L +3V3_L8 #PWR0112
U 1 1 59385B09
P 4550 3600
F 0 "#PWR0112" H 4550 3450 50  0001 C CNN
F 1 "+3V3_L8" H 4550 3740 50  0000 C CNN
F 2 "" H 4550 3600 50  0000 C CNN
F 3 "" H 4550 3600 50  0000 C CNN
	1    4550 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4550 3600 4550 3750
Wire Wire Line
	4500 3650 4550 3650
Connection ~ 4550 3650
$Comp
L GND_L8 #PWR0113
U 1 1 59385B12
P 4300 3650
F 0 "#PWR0113" H 4300 3400 50  0001 C CNN
F 1 "GND_L8" H 4300 3500 50  0000 C CNN
F 2 "" H 4300 3650 50  0000 C CNN
F 3 "" H 4300 3650 50  0000 C CNN
	1    4300 3650
	0    1    1    0   
$EndComp
Text Notes 7100 7050 0    118  ~ 24
PCB: Dual-Band Transmitter (p. 4/4)
Text GLabel 7350 3450 2    39   Input ~ 0
DI_SCL
Text GLabel 5650 3350 0    39   Input ~ 0
DI_SDA
Text Notes 7500 2700 0    47   ~ 0
Remove pull-up resistor R7 on \nAdafruit HUZZAH ESP8266 \nbreakout board if needed to\ndisable I2C pull-up.
$Comp
L Jumper_NO_Small J3001
U 1 1 59470F01
P 5750 3350
F 0 "J3001" H 5750 3430 50  0000 C CNN
F 1 "Jumper_NO_Small" H 5760 3290 50  0001 C CNN
F 2 "Wire_Connections_Bridges:Solder-Jumper-NO-SMD-Pad_Small" H 5750 3350 50  0001 C CNN
F 3 "" H 5750 3350 50  0000 C CNN
	1    5750 3350
	1    0    0    -1  
$EndComp
$Comp
L Jumper_NO_Small J3002
U 1 1 59471115
P 7250 3450
F 0 "J3002" H 7250 3530 50  0000 C CNN
F 1 "Jumper_NO_Small" H 7260 3390 50  0001 C CNN
F 2 "Wire_Connections_Bridges:Solder-Jumper-NO-SMD-Pad_Small" H 7250 3450 50  0001 C CNN
F 3 "" H 7250 3450 50  0000 C CNN
	1    7250 3450
	1    0    0    -1  
$EndComp
Text GLabel 6850 2300 2    39   Input ~ 0
USB_+5V_DIRECT
Wire Wire Line
	6450 2300 6850 2300
$Comp
L PWR_FLAG #FLG0116
U 1 1 594E4FD1
P 7200 4450
F 0 "#FLG0116" H 7200 4545 50  0001 C CNN
F 1 "PWR_FLAG" H 7200 4630 50  0000 C CNN
F 2 "" H 7200 4450 50  0000 C CNN
F 3 "" H 7200 4450 50  0000 C CNN
	1    7200 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 3950 3200 3950
$Comp
L Jumper_NO_Small J3006
U 1 1 5A68998A
P 6450 2550
F 0 "J3006" H 6450 2630 50  0000 C CNN
F 1 "Jumper_NO_Small" H 6460 2490 50  0001 C CNN
F 2 "Wire_Connections_Bridges:Solder-Jumper-NO-SMD-Pad_Small" H 6450 2550 50  0001 C CNN
F 3 "" H 6450 2550 50  0000 C CNN
	1    6450 2550
	0    1    1    0   
$EndComp
Wire Wire Line
	6450 2650 6450 2750
Text GLabel 5100 4050 2    39   Input ~ 0
~WIFI_RESET~
$Comp
L PWR_FLAG #FLG0120
U 1 1 5A6CDD7C
P 7050 2650
F 0 "#FLG0120" H 7050 2745 50  0001 C CNN
F 1 "PWR_FLAG" H 7050 2830 50  0000 C CNN
F 2 "" H 7050 2650 50  0000 C CNN
F 3 "" H 7050 2650 50  0000 C CNN
	1    7050 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	7050 2650 7050 2700
Wire Wire Line
	7050 2700 6450 2700
Connection ~ 6450 2700
$Comp
L Jumper_NC_Small J?
U 1 1 5BCC4FFE
P 3100 3950
F 0 "J?" H 3100 4030 50  0000 C CNN
F 1 "Jumper_NC_Small" H 3110 3890 50  0001 C CNN
F 2 "Wire_Connections_Bridges:Solder-Jumper-NC-SMD-Pad_Small" H 3100 3950 50  0001 C CNN
F 3 "" H 3100 3950 50  0000 C CNN
	1    3100 3950
	1    0    0    -1  
$EndComp
Text Notes 3700 4800 0    59   ~ 0
Resetting the WiFi module must also disable\nWiFi UART communication with the processor.\nThis allows the processor to block powerup\ngarbage sent by the WiFi board.
Wire Wire Line
	2350 3950 3000 3950
$Comp
L Jumper_NO_Small J?
U 1 1 5BCB5E41
P 3100 3600
F 0 "J?" H 3100 3680 50  0000 C CNN
F 1 "Jumper_NO_Small" H 3110 3540 50  0001 C CNN
F 2 "" H 3100 3600 50  0000 C CNN
F 3 "" H 3100 3600 50  0000 C CNN
	1    3100 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 3600 2650 3600
Wire Wire Line
	2650 3600 2650 3950
Connection ~ 2650 3950
Wire Wire Line
	3200 3600 3300 3600
Wire Wire Line
	3300 3600 3300 3200
Wire Wire Line
	3300 3200 3350 3200
$Comp
L 74HC1G08 U?
U 1 1 5BCB6B97
P 4350 3950
F 0 "U?" H 4350 4000 61  0000 C CNB
F 1 "74HC1G08" H 4350 3900 61  0000 C CNB
F 2 "TO_SOT_Packages_SMD:SOT-353_Handsolder" H 4350 3950 50  0001 C CNN
F 3 "http://www.onsemi.com/pub/Collateral/MC74HC1G08-D.PDF" H 4350 3950 50  0001 C CNN
	1    4350 3950
	-1   0    0    -1  
$EndComp
$Comp
L +12V #PWR?
U 1 1 5BCCB047
P 7450 4350
F 0 "#PWR?" H 7450 4200 50  0001 C CNN
F 1 "+12V" H 7450 4490 50  0000 C CNN
F 2 "" H 7450 4350 50  0000 C CNN
F 3 "" H 7450 4350 50  0000 C CNN
	1    7450 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	7200 4450 7450 4450
Connection ~ 7450 4450
$EndSCHEMATC
