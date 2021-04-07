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
LIBS:special
LIBS:ARDF 2 Band Transmitter-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 5 5
Title "WiFi Circuitry"
Date ""
Rev "X.1.2"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L GND_L8 #PWR0214
U 1 1 592F6B93
P 8200 3850
F 0 "#PWR0214" H 8200 3600 50  0001 C CNN
F 1 "GND_L8" H 8200 3700 50  0001 C CNN
F 2 "" H 8200 3850 50  0000 C CNN
F 3 "" H 8200 3850 50  0000 C CNN
	1    8200 3850
	0    -1   -1   0   
$EndComp
Text GLabel 3650 3900 2    39   Input ~ 0
PROCESSOR_RXD0_AND1
Wire Wire Line
	4800 4300 4650 4300
Text GLabel 3450 4450 0    39   Output ~ 0
PROCESSOR_RXD0
$Comp
L 74LS21 U501
U 1 1 592FF2EE
P 4050 4450
F 0 "U501" H 4050 4550 50  0000 C CNN
F 1 "74LS21" H 4050 4350 50  0000 C CNN
F 2 "" H 4050 4450 50  0001 C CNN
F 3 "" H 4050 4450 50  0000 C CNN
	1    4050 4450
	-1   0    0    -1  
$EndComp
Text GLabel 3800 2500 2    39   Input ~ 0
PROCESSOR_RXD0_AND3
Wire Wire Line
	4850 4400 4650 4400
$Comp
L 74LS32 U502
U 3 1 593040AB
P 5400 4850
F 0 "U502" H 5400 4900 50  0000 C CNN
F 1 "74LS32" H 5400 4800 50  0000 C CNN
F 2 "" H 5400 4850 50  0001 C CNN
F 3 "" H 5400 4850 50  0000 C CNN
	3    5400 4850
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6900 4750 6000 4750
Wire Wire Line
	4800 4850 4800 4600
Wire Wire Line
	4800 4600 4650 4600
Wire Wire Line
	4650 4500 5100 4500
Wire Wire Line
	5950 5800 6150 5800
Wire Wire Line
	6150 5800 6150 4950
Wire Wire Line
	6150 4950 6000 4950
Text Notes 3250 3000 0    100  ~ 20
UART-SHARING LOGIC
Text Notes 6950 2700 0    100  ~ 20
WiFi LAN Module
$Comp
L HUZZAH_ESP8266_Breakout MOD501
U 1 1 5934A1F8
P 7550 4300
F 0 "MOD501" H 7550 4300 61  0000 C CNB
F 1 "HUZZAH_ESP8266_Breakout" H 7550 4900 61  0000 C CNB
F 2 "" H 7550 4300 50  0001 C CNN
F 3 "" H 7550 4300 50  0000 C CNN
	1    7550 4300
	-1   0    0    1   
$EndComp
$Comp
L GND_L8 #PWR0215
U 1 1 5934A2DC
P 6900 3850
F 0 "#PWR0215" H 6900 3600 50  0001 C CNN
F 1 "GND_L8" H 6900 3700 50  0000 C CNN
F 2 "" H 6900 3850 50  0000 C CNN
F 3 "" H 6900 3850 50  0000 C CNN
	1    6900 3850
	0    1    -1   0   
$EndComp
$Comp
L GND_L8 #PWR0216
U 1 1 5934A30B
P 7250 3600
F 0 "#PWR0216" H 7250 3350 50  0001 C CNN
F 1 "GND_L8" H 7250 3450 50  0000 C CNN
F 2 "" H 7250 3600 50  0000 C CNN
F 3 "" H 7250 3600 50  0000 C CNN
	1    7250 3600
	0    1    -1   0   
$EndComp
$Comp
L CONN_01X06 P501
U 1 1 5934A32A
P 7550 2950
F 0 "P501" H 7550 3300 50  0000 C CNN
F 1 "CONN_01X06" V 7650 2950 50  0000 C CNN
F 2 "" H 7550 2950 50  0001 C CNN
F 3 "" H 7550 2950 50  0000 C CNN
	1    7550 2950
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7300 3150 7300 3650
Wire Wire Line
	7400 3150 7400 3650
Wire Wire Line
	7500 3150 7500 3650
Wire Wire Line
	7600 3150 7600 3650
Wire Wire Line
	7700 3150 7700 3650
Wire Wire Line
	7800 3150 7800 3650
Wire Wire Line
	7250 3600 7300 3600
Connection ~ 7300 3600
Text GLabel 8350 4750 2    39   Input ~ 0
WIFI_RESET
Wire Wire Line
	8350 4750 8200 4750
Wire Wire Line
	8200 3950 8800 3950
$Comp
L C_Small C506
U 1 1 5937008D
P 8900 3950
F 0 "C506" H 8910 4020 50  0000 L CNN
F 1 "10uF" H 8910 3870 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 8900 3950 50  0001 C CNN
F 3 "" H 8900 3950 50  0000 C CNN
	1    8900 3950
	0    -1   -1   0   
$EndComp
$Comp
L GND_L8 #PWR0217
U 1 1 593700CF
P 9000 3950
F 0 "#PWR0217" H 9000 3700 50  0001 C CNN
F 1 "GND_L8" H 9000 3800 50  0000 C CNN
F 2 "" H 9000 3950 50  0000 C CNN
F 3 "" H 9000 3950 50  0000 C CNN
	1    9000 3950
	0    -1   -1   0   
$EndComp
Connection ~ 8700 3950
$Comp
L MIC5219 U504
U 1 1 593705DA
P 9050 5500
F 0 "U504" H 9050 5800 61  0000 C CNB
F 1 "MIC5219-3.3BM5" H 9050 5700 61  0000 C CNB
F 2 "TO_SOT_Packages_SMD:SOT-23-5" H 9050 5600 50  0001 C CIN
F 3 "" H 9050 5500 50  0000 C CNN
	1    9050 5500
	1    0    0    -1  
$EndComp
$Comp
L C_Small C505
U 1 1 5937253B
P 8250 5550
F 0 "C505" H 8260 5620 50  0000 L CNN
F 1 "2.2uF" H 8260 5470 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 8250 5550 50  0001 C CNN
F 3 "" H 8250 5550 50  0000 C CNN
	1    8250 5550
	1    0    0    -1  
$EndComp
$Comp
L R R504
U 1 1 593725AE
P 8850 5900
F 0 "R504" V 8930 5900 50  0000 C CNN
F 1 "10k" V 8850 5900 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8780 5900 50  0001 C CNN
F 3 "" H 8850 5900 50  0000 C CNN
	1    8850 5900
	0    1    1    0   
$EndComp
$Comp
L GND_L8 #PWR0218
U 1 1 5937261D
P 9050 5950
F 0 "#PWR0218" H 9050 5700 50  0001 C CNN
F 1 "GND_L8" H 9050 5800 50  0000 C CNN
F 2 "" H 9050 5950 50  0000 C CNN
F 3 "" H 9050 5950 50  0000 C CNN
	1    9050 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 5800 9050 5950
Wire Wire Line
	9000 5900 9050 5900
Connection ~ 9050 5900
Text GLabel 8450 5900 0    39   Input ~ 0
WIFI_ENABLE
Wire Wire Line
	8450 5900 8700 5900
Wire Wire Line
	8650 5600 8600 5600
Wire Wire Line
	8600 5600 8600 5900
Connection ~ 8600 5900
$Comp
L GND_L8 #PWR0219
U 1 1 5937275D
P 8250 5650
F 0 "#PWR0219" H 8250 5400 50  0001 C CNN
F 1 "GND_L8" H 8250 5500 50  0000 C CNN
F 2 "" H 8250 5650 50  0000 C CNN
F 3 "" H 8250 5650 50  0000 C CNN
	1    8250 5650
	1    0    0    -1  
$EndComp
Text GLabel 8100 5350 0    39   Input ~ 0
VBAT_UNINTERRUPTED
Wire Wire Line
	8100 5350 8500 5350
Wire Wire Line
	8500 5350 8500 5450
Wire Wire Line
	8500 5450 8650 5450
Wire Wire Line
	8250 5450 8250 5350
Connection ~ 8250 5350
$Comp
L C_Small C508
U 1 1 593731B3
P 9750 5600
F 0 "C508" H 9760 5670 50  0000 L CNN
F 1 "2.2uF" H 9760 5520 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 9750 5600 50  0001 C CNN
F 3 "" H 9750 5600 50  0000 C CNN
	1    9750 5600
	1    0    0    -1  
$EndComp
$Comp
L C_Small C507
U 1 1 5937320B
P 9550 5750
F 0 "C507" H 9560 5820 50  0000 L CNN
F 1 "470pF" H 9560 5670 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 9550 5750 50  0001 C CNN
F 3 "" H 9550 5750 50  0000 C CNN
	1    9550 5750
	1    0    0    -1  
$EndComp
$Comp
L GND_L8 #PWR0220
U 1 1 59373275
P 9550 5850
F 0 "#PWR0220" H 9550 5600 50  0001 C CNN
F 1 "GND_L8" H 9550 5700 50  0000 C CNN
F 2 "" H 9550 5850 50  0000 C CNN
F 3 "" H 9550 5850 50  0000 C CNN
	1    9550 5850
	1    0    0    -1  
$EndComp
Wire Wire Line
	9450 5600 9550 5600
Wire Wire Line
	9550 5600 9550 5650
Wire Wire Line
	9750 4250 9750 5500
Wire Wire Line
	9750 5450 9450 5450
$Comp
L GND_L8 #PWR0221
U 1 1 59373340
P 9750 5700
F 0 "#PWR0221" H 9750 5450 50  0001 C CNN
F 1 "GND_L8" H 9750 5550 50  0000 C CNN
F 2 "" H 9750 5700 50  0000 C CNN
F 3 "" H 9750 5700 50  0000 C CNN
	1    9750 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	8700 4250 9750 4250
Connection ~ 9750 5450
Wire Wire Line
	8700 4250 8700 3950
NoConn ~ 8200 4050
NoConn ~ 8200 4550
NoConn ~ 6900 4050
NoConn ~ 6900 3950
$Comp
L TEST_1P W501
U 1 1 5937356F
P 6900 4150
F 0 "W501" V 6900 4420 50  0000 C CNN
F 1 "TEST_1P" H 6900 4350 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 7100 4150 50  0001 C CNN
F 3 "" H 7100 4150 50  0000 C CNN
	1    6900 4150
	0    -1   -1   0   
$EndComp
$Comp
L TEST_1P W502
U 1 1 59373749
P 6900 4350
F 0 "W502" V 6900 4620 50  0000 C CNN
F 1 "TEST_1P" H 6900 4550 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 7100 4350 50  0001 C CNN
F 3 "" H 7100 4350 50  0000 C CNN
	1    6900 4350
	0    -1   -1   0   
$EndComp
$Comp
L TEST_1P W503
U 1 1 59373790
P 6900 4450
F 0 "W503" V 6900 4720 50  0000 C CNN
F 1 "TEST_1P" H 6900 4650 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 7100 4450 50  0001 C CNN
F 3 "" H 7100 4450 50  0000 C CNN
	1    6900 4450
	0    -1   -1   0   
$EndComp
$Comp
L TEST_1P W504
U 1 1 593737DA
P 6900 4550
F 0 "W504" V 6900 4820 50  0000 C CNN
F 1 "TEST_1P" H 6900 4750 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 7100 4550 50  0001 C CNN
F 3 "" H 7100 4550 50  0000 C CNN
	1    6900 4550
	0    -1   -1   0   
$EndComp
$Comp
L TEST_1P W505
U 1 1 59373903
P 8200 4150
F 0 "W505" V 8200 4420 50  0000 C CNN
F 1 "TEST_1P" H 8200 4350 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 8400 4150 50  0001 C CNN
F 3 "" H 8400 4150 50  0000 C CNN
	1    8200 4150
	0    1    -1   0   
$EndComp
$Comp
L TEST_1P W506
U 1 1 5937396E
P 8200 4250
F 0 "W506" V 8200 4520 50  0000 C CNN
F 1 "TEST_1P" H 8200 4450 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 8400 4250 50  0001 C CNN
F 3 "" H 8400 4250 50  0000 C CNN
	1    8200 4250
	0    1    -1   0   
$EndComp
$Comp
L TEST_1P W507
U 1 1 59373A17
P 8200 4450
F 0 "W507" V 8200 4720 50  0000 C CNN
F 1 "TEST_1P" H 8200 4650 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 8400 4450 50  0001 C CNN
F 3 "" H 8400 4450 50  0000 C CNN
	1    8200 4450
	0    1    -1   0   
$EndComp
$Comp
L TEST_1P W508
U 1 1 59373A72
P 8200 4650
F 0 "W508" V 8200 4920 50  0000 C CNN
F 1 "TEST_1P" H 8200 4850 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 8400 4650 50  0001 C CNN
F 3 "" H 8400 4650 50  0000 C CNN
	1    8200 4650
	0    1    -1   0   
$EndComp
Text GLabel 6750 4650 0    39   Input ~ 0
PROCESSOR_TXD0
Wire Wire Line
	6750 4650 6900 4650
Text GLabel 4950 5800 0    39   Input ~ 0
WIFI_ENABLE
$Comp
L TC7W04 U503
U 1 1 593767D3
P 5550 5800
F 0 "U503" H 5700 5900 50  0000 C CNN
F 1 "TC7W04" H 5750 5700 50  0000 C CNN
F 2 "" H 5550 5800 50  0001 C CNN
F 3 "" H 5550 5800 50  0000 C CNN
	1    5550 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4950 5800 5200 5800
Wire Wire Line
	4850 4400 4850 4500
Connection ~ 4850 4500
Connection ~ 7500 3550
Text GLabel 2450 3200 0    39   Input ~ 0
FTDI_CABLE_+5V
$Comp
L DMP4025LSD Q205
U 2 1 5937B9BA
P 2850 3250
F 0 "Q205" H 3100 3325 50  0000 L CNN
F 1 "DMP4025LSD" H 3100 3250 50  0000 L CNN
F 2 "Power_Integrations:SO-8" H 3100 3175 50  0001 L CIN
F 3 "" H 2850 3250 50  0000 L CNN
	2    2850 3250
	1    0    0    1   
$EndComp
$Comp
L GND_L8 #PWR0222
U 1 1 5937B9C6
P 2950 3850
F 0 "#PWR0222" H 2950 3600 50  0001 C CNN
F 1 "GND_L8" H 2950 3700 50  0000 C CNN
F 2 "" H 2950 3850 50  0000 C CNN
F 3 "" H 2950 3850 50  0000 C CNN
	1    2950 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 3450 2950 3550
Wire Wire Line
	3050 3450 3050 3500
Wire Wire Line
	2950 3500 3600 3500
Connection ~ 2950 3500
$Comp
L R R502
U 1 1 5937B9D0
P 2950 3700
F 0 "R502" V 3030 3700 50  0000 C CNN
F 1 "10k" V 2950 3700 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 2880 3700 50  0001 C CNN
F 3 "" H 2950 3700 50  0000 C CNN
	1    2950 3700
	1    0    0    -1  
$EndComp
$Comp
L R R501
U 1 1 5937B9D6
P 2550 3450
F 0 "R501" V 2630 3450 50  0000 C CNN
F 1 "10k" V 2550 3450 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 2480 3450 50  0001 C CNN
F 3 "" H 2550 3450 50  0000 C CNN
	1    2550 3450
	1    0    0    -1  
$EndComp
$Comp
L GND_L8 #PWR0223
U 1 1 5937B9DC
P 2550 3600
F 0 "#PWR0223" H 2550 3350 50  0001 C CNN
F 1 "GND_L8" H 2550 3450 50  0000 C CNN
F 2 "" H 2550 3600 50  0000 C CNN
F 3 "" H 2550 3600 50  0000 C CNN
	1    2550 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 3200 2650 3200
Wire Wire Line
	2550 3300 2550 3200
Connection ~ 2550 3200
Wire Wire Line
	2950 3050 2950 2900
Connection ~ 3050 3500
$Comp
L 74LS32 U502
U 1 1 5937BA1F
P 4200 3600
F 0 "U502" H 4200 3650 50  0000 C CNN
F 1 "74LS32" H 4200 3550 50  0000 C CNN
F 2 "" H 4200 3600 50  0001 C CNN
F 3 "" H 4200 3600 50  0000 C CNN
	1    4200 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 3900 3450 3900
Wire Wire Line
	3450 3900 3450 3700
Wire Wire Line
	3450 3700 3600 3700
Wire Wire Line
	4800 3600 4800 3600
Wire Wire Line
	4800 3600 4800 4300
Wire Wire Line
	5100 4500 5100 2150
Wire Wire Line
	5100 2150 5000 2150
Wire Wire Line
	3800 2500 3650 2500
Wire Wire Line
	3650 2250 3800 2250
Wire Wire Line
	3650 2500 3650 2250
$Comp
L 74LS32 U502
U 2 1 5930D7C7
P 4400 2150
F 0 "U502" H 4400 2200 50  0000 C CNN
F 1 "74LS32" H 4400 2100 50  0000 C CNN
F 2 "" H 4400 2150 50  0001 C CNN
F 3 "" H 4400 2150 50  0000 C CNN
	2    4400 2150
	1    0    0    -1  
$EndComp
$Comp
L +3V3_L8 #PWR0224
U 1 1 5937B9C0
P 2950 2900
F 0 "#PWR0224" H 2950 2750 50  0001 C CNN
F 1 "+3V3_L8" H 2950 3040 50  0000 C CNN
F 2 "" H 2950 2900 50  0000 C CNN
F 3 "" H 2950 2900 50  0000 C CNN
	1    2950 2900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 1650 3150 1550
Text GLabel 2600 2300 0    39   Input ~ 0
CLONE_PWR_IN
$Comp
L R R503
U 1 1 5930D52E
P 3150 1800
F 0 "R503" V 3230 1800 50  0000 C CNN
F 1 "10k" V 3150 1800 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 3080 1800 50  0001 C CNN
F 3 "" H 3150 1800 50  0000 C CNN
	1    3150 1800
	1    0    0    -1  
$EndComp
$Comp
L GND_L8 #PWR0225
U 1 1 5930D445
P 3150 2500
F 0 "#PWR0225" H 3150 2250 50  0001 C CNN
F 1 "GND_L8" H 3150 2350 50  0000 C CNN
F 2 "" H 3150 2500 50  0000 C CNN
F 3 "" H 3150 2500 50  0000 C CNN
	1    3150 2500
	1    0    0    -1  
$EndComp
$Comp
L +3V3_L8 #PWR0226
U 1 1 5930D40B
P 3150 1550
F 0 "#PWR0226" H 3150 1400 50  0001 C CNN
F 1 "+3V3_L8" H 3150 1690 50  0000 C CNN
F 2 "" H 3150 1550 50  0000 C CNN
F 3 "" H 3150 1550 50  0000 C CNN
	1    3150 1550
	1    0    0    -1  
$EndComp
$Comp
L MUN5211DW1T1G Q501
U 1 1 59382BAC
P 3050 2300
F 0 "Q501" H 3250 2375 50  0000 L CNN
F 1 "MUN5211DW1T1G" H 3250 2300 50  0000 L CNN
F 2 "" H 3050 2300 50  0001 L CNN
F 3 "" H 3050 2300 50  0000 L CNN
	1    3050 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 1950 3150 2100
Wire Wire Line
	3150 2050 3800 2050
Connection ~ 3150 2050
Wire Wire Line
	2600 2300 2800 2300
$Comp
L C_Small C501
U 1 1 59383976
P 2700 2100
F 0 "C501" H 2710 2170 50  0000 L CNN
F 1 "100nF" H 2710 2020 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2700 2100 50  0001 C CNN
F 3 "" H 2700 2100 50  0000 C CNN
	1    2700 2100
	1    0    0    -1  
$EndComp
$Comp
L GND_L8 #PWR0227
U 1 1 59383A01
P 2700 2000
F 0 "#PWR0227" H 2700 1750 50  0001 C CNN
F 1 "GND_L8" H 2700 1850 50  0000 C CNN
F 2 "" H 2700 2000 50  0000 C CNN
F 3 "" H 2700 2000 50  0000 C CNN
	1    2700 2000
	1    0    0    1   
$EndComp
Wire Wire Line
	2700 2200 2700 2300
Connection ~ 2700 2300
$Comp
L GND_L8 #PWR0228
U 1 1 59384D16
P 4200 4650
F 0 "#PWR0228" H 4200 4400 50  0001 C CNN
F 1 "GND_L8" H 4200 4500 50  0000 C CNN
F 2 "" H 4200 4650 50  0000 C CNN
F 3 "" H 4200 4650 50  0000 C CNN
	1    4200 4650
	1    0    0    -1  
$EndComp
$Comp
L +3V3_L8 #PWR0229
U 1 1 59384DCF
P 4200 4100
F 0 "#PWR0229" H 4200 3950 50  0001 C CNN
F 1 "+3V3_L8" H 4200 4240 50  0000 C CNN
F 2 "" H 4200 4100 50  0000 C CNN
F 3 "" H 4200 4100 50  0000 C CNN
	1    4200 4100
	1    0    0    -1  
$EndComp
$Comp
L C_Small C502
U 1 1 59384EDF
P 4350 4150
F 0 "C502" H 4360 4220 50  0000 L CNN
F 1 "100nF" H 4360 4070 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4350 4150 50  0001 C CNN
F 3 "" H 4350 4150 50  0000 C CNN
	1    4350 4150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4200 4100 4200 4250
Wire Wire Line
	4200 4150 4250 4150
Connection ~ 4200 4150
$Comp
L GND_L8 #PWR0230
U 1 1 59385141
P 4450 4150
F 0 "#PWR0230" H 4450 3900 50  0001 C CNN
F 1 "GND_L8" H 4450 4000 50  0000 C CNN
F 2 "" H 4450 4150 50  0000 C CNN
F 3 "" H 4450 4150 50  0000 C CNN
	1    4450 4150
	0    -1   -1   0   
$EndComp
$Comp
L C_Small C503
U 1 1 593853C6
P 5300 5600
F 0 "C503" H 5310 5670 50  0000 L CNN
F 1 "100nF" H 5310 5520 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5300 5600 50  0001 C CNN
F 3 "" H 5300 5600 50  0000 C CNN
	1    5300 5600
	0    -1   -1   0   
$EndComp
$Comp
L +3V3_L8 #PWR0231
U 1 1 59385442
P 5450 5550
F 0 "#PWR0231" H 5450 5400 50  0001 C CNN
F 1 "+3V3_L8" H 5450 5690 50  0000 C CNN
F 2 "" H 5450 5550 50  0000 C CNN
F 3 "" H 5450 5550 50  0000 C CNN
	1    5450 5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 5550 5450 5700
Wire Wire Line
	5400 5600 5450 5600
Connection ~ 5450 5600
$Comp
L GND_L8 #PWR0232
U 1 1 593855FF
P 5200 5600
F 0 "#PWR0232" H 5200 5350 50  0001 C CNN
F 1 "GND_L8" H 5200 5450 50  0000 C CNN
F 2 "" H 5200 5600 50  0000 C CNN
F 3 "" H 5200 5600 50  0000 C CNN
	1    5200 5600
	0    1    1    0   
$EndComp
$Comp
L GND_L8 #PWR0233
U 1 1 59385667
P 5450 6000
F 0 "#PWR0233" H 5450 5750 50  0001 C CNN
F 1 "GND_L8" H 5450 5850 50  0000 C CNN
F 2 "" H 5450 6000 50  0000 C CNN
F 3 "" H 5450 6000 50  0000 C CNN
	1    5450 6000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 6000 5450 5900
$Comp
L GND_L8 #PWR0234
U 1 1 59385A08
P 5600 5100
F 0 "#PWR0234" H 5600 4850 50  0001 C CNN
F 1 "GND_L8" H 5600 4950 50  0000 C CNN
F 2 "" H 5600 5100 50  0000 C CNN
F 3 "" H 5600 5100 50  0000 C CNN
	1    5600 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 5100 5600 5050
$Comp
L C_Small C504
U 1 1 59385B03
P 5450 4550
F 0 "C504" H 5460 4620 50  0000 L CNN
F 1 "100nF" H 5460 4470 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5450 4550 50  0001 C CNN
F 3 "" H 5450 4550 50  0000 C CNN
	1    5450 4550
	0    -1   -1   0   
$EndComp
$Comp
L +3V3_L8 #PWR0235
U 1 1 59385B09
P 5600 4500
F 0 "#PWR0235" H 5600 4350 50  0001 C CNN
F 1 "+3V3_L8" H 5600 4640 50  0000 C CNN
F 2 "" H 5600 4500 50  0000 C CNN
F 3 "" H 5600 4500 50  0000 C CNN
	1    5600 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 4500 5600 4650
Wire Wire Line
	5550 4550 5600 4550
Connection ~ 5600 4550
$Comp
L GND_L8 #PWR0236
U 1 1 59385B12
P 5350 4550
F 0 "#PWR0236" H 5350 4300 50  0001 C CNN
F 1 "GND_L8" H 5350 4400 50  0000 C CNN
F 2 "" H 5350 4550 50  0000 C CNN
F 3 "" H 5350 4550 50  0000 C CNN
	1    5350 4550
	0    1    1    0   
$EndComp
Text GLabel 3800 3200 2    39   Output ~ 0
~FTDI_Present~
Wire Wire Line
	3800 3200 3500 3200
Wire Wire Line
	3500 3200 3500 3500
Connection ~ 3500 3500
Text GLabel 3850 1750 2    39   Output ~ 0
~Clone_Present~
Wire Wire Line
	3850 1750 3550 1750
Wire Wire Line
	3550 1750 3550 2050
Connection ~ 3550 2050
Text Notes 7100 7050 0    118  ~ 24
PCB: Dual-Band Transmitter Board (p. 5/5)
Text GLabel 8400 4350 2    39   Input ~ 0
TX_SCL
Text GLabel 6700 4250 0    39   Input ~ 0
TX_SDA
Text Notes 8550 3600 0    47   ~ 0
Remove pull-up resistor R7 on \nAdafruit HUZZAH ESP8266 \nbreakout board, otherwise \nI2C will probably not work.
$Comp
L Jumper_NO_Small J501
U 1 1 59470F01
P 6800 4250
F 0 "J501" H 6800 4330 50  0000 C CNN
F 1 "Jumper_NO_Small" H 6810 4190 50  0001 C CNN
F 2 "Wire_Connections_Bridges:Solder-Jumper-NO-SMD-Pad_Small" H 6800 4250 50  0001 C CNN
F 3 "" H 6800 4250 50  0000 C CNN
	1    6800 4250
	1    0    0    -1  
$EndComp
$Comp
L Jumper_NO_Small J502
U 1 1 59471115
P 8300 4350
F 0 "J502" H 8300 4430 50  0000 C CNN
F 1 "Jumper_NO_Small" H 8310 4290 50  0001 C CNN
F 2 "Wire_Connections_Bridges:Solder-Jumper-NO-SMD-Pad_Small" H 8300 4350 50  0001 C CNN
F 3 "" H 8300 4350 50  0000 C CNN
	1    8300 4350
	1    0    0    -1  
$EndComp
Text GLabel 6900 3350 0    39   Output ~ 0
HUZZAH_TXD0
Text GLabel 6900 3450 0    39   Input ~ 0
HUZZAH_RXD0
Wire Wire Line
	6900 3350 7700 3350
Connection ~ 7700 3350
Wire Wire Line
	6900 3450 7600 3450
Connection ~ 7600 3450
Text GLabel 7850 3550 2    39   Input ~ 0
USB_+5V_SWITCHED
Wire Wire Line
	7850 3550 7500 3550
$EndSCHEMATC
