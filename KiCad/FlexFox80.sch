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
Text Notes 2150 4650 0    79   ~ 16
FET Driver
Text Notes 4350 4300 0    79   ~ 16
HF Final\nAmplifier
Text Notes 7100 6950 0    118  ~ 24
PCB: Flex Fox 80 ARDF Transmitter (p. 1/3)
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR130
U 1 1 58A2833C
P 3600 2900
F 0 "#PWR130" H 3600 2650 50  0001 C CNN
F 1 "GND_L8" H 3600 2750 50  0001 C CNN
F 2 "" H 3600 2900 50  0000 C CNN
F 3 "" H 3600 2900 50  0000 C CNN
	1    3600 2900
	0    1    1    0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR134
U 1 1 58A28342
P 4200 2900
F 0 "#PWR134" H 4200 2650 50  0001 C CNN
F 1 "GND_L8" H 4200 2750 50  0000 C CNN
F 2 "" H 4200 2900 50  0000 C CNN
F 3 "" H 4200 2900 50  0000 C CNN
	1    4200 2900
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR136
U 1 1 58A73968
P 4250 4200
F 0 "#PWR136" H 4250 3950 50  0001 C CNN
F 1 "GND_L8" H 4250 4050 50  0001 C CNN
F 2 "" H 4250 4200 50  0000 C CNN
F 3 "" H 4250 4200 50  0000 C CNN
	1    4250 4200
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:INDUCTOR_SMALL-ARDF-2-Band-MiniTx-rescue L106
U 1 1 58A77A86
P 5700 3600
F 0 "L106" H 5700 3700 50  0000 C CNN
F 1 "2.7uH" H 5700 3550 50  0000 C CNN
F 2 "Inductors:Toroid_T-60_2TH_Vertical" H 5700 3600 50  0001 C CNN
F 3 "" H 5700 3600 50  0001 C CNN
F 4 "T60-2" H 5700 3600 60  0001 C CNN "Part No."
F 5 "<a href=\"http://www.amidoncorp.com/t60-2/\">Link</a>" H 5700 3600 60  0001 C CNN "Link"
	1    5700 3600
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C123
U 1 1 58A77CD9
P 5350 3800
F 0 "C123" H 5360 3870 50  0000 L CNN
F 1 "1100pF" H 5360 3720 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5350 3800 50  0001 C CNN
F 3 "" H 5350 3800 50  0000 C CNN
F 4 "399-14552-1-ND" H 5350 3800 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-14552-1-ND\">Link</a>" H 5350 3800 60  0001 C CNN "Link"
	1    5350 3800
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C122
U 1 1 58A78557
P 5100 3600
F 0 "C122" V 4900 3500 50  0000 L CNN
F 1 "470nF" V 5000 3500 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5100 3600 50  0001 C CNN
F 3 "" H 5100 3600 50  0000 C CNN
F 4 "478-9924-1-ND" H 5100 3600 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-9924-1-ND\">Link</a>" H 5100 3600 60  0001 C CNN "Link"
	1    5100 3600
	0    1    1    0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR146
U 1 1 58A7996F
P 6050 3900
F 0 "#PWR146" H 6050 3650 50  0001 C CNN
F 1 "GND_L8" H 6050 3750 50  0001 C CNN
F 2 "" H 6050 3900 50  0000 C CNN
F 3 "" H 6050 3900 50  0000 C CNN
	1    6050 3900
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR151
U 1 1 58A7A1D0
P 7050 3900
F 0 "#PWR151" H 7050 3650 50  0001 C CNN
F 1 "GND_L8" H 7050 3750 50  0001 C CNN
F 2 "" H 7050 3900 50  0000 C CNN
F 3 "" H 7050 3900 50  0000 C CNN
	1    7050 3900
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:+2to+12V-ARDF-2-Band-MiniTx-rescue #PWR133
U 1 1 58A9C144
P 3900 2750
F 0 "#PWR133" H 3900 2600 50  0001 C CNN
F 1 "+2to+12V" H 3950 2900 50  0000 C CNN
F 2 "" H 3900 2750 50  0000 C CNN
F 3 "" H 3900 2750 50  0000 C CNN
	1    3900 2750
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR117
U 1 1 58BF361E
P 2300 2950
F 0 "#PWR117" H 2300 2800 50  0001 C CNN
F 1 "+12V" H 2300 3090 50  0000 C CNN
F 2 "" H 2300 2950 50  0000 C CNN
F 3 "" H 2300 2950 50  0000 C CNN
	1    2300 2950
	1    0    0    -1  
$EndComp
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
	3800 2900 3900 2900
Connection ~ 3900 2900
Wire Wire Line
	5200 3600 5350 3600
Wire Wire Line
	6050 3450 6050 3600
Connection ~ 6050 3600
Wire Wire Line
	6950 3600 7050 3600
Connection ~ 7050 3600
Wire Wire Line
	5350 3450 5350 3600
Connection ~ 5350 3600
Wire Wire Line
	3900 2750 3900 2900
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
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR164
U 1 1 59D53C99
P 8700 3800
F 0 "#PWR164" H 8700 3550 50  0001 C CNN
F 1 "GND_L8" H 8700 3650 50  0001 C CNN
F 2 "" H 8700 3800 50  0000 C CNN
F 3 "" H 8700 3800 50  0000 C CNN
	1    8700 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	8700 3800 8700 3700
$Comp
L FlexFox80-rescue:CONN_01X01-ARDF-2-Band-MiniTx-rescue W116
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
	3150 2950 3350 2950
Wire Wire Line
	3350 2950 3350 2000
Wire Wire Line
	3350 2000 2600 2000
Wire Wire Line
	2600 2000 2600 2450
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR123
U 1 1 5A7B67DA
P 2900 2950
F 0 "#PWR123" H 2900 2700 50  0001 C CNN
F 1 "GND_L8" H 3050 2800 50  0001 C CNN
F 2 "" H 2900 2950 50  0000 C CNN
F 3 "" H 2900 2950 50  0000 C CNN
	1    2900 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 2950 2300 3000
Wire Wire Line
	2300 3000 2450 3000
Wire Wire Line
	2450 3000 2450 2950
Connection ~ 2700 3450
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR140
U 1 1 5A7DEA16
P 5350 3900
F 0 "#PWR140" H 5350 3650 50  0001 C CNN
F 1 "GND_L8" H 5350 3750 50  0001 C CNN
F 2 "" H 5350 3900 50  0000 C CNN
F 3 "" H 5350 3900 50  0000 C CNN
	1    5350 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 4050 3500 4050
Wire Wire Line
	2400 4250 2400 4350
$Comp
L power:PWR_FLAG #FLG105
U 1 1 5A7E62FA
P 2600 3800
F 0 "#FLG105" H 2600 3895 50  0001 C CNN
F 1 "PWR_FLAG" H 2600 3980 50  0000 C CNN
F 2 "" H 2600 3800 50  0000 C CNN
F 3 "" H 2600 3800 50  0000 C CNN
	1    2600 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 3800 2600 3800
Connection ~ 2700 3800
$Comp
L FlexFox80-rescue:TEST_1P-ARDF-2-Band-MiniTx-rescue W114
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
	1800 4050 2200 4050
Wire Wire Line
	2200 4050 2200 4000
Connection ~ 2200 4050
$Comp
L FlexFox80-rescue:LM5134-ARDF-2-Band-MiniTx-rescue U107
U 1 1 5AA8446F
P 2700 4150
F 0 "U107" H 3000 4550 61  0000 R CNB
F 1 "LM5134B" H 3150 4450 61  0000 R CNB
F 2 "TO_SOT_Packages_SMD:SOT-23-6_Handsolder" H 2850 3300 50  0001 L CNN
F 3 "" H 2900 3850 50  0001 C CNN
F 4 "296-40244-1-ND" H 2700 4150 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=296-40244-1-ND\">Link</a>" H 2700 4150 60  0001 C CNN "Link"
	1    2700 4150
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR119
U 1 1 5AAE6E6A
P 2400 4350
F 0 "#PWR119" H 2400 4100 50  0001 C CNN
F 1 "GND_L8" H 2400 4200 50  0001 C CNN
F 2 "" H 2400 4350 50  0000 C CNN
F 3 "" H 2400 4350 50  0000 C CNN
	1    2400 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	9200 2950 8900 2950
Text GLabel 9250 2700 2    39   Output ~ 0
~ANT_CONNECT_INT~
$Sheet
S -1850 1550 1050 650 
U 5BC7F38C
F0 "Digital" 61
F1 "file5BC7F38B.sch" 61
$EndSheet
Text GLabel 1800 4050 0    39   Input ~ 0
CLK1
$Comp
L FlexFox80-rescue:CONN_01X03-ARDF-2-Band-MiniTx-rescue P103
U 1 1 5BCC61E7
P 8900 3600
F 0 "P103" H 8900 3800 50  0000 C CNN
F 1 "Phoenix" V 9050 3600 50  0000 C CNN
F 2 "Sockets:Phoenix_3.81mm_3pos" H 8900 3600 50  0001 C CNN
F 3 "" H 8900 3600 50  0001 C CNN
F 4 "277-1207-ND" H 8900 3600 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=277-1207-ND\">Link</a>" H 8900 3600 60  0001 C CNN "Link"
	1    8900 3600
	1    0    0    1   
$EndComp
Text GLabel 3000 2450 1    39   Input ~ 0
HF_ENABLE
$Comp
L Device:C_Small C120
U 1 1 5BE9CF68
P 4100 2900
F 0 "C120" V 4200 2800 50  0000 L CNN
F 1 "470nF" V 3950 2800 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4100 2900 50  0001 C CNN
F 3 "" H 4100 2900 50  0000 C CNN
F 4 "478-9924-1-ND" H 4100 2900 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-9924-1-ND\">Link</a>" H 4100 2900 60  0001 C CNN "Link"
	1    4100 2900
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C126
U 1 1 5BEA0426
P 6050 3800
F 0 "C126" H 5800 3750 50  0000 L CNN
F 1 "1800pF" H 5750 3900 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 6050 3800 50  0001 C CNN
F 3 "" H 6050 3800 50  0000 C CNN
F 4 "445-5752-1-ND" H 6050 3800 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=445-5752-1-ND\">Link</a>" H 6050 3800 60  0001 C CNN "Link"
	1    6050 3800
	-1   0    0    1   
$EndComp
$Comp
L special:Heat-sink HS101
U 1 1 5BEA52AC
P 3900 3950
F 0 "HS101" H 3700 4250 50  0000 L CNN
F 1 "Heatsink" H 3700 4150 50  0000 L CNN
F 2 "Heatsinks:Heatsink_HF35G" H 3350 3925 50  0001 L CIN
F 3 "" H 3900 3950 50  0001 L CNN
F 4 "HS220-ND" H 3900 3950 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=HS220-ND\">Link</a>" H 3900 3950 60  0001 C CNN "Link"
	1    3900 3950
	1    0    0    -1  
$EndComp
Text Notes 8900 3900 0    39   ~ 8
3.5 MHz Out
$Comp
L FlexFox80-rescue:CONN_01X03-ARDF-2-Band-MiniTx-rescue P502
U 1 1 5BF4992C
P 9200 3600
F 0 "P502" H 9200 3800 50  0000 C CNN
F 1 "Phoenix" V 9350 3600 50  0001 C CNN
F 2 "Oddities:Dummy_Empty_3" H 9200 3600 50  0001 C CNN
F 3 "" H 9200 3600 50  0001 C CNN
F 4 "277-1162-ND" H 9200 3600 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=277-1162-ND\">Link</a>" H 9200 3600 60  0001 C CNN "Link"
	1    9200 3600
	-1   0    0    1   
$EndComp
NoConn ~ 9400 3600
$Sheet
S -1850 2450 1050 700 
U 5BF4C520
F0 "Non-PCB" 39
F1 "file5BF4C51F.sch" 39
$EndSheet
Text GLabel 8850 3200 2    39   Output ~ 0
80M_ANTENNA_DETECT
Wire Wire Line
	9400 3700 9650 3700
Wire Wire Line
	9650 3500 9400 3500
Wire Wire Line
	7050 3300 7050 3600
Text Notes 5850 2800 0    79   ~ 16
HF LPF
Text Notes 8800 4050 0    79   ~ 16
HF Antenna
$Comp
L Device:C_Small C131
U 1 1 5C0D126D
P 7050 3800
F 0 "C131" H 7060 3870 50  0000 L CNN
F 1 "1100pF" H 6750 3700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 7050 3800 50  0001 C CNN
F 3 "" H 7050 3800 50  0000 C CNN
F 4 "399-14552-1-ND" H 7050 3800 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-14552-1-ND\">Link</a>" H 7050 3800 60  0001 C CNN "Link"
	1    7050 3800
	1    0    0    -1  
$EndComp
Text Notes 5750 3000 0    39   ~ 0
6th order Chebychev\n3.9MHz 3dB cut-off\nw/ node at 10.7MHz
$Comp
L FlexFox80-rescue:INDUCTOR_SMALL-ARDF-2-Band-MiniTx-rescue L108
U 1 1 5C0D7F1D
P 6700 3600
F 0 "L108" H 6700 3700 50  0000 C CNN
F 1 "2.7uH" H 6700 3550 50  0000 C CNN
F 2 "Inductors:Toroid_T-60_2TH_Vertical" H 6700 3600 50  0001 C CNN
F 3 "" H 6700 3600 50  0001 C CNN
F 4 "T60-2" H 6700 3600 60  0001 C CNN "Part No."
F 5 "<a href=\"http://www.amidoncorp.com/t60-2/\">Link</a>" H 6700 3600 60  0001 C CNN "Link"
	1    6700 3600
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:TEST_SCOPE-ARDF-2-Band-MiniTx-rescue W105
U 1 1 5C0EAA8C
P 2200 4000
F 0 "W105" V 2400 4000 50  0000 C CNN
F 1 "T2" V 2250 4050 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 2400 4000 50  0001 C CNN
F 3 "" H 2400 4000 50  0001 C CNN
F 4 "np" H 2200 4000 60  0001 C CNN "Part No."
F 5 "np" H 2200 4000 60  0001 C CNN "Link"
	1    2200 4000
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR116
U 1 1 5C0EAA92
P 2200 3900
F 0 "#PWR116" H 2200 3650 50  0001 C CNN
F 1 "GND_L8" H 2200 3750 50  0001 C CNN
F 2 "" H 2200 3900 50  0000 C CNN
F 3 "" H 2200 3900 50  0000 C CNN
	1    2200 3900
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:TEST_SCOPE-ARDF-2-Band-MiniTx-rescue W108
U 1 1 5C0EB478
P 3500 4200
F 0 "W108" H 3450 4450 50  0000 C CNN
F 1 "T4" H 3550 4250 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 3700 4200 50  0001 C CNN
F 3 "" H 3700 4200 50  0001 C CNN
F 4 "np" H 3500 4200 60  0001 C CNN "Part No."
F 5 "np" H 3500 4200 60  0001 C CNN "Link"
	1    3500 4200
	-1   0    0    1   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR126
U 1 1 5C0EB47E
P 3400 4200
F 0 "#PWR126" H 3400 3950 50  0001 C CNN
F 1 "GND_L8" H 3400 4050 50  0001 C CNN
F 2 "" H 3400 4200 50  0000 C CNN
F 3 "" H 3400 4200 50  0000 C CNN
	1    3400 4200
	-1   0    0    1   
$EndComp
Wire Wire Line
	3500 4200 3500 4050
Connection ~ 3500 4050
$Comp
L FlexFox80-rescue:TEST_SCOPE-ARDF-2-Band-MiniTx-rescue W111
U 1 1 5C0EDCBD
P 5350 3450
F 0 "W111" H 5400 3750 50  0000 C CNN
F 1 "T5" H 5400 3500 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 5550 3450 50  0001 C CNN
F 3 "" H 5550 3450 50  0001 C CNN
F 4 "np" H 5350 3450 60  0001 C CNN "Part No."
F 5 "np" H 5350 3450 60  0001 C CNN "Link"
	1    5350 3450
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR144
U 1 1 5C0EDCC3
P 5450 3450
F 0 "#PWR144" H 5450 3200 50  0001 C CNN
F 1 "GND_L8" H 5450 3300 50  0001 C CNN
F 2 "" H 5450 3450 50  0000 C CNN
F 3 "" H 5450 3450 50  0000 C CNN
	1    5450 3450
	-1   0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:TEST_SCOPE-ARDF-2-Band-MiniTx-rescue W113
U 1 1 5C0EE9E8
P 6050 3450
F 0 "W113" H 6100 3750 50  0000 C CNN
F 1 "T6" H 6100 3500 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 6250 3450 50  0001 C CNN
F 3 "" H 6250 3450 50  0001 C CNN
F 4 "np" H 6050 3450 60  0001 C CNN "Part No."
F 5 "np" H 6050 3450 60  0001 C CNN "Link"
	1    6050 3450
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR148
U 1 1 5C0EE9EE
P 6150 3450
F 0 "#PWR148" H 6150 3200 50  0001 C CNN
F 1 "GND_L8" H 6150 3300 50  0001 C CNN
F 2 "" H 6150 3450 50  0000 C CNN
F 3 "" H 6150 3450 50  0000 C CNN
	1    6150 3450
	-1   0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:TEST_SCOPE-ARDF-2-Band-MiniTx-rescue W117
U 1 1 5C0EEADD
P 7250 3450
F 0 "W117" H 7300 3700 50  0000 C CNN
F 1 "T7" H 7300 3500 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 7450 3450 50  0001 C CNN
F 3 "" H 7450 3450 50  0001 C CNN
F 4 "np" H 7250 3450 60  0001 C CNN "Part No."
F 5 "np" H 7250 3450 60  0001 C CNN "Link"
	1    7250 3450
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR154
U 1 1 5C0EEAE3
P 7350 3450
F 0 "#PWR154" H 7350 3200 50  0001 C CNN
F 1 "GND_L8" H 7350 3300 50  0001 C CNN
F 2 "" H 7350 3450 50  0000 C CNN
F 3 "" H 7350 3450 50  0000 C CNN
	1    7350 3450
	-1   0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:TEST_SCOPE-ARDF-2-Band-MiniTx-rescue W106
U 1 1 5C0F543D
P 2300 3450
F 0 "W106" V 2350 3700 50  0000 C CNN
F 1 "T3" V 2350 3500 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 2500 3450 50  0001 C CNN
F 3 "" H 2500 3450 50  0001 C CNN
F 4 "np" H 2300 3450 60  0001 C CNN "Part No."
F 5 "np" H 2300 3450 60  0001 C CNN "Link"
	1    2300 3450
	0    -1   1    0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR118
U 1 1 5C0F5443
P 2300 3550
F 0 "#PWR118" H 2300 3300 50  0001 C CNN
F 1 "GND_L8" H 2300 3400 50  0001 C CNN
F 2 "" H 2300 3550 50  0000 C CNN
F 3 "" H 2300 3550 50  0000 C CNN
	1    2300 3550
	0    -1   1    0   
$EndComp
$Comp
L Device:C_Small C128
U 1 1 5C0F9847
P 6650 3300
F 0 "C128" V 6550 3200 50  0000 L CNN
F 1 "91pF" V 6750 3200 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 6650 3300 50  0001 C CNN
F 3 "" H 6650 3300 50  0000 C CNN
F 4 "399-17459-1-ND" H 6650 3300 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-17459-1-ND\">Link</a>" H 6650 3300 60  0001 C CNN "Link"
	1    6650 3300
	0    1    1    0   
$EndComp
Wire Wire Line
	6750 3300 7050 3300
Wire Wire Line
	6550 3300 6400 3300
Wire Wire Line
	6400 3300 6400 3600
Connection ~ 6400 3600
Wire Wire Line
	5950 3600 6050 3600
$Comp
L FlexFox80-rescue:IRF610_TO220-ARDF-2-Band-MiniTx-rescue Q102
U 1 1 5C100056
P 4150 4000
F 0 "Q102" H 4350 4075 50  0000 L CNN
F 1 "IRF610" H 4350 4000 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-220_Neutral123_Vertical_LargePads" H 4350 3925 50  0001 L CIN
F 3 "" H 4150 4000 50  0001 L CNN
F 4 "IRF610PBF-ND" H 4150 4000 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=IRF610PBF-ND\">Link</a>" H 4150 4000 60  0001 C CNN "Link"
	1    4150 4000
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1_Small C119
U 1 1 5C11AA96
P 3700 2900
F 0 "C119" V 3800 2850 50  0000 L CNN
F 1 "47uF low ESR" V 3550 2550 50  0000 L CNN
F 2 "Capacitors_SMD:C_2917_7343_Metric_HandSoldering" H 3700 2900 50  0001 C CNN
F 3 "https://content.kemet.com/datasheets/KEM_T2009_T495.pdf" H 3700 2900 50  0001 C CNN
F 4 "399-3885-1-ND" H 3700 2900 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-3885-1-ND\">Link</a>" H 3700 2900 60  0001 C CNN "Link"
	1    3700 2900
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
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR102
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
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR124
U 1 1 5A7DD6BD
P 3000 3450
F 0 "#PWR124" H 3000 3200 50  0001 C CNN
F 1 "GND_L8" H 3150 3300 50  0001 C CNN
F 2 "" H 3000 3450 50  0000 C CNN
F 3 "" H 3000 3450 50  0000 C CNN
	1    3000 3450
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C117
U 1 1 5A7D307D
P 2900 3450
F 0 "C117" V 2800 3100 50  0000 L CNN
F 1 "22uF" V 2800 3350 50  0000 L CNN
F 2 "Capacitors_SMD:C_1210_HandSoldering" H 2900 3450 50  0001 C CNN
F 3 "" H 2900 3450 50  0000 C CNN
F 4 "1276-3373-1-ND" H 2900 3450 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-3373-1-ND\">Link</a>" H 2900 3450 60  0001 C CNN "Link"
	1    2900 3450
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:LMN200B01-ARDF-2-Band-MiniTx-rescue Q101
U 1 1 5C64951A
P 2800 2700
F 0 "Q101" V 2500 2550 61  0000 L CNB
F 1 "LMN400E01" V 2750 1750 61  0000 L CNB
F 2 "TO_SOT_Packages_SMD:SOT-363_Handsolder" H 2815 2500 50  0001 L CNN
F 3 "" H 2815 2500 50  0000 L CNN
F 4 "LMN400E01-7" H 2800 2700 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=LMN400E01DICT-ND\">Link</a>" H 2800 2700 60  0001 C CNN "Link"
	1    2800 2700
	0    1    1    0   
$EndComp
Wire Wire Line
	9250 2700 9200 2700
Wire Wire Line
	8500 3500 8650 3500
Text Notes 1750 3250 0    39   ~ 0
Optional voltage-dropping\ndiode for batteries > 12V.\nShort across if not used.
Wire Wire Line
	2300 3450 2700 3450
$Comp
L FlexFox80-rescue:ZENER-ARDF-2-Band-MiniTx-rescue D102
U 1 1 5C71D9FE
P 2700 3150
F 0 "D102" H 2700 3250 50  0000 C CNN
F 1 "4.7V" H 2700 3050 50  0000 C CNN
F 2 "Diodes_SMD:SOD-123_Handsolder" H 2700 3150 50  0001 C CNN
F 3 "" H 2700 3150 50  0000 C CNN
F 4 "BZT52C4V7-13FDICT-ND" H 2700 3150 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=BZT52C4V7-13FDICT-ND\">Link</a>" H 2700 3150 60  0001 C CNN "Link"
	1    2700 3150
	0    1    1    0   
$EndComp
Wire Wire Line
	2700 3350 2700 3450
$Comp
L Device:C_Small C502
U 1 1 5C740B6F
P 9800 3600
F 0 "C502" H 9810 3670 50  0000 L CNN
F 1 "np" H 9810 3520 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 9800 3600 50  0001 C CNN
F 3 "" H 9800 3600 50  0000 C CNN
F 4 "np" H 9800 3600 60  0001 C CNN "Part No."
F 5 "np" H 9800 3600 60  0001 C CNN "Link"
	1    9800 3600
	1    0    0    -1  
$EndComp
$Comp
L Device:R R502
U 1 1 5C740B77
P 10100 3600
F 0 "R502" V 10000 3600 50  0000 C CNN
F 1 "0" V 10100 3600 50  0000 C CNN
F 2 "Resistors_SMD:Zero_Ohm_0805_HandSoldering" H 10030 3600 50  0001 C CNN
F 3 "" H 10100 3600 50  0000 C CNN
F 4 "np" H 10100 3600 60  0001 C CNN "Part No."
F 5 "np" H 10100 3600 60  0001 C CNN "Link"
	1    10100 3600
	-1   0    0    1   
$EndComp
Wire Wire Line
	9650 3400 9800 3400
Wire Wire Line
	10100 3400 10100 3450
Wire Wire Line
	9650 3800 9800 3800
Wire Wire Line
	10100 3800 10100 3750
Wire Wire Line
	9800 3500 9800 3400
Connection ~ 9800 3400
Wire Wire Line
	9800 3700 9800 3800
Connection ~ 9800 3800
Wire Wire Line
	9650 3400 9650 3500
Wire Wire Line
	9650 3700 9650 3800
Connection ~ 10100 3800
$Comp
L FlexFox80-rescue:GND_L1-ARDF-2-Band-MiniTx-rescue #PWR174
U 1 1 5C7426BB
P 10200 3800
F 0 "#PWR174" H 10200 3550 50  0001 C CNN
F 1 "GND_L1" H 10200 3650 50  0000 C CNN
F 2 "" H 10200 3800 50  0000 C CNN
F 3 "" H 10200 3800 50  0000 C CNN
	1    10200 3800
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:CONN_01X01-ARDF-2-Band-MiniTx-rescue J503
U 1 1 5C743B2D
P 9800 3200
F 0 "J503" H 9800 3300 50  0000 C CNN
F 1 "DET" V 9900 3200 50  0001 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01_circle_pad" H 9800 3200 50  0001 C CNN
F 3 "" H 9800 3200 50  0001 C CNN
F 4 "np" H 9800 3200 60  0001 C CNN "Part No."
F 5 "np" H 9800 3200 60  0001 C CNN "Link"
	1    9800 3200
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:CONN_01X01-ARDF-2-Band-MiniTx-rescue J504
U 1 1 5C743CAE
P 9800 4000
F 0 "J504" H 9800 4100 50  0000 C CNN
F 1 "GND" V 9900 4000 50  0001 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01" H 9800 4000 50  0001 C CNN
F 3 "" H 9800 4000 50  0001 C CNN
F 4 "np" H 9800 4000 60  0001 C CNN "Part No."
F 5 "np" H 9800 4000 60  0001 C CNN "Link"
	1    9800 4000
	0    1    1    0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L1-ARDF-2-Band-MiniTx-rescue #PWR104
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
L FlexFox80-rescue:GND_L0-ARDF-2-Band-MiniTx-rescue #PWR105
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
Text Notes 9650 3000 0    39   ~ 0
Strain Relief Board #2
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR156
U 1 1 5CB334EC
P 7900 3500
F 0 "#PWR156" H 7900 3250 50  0001 C CNN
F 1 "GND_L8" H 7900 3350 50  0001 C CNN
F 2 "" H 7900 3500 50  0000 C CNN
F 3 "" H 7900 3500 50  0000 C CNN
	1    7900 3500
	0    1    1    0   
$EndComp
Connection ~ 8650 3500
$Comp
L FlexFox80-rescue:TVS-ARDF-2-Band-MiniTx-rescue D104
U 1 1 5CB36EAB
P 8200 3500
F 0 "D104" H 8150 3750 50  0000 C CNN
F 1 "5V" H 8150 3650 50  0000 C CNN
F 2 "Diodes_SMD:SOD-323F_Handsolder" H 8200 3500 50  0001 C CNN
F 3 "" H 8200 3500 50  0000 C CNN
F 4 "641-1219-1-ND" H 8200 3500 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/product-detail/en/comchip-technology/CPDFR5V0/641-1219-1-ND/1678637\">Link</a>" H 8200 3500 60  0001 C CNN "Link"
	1    8200 3500
	-1   0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:INDUCTOR_CT-ARDF-2-Band-MiniTx-rescue L104
U 1 1 5CB66258
P 4250 3600
F 0 "L104" V 4400 3600 50  0000 C CNN
F 1 "9T #24 Bifilar T50-43" V 4400 3500 50  0001 C CNN
F 2 "Inductors:Toroid_T-50A_TH_BF_CT_Vertical" H 4250 3600 50  0001 C CNN
F 3 "" H 4250 3600 50  0000 C CNN
F 4 "Amidon FT50A-43" H 4250 3600 60  0001 C CNN "Part No."
F 5 "<a href=\"http://www.amidoncorp.com/ft-50A-43/\">Link</a>" H 4250 3600 60  0001 C CNN "Link"
	1    4250 3600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3900 3600 3950 3600
Wire Wire Line
	4250 3700 4250 3800
Wire Wire Line
	4550 3600 5000 3600
Wire Wire Line
	8850 3200 8650 3200
Connection ~ 8650 3200
Text Notes 5550 4150 0    39   ~ 0
L106 & L108:\n20T (46 cm) #24 T60-2 70% Coverage
Text Notes 4450 3150 0    39   ~ 0
L104:\n9T (24 cm) #24 Bifilar FT-50A-43 or\n6T #24 Bifilar FT-50-77
Wire Wire Line
	7250 3450 7250 3600
Connection ~ 7250 3600
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
L FlexFox80-rescue:CONN_01X01-ARDF-2-Band-MiniTx-rescue J505
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
L FlexFox80-rescue:CONN_01X01-ARDF-2-Band-MiniTx-rescue J506
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
L FlexFox80-rescue:GND_L2-ARDF-2-Band-MiniTx-rescue #PWR145
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
L FlexFox80-rescue:CONN_01X01-ARDF-2-Band-MiniTx-rescue J507
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
L FlexFox80-rescue:CONN_01X01-ARDF-2-Band-MiniTx-rescue J508
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
L FlexFox80-rescue:GND_L3-ARDF-2-Band-MiniTx-rescue #PWR149
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
	9600 3100 10400 3100
Wire Notes Line
	9600 4100 10400 4100
Wire Notes Line
	10400 4100 10400 3100
Wire Notes Line
	9600 3100 9600 4100
$Comp
L FlexFox80-rescue:GND_L2-ARDF-2-Band-MiniTx-rescue #PWR108
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
L FlexFox80-rescue:GND_L3-ARDF-2-Band-MiniTx-rescue #PWR112
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
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR120
U 1 1 5A7DF174
P 2700 4450
F 0 "#PWR120" H 2700 4200 50  0001 C CNN
F 1 "GND_L8" H 2700 4300 50  0001 C CNN
F 2 "" H 2700 4450 50  0000 C CNN
F 3 "" H 2700 4450 50  0000 C CNN
	1    2700 4450
	1    0    0    -1  
$EndComp
NoConn ~ 3000 4250
$Comp
L FlexFox80-rescue:D_Schottky_x2_KCom_Dual-ARDF-2-Band-MiniTx-rescue D301
U 2 1 5F8505B7
P 8650 2950
F 0 "D301" H 8650 3100 50  0000 C CNN
F 1 "D_Schottky_x2_KCom_Dual" H 8650 3050 50  0001 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-363_Handsolder" H 8650 2950 50  0001 C CNN
F 3 "" H 8650 2950 50  0000 C CNN
F 4 "BAT54CDW-FDICT-ND" H 8650 2950 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=BAT54CDW-FDICT-ND\">Link</a>" H 8650 2950 60  0001 C CNN "Link"
	2    8650 2950
	1    0    0    -1  
$EndComp
NoConn ~ 8400 2950
Wire Wire Line
	8650 3100 8650 3200
$Comp
L Device:Jumper_NO_Small J102
U 1 1 5F871998
P 8550 3200
F 0 "J102" H 8550 3280 50  0000 C CNN
F 1 "Jumper_NO_Small" H 8560 3140 50  0001 C CNN
F 2 "Wire_Connections_Bridges:Solder-Jumper-NO-SMD-Pad_Small" H 8550 3200 50  0001 C CNN
F 3 "" H 8550 3200 50  0000 C CNN
F 4 "np" H 8550 3200 60  0001 C CNN "Part No."
F 5 "np" H 8550 3200 60  0001 C CNN "Link"
	1    8550 3200
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR160
U 1 1 5F871B53
P 8450 3200
F 0 "#PWR160" H 8450 2950 50  0001 C CNN
F 1 "GND_L8" H 8450 3050 50  0001 C CNN
F 2 "" H 8450 3200 50  0000 C CNN
F 3 "" H 8450 3200 50  0000 C CNN
	1    8450 3200
	0    1    1    0   
$EndComp
Wire Wire Line
	3900 2900 4000 2900
Wire Wire Line
	3900 2900 3900 3600
Wire Wire Line
	6050 3600 6050 3700
Wire Wire Line
	6050 3600 6400 3600
Wire Wire Line
	7050 3600 7250 3600
Wire Wire Line
	7050 3600 7050 3700
Wire Wire Line
	5350 3600 5450 3600
Wire Wire Line
	5350 3600 5350 3700
Wire Wire Line
	850  7450 950  7450
Wire Wire Line
	850  7350 850  7450
Wire Wire Line
	2700 3450 2800 3450
Wire Wire Line
	2700 3450 2700 3800
Wire Wire Line
	2700 3800 2700 3850
Wire Wire Line
	2200 4050 2400 4050
Wire Wire Line
	3500 4050 3950 4050
Wire Wire Line
	6400 3600 6450 3600
Wire Wire Line
	9200 2700 9200 2950
Wire Wire Line
	9800 3400 10100 3400
Wire Wire Line
	9800 3800 10100 3800
Wire Wire Line
	10100 3800 10200 3800
Wire Wire Line
	8650 3500 8700 3500
Wire Wire Line
	8650 3200 8650 3500
Wire Wire Line
	7250 3600 8700 3600
Wire Wire Line
	5900 7450 6000 7450
Wire Wire Line
	6650 7450 6750 7450
$EndSCHEMATC
