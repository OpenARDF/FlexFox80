EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 4
Title "ARDF 2-Band Mini Transmitter"
Date "2020-10-16"
Rev "P2.1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 5500 3950 0    79   ~ 16
VHF Final\nAmplifier
Text Notes 7600 2850 0    79   ~ 16
VHF LPF
Text Notes 2600 6950 0    79   ~ 16
FET Driver
Text Notes 4800 6600 0    79   ~ 16
HF Final\nAmplifier
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR113
U 1 1 589A12B4
P 2500 3700
F 0 "#PWR113" H 2500 3450 50  0001 C CNN
F 1 "GND_L8" H 2500 3550 50  0000 C CNN
F 2 "" H 2500 3700 50  0000 C CNN
F 3 "" H 2500 3700 50  0000 C CNN
	1    2500 3700
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C110
U 1 1 589A3A05
P 2150 2850
F 0 "C110" V 2000 2750 50  0000 L CNN
F 1 "10nF" V 2100 2650 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2150 2850 50  0001 C CNN
F 3 "" H 2150 2850 50  0000 C CNN
F 4 "311-1136-1-ND" H 2150 2850 60  0001 C CNN "Part No."
F 5 "<a href=\"http://www.digikey.com/products/en?keywords=311-1136-1-ND\">Link</a>" H 2150 2850 60  0001 C CNN "Link"
	1    2150 2850
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR110
U 1 1 589A3C2E
P 2250 2850
F 0 "#PWR110" H 2250 2600 50  0001 C CNN
F 1 "GND_L8" H 2250 2700 50  0001 C CNN
F 2 "" H 2250 2850 50  0000 C CNN
F 3 "" H 2250 2850 50  0000 C CNN
	1    2250 2850
	0    -1   -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:MIC5219 U110
U 1 1 589BA720
P 9850 1150
F 0 "U110" H 9850 1450 61  0000 C CNB
F 1 "MIC5219-3.3BM5" H 9850 1350 61  0000 C CNB
F 2 "TO_SOT_Packages_SMD:SOT-23-5_Handsolder" H 9850 1250 50  0001 C CIN
F 3 "" H 9850 1150 50  0000 C CNN
F 4 "576-1281-1-ND" H 9850 1150 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=576-1281-1-ND\">Link</a>" H 9850 1150 60  0001 C CNN "Link"
	1    9850 1150
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR167
U 1 1 589BA74D
P 9850 1500
F 0 "#PWR167" H 9850 1250 50  0001 C CNN
F 1 "GND_L8" H 9850 1350 50  0000 C CNN
F 2 "" H 9850 1500 50  0000 C CNN
F 3 "" H 9850 1500 50  0000 C CNN
	1    9850 1500
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR171
U 1 1 589BA759
P 10600 1400
F 0 "#PWR171" H 10600 1150 50  0001 C CNN
F 1 "GND_L8" H 10600 1250 50  0000 C CNN
F 2 "" H 10600 1400 50  0000 C CNN
F 3 "" H 10600 1400 50  0000 C CNN
	1    10600 1400
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:+3V3_L8 #PWR170
U 1 1 589BAB75
P 10600 850
F 0 "#PWR170" H 10600 700 50  0001 C CNN
F 1 "+3V3_L8" H 10600 990 50  0000 C CNN
F 2 "" H 10600 850 50  0000 C CNN
F 3 "" H 10600 850 50  0000 C CNN
	1    10600 850 
	1    0    0    -1  
$EndComp
Text Notes 7100 6950 0    118  ~ 24
PCB: Dual-Band Transmitter (p. 1/3)
$Comp
L Device:C_Small C109
U 1 1 589DDB66
P 2050 3400
F 0 "C109" V 2150 3250 50  0000 L CNN
F 1 "1nF" V 1950 3250 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2050 3400 50  0001 C CNN
F 3 "" H 2050 3400 50  0000 C CNN
F 4 "490-6445-1-ND" H 2050 3400 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=490-6445-1-ND\">Link</a>" H 2050 3400 60  0001 C CNN "Link"
	1    2050 3400
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C135
U 1 1 589E4FAF
P 10600 1250
F 0 "C135" H 10350 1200 50  0000 L CNN
F 1 "2.2uF" H 10350 1350 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 10600 1250 50  0001 C CNN
F 3 "" H 10600 1250 50  0000 C CNN
F 4 "1276-1763-1-ND" H 10600 1250 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-1763-1-ND\">Link</a>" H 10600 1250 60  0001 C CNN "Link"
	1    10600 1250
	-1   0    0    1   
$EndComp
$Comp
L Device:C_Small C133
U 1 1 589E50CA
P 9250 1100
F 0 "C133" V 9350 1000 50  0000 L CNN
F 1 "2.2uF" V 9150 850 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 9250 1100 50  0001 C CNN
F 3 "" H 9250 1100 50  0000 C CNN
F 4 "1276-1763-1-ND" H 9250 1100 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-1763-1-ND\">Link</a>" H 9250 1100 60  0001 C CNN "Link"
	1    9250 1100
	0    -1   -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR130
U 1 1 58A2833C
P 4050 5200
F 0 "#PWR130" H 4050 4950 50  0001 C CNN
F 1 "GND_L8" H 4050 5050 50  0001 C CNN
F 2 "" H 4050 5200 50  0000 C CNN
F 3 "" H 4050 5200 50  0000 C CNN
	1    4050 5200
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR134
U 1 1 58A28342
P 4650 5200
F 0 "#PWR134" H 4650 4950 50  0001 C CNN
F 1 "GND_L8" H 4650 5050 50  0000 C CNN
F 2 "" H 4650 5200 50  0000 C CNN
F 3 "" H 4650 5200 50  0000 C CNN
	1    4650 5200
	0    -1   -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR150
U 1 1 58A5061D
P 7000 3850
F 0 "#PWR150" H 7000 3600 50  0001 C CNN
F 1 "GND_L8" H 7000 3700 50  0001 C CNN
F 2 "" H 7000 3850 50  0000 C CNN
F 3 "" H 7000 3850 50  0000 C CNN
	1    7000 3850
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR136
U 1 1 58A73968
P 4700 6500
F 0 "#PWR136" H 4700 6250 50  0001 C CNN
F 1 "GND_L8" H 4700 6350 50  0001 C CNN
F 2 "" H 4700 6500 50  0000 C CNN
F 3 "" H 4700 6500 50  0000 C CNN
	1    4700 6500
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:INDUCTOR_SMALL L106
U 1 1 58A77A86
P 6150 5900
F 0 "L106" H 6150 6000 50  0000 C CNN
F 1 "2.7uH" H 6150 5850 50  0000 C CNN
F 2 "Inductors:Toroid_T-60_2TH_Vertical" H 6150 5900 50  0001 C CNN
F 3 "" H 6150 5900 50  0001 C CNN
F 4 "T60-2" H 6150 5900 60  0001 C CNN "Part No."
F 5 "<a href=\"http://www.amidoncorp.com/t60-2/\">Link</a>" H 6150 5900 60  0001 C CNN "Link"
	1    6150 5900
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C123
U 1 1 58A77CD9
P 5800 6100
F 0 "C123" H 5810 6170 50  0000 L CNN
F 1 "1100pF" H 5810 6020 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5800 6100 50  0001 C CNN
F 3 "" H 5800 6100 50  0000 C CNN
F 4 "399-14552-1-ND" H 5800 6100 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-14552-1-ND\">Link</a>" H 5800 6100 60  0001 C CNN "Link"
	1    5800 6100
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C122
U 1 1 58A78557
P 5550 5900
F 0 "C122" V 5350 5800 50  0000 L CNN
F 1 "470nF" V 5450 5800 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5550 5900 50  0001 C CNN
F 3 "" H 5550 5900 50  0000 C CNN
F 4 "478-9924-1-ND" H 5550 5900 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-9924-1-ND\">Link</a>" H 5550 5900 60  0001 C CNN "Link"
	1    5550 5900
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR146
U 1 1 58A7996F
P 6500 6200
F 0 "#PWR146" H 6500 5950 50  0001 C CNN
F 1 "GND_L8" H 6500 6050 50  0001 C CNN
F 2 "" H 6500 6200 50  0000 C CNN
F 3 "" H 6500 6200 50  0000 C CNN
	1    6500 6200
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR151
U 1 1 58A7A1D0
P 7500 6200
F 0 "#PWR151" H 7500 5950 50  0001 C CNN
F 1 "GND_L8" H 7500 6050 50  0001 C CNN
F 2 "" H 7500 6200 50  0000 C CNN
F 3 "" H 7500 6200 50  0000 C CNN
	1    7500 6200
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:+2to+12V #PWR133
U 1 1 58A9C144
P 4350 5050
F 0 "#PWR133" H 4350 4900 50  0001 C CNN
F 1 "+2to+12V" H 4400 5200 50  0000 C CNN
F 2 "" H 4350 5050 50  0000 C CNN
F 3 "" H 4350 5050 50  0000 C CNN
	1    4350 5050
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR117
U 1 1 58BF361E
P 2750 5250
F 0 "#PWR117" H 2750 5100 50  0001 C CNN
F 1 "+12V" H 2750 5390 50  0000 C CNN
F 2 "" H 2750 5250 50  0000 C CNN
F 3 "" H 2750 5250 50  0000 C CNN
	1    2750 5250
	1    0    0    -1  
$EndComp
Text Notes 10400 700  2    79   ~ 16
Main +3.3V Pwr
$Sheet
S -1850 700  1050 650 
U 591BC960
F0 "ARDF Tx Continued" 60
F1 "file591BC95F.sch" 60
$EndSheet
Text Notes 800  6750 1    157  ~ 31
80m RF Section
Text Notes 800  4150 1    157  ~ 31
2m RF Section
Wire Wire Line
	1950 2600 1950 2750
Wire Wire Line
	10600 850  10600 1100
Wire Wire Line
	10250 1100 10600 1100
Connection ~ 10600 1100
Wire Wire Line
	9850 1500 9850 1450
Wire Wire Line
	10600 1400 10600 1350
Wire Wire Line
	8900 1250 9450 1250
Wire Wire Line
	4250 5200 4350 5200
Connection ~ 4350 5200
Wire Wire Line
	7550 3400 7650 3400
Wire Wire Line
	7000 3400 7000 3600
Connection ~ 7800 3400
Wire Wire Line
	7000 3850 7000 3800
Connection ~ 7650 3400
Wire Wire Line
	5650 5900 5800 5900
Wire Wire Line
	6500 5750 6500 5900
Connection ~ 6500 5900
Wire Wire Line
	7400 5900 7500 5900
Connection ~ 7500 5900
Wire Wire Line
	5800 5750 5800 5900
Connection ~ 5800 5900
Wire Wire Line
	4350 5050 4350 5200
Wire Wire Line
	650  7500 650  7450
Wire Wire Line
	650  7450 850  7450
Wire Wire Line
	950  7450 950  7500
Wire Wire Line
	850  7200 850  7350
Connection ~ 850  7450
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR164
U 1 1 59D53C99
P 9150 6100
F 0 "#PWR164" H 9150 5850 50  0001 C CNN
F 1 "GND_L8" H 9150 5950 50  0001 C CNN
F 2 "" H 9150 6100 50  0000 C CNN
F 3 "" H 9150 6100 50  0000 C CNN
	1    9150 6100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 6100 9150 6000
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR161
U 1 1 59D57AF7
P 9100 3750
F 0 "#PWR161" H 9100 3500 50  0001 C CNN
F 1 "GND_L8" H 9100 3600 50  0001 C CNN
F 2 "" H 9100 3750 50  0000 C CNN
F 3 "" H 9100 3750 50  0000 C CNN
F 4 "Value" H 9100 3750 60  0001 C CNN "Part No."
F 5 "<a href=\"\">Link</a>" H 9100 3750 60  0001 C CNN "Link"
	1    9100 3750
	1    0    0    1   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:CONN_01X01 W116
U 1 1 59FE205E
P 1050 7200
F 0 "W116" H 1050 7300 50  0000 C CNN
F 1 "GND" V 1150 7200 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01" H 1050 7200 50  0001 C CNN
F 3 "" H 1050 7200 50  0001 C CNN
F 4 "np" H 1050 7200 60  0001 C CNN "Part No."
F 5 "np" H 1050 7200 60  0001 C CNN "Link"
	1    1050 7200
	1    0    0    -1  
$EndComp
Connection ~ 850  7350
Wire Wire Line
	3600 5250 3800 5250
Wire Wire Line
	3800 5250 3800 4300
Wire Wire Line
	3800 4300 3050 4300
Wire Wire Line
	3050 4300 3050 4750
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR123
U 1 1 5A7B67DA
P 3350 5250
F 0 "#PWR123" H 3350 5000 50  0001 C CNN
F 1 "GND_L8" H 3500 5100 50  0001 C CNN
F 2 "" H 3350 5250 50  0000 C CNN
F 3 "" H 3350 5250 50  0000 C CNN
	1    3350 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 5250 2750 5300
Wire Wire Line
	2750 5300 2900 5300
Wire Wire Line
	2900 5300 2900 5250
Connection ~ 3150 5750
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR140
U 1 1 5A7DEA16
P 5800 6200
F 0 "#PWR140" H 5800 5950 50  0001 C CNN
F 1 "GND_L8" H 5800 6050 50  0001 C CNN
F 2 "" H 5800 6200 50  0000 C CNN
F 3 "" H 5800 6200 50  0000 C CNN
	1    5800 6200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 6350 3950 6350
Wire Wire Line
	2850 6550 2850 6650
$Comp
L power:PWR_FLAG #FLG105
U 1 1 5A7E62FA
P 3050 6100
F 0 "#FLG105" H 3050 6195 50  0001 C CNN
F 1 "PWR_FLAG" H 3050 6280 50  0000 C CNN
F 2 "" H 3050 6100 50  0000 C CNN
F 3 "" H 3050 6100 50  0000 C CNN
	1    3050 6100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 6100 3050 6100
Connection ~ 3150 6100
Wire Wire Line
	1450 3400 1800 3400
Wire Wire Line
	1450 3250 1450 3400
Wire Wire Line
	2200 3400 2150 3400
Wire Wire Line
	2800 3400 2850 3400
Connection ~ 1950 2750
$Comp
L special:THS9001 U104
U 1 1 5A864765
P 2500 3400
F 0 "U104" H 2750 3100 61  0000 C CNB
F 1 "THS9001" H 2850 3000 61  0000 C CNB
F 2 "TO_SOT_Packages_SMD:SOT-23-6_Handsolder" H 2450 3500 50  0001 C CNN
F 3 "" H 2550 3600 50  0000 C CNN
F 4 "296-16645-1-ND" H 2500 3400 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=%09296-16645-1-ND\">Link</a>" H 2500 3400 60  0001 C CNN "Link"
	1    2500 3400
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:INDUCTOR_SMALL L102
U 1 1 5A86739B
P 2250 3000
F 0 "L102" H 2400 2950 50  0000 C CNN
F 1 "470nH" H 2150 2950 50  0000 C CNN
F 2 "Inductors:L_0805_2015_Handsolder" H 2250 3000 50  0001 C CNN
F 3 "" H 2250 3000 50  0001 C CNN
F 4 "CW201212-R47JCT-ND" H 2250 3000 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=CW201212-R47JCT-ND\">Link</a>" H 2250 3000 60  0001 C CNN "Link"
	1    2250 3000
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2500 3000 2500 3100
Wire Wire Line
	1950 3100 2400 3100
Wire Wire Line
	1950 3000 2000 3000
Connection ~ 1950 3000
$Comp
L Device:R R106
U 1 1 5A86812D
P 2600 2950
F 0 "R106" V 2680 2950 50  0000 C CNN
F 1 "240 2W" V 2750 2950 50  0000 C CNN
F 2 "Resistors_SMD:R_2512_HandSoldering" H 2530 2950 50  0001 C CNN
F 3 "" H 2600 2950 50  0000 C CNN
F 4 "A116017CT-ND" H 2600 2950 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=A116017CT-ND\">Link</a>" H 2600 2950 60  0001 C CNN "Link"
	1    2600 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 2850 1950 2850
Connection ~ 1950 2850
Wire Wire Line
	2600 2750 2600 2800
Wire Wire Line
	2600 2750 1950 2750
$Comp
L ARDF-2-Band-MiniTx-rescue:TEST_1P W114
U 1 1 5A8B3DC0
P 850 7350
F 0 "W114" V 850 7620 50  0000 C CNN
F 1 "GND" V 750 7550 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 1050 7350 50  0001 C CNN
F 3 "" H 1050 7350 50  0000 C CNN
F 4 "np" H 850 7350 60  0001 C CNN "Part No."
F 5 "np" H 850 7350 60  0001 C CNN "Link"
	1    850  7350
	0    1    -1   0   
$EndComp
Wire Wire Line
	2250 6350 2650 6350
Wire Wire Line
	2650 6350 2650 6300
Connection ~ 2650 6350
$Comp
L ARDF-2-Band-MiniTx-rescue:LM5134 U107
U 1 1 5AA8446F
P 3150 6450
F 0 "U107" H 3450 6850 61  0000 R CNB
F 1 "LM5134B" H 3600 6750 61  0000 R CNB
F 2 "TO_SOT_Packages_SMD:SOT-23-6_Handsolder" H 3300 5600 50  0001 L CNN
F 3 "" H 3350 6150 50  0001 C CNN
F 4 "296-40244-1-ND" H 3150 6450 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=296-40244-1-ND\">Link</a>" H 3150 6450 60  0001 C CNN "Link"
	1    3150 6450
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR119
U 1 1 5AAE6E6A
P 2850 6650
F 0 "#PWR119" H 2850 6400 50  0001 C CNN
F 1 "GND_L8" H 2850 6500 50  0001 C CNN
F 2 "" H 2850 6650 50  0000 C CNN
F 3 "" H 2850 6650 50  0000 C CNN
	1    2850 6650
	1    0    0    -1  
$EndComp
Wire Wire Line
	9650 5250 9350 5250
Wire Wire Line
	9550 4800 9650 4800
Text GLabel 9700 5000 2    39   Output ~ 0
~ANT_CONNECT_INT~
$Comp
L ARDF-2-Band-MiniTx-rescue:CONN_01X03 P101
U 1 1 5AC927FB
P 9350 3950
F 0 "P101" H 9350 4150 50  0000 C CNN
F 1 "Phoenix" V 9500 3950 50  0000 C CNN
F 2 "Sockets:Phoenix_3.81mm_3pos" H 9350 3950 50  0001 C CNN
F 3 "" H 9350 3950 50  0001 C CNN
F 4 "277-1207-ND" H 9350 3950 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=277-1207-ND\">Link</a>" H 9350 3950 60  0001 C CNN "Link"
	1    9350 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	9100 3750 9100 3850
Wire Wire Line
	9100 3850 9150 3850
$Sheet
S -1850 1550 1050 650 
U 5BC7F38C
F0 "Digital" 61
F1 "file5BC7F38B.sch" 61
$EndSheet
Text GLabel 2250 6350 0    39   Input ~ 0
CLK1
Text GLabel 1450 3250 1    39   Input ~ 0
VHF_CLK
Wire Wire Line
	9650 4800 9650 5000
$Comp
L ARDF-2-Band-MiniTx-rescue:CONN_01X03 P103
U 1 1 5BCC61E7
P 9350 5900
F 0 "P103" H 9350 6100 50  0000 C CNN
F 1 "Phoenix" V 9500 5900 50  0000 C CNN
F 2 "Sockets:Phoenix_3.81mm_3pos" H 9350 5900 50  0001 C CNN
F 3 "" H 9350 5900 50  0001 C CNN
F 4 "277-1207-ND" H 9350 5900 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=277-1207-ND\">Link</a>" H 9350 5900 60  0001 C CNN "Link"
	1    9350 5900
	1    0    0    1   
$EndComp
Text GLabel 3450 4750 1    39   Input ~ 0
HF_ENABLE
$Comp
L Device:C_Small C113
U 1 1 5BE8CD7B
P 2950 3400
F 0 "C113" V 2800 3350 50  0000 L CNN
F 1 "1nF" V 2700 3350 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2950 3400 50  0001 C CNN
F 3 "" H 2950 3400 50  0000 C CNN
F 4 "490-6445-1-ND" H 2950 3400 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=490-6445-1-ND\">Link</a>" H 2950 3400 60  0001 C CNN "Link"
	1    2950 3400
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C120
U 1 1 5BE9CF68
P 4550 5200
F 0 "C120" V 4650 5100 50  0000 L CNN
F 1 "470nF" V 4400 5100 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4550 5200 50  0001 C CNN
F 3 "" H 4550 5200 50  0000 C CNN
F 4 "478-9924-1-ND" H 4550 5200 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-9924-1-ND\">Link</a>" H 4550 5200 60  0001 C CNN "Link"
	1    4550 5200
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C126
U 1 1 5BEA0426
P 6500 6100
F 0 "C126" H 6250 6050 50  0000 L CNN
F 1 "1800pF" H 6200 6200 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 6500 6100 50  0001 C CNN
F 3 "" H 6500 6100 50  0000 C CNN
F 4 "445-5752-1-ND" H 6500 6100 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=445-5752-1-ND\">Link</a>" H 6500 6100 60  0001 C CNN "Link"
	1    6500 6100
	-1   0    0    1   
$EndComp
$Comp
L special:Heat-sink HS101
U 1 1 5BEA52AC
P 4350 6250
F 0 "HS101" H 4150 6550 50  0000 L CNN
F 1 "Heatsink" H 4150 6450 50  0000 L CNN
F 2 "Heatsinks:Heatsink_HF35G" H 3800 6225 50  0001 L CIN
F 3 "" H 4350 6250 50  0001 L CNN
F 4 "HS220-ND" H 4350 6250 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=HS220-ND\">Link</a>" H 4350 6250 60  0001 C CNN "Link"
	1    4350 6250
	1    0    0    -1  
$EndComp
Text Notes 9300 4200 0    39   ~ 8
146 MHz Out
Text Notes 9350 6200 0    39   ~ 8
3.5 MHz Out
$Comp
L ARDF-2-Band-MiniTx-rescue:CONN_01X03 P502
U 1 1 5BF4992C
P 9650 5900
F 0 "P502" H 9650 6100 50  0000 C CNN
F 1 "Phoenix" V 9800 5900 50  0001 C CNN
F 2 "Oddities:Dummy_Empty_3" H 9650 5900 50  0001 C CNN
F 3 "" H 9650 5900 50  0001 C CNN
F 4 "277-1162-ND" H 9650 5900 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=277-1162-ND\">Link</a>" H 9650 5900 60  0001 C CNN "Link"
	1    9650 5900
	-1   0    0    1   
$EndComp
NoConn ~ 9850 5900
$Comp
L ARDF-2-Band-MiniTx-rescue:CONN_01X03 P501
U 1 1 5BF4AAA7
P 9650 3950
F 0 "P501" H 9650 4150 50  0000 C CNN
F 1 "Phoenix" V 9800 3950 50  0001 C CNN
F 2 "Oddities:Dummy_Empty_3" H 9650 3950 50  0001 C CNN
F 3 "" H 9650 3950 50  0001 C CNN
F 4 "277-1162-ND" H 9650 3950 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=277-1162-ND\">Link</a>" H 9650 3950 60  0001 C CNN "Link"
	1    9650 3950
	-1   0    0    -1  
$EndComp
NoConn ~ 9850 3950
$Sheet
S -1850 2450 1050 700 
U 5BF4C520
F0 "Non-PCB" 39
F1 "file5BF4C51F.sch" 39
$EndSheet
$Comp
L ARDF-2-Band-MiniTx-rescue:+2to+12V #PWR138
U 1 1 5BF5ED4F
P 5600 1400
F 0 "#PWR138" H 5600 1250 50  0001 C CNN
F 1 "+2to+12V" H 5650 1550 50  0000 C CNN
F 2 "" H 5600 1400 50  0000 C CNN
F 3 "" H 5600 1400 50  0000 C CNN
	1    5600 1400
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR142
U 1 1 5BF655C9
P 5900 1850
F 0 "#PWR142" H 5900 1600 50  0001 C CNN
F 1 "GND_L8" H 5900 1700 50  0001 C CNN
F 2 "" H 5900 1850 50  0000 C CNN
F 3 "" H 5900 1850 50  0000 C CNN
	1    5900 1850
	0    -1   -1   0   
$EndComp
Wire Wire Line
	9100 4800 9150 4800
Text GLabel 9300 4550 2    39   Output ~ 0
2M_ANTENNA_DETECT
Text GLabel 9300 5500 2    39   Output ~ 0
80M_ANTENNA_DETECT
Wire Wire Line
	9850 3850 10000 3850
Wire Wire Line
	10000 4050 9850 4050
Wire Wire Line
	9850 6000 10100 6000
Wire Wire Line
	10100 5800 9850 5800
Wire Wire Line
	7500 5600 7500 5900
Text Notes 6300 5100 0    79   ~ 16
HF LPF
Text Notes 9150 3600 0    79   ~ 16
VHF Antenna
Text Notes 9250 6350 0    79   ~ 16
HF Antenna
Wire Wire Line
	8500 3400 8550 3400
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR157
U 1 1 5C0C909C
P 8550 3900
F 0 "#PWR157" H 8550 3650 50  0001 C CNN
F 1 "GND_L8" H 8350 3850 50  0001 C CNN
F 2 "" H 8550 3900 50  0000 C CNN
F 3 "" H 8550 3900 50  0000 C CNN
	1    8550 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 3900 8550 3800
Connection ~ 8550 3400
$Comp
L Device:C_Small C130
U 1 1 5C0CF13F
P 8550 3700
F 0 "C130" H 8560 3770 50  0000 L CNN
F 1 "20pF" H 8560 3620 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 8550 3700 50  0001 C CNN
F 3 "" H 8550 3700 50  0000 C CNN
F 4 "478-10507-1-ND" H 8550 3700 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-10507-1-ND\">Link</a>" H 8550 3700 60  0001 C CNN "Link"
	1    8550 3700
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C131
U 1 1 5C0D126D
P 7500 6100
F 0 "C131" H 7510 6170 50  0000 L CNN
F 1 "1100pF" H 7200 6000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 7500 6100 50  0001 C CNN
F 3 "" H 7500 6100 50  0000 C CNN
F 4 "399-14552-1-ND" H 7500 6100 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-14552-1-ND\">Link</a>" H 7500 6100 60  0001 C CNN "Link"
	1    7500 6100
	1    0    0    -1  
$EndComp
Text Notes 6200 5300 0    39   ~ 0
6th order Chebychev\n3.9MHz 3dB cut-off\nw/ node at 10.7MHz
$Comp
L ARDF-2-Band-MiniTx-rescue:INDUCTOR_SMALL L108
U 1 1 5C0D7F1D
P 7150 5900
F 0 "L108" H 7150 6000 50  0000 C CNN
F 1 "2.7uH" H 7150 5850 50  0000 C CNN
F 2 "Inductors:Toroid_T-60_2TH_Vertical" H 7150 5900 50  0001 C CNN
F 3 "" H 7150 5900 50  0001 C CNN
F 4 "T60-2" H 7150 5900 60  0001 C CNN "Part No."
F 5 "<a href=\"http://www.amidoncorp.com/t60-2/\">Link</a>" H 7150 5900 60  0001 C CNN "Link"
	1    7150 5900
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:TEST_SCOPE W102
U 1 1 5C0E2344
P 1800 3300
F 0 "W102" V 2000 3300 50  0000 C CNN
F 1 "P1" V 1850 3350 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 2000 3300 50  0001 C CNN
F 3 "" H 2000 3300 50  0001 C CNN
F 4 "np" H 1800 3300 60  0001 C CNN "Part No."
F 5 "np" H 1800 3300 60  0001 C CNN "Link"
	1    1800 3300
	0    1    -1   0   
$EndComp
Wire Wire Line
	1800 3300 1800 3400
Connection ~ 1800 3400
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR106
U 1 1 5C0E31E2
P 1800 3200
F 0 "#PWR106" H 1800 2950 50  0001 C CNN
F 1 "GND_L8" H 1800 3050 50  0001 C CNN
F 2 "" H 1800 3200 50  0000 C CNN
F 3 "" H 1800 3200 50  0000 C CNN
	1    1800 3200
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:TEST_SCOPE W104
U 1 1 5C0E516F
P 4200 3250
F 0 "W104" V 4400 3250 50  0000 C CNN
F 1 "P2" V 4250 3300 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 4400 3250 50  0001 C CNN
F 3 "" H 4400 3250 50  0001 C CNN
F 4 "np" H 4200 3250 60  0001 C CNN "Part No."
F 5 "np" H 4200 3250 60  0001 C CNN "Link"
	1    4200 3250
	0    1    -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:TEST_SCOPE W110
U 1 1 5C0E5B1C
P 7650 3300
F 0 "W110" V 7850 3300 50  0000 C CNN
F 1 "P4" V 7700 3350 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 7850 3300 50  0001 C CNN
F 3 "" H 7850 3300 50  0001 C CNN
F 4 "np" H 7650 3300 60  0001 C CNN "Part No."
F 5 "np" H 7650 3300 60  0001 C CNN "Link"
	1    7650 3300
	0    1    -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR152
U 1 1 5C0E646B
P 7650 3200
F 0 "#PWR152" H 7650 2950 50  0001 C CNN
F 1 "GND_L8" H 7650 3050 50  0001 C CNN
F 2 "" H 7650 3200 50  0000 C CNN
F 3 "" H 7650 3200 50  0000 C CNN
	1    7650 3200
	0    1    1    0   
$EndComp
Wire Wire Line
	7650 3300 7650 3400
$Comp
L ARDF-2-Band-MiniTx-rescue:TEST_SCOPE W105
U 1 1 5C0EAA8C
P 2650 6300
F 0 "W105" V 2850 6300 50  0000 C CNN
F 1 "T2" V 2700 6350 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 2850 6300 50  0001 C CNN
F 3 "" H 2850 6300 50  0001 C CNN
F 4 "np" H 2650 6300 60  0001 C CNN "Part No."
F 5 "np" H 2650 6300 60  0001 C CNN "Link"
	1    2650 6300
	0    -1   -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR116
U 1 1 5C0EAA92
P 2650 6200
F 0 "#PWR116" H 2650 5950 50  0001 C CNN
F 1 "GND_L8" H 2650 6050 50  0001 C CNN
F 2 "" H 2650 6200 50  0000 C CNN
F 3 "" H 2650 6200 50  0000 C CNN
	1    2650 6200
	0    -1   -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:TEST_SCOPE W108
U 1 1 5C0EB478
P 3950 6500
F 0 "W108" H 3900 6750 50  0000 C CNN
F 1 "T4" H 4000 6550 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 4150 6500 50  0001 C CNN
F 3 "" H 4150 6500 50  0001 C CNN
F 4 "np" H 3950 6500 60  0001 C CNN "Part No."
F 5 "np" H 3950 6500 60  0001 C CNN "Link"
	1    3950 6500
	-1   0    0    1   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR126
U 1 1 5C0EB47E
P 3850 6500
F 0 "#PWR126" H 3850 6250 50  0001 C CNN
F 1 "GND_L8" H 3850 6350 50  0001 C CNN
F 2 "" H 3850 6500 50  0000 C CNN
F 3 "" H 3850 6500 50  0000 C CNN
	1    3850 6500
	-1   0    0    1   
$EndComp
Wire Wire Line
	3950 6500 3950 6350
Connection ~ 3950 6350
$Comp
L ARDF-2-Band-MiniTx-rescue:TEST_SCOPE W111
U 1 1 5C0EDCBD
P 5800 5750
F 0 "W111" H 5850 6050 50  0000 C CNN
F 1 "T5" H 5850 5800 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 6000 5750 50  0001 C CNN
F 3 "" H 6000 5750 50  0001 C CNN
F 4 "np" H 5800 5750 60  0001 C CNN "Part No."
F 5 "np" H 5800 5750 60  0001 C CNN "Link"
	1    5800 5750
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR144
U 1 1 5C0EDCC3
P 5900 5750
F 0 "#PWR144" H 5900 5500 50  0001 C CNN
F 1 "GND_L8" H 5900 5600 50  0001 C CNN
F 2 "" H 5900 5750 50  0000 C CNN
F 3 "" H 5900 5750 50  0000 C CNN
	1    5900 5750
	-1   0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:TEST_SCOPE W113
U 1 1 5C0EE9E8
P 6500 5750
F 0 "W113" H 6550 6050 50  0000 C CNN
F 1 "T6" H 6550 5800 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 6700 5750 50  0001 C CNN
F 3 "" H 6700 5750 50  0001 C CNN
F 4 "np" H 6500 5750 60  0001 C CNN "Part No."
F 5 "np" H 6500 5750 60  0001 C CNN "Link"
	1    6500 5750
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR148
U 1 1 5C0EE9EE
P 6600 5750
F 0 "#PWR148" H 6600 5500 50  0001 C CNN
F 1 "GND_L8" H 6600 5600 50  0001 C CNN
F 2 "" H 6600 5750 50  0000 C CNN
F 3 "" H 6600 5750 50  0000 C CNN
	1    6600 5750
	-1   0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:TEST_SCOPE W117
U 1 1 5C0EEADD
P 7700 5750
F 0 "W117" H 7750 6000 50  0000 C CNN
F 1 "T7" H 7750 5800 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 7900 5750 50  0001 C CNN
F 3 "" H 7900 5750 50  0001 C CNN
F 4 "np" H 7700 5750 60  0001 C CNN "Part No."
F 5 "np" H 7700 5750 60  0001 C CNN "Link"
	1    7700 5750
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR154
U 1 1 5C0EEAE3
P 7800 5750
F 0 "#PWR154" H 7800 5500 50  0001 C CNN
F 1 "GND_L8" H 7800 5600 50  0001 C CNN
F 2 "" H 7800 5750 50  0000 C CNN
F 3 "" H 7800 5750 50  0000 C CNN
	1    7800 5750
	-1   0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:TEST_SCOPE W119
U 1 1 5C0F0904
P 10800 1100
F 0 "W119" V 11000 1100 50  0000 C CNN
F 1 "V2" V 10850 1150 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 11000 1100 50  0001 C CNN
F 3 "" H 11000 1100 50  0001 C CNN
F 4 "np" H 10800 1100 60  0001 C CNN "Part No."
F 5 "np" H 10800 1100 60  0001 C CNN "Link"
	1    10800 1100
	0    1    -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR175
U 1 1 5C0F090A
P 10800 1000
F 0 "#PWR175" H 10800 750 50  0001 C CNN
F 1 "GND_L8" H 10800 850 50  0001 C CNN
F 2 "" H 10800 1000 50  0000 C CNN
F 3 "" H 10800 1000 50  0000 C CNN
	1    10800 1000
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:TEST_SCOPE W106
U 1 1 5C0F543D
P 2750 5750
F 0 "W106" V 2800 6000 50  0000 C CNN
F 1 "T3" V 2800 5800 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 2950 5750 50  0001 C CNN
F 3 "" H 2950 5750 50  0001 C CNN
F 4 "np" H 2750 5750 60  0001 C CNN "Part No."
F 5 "np" H 2750 5750 60  0001 C CNN "Link"
	1    2750 5750
	0    -1   1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR118
U 1 1 5C0F5443
P 2750 5850
F 0 "#PWR118" H 2750 5600 50  0001 C CNN
F 1 "GND_L8" H 2750 5700 50  0001 C CNN
F 2 "" H 2750 5850 50  0000 C CNN
F 3 "" H 2750 5850 50  0000 C CNN
	1    2750 5850
	0    -1   1    0   
$EndComp
$Comp
L Device:C_Small C128
U 1 1 5C0F9847
P 7100 5600
F 0 "C128" V 7000 5500 50  0000 L CNN
F 1 "91pF" V 7200 5500 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 7100 5600 50  0001 C CNN
F 3 "" H 7100 5600 50  0000 C CNN
F 4 "399-17459-1-ND" H 7100 5600 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-17459-1-ND\">Link</a>" H 7100 5600 60  0001 C CNN "Link"
	1    7100 5600
	0    1    1    0   
$EndComp
Wire Wire Line
	7200 5600 7500 5600
Wire Wire Line
	7000 5600 6850 5600
Wire Wire Line
	6850 5600 6850 5900
Connection ~ 6850 5900
Wire Wire Line
	6400 5900 6500 5900
$Comp
L ARDF-2-Band-MiniTx-rescue:IRF610_TO220 Q102
U 1 1 5C100056
P 4600 6300
F 0 "Q102" H 4800 6375 50  0000 L CNN
F 1 "IRF610" H 4800 6300 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-220_Neutral123_Vertical_LargePads" H 4800 6225 50  0001 L CIN
F 3 "" H 4600 6300 50  0001 L CNN
F 4 "IRF610PBF-ND" H 4600 6300 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=IRF610PBF-ND\">Link</a>" H 4600 6300 60  0001 C CNN "Link"
	1    4600 6300
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1_Small C125
U 1 1 5C118217
P 5800 1850
F 0 "C125" V 5900 1700 50  0000 L CNN
F 1 "47uF low ESR" V 5700 1250 50  0000 L CNN
F 2 "Capacitors_SMD:C_2917_7343_Metric_HandSoldering" H 5800 1850 50  0001 C CNN
F 3 "https://content.kemet.com/datasheets/KEM_T2009_T495.pdf" H 5800 1850 50  0001 C CNN
F 4 "399-3885-1-ND" H 5800 1850 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-3885-1-ND\">Link</a>" H 5800 1850 60  0001 C CNN "Link"
	1    5800 1850
	0    -1   -1   0   
$EndComp
$Comp
L Device:CP1_Small C119
U 1 1 5C11AA96
P 4150 5200
F 0 "C119" V 4250 5150 50  0000 L CNN
F 1 "47uF low ESR" V 4000 4850 50  0000 L CNN
F 2 "Capacitors_SMD:C_2917_7343_Metric_HandSoldering" H 4150 5200 50  0001 C CNN
F 3 "https://content.kemet.com/datasheets/KEM_T2009_T495.pdf" H 4150 5200 50  0001 C CNN
F 4 "399-3885-1-ND" H 4150 5200 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-3885-1-ND\">Link</a>" H 4150 5200 60  0001 C CNN "Link"
	1    4150 5200
	0    1    -1   0   
$EndComp
$Comp
L power:GND #PWR101
U 1 1 5C132372
P 650 7500
F 0 "#PWR101" H 650 7250 50  0001 C CNN
F 1 "GND" H 650 7350 50  0000 C CNN
F 2 "" H 650 7500 50  0000 C CNN
F 3 "" H 650 7500 50  0000 C CNN
	1    650  7500
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR102
U 1 1 5C132EBB
P 950 7500
F 0 "#PWR102" H 950 7250 50  0001 C CNN
F 1 "GND_L8" H 950 7350 50  0000 C CNN
F 2 "" H 950 7500 50  0000 C CNN
F 3 "" H 950 7500 50  0000 C CNN
	1    950  7500
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR124
U 1 1 5A7DD6BD
P 3450 5750
F 0 "#PWR124" H 3450 5500 50  0001 C CNN
F 1 "GND_L8" H 3600 5600 50  0001 C CNN
F 2 "" H 3450 5750 50  0000 C CNN
F 3 "" H 3450 5750 50  0000 C CNN
	1    3450 5750
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C117
U 1 1 5A7D307D
P 3350 5750
F 0 "C117" V 3250 5400 50  0000 L CNN
F 1 "22uF" V 3250 5650 50  0000 L CNN
F 2 "Capacitors_SMD:C_1210_HandSoldering" H 3350 5750 50  0001 C CNN
F 3 "" H 3350 5750 50  0000 C CNN
F 4 "1276-3373-1-ND" H 3350 5750 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-3373-1-ND\">Link</a>" H 3350 5750 60  0001 C CNN "Link"
	1    3350 5750
	0    -1   -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:LMN200B01 Q101
U 1 1 5C64951A
P 3250 5000
F 0 "Q101" V 2950 4850 61  0000 L CNB
F 1 "LMN400E01" V 3200 4050 61  0000 L CNB
F 2 "TO_SOT_Packages_SMD:SOT-363_Handsolder" H 3265 4800 50  0001 L CNN
F 3 "" H 3265 4800 50  0000 L CNN
F 4 "LMN400E01-7" H 3250 5000 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=LMN400E01DICT-ND\">Link</a>" H 3250 5000 60  0001 C CNN "Link"
	1    3250 5000
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:MIC5219 U105
U 1 1 5C6C50D8
P 9850 2500
F 0 "U105" H 9850 2800 61  0000 C CNB
F 1 "MIC5219-5.0YM5" H 9850 2700 61  0000 C CNB
F 2 "TO_SOT_Packages_SMD:SOT-23-5_Handsolder" H 9850 2600 50  0001 C CIN
F 3 "" H 9850 2500 50  0000 C CNN
F 4 "576-2770-1-ND" H 9850 2500 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=576-2770-1-ND\">Link</a>" H 9850 2500 60  0001 C CNN "Link"
	1    9850 2500
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR168
U 1 1 5C6C50DE
P 9850 2850
F 0 "#PWR168" H 9850 2600 50  0001 C CNN
F 1 "GND_L8" H 9850 2700 50  0000 C CNN
F 2 "" H 9850 2850 50  0000 C CNN
F 3 "" H 9850 2850 50  0000 C CNN
	1    9850 2850
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR173
U 1 1 5C6C50EA
P 10600 2750
F 0 "#PWR173" H 10600 2500 50  0001 C CNN
F 1 "GND_L8" H 10600 2600 50  0000 C CNN
F 2 "" H 10600 2750 50  0000 C CNN
F 3 "" H 10600 2750 50  0000 C CNN
	1    10600 2750
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C141
U 1 1 5C6C50F8
P 10600 2600
F 0 "C141" H 10350 2550 50  0000 L CNN
F 1 "2.2uF" H 10350 2700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 10600 2600 50  0001 C CNN
F 3 "" H 10600 2600 50  0000 C CNN
F 4 "1276-1763-1-ND" H 10600 2600 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-1763-1-ND\">Link</a>" H 10600 2600 60  0001 C CNN "Link"
	1    10600 2600
	-1   0    0    1   
$EndComp
$Comp
L Device:C_Small C132
U 1 1 5C6C5108
P 9250 2450
F 0 "C132" V 9350 2450 50  0000 L CNN
F 1 "2.2uF" V 9100 2450 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 9250 2450 50  0001 C CNN
F 3 "" H 9250 2450 50  0000 C CNN
F 4 "1276-1763-1-ND" H 9250 2450 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-1763-1-ND\">Link</a>" H 9250 2450 60  0001 C CNN "Link"
	1    9250 2450
	0    -1   -1   0   
$EndComp
$Comp
L power:+12V #PWR165
U 1 1 5C6C510E
P 9350 2300
F 0 "#PWR165" H 9350 2150 50  0001 C CNN
F 1 "+12V" H 9350 2440 50  0000 C CNN
F 2 "" H 9350 2300 50  0000 C CNN
F 3 "" H 9350 2300 50  0000 C CNN
	1    9350 2300
	1    0    0    -1  
$EndComp
Text Notes 10400 2050 2    79   ~ 16
VHF 5V Pwr
Wire Wire Line
	10600 2200 10600 2450
Wire Wire Line
	10250 2450 10600 2450
Connection ~ 10600 2450
Wire Wire Line
	9850 2850 9850 2800
Wire Wire Line
	10600 2750 10600 2700
Wire Wire Line
	9350 2600 9450 2600
Connection ~ 9350 2450
Wire Wire Line
	9350 2450 9450 2450
$Comp
L ARDF-2-Band-MiniTx-rescue:TEST_SCOPE W101
U 1 1 5C6C5125
P 10800 2450
F 0 "W101" V 11000 2450 50  0000 C CNN
F 1 "V3" V 10850 2500 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 11000 2450 50  0001 C CNN
F 3 "" H 11000 2450 50  0001 C CNN
F 4 "np" H 10800 2450 60  0001 C CNN "Part No."
F 5 "np" H 10800 2450 60  0001 C CNN "Link"
	1    10800 2450
	0    1    -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR176
U 1 1 5C6C512B
P 10800 2350
F 0 "#PWR176" H 10800 2100 50  0001 C CNN
F 1 "GND_L8" H 10800 2200 50  0001 C CNN
F 2 "" H 10800 2350 50  0000 C CNN
F 3 "" H 10800 2350 50  0000 C CNN
	1    10800 2350
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:+5V_L8 #PWR172
U 1 1 5C6C53AA
P 10600 2200
F 0 "#PWR172" H 10600 2050 50  0001 C CNN
F 1 "+5V_L8" H 10600 2340 50  0000 C CNN
F 2 "" H 10600 2200 50  0000 C CNN
F 3 "" H 10600 2200 50  0000 C CNN
	1    10600 2200
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:+5V_L8 #PWR107
U 1 1 5C6C56F1
P 1950 2600
F 0 "#PWR107" H 1950 2450 50  0001 C CNN
F 1 "+5V_L8" H 1950 2740 50  0000 C CNN
F 2 "" H 1950 2600 50  0000 C CNN
F 3 "" H 1950 2600 50  0000 C CNN
	1    1950 2600
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C105
U 1 1 5C6C779B
P 5800 2150
F 0 "C105" V 5900 2050 50  0000 L CNN
F 1 "100nF" V 5700 2000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5800 2150 50  0001 C CNN
F 3 "" H 5800 2150 50  0000 C CNN
F 4 "478-1395-1-ND" H 5800 2150 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 5800 2150 60  0001 C CNN "Link"
	1    5800 2150
	0    -1   -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR163
U 1 1 5C6D07EC
P 9150 2450
F 0 "#PWR163" H 9150 2200 50  0001 C CNN
F 1 "GND_L8" H 9150 2300 50  0001 C CNN
F 2 "" H 9150 2450 50  0000 C CNN
F 3 "" H 9150 2450 50  0000 C CNN
	1    9150 2450
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR162
U 1 1 5C6D097F
P 9150 1100
F 0 "#PWR162" H 9150 850 50  0001 C CNN
F 1 "GND_L8" H 9150 950 50  0001 C CNN
F 2 "" H 9150 1100 50  0000 C CNN
F 3 "" H 9150 1100 50  0000 C CNN
	1    9150 1100
	0    1    1    0   
$EndComp
Wire Wire Line
	9100 4050 9100 4250
Wire Wire Line
	9100 4050 9150 4050
Wire Wire Line
	9700 5000 9650 5000
Connection ~ 9650 5000
Wire Wire Line
	8950 5800 9100 5800
Wire Wire Line
	8950 4550 9100 4550
Wire Wire Line
	3050 3400 3350 3400
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR122
U 1 1 5C6D4DD1
P 3350 3750
F 0 "#PWR122" H 3350 3500 50  0001 C CNN
F 1 "GND_L8" H 3350 3600 50  0001 C CNN
F 2 "" H 3350 3750 50  0000 C CNN
F 3 "" H 3350 3750 50  0000 C CNN
	1    3350 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3350 3400 3350 3500
Wire Wire Line
	3350 3750 3350 3700
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR129
U 1 1 5C6D4EBC
P 4050 3750
F 0 "#PWR129" H 4050 3500 50  0001 C CNN
F 1 "GND_L8" H 4050 3600 50  0001 C CNN
F 2 "" H 4050 3750 50  0000 C CNN
F 3 "" H 4050 3750 50  0000 C CNN
F 4 "Value" H 4050 3750 60  0001 C CNN "Part No."
F 5 "<a href=\"\">Link</a>" H 4050 3750 60  0001 C CNN "Link"
	1    4050 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 3250 4050 3400
Wire Wire Line
	4050 3750 4050 3700
Connection ~ 3350 3400
Connection ~ 4050 3400
Wire Wire Line
	3950 3400 4050 3400
Wire Wire Line
	4850 3400 5050 3400
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR115
U 1 1 5C6DACC6
P 2600 2150
F 0 "#PWR115" H 2600 1900 50  0001 C CNN
F 1 "GND_L8" H 2600 2000 50  0001 C CNN
F 2 "" H 2600 2150 50  0000 C CNN
F 3 "" H 2600 2150 50  0000 C CNN
	1    2600 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 3250 4050 3250
$Comp
L ARDF-2-Band-MiniTx-rescue:INDUCTOR_SMALL L111
U 1 1 5C6E1A2F
P 3700 3400
F 0 "L111" H 3700 3500 50  0000 C CNN
F 1 "56nH" H 3700 3350 50  0000 C CNN
F 2 "Inductors:Wurth_744912212" H 3700 3400 50  0001 C CNN
F 3 "" H 3700 3400 50  0000 C CNN
F 4 "732-5641-1-ND" H 3700 3400 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=732-5641-1-ND\">Link</a>" H 3700 3400 60  0001 C CNN "Link"
	1    3700 3400
	1    0    0    -1  
$EndComp
Text GLabel 9350 2700 3    39   Input ~ 0
VHF_ENABLE
Wire Wire Line
	9350 2700 9350 2600
Wire Wire Line
	9350 2300 9350 2450
Text Notes 3450 2950 0    79   ~ 16
VHF LPF
Text Notes 3400 3100 0    39   ~ 0
3rd order Chebychev\n148MHz 3dB cut-off
Text Notes 2200 5550 0    39   ~ 0
Optional voltage-dropping\ndiode for batteries > 12V.\nShort across if not used.
Wire Wire Line
	2750 5750 3150 5750
$Comp
L ARDF-2-Band-MiniTx-rescue:ZENER D102
U 1 1 5C71D9FE
P 3150 5450
F 0 "D102" H 3150 5550 50  0000 C CNN
F 1 "4.7V" H 3150 5350 50  0000 C CNN
F 2 "Diodes_SMD:SOD-123_Handsolder" H 3150 5450 50  0001 C CNN
F 3 "" H 3150 5450 50  0000 C CNN
F 4 "BZT52C4V7-13FDICT-ND" H 3150 5450 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=BZT52C4V7-13FDICT-ND\">Link</a>" H 3150 5450 60  0001 C CNN "Link"
	1    3150 5450
	0    1    1    0   
$EndComp
Wire Wire Line
	3150 5650 3150 5750
$Comp
L Device:C_Small C121
U 1 1 5C72E807
P 7000 3700
F 0 "C121" H 7010 3770 50  0000 L CNN
F 1 "100pF" H 7010 3620 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 7000 3700 50  0001 C CNN
F 3 "" H 7000 3700 50  0000 C CNN
F 4 "399-1122-1-ND" H 7000 3700 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-1122-1-ND\">Link</a>" H 7000 3700 60  0001 C CNN "Link"
	1    7000 3700
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C104
U 1 1 5C72ED55
P 4050 3600
F 0 "C104" H 4060 3670 50  0000 L CNN
F 1 "68pF" H 4060 3520 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4050 3600 50  0001 C CNN
F 3 "" H 4050 3600 50  0000 C CNN
F 4 "399-1119-1-ND" H 4050 3600 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-1119-1-ND\">Link</a>" H 4050 3600 60  0001 C CNN "Link"
	1    4050 3600
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C103
U 1 1 5C72F323
P 3350 3600
F 0 "C103" H 3360 3670 50  0000 L CNN
F 1 "20pF" H 3360 3520 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 3350 3600 50  0001 C CNN
F 3 "" H 3350 3600 50  0000 C CNN
F 4 "478-10507-1-ND" H 3350 3600 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-10507-1-ND\">Link</a>" H 3350 3600 60  0001 C CNN "Link"
	1    3350 3600
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C501
U 1 1 5C73FA8A
P 10150 3950
F 0 "C501" H 10160 4020 50  0000 L CNN
F 1 "np" H 10160 3870 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 10150 3950 50  0001 C CNN
F 3 "" H 10150 3950 50  0000 C CNN
F 4 "np" H 10150 3950 60  0001 C CNN "Part No."
F 5 "np" H 10150 3950 60  0001 C CNN "Link"
	1    10150 3950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R501
U 1 1 5C73FE72
P 10450 3950
F 0 "R501" V 10350 3950 50  0000 C CNN
F 1 "0" V 10450 3950 50  0000 C CNN
F 2 "Resistors_SMD:Zero_Ohm_0805_HandSoldering" H 10380 3950 50  0001 C CNN
F 3 "" H 10450 3950 50  0000 C CNN
F 4 "np" H 10450 3950 60  0001 C CNN "Part No."
F 5 "np" H 10450 3950 60  0001 C CNN "Link"
	1    10450 3950
	-1   0    0    1   
$EndComp
Wire Wire Line
	10000 3850 10000 3750
Wire Wire Line
	10000 3750 10150 3750
Wire Wire Line
	10450 3750 10450 3800
Wire Wire Line
	10000 4050 10000 4150
Wire Wire Line
	10000 4150 10150 4150
Wire Wire Line
	10450 4150 10450 4100
Wire Wire Line
	10150 3850 10150 3750
Connection ~ 10150 3750
Wire Wire Line
	10150 4050 10150 4150
Connection ~ 10150 4150
$Comp
L Device:C_Small C502
U 1 1 5C740B6F
P 10250 5900
F 0 "C502" H 10260 5970 50  0000 L CNN
F 1 "np" H 10260 5820 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 10250 5900 50  0001 C CNN
F 3 "" H 10250 5900 50  0000 C CNN
F 4 "np" H 10250 5900 60  0001 C CNN "Part No."
F 5 "np" H 10250 5900 60  0001 C CNN "Link"
	1    10250 5900
	1    0    0    -1  
$EndComp
$Comp
L Device:R R502
U 1 1 5C740B77
P 10550 5900
F 0 "R502" V 10450 5900 50  0000 C CNN
F 1 "0" V 10550 5900 50  0000 C CNN
F 2 "Resistors_SMD:Zero_Ohm_0805_HandSoldering" H 10480 5900 50  0001 C CNN
F 3 "" H 10550 5900 50  0000 C CNN
F 4 "np" H 10550 5900 60  0001 C CNN "Part No."
F 5 "np" H 10550 5900 60  0001 C CNN "Link"
	1    10550 5900
	-1   0    0    1   
$EndComp
Wire Wire Line
	10100 5700 10250 5700
Wire Wire Line
	10550 5700 10550 5750
Wire Wire Line
	10100 6100 10250 6100
Wire Wire Line
	10550 6100 10550 6050
Wire Wire Line
	10250 5800 10250 5700
Connection ~ 10250 5700
Wire Wire Line
	10250 6000 10250 6100
Connection ~ 10250 6100
Wire Wire Line
	10100 5700 10100 5800
Wire Wire Line
	10100 6000 10100 6100
Connection ~ 10550 6100
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L0 #PWR169
U 1 1 5C7424F7
P 10550 3750
F 0 "#PWR169" H 10550 3500 50  0001 C CNN
F 1 "GND_L0" H 10550 3600 50  0000 C CNN
F 2 "" H 10550 3750 50  0000 C CNN
F 3 "" H 10550 3750 50  0000 C CNN
	1    10550 3750
	0    -1   -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L1 #PWR174
U 1 1 5C7426BB
P 10650 6100
F 0 "#PWR174" H 10650 5850 50  0001 C CNN
F 1 "GND_L1" H 10650 5950 50  0000 C CNN
F 2 "" H 10650 6100 50  0000 C CNN
F 3 "" H 10650 6100 50  0000 C CNN
	1    10650 6100
	0    -1   -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:CONN_01X01 J501
U 1 1 5C7435B3
P 10150 3550
F 0 "J501" H 10150 3650 50  0000 C CNN
F 1 "GND" V 10250 3550 50  0001 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01" H 10150 3550 50  0001 C CNN
F 3 "" H 10150 3550 50  0001 C CNN
F 4 "np" H 10150 3550 60  0001 C CNN "Part No."
F 5 "np" H 10150 3550 60  0001 C CNN "Link"
	1    10150 3550
	0    -1   -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:CONN_01X01 J502
U 1 1 5C7439B5
P 10150 4350
F 0 "J502" H 10150 4450 50  0000 C CNN
F 1 "DET" V 10250 4350 50  0001 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01_circle_pad" H 10150 4350 50  0001 C CNN
F 3 "" H 10150 4350 50  0001 C CNN
F 4 "np" H 10150 4350 60  0001 C CNN "Part No."
F 5 "np" H 10150 4350 60  0001 C CNN "Link"
	1    10150 4350
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:CONN_01X01 J503
U 1 1 5C743B2D
P 10250 5500
F 0 "J503" H 10250 5600 50  0000 C CNN
F 1 "DET" V 10350 5500 50  0001 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01_circle_pad" H 10250 5500 50  0001 C CNN
F 3 "" H 10250 5500 50  0001 C CNN
F 4 "np" H 10250 5500 60  0001 C CNN "Part No."
F 5 "np" H 10250 5500 60  0001 C CNN "Link"
	1    10250 5500
	0    -1   -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:CONN_01X01 J504
U 1 1 5C743CAE
P 10250 6300
F 0 "J504" H 10250 6400 50  0000 C CNN
F 1 "GND" V 10350 6300 50  0001 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01" H 10250 6300 50  0001 C CNN
F 3 "" H 10250 6300 50  0001 C CNN
F 4 "np" H 10250 6300 60  0001 C CNN "Part No."
F 5 "np" H 10250 6300 60  0001 C CNN "Link"
	1    10250 6300
	0    1    1    0   
$EndComp
Text GLabel 8900 1450 3    39   Input ~ 0
3V3_PWR_ENABLE
$Comp
L power:+12V #PWR166
U 1 1 5C785C39
P 9400 950
F 0 "#PWR166" H 9400 800 50  0001 C CNN
F 1 "+12V" H 9400 1090 50  0000 C CNN
F 2 "" H 9400 950 50  0000 C CNN
F 3 "" H 9400 950 50  0000 C CNN
	1    9400 950 
	1    0    0    -1  
$EndComp
Wire Wire Line
	9350 1100 9400 1100
Wire Wire Line
	9400 950  9400 1100
Connection ~ 9400 1100
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L1 #PWR104
U 1 1 5C78B16C
P 1450 7500
F 0 "#PWR104" H 1450 7250 50  0001 C CNN
F 1 "GND_L1" H 1450 7350 50  0000 C CNN
F 2 "" H 1450 7500 50  0000 C CNN
F 3 "" H 1450 7500 50  0000 C CNN
	1    1450 7500
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L0 #PWR105
U 1 1 5C78B9DA
P 1750 7500
F 0 "#PWR105" H 1750 7250 50  0001 C CNN
F 1 "GND_L0" H 1750 7350 50  0000 C CNN
F 2 "" H 1750 7500 50  0000 C CNN
F 3 "" H 1750 7500 50  0000 C CNN
	1    1750 7500
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG101
U 1 1 5C78BCFE
P 1450 7500
F 0 "#FLG101" H 1450 7595 50  0001 C CNN
F 1 "PWR_FLAG" H 1450 7680 50  0000 C CNN
F 2 "" H 1450 7500 50  0000 C CNN
F 3 "" H 1450 7500 50  0000 C CNN
	1    1450 7500
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG102
U 1 1 5C78BE63
P 1750 7500
F 0 "#FLG102" H 1750 7595 50  0001 C CNN
F 1 "PWR_FLAG" H 1750 7680 50  0000 C CNN
F 2 "" H 1750 7500 50  0000 C CNN
F 3 "" H 1750 7500 50  0000 C CNN
	1    1750 7500
	1    0    0    -1  
$EndComp
Text Notes 10000 3450 0    39   ~ 0
Strain Relief Board #1
Text Notes 10100 5300 0    39   ~ 0
Strain Relief Board #2
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR143
U 1 1 5C7CA2AA
P 5900 2150
F 0 "#PWR143" H 5900 1900 50  0001 C CNN
F 1 "GND_L8" H 5900 2000 50  0001 C CNN
F 2 "" H 5900 2150 50  0000 C CNN
F 3 "" H 5900 2150 50  0000 C CNN
	1    5900 2150
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C144
U 1 1 5C7D0449
P 4800 2500
F 0 "C144" V 4650 2450 50  0000 L CNN
F 1 "1nF" V 4900 2550 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4800 2500 50  0001 C CNN
F 3 "" H 4800 2500 50  0000 C CNN
F 4 "490-6445-1-ND" H 4800 2500 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=490-6445-1-ND\">Link</a>" H 4800 2500 60  0001 C CNN "Link"
	1    4800 2500
	0    1    1    0   
$EndComp
Wire Wire Line
	5600 1400 5600 1500
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR139
U 1 1 5C7D9EF9
P 5750 3550
F 0 "#PWR139" H 5750 3300 50  0001 C CNN
F 1 "GND_L8" H 5750 3400 50  0001 C CNN
F 2 "" H 5750 3550 50  0000 C CNN
F 3 "" H 5750 3550 50  0000 C CNN
	1    5750 3550
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:TVS D103
U 1 1 5CB32928
P 8650 4550
F 0 "D103" H 8650 4400 50  0000 C CNN
F 1 "5V" H 8650 4300 50  0000 C CNN
F 2 "Diodes_SMD:SOD-323F_Handsolder" H 8650 4550 50  0001 C CNN
F 3 "" H 8650 4550 50  0000 C CNN
F 4 "641-1219-1-ND" H 8650 4550 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/product-detail/en/comchip-technology/CPDFR5V0/641-1219-1-ND/1678637\">Link</a>" H 8650 4550 60  0001 C CNN "Link"
	1    8650 4550
	-1   0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR155
U 1 1 5CB333A3
P 8350 4550
F 0 "#PWR155" H 8350 4300 50  0001 C CNN
F 1 "GND_L8" H 8350 4400 50  0001 C CNN
F 2 "" H 8350 4550 50  0000 C CNN
F 3 "" H 8350 4550 50  0000 C CNN
	1    8350 4550
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR156
U 1 1 5CB334EC
P 8350 5800
F 0 "#PWR156" H 8350 5550 50  0001 C CNN
F 1 "GND_L8" H 8350 5650 50  0001 C CNN
F 2 "" H 8350 5800 50  0000 C CNN
F 3 "" H 8350 5800 50  0000 C CNN
	1    8350 5800
	0    1    1    0   
$EndComp
Connection ~ 9100 5800
Connection ~ 9100 4550
$Comp
L ARDF-2-Band-MiniTx-rescue:TVS D104
U 1 1 5CB36EAB
P 8650 5800
F 0 "D104" H 8600 6050 50  0000 C CNN
F 1 "5V" H 8600 5950 50  0000 C CNN
F 2 "Diodes_SMD:SOD-323F_Handsolder" H 8650 5800 50  0001 C CNN
F 3 "" H 8650 5800 50  0000 C CNN
F 4 "641-1219-1-ND" H 8650 5800 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/product-detail/en/comchip-technology/CPDFR5V0/641-1219-1-ND/1678637\">Link</a>" H 8650 5800 60  0001 C CNN "Link"
	1    8650 5800
	-1   0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:INDUCTOR_CT L104
U 1 1 5CB66258
P 4700 5900
F 0 "L104" V 4850 5900 50  0000 C CNN
F 1 "9T #24 Bifilar T50-43" V 4850 5800 50  0001 C CNN
F 2 "Inductors:Toroid_T-50A_TH_BF_CT_Vertical" H 4700 5900 50  0001 C CNN
F 3 "" H 4700 5900 50  0000 C CNN
F 4 "Amidon FT50A-43" H 4700 5900 60  0001 C CNN "Part No."
F 5 "<a href=\"http://www.amidoncorp.com/ft-50A-43/\">Link</a>" H 4700 5900 60  0001 C CNN "Link"
	1    4700 5900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4350 5900 4400 5900
Wire Wire Line
	4700 6000 4700 6100
Wire Wire Line
	5000 5900 5450 5900
Wire Wire Line
	9300 5500 9100 5500
Connection ~ 9100 5500
Text Notes 6000 6450 0    39   ~ 0
L106 & L108:\n20T (46 cm) #24 T60-2 70% Coverage
Text Notes 4900 5450 0    39   ~ 0
L104:\n9T (24 cm) #24 Bifilar FT-50A-43 or\n6T #24 Bifilar FT-50-77
Wire Wire Line
	7700 5750 7700 5900
Connection ~ 7700 5900
$Comp
L ARDF-2-Band-MiniTx-rescue:MBD54DWT1G_Schottky_Ind D303
U 2 1 5CE0AD33
P 9350 4800
F 0 "D303" H 9350 4900 50  0000 C CNN
F 1 "MBD54DWT1G_Schottky_Ind" H 9375 5075 50  0001 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-363_Handsolder" H 9350 4800 50  0001 C CNN
F 3 "" H 9350 4800 50  0000 C CNN
F 4 "MBD54DWT1GOSCT-ND" H 9350 4800 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=MBD54DWT1GOSCT-ND\">Link</a>" H 9350 4800 60  0001 C CNN "Link"
	2    9350 4800
	-1   0    0    -1  
$EndComp
Connection ~ 10450 3750
$Comp
L Device:C_Small C503
U 1 1 5CE67389
P 5600 7250
F 0 "C503" H 5610 7320 50  0000 L CNN
F 1 "np" H 5610 7170 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5600 7250 50  0001 C CNN
F 3 "" H 5600 7250 50  0000 C CNN
F 4 "np" H 5600 7250 60  0001 C CNN "Part No."
F 5 "np" H 5600 7250 60  0001 C CNN "Link"
	1    5600 7250
	1    0    0    -1  
$EndComp
$Comp
L Device:R R503
U 1 1 5CE67391
P 5900 7250
F 0 "R503" V 5800 7250 50  0000 C CNN
F 1 "0" V 5900 7250 50  0000 C CNN
F 2 "Resistors_SMD:Zero_Ohm_0805_HandSoldering" H 5830 7250 50  0001 C CNN
F 3 "" H 5900 7250 50  0000 C CNN
F 4 "np" H 5900 7250 60  0001 C CNN "Part No."
F 5 "np" H 5900 7250 60  0001 C CNN "Link"
	1    5900 7250
	-1   0    0    1   
$EndComp
Wire Wire Line
	5900 7050 5900 7100
Wire Wire Line
	5900 7450 5900 7400
Wire Wire Line
	5600 7150 5600 7050
Connection ~ 5600 7050
Wire Wire Line
	5600 7350 5600 7450
Connection ~ 5600 7450
Connection ~ 5900 7450
$Comp
L ARDF-2-Band-MiniTx-rescue:CONN_01X01 J505
U 1 1 5CE673AA
P 5600 6850
F 0 "J505" H 5600 6950 50  0000 C CNN
F 1 "DET" V 5700 6850 50  0001 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01_circle_pad" H 5600 6850 50  0001 C CNN
F 3 "" H 5600 6850 50  0001 C CNN
F 4 "np" H 5600 6850 60  0001 C CNN "Part No."
F 5 "np" H 5600 6850 60  0001 C CNN "Link"
	1    5600 6850
	0    -1   -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:CONN_01X01 J506
U 1 1 5CE673B2
P 5600 7650
F 0 "J506" H 5600 7750 50  0000 C CNN
F 1 "GND" V 5700 7650 50  0001 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01" H 5600 7650 50  0001 C CNN
F 3 "" H 5600 7650 50  0001 C CNN
F 4 "np" H 5600 7650 60  0001 C CNN "Part No."
F 5 "np" H 5600 7650 60  0001 C CNN "Link"
	1    5600 7650
	0    1    1    0   
$EndComp
Text Notes 5500 6750 0    39   ~ 0
Strain Relief Board #3
Wire Wire Line
	5600 7050 5900 7050
Wire Wire Line
	5600 7450 5900 7450
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L2 #PWR145
U 1 1 5CE67DE1
P 6000 7450
F 0 "#PWR145" H 6000 7200 50  0001 C CNN
F 1 "GND_L2" H 6000 7300 50  0000 C CNN
F 2 "" H 6000 7450 50  0000 C CNN
F 3 "" H 6000 7450 50  0000 C CNN
	1    6000 7450
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C504
U 1 1 5CE68F7C
P 6350 7250
F 0 "C504" H 6360 7320 50  0000 L CNN
F 1 "np" H 6360 7170 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 6350 7250 50  0001 C CNN
F 3 "" H 6350 7250 50  0000 C CNN
F 4 "np" H 6350 7250 60  0001 C CNN "Part No."
F 5 "np" H 6350 7250 60  0001 C CNN "Link"
	1    6350 7250
	1    0    0    -1  
$EndComp
$Comp
L Device:R R504
U 1 1 5CE68F84
P 6650 7250
F 0 "R504" V 6550 7250 50  0000 C CNN
F 1 "0" V 6650 7250 50  0000 C CNN
F 2 "Resistors_SMD:Zero_Ohm_0805_HandSoldering" H 6580 7250 50  0001 C CNN
F 3 "" H 6650 7250 50  0000 C CNN
F 4 "np" H 6650 7250 60  0001 C CNN "Part No."
F 5 "np" H 6650 7250 60  0001 C CNN "Link"
	1    6650 7250
	-1   0    0    1   
$EndComp
Wire Wire Line
	6650 7050 6650 7100
Wire Wire Line
	6650 7450 6650 7400
Wire Wire Line
	6350 7150 6350 7050
Connection ~ 6350 7050
Wire Wire Line
	6350 7350 6350 7450
Connection ~ 6350 7450
Connection ~ 6650 7450
$Comp
L ARDF-2-Band-MiniTx-rescue:CONN_01X01 J507
U 1 1 5CE68F93
P 6350 6850
F 0 "J507" H 6350 6950 50  0000 C CNN
F 1 "DET" V 6450 6850 50  0001 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01_circle_pad" H 6350 6850 50  0001 C CNN
F 3 "" H 6350 6850 50  0001 C CNN
F 4 "np" H 6350 6850 60  0001 C CNN "Part No."
F 5 "np" H 6350 6850 60  0001 C CNN "Link"
	1    6350 6850
	0    -1   -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:CONN_01X01 J508
U 1 1 5CE68F9B
P 6350 7650
F 0 "J508" H 6350 7750 50  0000 C CNN
F 1 "GND" V 6450 7650 50  0001 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01" H 6350 7650 50  0001 C CNN
F 3 "" H 6350 7650 50  0001 C CNN
F 4 "np" H 6350 7650 60  0001 C CNN "Part No."
F 5 "np" H 6350 7650 60  0001 C CNN "Link"
	1    6350 7650
	0    1    1    0   
$EndComp
Text Notes 6250 6750 0    39   ~ 0
Strain Relief Board #4
Wire Wire Line
	6350 7050 6650 7050
Wire Wire Line
	6350 7450 6650 7450
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L3 #PWR149
U 1 1 5CE692BA
P 6750 7450
F 0 "#PWR149" H 6750 7200 50  0001 C CNN
F 1 "GND_L3" H 6750 7300 50  0000 C CNN
F 2 "" H 6750 7450 50  0000 C CNN
F 3 "" H 6750 7450 50  0000 C CNN
	1    6750 7450
	0    -1   -1   0   
$EndComp
Wire Notes Line
	5450 7750 6950 7750
Wire Notes Line
	5450 6750 6950 6750
Wire Notes Line
	5450 6750 5450 7750
Wire Notes Line
	6200 6750 6200 7750
Wire Notes Line
	6950 6750 6950 7750
Wire Notes Line
	9950 3450 10750 3450
Wire Notes Line
	9950 4450 10750 4450
Wire Notes Line
	10750 4450 10750 3450
Wire Notes Line
	9950 3450 9950 4450
Wire Notes Line
	10050 5400 10850 5400
Wire Notes Line
	10050 6400 10850 6400
Wire Notes Line
	10850 6400 10850 5400
Wire Notes Line
	10050 5400 10050 6400
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L2 #PWR108
U 1 1 5CE8DCED
P 2050 7550
F 0 "#PWR108" H 2050 7300 50  0001 C CNN
F 1 "GND_L2" H 2050 7400 50  0000 C CNN
F 2 "" H 2050 7550 50  0000 C CNN
F 3 "" H 2050 7550 50  0000 C CNN
	1    2050 7550
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L3 #PWR112
U 1 1 5CE8DEB4
P 2350 7550
F 0 "#PWR112" H 2350 7300 50  0001 C CNN
F 1 "GND_L3" H 2350 7400 50  0000 C CNN
F 2 "" H 2350 7550 50  0000 C CNN
F 3 "" H 2350 7550 50  0000 C CNN
	1    2350 7550
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG103
U 1 1 5CE8E00C
P 2050 7550
F 0 "#FLG103" H 2050 7645 50  0001 C CNN
F 1 "PWR_FLAG" H 2050 7730 50  0000 C CNN
F 2 "" H 2050 7550 50  0000 C CNN
F 3 "" H 2050 7550 50  0000 C CNN
	1    2050 7550
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG104
U 1 1 5CE8E164
P 2350 7550
F 0 "#FLG104" H 2350 7645 50  0001 C CNN
F 1 "PWR_FLAG" H 2350 7730 50  0000 C CNN
F 2 "" H 2350 7550 50  0000 C CNN
F 3 "" H 2350 7550 50  0000 C CNN
	1    2350 7550
	1    0    0    -1  
$EndComp
Wire Wire Line
	8900 1150 8900 1250
$Comp
L Device:R R101
U 1 1 5E0F9379
P 8900 1000
F 0 "R101" V 8800 1000 50  0000 C CNN
F 1 "49.9k" V 8900 1000 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 8830 1000 50  0001 C CNN
F 3 "" H 8900 1000 50  0000 C CNN
F 4 "311-49.9KCRCT-ND" H 8900 1000 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-49.9KCRCT-ND\">Link</a>" H 8900 1000 60  0001 C CNN "Link"
	1    8900 1000
	-1   0    0    1   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR158
U 1 1 5E0F9511
P 8850 750
F 0 "#PWR158" H 8850 500 50  0001 C CNN
F 1 "GND_L8" H 8850 600 50  0001 C CNN
F 2 "" H 8850 750 50  0000 C CNN
F 3 "" H 8850 750 50  0000 C CNN
	1    8850 750 
	0    1    1    0   
$EndComp
Wire Wire Line
	8850 750  8900 750 
Wire Wire Line
	8900 750  8900 850 
Connection ~ 8900 1250
Wire Wire Line
	8800 3400 8800 3950
Wire Wire Line
	8800 3950 9150 3950
$Comp
L ARDF-2-Band-MiniTx-rescue:AFT05MS004N Q103
U 1 1 5EC32961
P 5650 3350
F 0 "Q103" H 5850 3300 61  0000 L CNB
F 1 "AFT05MS004N" H 5850 3200 61  0000 L CNB
F 2 "TO_SOT_Packages_SMD:SOT89-3_Housing_Handsoldering" H 5850 3275 50  0001 L CIN
F 3 "" H 5650 3350 50  0001 L CNN
F 4 "AFT05MS004NT1CT-ND" H 5650 3350 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=AFT05MS004NT1CT-ND\">Link</a>" H 5650 3350 60  0001 C CNN "Link"
	1    5650 3350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R104
U 1 1 5EC32FD0
P 5050 3100
F 0 "R104" V 4950 3100 50  0000 C CNN
F 1 "51" V 5050 3100 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 4980 3100 50  0001 C CNN
F 3 "" H 5050 3100 50  0000 C CNN
F 4 "311-51.0CRCT-ND" H 5050 3100 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-51.0CRCT-ND\">Link</a>" H 5050 3100 60  0001 C CNN "Link"
	1    5050 3100
	-1   0    0    1   
$EndComp
Wire Wire Line
	5050 3250 5050 3400
Connection ~ 5050 3400
$Comp
L Device:C_Small C106
U 1 1 5EC33F13
P 4850 2850
F 0 "C106" V 4750 2800 50  0000 L CNN
F 1 "10nF" V 5000 2750 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4850 2850 50  0001 C CNN
F 3 "" H 4850 2850 50  0000 C CNN
F 4 "311-1136-1-ND" H 4850 2850 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-1136-1-ND\">Link</a>" H 4850 2850 60  0001 C CNN "Link"
	1    4850 2850
	0    -1   1    0   
$EndComp
$Comp
L Device:C_Small C102
U 1 1 5EC34079
P 2600 2050
F 0 "C102" V 2500 2000 50  0000 L CNN
F 1 "10uF" V 2750 1950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2600 2050 50  0001 C CNN
F 3 "" H 2600 2050 50  0000 C CNN
F 4 "490-5523-1-ND" H 2600 2050 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=490-5523-1-ND\">Link</a>" H 2600 2050 60  0001 C CNN "Link"
	1    2600 2050
	1    0    0    1   
$EndComp
Wire Wire Line
	5050 2850 4950 2850
Connection ~ 5050 2850
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR137
U 1 1 5EC349CF
P 4750 2850
F 0 "#PWR137" H 4750 2600 50  0001 C CNN
F 1 "GND_L8" H 4750 2700 50  0001 C CNN
F 2 "" H 4750 2850 50  0000 C CNN
F 3 "" H 4750 2850 50  0000 C CNN
	1    4750 2850
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR135
U 1 1 5EC35511
P 4700 2500
F 0 "#PWR135" H 4700 2250 50  0001 C CNN
F 1 "GND_L8" H 4700 2350 50  0001 C CNN
F 2 "" H 4700 2500 50  0000 C CNN
F 3 "" H 4700 2500 50  0000 C CNN
	1    4700 2500
	0    1    -1   0   
$EndComp
$Comp
L Device:R R105
U 1 1 5EC35D87
P 5400 3100
F 0 "R105" V 5300 3100 50  0000 C CNN
F 1 "270" V 5400 3100 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 5330 3100 50  0001 C CNN
F 3 "" H 5400 3100 50  0000 C CNN
F 4 "311-270CRCT-ND" H 5400 3100 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-270CRCT-ND\">Link</a>" H 5400 3100 60  0001 C CNN "Link"
	1    5400 3100
	-1   0    0    1   
$EndComp
Wire Wire Line
	5750 3150 5750 2950
Wire Wire Line
	5500 2950 5400 2950
Wire Wire Line
	5400 3250 5400 3400
Connection ~ 5400 3400
Wire Wire Line
	5950 2950 5950 2700
Connection ~ 5750 2950
Text GLabel 1450 1700 0    39   Input ~ 0
DI_SCL
Text GLabel 1450 1800 0    39   Input ~ 0
DI_SDA
Wire Wire Line
	2600 750  2600 800 
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR109
U 1 1 5EC3A4C3
P 2200 800
F 0 "#PWR109" H 2200 550 50  0001 C CNN
F 1 "GND_L8" H 2200 650 50  0001 C CNN
F 2 "" H 2200 800 50  0000 C CNN
F 3 "" H 2200 800 50  0000 C CNN
	1    2200 800 
	0    1    1    0   
$EndComp
Wire Wire Line
	2400 800  2450 800 
Connection ~ 2600 800 
$Comp
L ARDF-2-Band-MiniTx-rescue:TEST_SCOPE W103
U 1 1 5EC3B60C
P 6550 2600
F 0 "W103" V 6600 2950 50  0000 C CNN
F 1 "P3" V 6600 2650 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 6750 2600 50  0001 C CNN
F 3 "" H 6750 2600 50  0001 C CNN
F 4 "np" H 6550 2600 60  0001 C CNN "Part No."
F 5 "np" H 6550 2600 60  0001 C CNN "Link"
	1    6550 2600
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR147
U 1 1 5EC3B8EF
P 6550 2700
F 0 "#PWR147" H 6550 2450 50  0001 C CNN
F 1 "GND_L8" H 6550 2550 50  0001 C CNN
F 2 "" H 6550 2700 50  0000 C CNN
F 3 "" H 6550 2700 50  0000 C CNN
	1    6550 2700
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:INDUCTOR_SMALL L101
U 1 1 5EC3DD68
P 7300 3400
F 0 "L101" H 7300 3500 50  0000 C CNN
F 1 "82nH" H 7300 3350 50  0000 C CNN
F 2 "Inductors:Wurth_744912212" H 7300 3400 50  0001 C CNN
F 3 "" H 7300 3400 50  0000 C CNN
F 4 "732-5643-1-ND" H 7300 3400 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=732-5643-1-ND\">Link</a>" H 7300 3400 60  0001 C CNN "Link"
	1    7300 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 3400 7000 3400
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR153
U 1 1 5EC3E2C1
P 7800 3850
F 0 "#PWR153" H 7800 3600 50  0001 C CNN
F 1 "GND_L8" H 7800 3700 50  0001 C CNN
F 2 "" H 7800 3850 50  0000 C CNN
F 3 "" H 7800 3850 50  0000 C CNN
F 4 "Value" H 7800 3850 60  0001 C CNN "Part No."
F 5 "<a href=\"\">Link</a>" H 7800 3850 60  0001 C CNN "Link"
	1    7800 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 3850 7800 3800
$Comp
L Device:C_Small C112
U 1 1 5EC3E2CA
P 7800 3700
F 0 "C112" H 7810 3770 50  0000 L CNN
F 1 "33pF" H 7810 3620 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 7800 3700 50  0001 C CNN
F 3 "" H 7800 3700 50  0000 C CNN
F 4 "399-8061-1-ND" H 7800 3700 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-8061-1-ND\">Link</a>" H 7800 3700 60  0001 C CNN "Link"
	1    7800 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	7800 3600 7800 3400
Text Notes 7500 3000 0    39   ~ 0
5th order Chebychev\n148MHz 3dB cut-off
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR103
U 1 1 5ECE769F
P 1450 1600
F 0 "#PWR103" H 1450 1350 50  0001 C CNN
F 1 "GND_L8" H 1450 1450 50  0001 C CNN
F 2 "" H 1450 1600 50  0000 C CNN
F 3 "" H 1450 1600 50  0000 C CNN
	1    1450 1600
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR111
U 1 1 5ECE77E8
P 2350 1800
F 0 "#PWR111" H 2350 1550 50  0001 C CNN
F 1 "GND_L8" H 2350 1650 50  0001 C CNN
F 2 "" H 2350 1800 50  0000 C CNN
F 3 "" H 2350 1800 50  0000 C CNN
	1    2350 1800
	0    -1   1    0   
$EndComp
$Comp
L Device:C_Small C108
U 1 1 5ECE9E9C
P 5600 2950
F 0 "C108" V 5450 2900 50  0000 L CNN
F 1 "1nF" V 5700 2950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5600 2950 50  0001 C CNN
F 3 "" H 5600 2950 50  0000 C CNN
F 4 "490-6445-1-ND" H 5600 2950 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=490-6445-1-ND\">Link</a>" H 5600 2950 60  0001 C CNN "Link"
	1    5600 2950
	0    1    1    0   
$EndComp
Connection ~ 7000 3400
Connection ~ 5600 1850
Connection ~ 5600 2150
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR132
U 1 1 5C0E36D1
P 4200 3150
F 0 "#PWR132" H 4200 2900 50  0001 C CNN
F 1 "GND_L8" H 4200 3000 50  0001 C CNN
F 2 "" H 4200 3150 50  0000 C CNN
F 3 "" H 4200 3150 50  0000 C CNN
	1    4200 3150
	0    1    1    0   
$EndComp
Wire Wire Line
	5700 2150 5600 2150
$Comp
L Device:C_Small C111
U 1 1 5EE1E9C9
P 5800 1500
F 0 "C111" V 5650 1450 50  0000 L CNN
F 1 "1nF" V 5900 1400 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5800 1500 50  0001 C CNN
F 3 "" H 5800 1500 50  0000 C CNN
F 4 "490-6445-1-ND" H 5800 1500 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=490-6445-1-ND\">Link</a>" H 5800 1500 60  0001 C CNN "Link"
	1    5800 1500
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR141
U 1 1 5EE1E9CF
P 5900 1500
F 0 "#PWR141" H 5900 1250 50  0001 C CNN
F 1 "GND_L8" H 5900 1350 50  0001 C CNN
F 2 "" H 5900 1500 50  0000 C CNN
F 3 "" H 5900 1500 50  0000 C CNN
	1    5900 1500
	0    -1   -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:INDUCTOR_SMALL L107
U 1 1 5F273274
P 4600 3400
F 0 "L107" H 4600 3500 50  0000 C CNN
F 1 "8nH" H 4600 3350 50  0000 C CNN
F 2 "Inductors:Wurth_744913080" H 4600 3400 50  0001 C CNN
F 3 "" H 4600 3400 50  0000 C CNN
F 4 "732-7281-1-ND" H 4600 3400 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=732-7281-1-ND\">Link</a>" H 4600 3400 60  0001 C CNN "Link"
	1    4600 3400
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:INDUCTOR_SMALL L109
U 1 1 5F274D05
P 6700 3400
F 0 "L109" H 6700 3500 50  0000 C CNN
F 1 "13nH" H 6700 3350 50  0000 C CNN
F 2 "Inductors:Wurth_744911112" H 6700 3400 50  0001 C CNN
F 3 "" H 6700 3400 50  0000 C CNN
F 4 "732-6192-1-ND" H 6700 3400 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=732-6192-1-ND\">Link</a>" H 6700 3400 60  0001 C CNN "Link"
	1    6700 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 2950 5750 2950
Wire Wire Line
	6450 3400 6350 3400
Wire Wire Line
	6350 3400 6350 3250
$Comp
L Device:C_Small C118
U 1 1 5F27842E
P 6350 3150
F 0 "C118" V 6250 3100 50  0000 L CNN
F 1 "100nF" V 6500 3050 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 6350 3150 50  0001 C CNN
F 3 "" H 6350 3150 50  0000 C CNN
F 4 "478-1395-1-ND" H 6350 3150 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 6350 3150 60  0001 C CNN "Link"
	1    6350 3150
	-1   0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:+3V3_L8 #PWR114
U 1 1 5F78DF45
P 2600 750
F 0 "#PWR114" H 2600 600 50  0001 C CNN
F 1 "+3V3_L8" H 2600 890 50  0000 C CNN
F 2 "" H 2600 750 50  0000 C CNN
F 3 "" H 2600 750 50  0000 C CNN
	1    2600 750 
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:INDUCTOR_SMALL L105
U 1 1 5F8210E2
P 8250 3400
F 0 "L105" H 8250 3500 50  0000 C CNN
F 1 "120nH" H 8250 3350 50  0000 C CNN
F 2 "Inductors:Wurth_744912212" H 8250 3400 50  0001 C CNN
F 3 "" H 8250 3400 50  0000 C CNN
F 4 "732-5645-1-ND" H 8250 3400 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=732-5645-1-ND\">Link</a>" H 8250 3400 60  0001 C CNN "Link"
	1    8250 3400
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:INDUCTOR_CT L103
U 1 1 5F821AF3
P 5950 2600
F 0 "L103" V 6100 2600 50  0000 C CNN
F 1 "7T #24 Bifilar T50-0" V 6100 2500 50  0001 C CNN
F 2 "Inductors:Toroid_T-50A_TH_BF_CT_Vertical" H 5950 2600 50  0001 C CNN
F 3 "" H 5950 2600 50  0000 C CNN
F 4 "Amidon T50-0" H 5950 2600 60  0001 C CNN "Part No."
F 5 "<a href=\"http://www.amidoncorp.com/t-50-0/\">Link</a>" H 5950 2600 60  0001 C CNN "Link"
	1    5950 2600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4900 2500 5050 2500
Connection ~ 5050 2500
Wire Wire Line
	5700 1500 5600 1500
Connection ~ 5600 1500
Wire Wire Line
	5700 1850 5600 1850
Wire Wire Line
	5600 2600 5650 2600
Wire Wire Line
	6250 2600 6350 2600
Wire Wire Line
	6350 2600 6350 3050
Connection ~ 6350 2600
$Comp
L ARDF-2-Band-MiniTx-rescue:DAC081C085 U101
U 1 1 5F828F71
P 1850 1650
F 0 "U101" H 1850 2150 61  0000 L CNB
F 1 "DAC081C085" H 1650 2000 61  0000 L CNB
F 2 "SMD_Packages:SSOP-8_Handsolder" H 1850 1650 50  0001 C CIN
F 3 "" H 1850 1650 50  0000 C CNN
F 4 "DAC081C085CIMM%2FNOPBCT-ND" H 1850 1650 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=DAC081C085CIMM%2FNOPBCT-ND\">Link</a>" H 1850 1650 60  0001 C CNN "Link"
	1    1850 1650
	1    0    0    -1  
$EndComp
NoConn ~ 1450 1500
$Comp
L ARDF-2-Band-MiniTx-rescue:INDUCTOR_SMALL L110
U 1 1 5F82A3E0
P 2600 1150
F 0 "L110" V 2500 1300 50  0000 C CNN
F 1 "10uH" V 2650 1300 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 2600 1150 50  0001 C CNN
F 3 "" H 2600 1150 50  0000 C CNN
F 4 "445-6396-1-ND" H 2600 1150 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=445-6396-1-ND\">Link</a>" H 2600 1150 60  0001 C CNN "Link"
	1    2600 1150
	0    1    1    0   
$EndComp
Wire Wire Line
	2350 1700 2450 1700
Wire Wire Line
	2450 1700 2450 800 
Connection ~ 2450 800 
Wire Wire Line
	2600 1400 2600 1600
Wire Wire Line
	2350 1600 2600 1600
Connection ~ 2600 1600
$Comp
L Device:C_Small C101
U 1 1 5F82BF8A
P 2300 800
F 0 "C101" V 2200 750 50  0000 L CNN
F 1 "10uF" V 2450 700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2300 800 50  0001 C CNN
F 3 "" H 2300 800 50  0000 C CNN
F 4 "490-5523-1-ND" H 2300 800 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=490-5523-1-ND\">Link</a>" H 2300 800 60  0001 C CNN "Link"
	1    2300 800 
	0    -1   1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR120
U 1 1 5A7DF174
P 3150 6750
F 0 "#PWR120" H 3150 6500 50  0001 C CNN
F 1 "GND_L8" H 3150 6600 50  0001 C CNN
F 2 "" H 3150 6750 50  0000 C CNN
F 3 "" H 3150 6750 50  0000 C CNN
	1    3150 6750
	1    0    0    -1  
$EndComp
NoConn ~ 3450 6550
Wire Wire Line
	8550 3600 8550 3400
Text Notes 6100 2400 0    39   ~ 0
L103:\n7T (24 cm) #24 Bifilar T50-0
NoConn ~ 10250 1250
NoConn ~ 10250 2600
$Comp
L ARDF-2-Band-MiniTx-rescue:D_Schottky_x2_KCom_Dual D301
U 2 1 5F8505B7
P 9100 5250
F 0 "D301" H 9100 5400 50  0000 C CNN
F 1 "D_Schottky_x2_KCom_Dual" H 9100 5350 50  0001 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-363_Handsolder" H 9100 5250 50  0001 C CNN
F 3 "" H 9100 5250 50  0000 C CNN
F 4 "BAT54CDW-FDICT-ND" H 9100 5250 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=BAT54CDW-FDICT-ND\">Link</a>" H 9100 5250 60  0001 C CNN "Link"
	2    9100 5250
	1    0    0    -1  
$EndComp
NoConn ~ 8850 5250
Wire Wire Line
	9100 5400 9100 5500
$Comp
L Device:R R102
U 1 1 5F857863
P 5050 2150
F 0 "R102" V 4950 2150 50  0000 C CNN
F 1 "10" V 5050 2150 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 4980 2150 50  0001 C CNN
F 3 "" H 5050 2150 50  0000 C CNN
F 4 "311-10.0CRCT-ND" H 5050 2150 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0CRCT-ND\">Link</a>" H 5050 2150 60  0001 C CNN "Link"
	1    5050 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2350 1500 2900 1500
$Comp
L Device:Jumper_NO_Small J101
U 1 1 5F87164F
P 9000 4250
F 0 "J101" H 9000 4330 50  0000 C CNN
F 1 "Jumper_NO_Small" H 9010 4190 50  0001 C CNN
F 2 "Wire_Connections_Bridges:Solder-Jumper-NO-SMD-Pad_Small" H 9000 4250 50  0001 C CNN
F 3 "" H 9000 4250 50  0000 C CNN
F 4 "np" H 9000 4250 60  0001 C CNN "Part No."
F 5 "np" H 9000 4250 60  0001 C CNN "Link"
	1    9000 4250
	1    0    0    -1  
$EndComp
$Comp
L Device:Jumper_NO_Small J102
U 1 1 5F871998
P 9000 5500
F 0 "J102" H 9000 5580 50  0000 C CNN
F 1 "Jumper_NO_Small" H 9010 5440 50  0001 C CNN
F 2 "Wire_Connections_Bridges:Solder-Jumper-NO-SMD-Pad_Small" H 9000 5500 50  0001 C CNN
F 3 "" H 9000 5500 50  0000 C CNN
F 4 "np" H 9000 5500 60  0001 C CNN "Part No."
F 5 "np" H 9000 5500 60  0001 C CNN "Link"
	1    9000 5500
	1    0    0    -1  
$EndComp
Connection ~ 9100 4250
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR160
U 1 1 5F871B53
P 8900 5500
F 0 "#PWR160" H 8900 5250 50  0001 C CNN
F 1 "GND_L8" H 8900 5350 50  0001 C CNN
F 2 "" H 8900 5500 50  0000 C CNN
F 3 "" H 8900 5500 50  0000 C CNN
	1    8900 5500
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR159
U 1 1 5F871C8A
P 8900 4250
F 0 "#PWR159" H 8900 4000 50  0001 C CNN
F 1 "GND_L8" H 8900 4100 50  0001 C CNN
F 2 "" H 8900 4250 50  0000 C CNN
F 3 "" H 8900 4250 50  0000 C CNN
	1    8900 4250
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:OPA171 U102
U 1 1 5F8738F9
P 4050 1600
F 0 "U102" H 4050 1850 61  0000 L CNB
F 1 "OPA171" H 4050 1750 61  0000 L CNB
F 2 "TO_SOT_Packages_SMD:SOT-23-5_Handsolder" H 4100 1800 50  0001 C CNN
F 3 "" H 4050 1850 50  0000 C CNN
	1    4050 1600
	1    0    0    -1  
$EndComp
$Comp
L Device:R R109
U 1 1 5F873A78
P 3950 2100
F 0 "R109" V 3850 2100 50  0000 C CNN
F 1 "7.5k" V 3950 2100 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 3880 2100 50  0001 C CNN
F 3 "" H 3950 2100 50  0000 C CNN
F 4 "311-7.50KCRCT-ND" H 3950 2100 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-7.50KCRCT-ND\">Link</a>" H 3950 2100 60  0001 C CNN "Link"
	1    3950 2100
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R107
U 1 1 5F873CFB
P 3450 1500
F 0 "R107" V 3350 1500 50  0000 C CNN
F 1 "11k" V 3450 1500 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 3380 1500 50  0001 C CNN
F 3 "" H 3450 1500 50  0000 C CNN
F 4 "311-11.0KCRCT-ND" H 3450 1500 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-11.0KCRCT-ND\">Link</a>" H 3450 1500 60  0001 C CNN "Link"
	1    3450 1500
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R103
U 1 1 5F873E4E
P 3050 1500
F 0 "R103" V 2950 1500 50  0000 C CNN
F 1 "16k" V 3050 1500 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 2980 1500 50  0001 C CNN
F 3 "" H 3050 1500 50  0000 C CNN
F 4 "311-16.0KCRCT-ND" H 3050 1500 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-16.0KCRCT-ND\">Link</a>" H 3050 1500 60  0001 C CNN "Link"
	1    3050 1500
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C107
U 1 1 5F873FB1
P 3450 1000
F 0 "C107" V 3300 950 50  0000 L CNN
F 1 "22nF" V 3550 900 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 3450 1000 50  0001 C CNN
F 3 "" H 3450 1000 50  0000 C CNN
F 4 "490-8308-1-ND" H 3450 1000 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=490-8308-1-ND\">Link</a>" H 3450 1000 60  0001 C CNN "Link"
	1    3450 1000
	0    1    1    0   
$EndComp
$Comp
L Device:R R108
U 1 1 5F874349
P 3500 1700
F 0 "R108" V 3400 1700 50  0000 C CNN
F 1 "15k" V 3500 1700 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 3430 1700 50  0001 C CNN
F 3 "" H 3500 1700 50  0000 C CNN
F 4 "311-15.0KCRCT-ND" H 3500 1700 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-15.0KCRCT-ND\">Link</a>" H 3500 1700 60  0001 C CNN "Link"
	1    3500 1700
	0    -1   -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR121
U 1 1 5F8744A7
P 3350 1700
F 0 "#PWR121" H 3350 1450 50  0001 C CNN
F 1 "GND_L8" H 3350 1550 50  0001 C CNN
F 2 "" H 3350 1700 50  0000 C CNN
F 3 "" H 3350 1700 50  0000 C CNN
	1    3350 1700
	0    1    -1   0   
$EndComp
Wire Wire Line
	3650 1700 3700 1700
Wire Wire Line
	3600 1500 3700 1500
Wire Wire Line
	3200 1500 3250 1500
Wire Wire Line
	3250 1000 3250 1500
Connection ~ 3250 1500
Wire Wire Line
	4650 1600 4650 1000
Wire Wire Line
	4350 1600 4450 1600
Wire Wire Line
	3800 2100 3700 2100
Wire Wire Line
	3700 2100 3700 1700
Connection ~ 3700 1700
Wire Wire Line
	4100 2100 4450 2100
Wire Wire Line
	4450 2100 4450 1600
Connection ~ 4450 1600
$Comp
L Device:C_Small C114
U 1 1 5F8751AB
P 3700 1350
F 0 "C114" V 3800 1250 50  0000 L CNN
F 1 "33nF" V 3600 1250 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 3700 1350 50  0001 C CNN
F 3 "" H 3700 1350 50  0000 C CNN
F 4 "445-7523-1-ND" H 3700 1350 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=445-7523-1-ND\">Link</a>" H 3700 1350 60  0001 C CNN "Link"
	1    3700 1350
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR128
U 1 1 5F875320
P 3950 1900
F 0 "#PWR128" H 3950 1650 50  0001 C CNN
F 1 "GND_L8" H 3950 1750 50  0001 C CNN
F 2 "" H 3950 1900 50  0000 C CNN
F 3 "" H 3950 1900 50  0000 C CNN
	1    3950 1900
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:+5V_L8 #PWR127
U 1 1 5F875542
P 3950 1200
F 0 "#PWR127" H 3950 1050 50  0001 C CNN
F 1 "+5V_L8" H 3950 1340 50  0000 C CNN
F 2 "" H 3950 1200 50  0000 C CNN
F 3 "" H 3950 1200 50  0000 C CNN
	1    3950 1200
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR125
U 1 1 5F8758EE
P 3700 1250
F 0 "#PWR125" H 3700 1000 50  0001 C CNN
F 1 "GND_L8" H 3700 1100 50  0001 C CNN
F 2 "" H 3700 1250 50  0000 C CNN
F 3 "" H 3700 1250 50  0000 C CNN
	1    3700 1250
	1    0    0    1   
$EndComp
Wire Wire Line
	3700 1450 3700 1500
Connection ~ 3700 1500
Wire Wire Line
	5050 2300 5050 2500
Wire Wire Line
	5050 1600 5050 2000
Connection ~ 4650 1600
Wire Wire Line
	4650 1000 3550 1000
Wire Wire Line
	3250 1000 3350 1000
$Comp
L Device:C_Small C115
U 1 1 5F87BBB3
P 4100 1250
F 0 "C115" V 3950 1300 50  0000 L CNN
F 1 "1uF" V 4050 1300 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4100 1250 50  0001 C CNN
F 3 "" H 4100 1250 50  0000 C CNN
F 4 "311-1365-1-ND" H 4100 1250 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-1365-1-ND\">Link</a>" H 4100 1250 60  0001 C CNN "Link"
	1    4100 1250
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR131
U 1 1 5F87BD09
P 4200 1250
F 0 "#PWR131" H 4200 1000 50  0001 C CNN
F 1 "GND_L8" H 4200 1100 50  0001 C CNN
F 2 "" H 4200 1250 50  0000 C CNN
F 3 "" H 4200 1250 50  0000 C CNN
	1    4200 1250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3950 1300 3950 1250
Wire Wire Line
	4000 1250 3950 1250
Connection ~ 3950 1250
Wire Wire Line
	10600 1100 10600 1150
Wire Wire Line
	10600 1100 10800 1100
Wire Wire Line
	4350 5200 4450 5200
Wire Wire Line
	4350 5200 4350 5900
Wire Wire Line
	7800 3400 8000 3400
Wire Wire Line
	7650 3400 7800 3400
Wire Wire Line
	6500 5900 6500 6000
Wire Wire Line
	6500 5900 6850 5900
Wire Wire Line
	7500 5900 7700 5900
Wire Wire Line
	7500 5900 7500 6000
Wire Wire Line
	5800 5900 5900 5900
Wire Wire Line
	5800 5900 5800 6000
Wire Wire Line
	850  7450 950  7450
Wire Wire Line
	850  7350 850  7450
Wire Wire Line
	3150 5750 3250 5750
Wire Wire Line
	3150 5750 3150 6100
Wire Wire Line
	3150 6100 3150 6150
Wire Wire Line
	1950 2750 1950 2850
Wire Wire Line
	1950 3000 1950 3100
Wire Wire Line
	1950 2850 1950 3000
Wire Wire Line
	2650 6350 2850 6350
Wire Wire Line
	8550 3400 8800 3400
Wire Wire Line
	1800 3400 1950 3400
Wire Wire Line
	3950 6350 4400 6350
Wire Wire Line
	6850 5900 6900 5900
Wire Wire Line
	10600 2450 10600 2500
Wire Wire Line
	10600 2450 10800 2450
Wire Wire Line
	9650 5000 9650 5250
Wire Wire Line
	3350 3400 3450 3400
Wire Wire Line
	4050 3400 4050 3500
Wire Wire Line
	4050 3400 4350 3400
Wire Wire Line
	10150 3750 10450 3750
Wire Wire Line
	10150 4150 10450 4150
Wire Wire Line
	10250 5700 10550 5700
Wire Wire Line
	10250 6100 10550 6100
Wire Wire Line
	10550 6100 10650 6100
Wire Wire Line
	9400 1100 9450 1100
Wire Wire Line
	9100 5800 9150 5800
Wire Wire Line
	9100 4550 9100 4800
Wire Wire Line
	9100 4550 9300 4550
Wire Wire Line
	9100 5500 9100 5800
Wire Wire Line
	7700 5900 9150 5900
Wire Wire Line
	10450 3750 10550 3750
Wire Wire Line
	5900 7450 6000 7450
Wire Wire Line
	6650 7450 6750 7450
Wire Wire Line
	8900 1250 8900 1450
Wire Wire Line
	5050 3400 5400 3400
Wire Wire Line
	5050 2850 5050 2950
Wire Wire Line
	5400 3400 5450 3400
Wire Wire Line
	5750 2950 5950 2950
Wire Wire Line
	2600 800  2600 900 
Wire Wire Line
	7000 3400 7050 3400
Wire Wire Line
	5600 1850 5600 2150
Wire Wire Line
	5600 2150 5600 2600
Wire Wire Line
	5050 2500 5050 2850
Wire Wire Line
	5600 1500 5600 1850
Wire Wire Line
	6350 2600 6550 2600
Wire Wire Line
	2450 800  2600 800 
Wire Wire Line
	2600 1600 2600 1950
Wire Wire Line
	9100 4250 9100 4550
Wire Wire Line
	3250 1500 3300 1500
Wire Wire Line
	3700 1700 3750 1700
Wire Wire Line
	4450 1600 4650 1600
Wire Wire Line
	3700 1500 3750 1500
Wire Wire Line
	4650 1600 5050 1600
Wire Wire Line
	3950 1250 3950 1200
$EndSCHEMATC
