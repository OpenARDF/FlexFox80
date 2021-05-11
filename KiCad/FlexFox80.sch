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
Text Notes 2600 4450 0    79   ~ 16
FET Driver
Text Notes 4800 4100 0    79   ~ 16
HF Final\nAmplifier
Text Notes 7100 6950 0    118  ~ 24
PCB: Flex Fox 80 ARDF Transmitter (p. 1/3)
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0130
U 1 1 58A2833C
P 4050 2700
F 0 "#PWR0130" H 4050 2450 50  0001 C CNN
F 1 "GND_L8" H 4050 2550 50  0001 C CNN
F 2 "" H 4050 2700 50  0000 C CNN
F 3 "" H 4050 2700 50  0000 C CNN
	1    4050 2700
	0    1    1    0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0134
U 1 1 58A28342
P 4650 2700
F 0 "#PWR0134" H 4650 2450 50  0001 C CNN
F 1 "GND_L8" H 4650 2550 50  0000 C CNN
F 2 "" H 4650 2700 50  0000 C CNN
F 3 "" H 4650 2700 50  0000 C CNN
	1    4650 2700
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0136
U 1 1 58A73968
P 4700 4000
F 0 "#PWR0136" H 4700 3750 50  0001 C CNN
F 1 "GND_L8" H 4700 3850 50  0001 C CNN
F 2 "" H 4700 4000 50  0000 C CNN
F 3 "" H 4700 4000 50  0000 C CNN
	1    4700 4000
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:INDUCTOR_SMALL-ARDF-2-Band-MiniTx-rescue L106
U 1 1 58A77A86
P 6150 3400
F 0 "L106" H 6150 3500 50  0000 C CNN
F 1 "2.7uH" H 6150 3350 50  0000 C CNN
F 2 "Inductors:Toroid_T-60_2TH_Vertical" H 6150 3400 50  0001 C CNN
F 3 "" H 6150 3400 50  0001 C CNN
F 4 "T60-2 (Amidon)" H 6150 3400 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"http://www.amidoncorp.com/t60-2/\">Link</a>" H 6150 3400 60  0001 C CNN "Link"
F 6 "TH" H 6150 3400 50  0001 C CNN "Manufacturer"
F 7 "TH" H 6150 3400 50  0001 C CNN "Manufacturer PN"
F 8 "TH" H 6150 3400 50  0001 C CNN "Description"
	1    6150 3400
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C123
U 1 1 58A77CD9
P 5800 3600
F 0 "C123" H 5810 3670 50  0000 L CNN
F 1 "1100pF" H 5810 3520 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 5800 3600 50  0001 C CNN
F 3 "" H 5800 3600 50  0000 C CNN
F 4 "399-14552-1-ND" H 5800 3600 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-14552-1-ND\">Link</a>" H 5800 3600 60  0001 C CNN "Link"
F 6 "KEMET" H 5800 3600 50  0001 C CNN "Manufacturer"
F 7 "C0805C112J5GAC7800" H 5800 3600 50  0001 C CNN "Manufacturer PN"
F 8 "CAP CER 1100PF 50V NP0 0805" H 5800 3600 50  0001 C CNN "Description"
	1    5800 3600
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0146
U 1 1 58A7996F
P 6500 3700
F 0 "#PWR0146" H 6500 3450 50  0001 C CNN
F 1 "GND_L8" H 6500 3550 50  0001 C CNN
F 2 "" H 6500 3700 50  0000 C CNN
F 3 "" H 6500 3700 50  0000 C CNN
	1    6500 3700
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0151
U 1 1 58A7A1D0
P 7500 3700
F 0 "#PWR0151" H 7500 3450 50  0001 C CNN
F 1 "GND_L8" H 7500 3550 50  0001 C CNN
F 2 "" H 7500 3700 50  0000 C CNN
F 3 "" H 7500 3700 50  0000 C CNN
	1    7500 3700
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:+2to+12V-ARDF-2-Band-MiniTx-rescue #PWR0133
U 1 1 58A9C144
P 4350 2550
F 0 "#PWR0133" H 4350 2400 50  0001 C CNN
F 1 "+2to+12V" H 4400 2700 50  0000 C CNN
F 2 "" H 4350 2550 50  0000 C CNN
F 3 "" H 4350 2550 50  0000 C CNN
	1    4350 2550
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0117
U 1 1 58BF361E
P 2750 2750
F 0 "#PWR0117" H 2750 2600 50  0001 C CNN
F 1 "+12V" H 2750 2890 50  0000 C CNN
F 2 "" H 2750 2750 50  0000 C CNN
F 3 "" H 2750 2750 50  0000 C CNN
	1    2750 2750
	1    0    0    -1  
$EndComp
$Sheet
S -1850 700  1050 650 
U 591BC960
F0 "ARDF Tx Continued" 60
F1 "file591BC95F.sch" 60
$EndSheet
Text Notes 1650 4200 1    157  ~ 31
80m RF Section
Wire Wire Line
	4250 2700 4350 2700
Connection ~ 4350 2700
Wire Wire Line
	5650 3400 5800 3400
Wire Wire Line
	6500 3250 6500 3400
Connection ~ 6500 3400
Wire Wire Line
	7400 3400 7500 3400
Connection ~ 7500 3400
Wire Wire Line
	5800 3250 5800 3400
Connection ~ 5800 3400
Wire Wire Line
	4350 2550 4350 2700
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
	3600 2750 3800 2750
Wire Wire Line
	3800 2750 3800 1800
Wire Wire Line
	3800 1800 3050 1800
Wire Wire Line
	3050 1800 3050 2250
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0123
U 1 1 5A7B67DA
P 3350 2750
F 0 "#PWR0123" H 3350 2500 50  0001 C CNN
F 1 "GND_L8" H 3500 2600 50  0001 C CNN
F 2 "" H 3350 2750 50  0000 C CNN
F 3 "" H 3350 2750 50  0000 C CNN
	1    3350 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 2750 2750 2800
Wire Wire Line
	2750 2800 2900 2800
Wire Wire Line
	2900 2800 2900 2750
Connection ~ 3150 3250
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0140
U 1 1 5A7DEA16
P 5800 3700
F 0 "#PWR0140" H 5800 3450 50  0001 C CNN
F 1 "GND_L8" H 5800 3550 50  0001 C CNN
F 2 "" H 5800 3700 50  0000 C CNN
F 3 "" H 5800 3700 50  0000 C CNN
	1    5800 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 3850 3950 3850
Wire Wire Line
	2850 4050 2850 4150
$Comp
L power:PWR_FLAG #FLG0105
U 1 1 5A7E62FA
P 3050 3600
F 0 "#FLG0105" H 3050 3695 50  0001 C CNN
F 1 "PWR_FLAG" H 3050 3780 50  0000 C CNN
F 2 "" H 3050 3600 50  0000 C CNN
F 3 "" H 3050 3600 50  0000 C CNN
	1    3050 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 3600 3050 3600
Connection ~ 3150 3600
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
	2250 3850 2650 3850
Wire Wire Line
	2650 3850 2650 3800
Connection ~ 2650 3850
$Comp
L FlexFox80-rescue:LM5134-ARDF-2-Band-MiniTx-rescue U107
U 1 1 5AA8446F
P 3150 3950
F 0 "U107" H 3450 4350 61  0000 R CNB
F 1 "LM5134B" H 3600 4250 61  0000 R CNB
F 2 "TO_SOT_Packages_SMD:SOT-23-6-LM5134" H 3300 3100 50  0001 L CNN
F 3 "" H 3350 3650 50  0001 C CNN
F 4 "LM5134BMFX/NOPB-ND" H 3150 3950 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=LM5134BMFX/NOPB-ND\">Link</a>" H 3150 3950 60  0001 C CNN "Link"
F 6 "Texas Instruments" H 3150 3950 50  0001 C CNN "Manufacturer"
F 7 "LM5134BMFX/NOPB" H 3150 3950 50  0001 C CNN "Manufacturer PN"
F 8 "IC GATE DRVR LOW-SIDE SOT23-6" H 3150 3950 50  0001 C CNN "Description"
	1    3150 3950
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0119
U 1 1 5AAE6E6A
P 2850 4150
F 0 "#PWR0119" H 2850 3900 50  0001 C CNN
F 1 "GND_L8" H 2850 4000 50  0001 C CNN
F 2 "" H 2850 4150 50  0000 C CNN
F 3 "" H 2850 4150 50  0000 C CNN
	1    2850 4150
	1    0    0    -1  
$EndComp
Text GLabel 9550 2100 2    39   Output ~ 0
~ANT_CONNECT_INT~
$Sheet
S -1850 1550 1050 650 
U 5BC7F38C
F0 "Digital" 61
F1 "file5BC7F38B.sch" 61
$EndSheet
Text GLabel 2250 3850 0    39   Input ~ 0
CLK1
Text GLabel 3450 2250 1    39   Input ~ 0
HF_ENABLE
$Comp
L Device:C_Small C120
U 1 1 5BE9CF68
P 4550 2700
F 0 "C120" V 4650 2600 50  0000 L CNN
F 1 "470nF" V 4400 2600 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 4550 2700 50  0001 C CNN
F 3 "" H 4550 2700 50  0001 C CNN
F 4 "478-9924-1-ND" H 4550 2700 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-9924-1-ND\">Link</a>" H 4550 2700 60  0001 C CNN "Link"
F 6 "AVX Corporation" V 4550 2700 50  0001 C CNN "Manufacturer"
F 7 "08051C474K4Z2A" V 4550 2700 50  0001 C CNN "Manufacturer PN"
F 8 "CAP CER 0.47UF 100V X7R 0805" V 4550 2700 50  0001 C CNN "Description"
	1    4550 2700
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C126
U 1 1 5BEA0426
P 6500 3600
F 0 "C126" H 6250 3550 50  0000 L CNN
F 1 "1800pF" H 6200 3700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 6500 3600 50  0001 C CNN
F 3 "" H 6500 3600 50  0000 C CNN
F 4 "445-5752-1-ND" H 6500 3600 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=445-5752-1-ND\">Link</a>" H 6500 3600 60  0001 C CNN "Link"
F 6 "TDK Corporation" H 6500 3600 50  0001 C CNN "Manufacturer"
F 7 "CGA4F2C0G2A182J085AA" H 6500 3600 50  0001 C CNN "Manufacturer PN"
F 8 "CAP CER 1800PF 100V C0G 0805" H 6500 3600 50  0001 C CNN "Description"
	1    6500 3600
	-1   0    0    1   
$EndComp
$Comp
L special:Heat-sink HS101
U 1 1 5BEA52AC
P 4350 3750
F 0 "HS101" H 4150 4050 50  0000 L CNN
F 1 "Heatsink" H 4150 3950 50  0000 L CNN
F 2 "Heatsinks:Heatsink_HF35G" H 3800 3725 50  0001 L CIN
F 3 "" H 4350 3750 50  0001 L CNN
F 4 "HS220-ND" H 4350 3750 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=HS220-ND\">Link</a>" H 4350 3750 60  0001 C CNN "Link"
F 6 "TH" H 4350 3750 50  0001 C CNN "Manufacturer"
F 7 "TH" H 4350 3750 50  0001 C CNN "Manufacturer PN"
F 8 "TH" H 4350 3750 50  0001 C CNN "Description"
	1    4350 3750
	1    0    0    -1  
$EndComp
Text Notes 9200 3100 0    39   ~ 8
3.5 MHz Out
$Sheet
S -1850 2450 1050 700 
U 5BF4C520
F0 "Non-PCB" 39
F1 "file5BF4C51F.sch" 39
$EndSheet
Text GLabel 9150 2350 2    39   Output ~ 0
80M_ANTENNA_DETECT
Wire Wire Line
	7500 3100 7500 3400
Text Notes 6300 2600 0    79   ~ 16
HF LPF
Text Notes 9050 3000 0    79   ~ 16
HF Antenna
Text Notes 6200 2800 0    39   ~ 0
6th order Chebychev\n3.9MHz 3dB cut-off\nw/ node at 10.7MHz
$Comp
L FlexFox80-rescue:INDUCTOR_SMALL-ARDF-2-Band-MiniTx-rescue L108
U 1 1 5C0D7F1D
P 7150 3400
F 0 "L108" H 7150 3500 50  0000 C CNN
F 1 "2.7uH" H 7150 3350 50  0000 C CNN
F 2 "Inductors:Toroid_T-60_2TH_Vertical" H 7150 3400 50  0001 C CNN
F 3 "" H 7150 3400 50  0001 C CNN
F 4 "T60-2 (Amidon)" H 7150 3400 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"http://www.amidoncorp.com/t60-2/\">Link</a>" H 7150 3400 60  0001 C CNN "Link"
F 6 "TH" H 7150 3400 50  0001 C CNN "Manufacturer"
F 7 "TH" H 7150 3400 50  0001 C CNN "Manufacturer PN"
F 8 "TH" H 7150 3400 50  0001 C CNN "Description"
	1    7150 3400
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:TEST_SCOPE-ARDF-2-Band-MiniTx-rescue W105
U 1 1 5C0EAA8C
P 2650 3800
F 0 "W105" V 2850 3800 50  0000 C CNN
F 1 "T2" V 2700 3850 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 2850 3800 50  0001 C CNN
F 3 "" H 2850 3800 50  0001 C CNN
F 4 "np" H 2650 3800 50  0001 C CNN "Digi-Key Part No."
F 5 "np" H 2650 3800 60  0001 C CNN "Link"
	1    2650 3800
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0116
U 1 1 5C0EAA92
P 2650 3700
F 0 "#PWR0116" H 2650 3450 50  0001 C CNN
F 1 "GND_L8" H 2650 3550 50  0001 C CNN
F 2 "" H 2650 3700 50  0000 C CNN
F 3 "" H 2650 3700 50  0000 C CNN
	1    2650 3700
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:TEST_SCOPE-ARDF-2-Band-MiniTx-rescue W108
U 1 1 5C0EB478
P 3950 4000
F 0 "W108" H 3900 4250 50  0000 C CNN
F 1 "T4" H 4000 4050 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 4150 4000 50  0001 C CNN
F 3 "" H 4150 4000 50  0001 C CNN
F 4 "np" H 3950 4000 50  0001 C CNN "Digi-Key Part No."
F 5 "np" H 3950 4000 60  0001 C CNN "Link"
	1    3950 4000
	-1   0    0    1   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0126
U 1 1 5C0EB47E
P 3850 4000
F 0 "#PWR0126" H 3850 3750 50  0001 C CNN
F 1 "GND_L8" H 3850 3850 50  0001 C CNN
F 2 "" H 3850 4000 50  0000 C CNN
F 3 "" H 3850 4000 50  0000 C CNN
	1    3850 4000
	-1   0    0    1   
$EndComp
Wire Wire Line
	3950 4000 3950 3850
Connection ~ 3950 3850
$Comp
L FlexFox80-rescue:TEST_SCOPE-ARDF-2-Band-MiniTx-rescue W111
U 1 1 5C0EDCBD
P 5800 3250
F 0 "W111" H 5850 3550 50  0000 C CNN
F 1 "T5" H 5850 3300 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 6000 3250 50  0001 C CNN
F 3 "" H 6000 3250 50  0001 C CNN
F 4 "np" H 5800 3250 50  0001 C CNN "Digi-Key Part No."
F 5 "np" H 5800 3250 60  0001 C CNN "Link"
	1    5800 3250
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0144
U 1 1 5C0EDCC3
P 5900 3250
F 0 "#PWR0144" H 5900 3000 50  0001 C CNN
F 1 "GND_L8" H 5900 3100 50  0001 C CNN
F 2 "" H 5900 3250 50  0000 C CNN
F 3 "" H 5900 3250 50  0000 C CNN
	1    5900 3250
	-1   0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:TEST_SCOPE-ARDF-2-Band-MiniTx-rescue W113
U 1 1 5C0EE9E8
P 6500 3250
F 0 "W113" H 6550 3550 50  0000 C CNN
F 1 "T6" H 6550 3300 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 6700 3250 50  0001 C CNN
F 3 "" H 6700 3250 50  0001 C CNN
F 4 "np" H 6500 3250 50  0001 C CNN "Digi-Key Part No."
F 5 "np" H 6500 3250 60  0001 C CNN "Link"
	1    6500 3250
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0148
U 1 1 5C0EE9EE
P 6600 3250
F 0 "#PWR0148" H 6600 3000 50  0001 C CNN
F 1 "GND_L8" H 6600 3100 50  0001 C CNN
F 2 "" H 6600 3250 50  0000 C CNN
F 3 "" H 6600 3250 50  0000 C CNN
	1    6600 3250
	-1   0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:TEST_SCOPE-ARDF-2-Band-MiniTx-rescue W117
U 1 1 5C0EEADD
P 7700 3250
F 0 "W117" H 7750 3500 50  0000 C CNN
F 1 "T7" H 7750 3300 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 7900 3250 50  0001 C CNN
F 3 "" H 7900 3250 50  0001 C CNN
F 4 "np" H 7700 3250 50  0001 C CNN "Digi-Key Part No."
F 5 "np" H 7700 3250 60  0001 C CNN "Link"
	1    7700 3250
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0154
U 1 1 5C0EEAE3
P 7800 3250
F 0 "#PWR0154" H 7800 3000 50  0001 C CNN
F 1 "GND_L8" H 7800 3100 50  0001 C CNN
F 2 "" H 7800 3250 50  0000 C CNN
F 3 "" H 7800 3250 50  0000 C CNN
	1    7800 3250
	-1   0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:TEST_SCOPE-ARDF-2-Band-MiniTx-rescue W106
U 1 1 5C0F543D
P 2750 3250
F 0 "W106" V 2800 3500 50  0000 C CNN
F 1 "T3" V 2800 3300 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 2950 3250 50  0001 C CNN
F 3 "" H 2950 3250 50  0001 C CNN
F 4 "np" H 2750 3250 50  0001 C CNN "Digi-Key Part No."
F 5 "np" H 2750 3250 60  0001 C CNN "Link"
	1    2750 3250
	0    -1   1    0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0118
U 1 1 5C0F5443
P 2750 3350
F 0 "#PWR0118" H 2750 3100 50  0001 C CNN
F 1 "GND_L8" H 2750 3200 50  0001 C CNN
F 2 "" H 2750 3350 50  0000 C CNN
F 3 "" H 2750 3350 50  0000 C CNN
	1    2750 3350
	0    -1   1    0   
$EndComp
$Comp
L Device:C_Small C128
U 1 1 5C0F9847
P 7100 3100
F 0 "C128" V 7000 3000 50  0000 L CNN
F 1 "91pF" V 7200 3000 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 7100 3100 50  0001 C CNN
F 3 "" H 7100 3100 50  0000 C CNN
F 4 "399-17459-1-ND" H 7100 3100 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-17459-1-ND\">Link</a>" H 7100 3100 60  0001 C CNN "Link"
F 6 "KEMET" V 7100 3100 50  0001 C CNN "Manufacturer"
F 7 "C0805C910J1GAC7800" V 7100 3100 50  0001 C CNN "Manufacturer PN"
F 8 "CAP CER 91PF 100V NP0 0805" V 7100 3100 50  0001 C CNN "Description"
	1    7100 3100
	0    1    1    0   
$EndComp
Wire Wire Line
	7200 3100 7500 3100
Wire Wire Line
	7000 3100 6850 3100
Wire Wire Line
	6850 3100 6850 3400
Connection ~ 6850 3400
Wire Wire Line
	6400 3400 6500 3400
$Comp
L FlexFox80-rescue:IRF610_TO220-ARDF-2-Band-MiniTx-rescue Q102
U 1 1 5C100056
P 4600 3800
F 0 "Q102" H 4800 3875 50  0000 L CNN
F 1 "IRF610" H 4800 3800 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-220_Neutral123_Vertical_LargePads" H 4800 3725 50  0001 L CIN
F 3 "" H 4600 3800 50  0001 L CNN
F 4 "IRF610PBF-ND" H 4600 3800 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=IRF610PBF-ND\">Link</a>" H 4600 3800 60  0001 C CNN "Link"
F 6 "TH" H 4600 3800 50  0001 C CNN "Manufacturer"
F 7 "TH" H 4600 3800 50  0001 C CNN "Manufacturer PN"
F 8 "TH" H 4600 3800 50  0001 C CNN "Description"
	1    4600 3800
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1_Small C119
U 1 1 5C11AA96
P 4150 2700
F 0 "C119" V 4250 2650 50  0000 L CNN
F 1 "47uF low ESR" V 4000 2350 50  0000 L CNN
F 2 "Capacitors_SMD:C_2917" H 4150 2700 50  0001 C CNN
F 3 "" H 4150 2700 50  0001 C CNN
F 4 "399-3904-1-ND" H 4150 2700 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-3904-1-ND\">Link</a>" H 4150 2700 60  0001 C CNN "Link"
F 6 "KEMET" V 4150 2700 50  0001 C CNN "Manufacturer"
F 7 "T495X476K020ATE150" V 4150 2700 50  0001 C CNN "Manufacturer PN"
F 8 "CAP TANT 47UF 10% 20V 2917" V 4150 2700 50  0001 C CNN "Description"
	1    4150 2700
	0    1    -1   0   
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 5C132372
P 650 7500
F 0 "#PWR0101" H 650 7250 50  0001 C CNN
F 1 "GND" H 650 7350 50  0000 C CNN
F 2 "" H 650 7500 50  0000 C CNN
F 3 "" H 650 7500 50  0000 C CNN
	1    650  7500
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0102
U 1 1 5C132EBB
P 950 7500
F 0 "#PWR0102" H 950 7250 50  0001 C CNN
F 1 "GND_L8" H 950 7350 50  0000 C CNN
F 2 "" H 950 7500 50  0000 C CNN
F 3 "" H 950 7500 50  0000 C CNN
	1    950  7500
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0124
U 1 1 5A7DD6BD
P 3450 3250
F 0 "#PWR0124" H 3450 3000 50  0001 C CNN
F 1 "GND_L8" H 3600 3100 50  0001 C CNN
F 2 "" H 3450 3250 50  0000 C CNN
F 3 "" H 3450 3250 50  0000 C CNN
	1    3450 3250
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C117
U 1 1 5A7D307D
P 3350 3250
F 0 "C117" V 3250 2900 50  0000 L CNN
F 1 "22uF" V 3250 3150 50  0000 L CNN
F 2 "Capacitors_SMD:C_1210" H 3350 3250 50  0001 C CNN
F 3 "" H 3350 3250 50  0001 C CNN
F 4 "1276-3373-1-ND" H 3350 3250 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-3373-1-ND\">Link</a>" H 3350 3250 60  0001 C CNN "Link"
F 6 "Samsung Electro-Mechanics" V 3350 3250 50  0001 C CNN "Manufacturer"
F 7 "CL32A226MAJNNNE" V 3350 3250 50  0001 C CNN "Manufacturer PN"
F 8 "CAP CER 22UF 25V X5R 1210" V 3350 3250 50  0001 C CNN "Description"
	1    3350 3250
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:LMN200B01-ARDF-2-Band-MiniTx-rescue Q101
U 1 1 5C64951A
P 3250 2500
F 0 "Q101" V 2950 2350 61  0000 L CNB
F 1 "LMN200B02" V 3200 1550 61  0000 L CNB
F 2 "TO_SOT_Packages_SMD:SOT-363-0.65" H 3265 2300 50  0001 L CNN
F 3 "https://www.diodes.com/assets/Datasheets/ds30658.pdf" H 3265 2300 50  0001 L CNN
F 4 "LMN200B02DICT-ND" H 3250 2500 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=LMN200B02DICT-ND\">Link</a>" H 3250 2500 60  0001 C CNN "Link"
F 6 "Diodes Incorporated" V 3250 2500 50  0001 C CNN "Manufacturer"
F 7 "LMN200B02-7" V 3250 2500 50  0001 C CNN "Manufacturer PN"
F 8 "MCU LOAD SWITCH 200MA SOT-363" V 3250 2500 50  0001 C CNN "Description"
	1    3250 2500
	0    1    1    0   
$EndComp
Text Notes 2200 3050 0    39   ~ 0
Optional voltage-dropping\ndiode for batteries > 12V.\nShort across if not used.
Wire Wire Line
	2750 3250 3150 3250
$Comp
L FlexFox80-rescue:ZENER-ARDF-2-Band-MiniTx-rescue D102
U 1 1 5C71D9FE
P 3150 2950
F 0 "D102" H 3150 3050 50  0000 C CNN
F 1 "4.7V" H 3150 2850 50  0000 C CNN
F 2 "Diodes_SMD:SOD-123_Handsolder" H 3150 2950 50  0001 C CNN
F 3 "" H 3150 2950 50  0001 C CNN
F 4 "BZT52C4V7-13FDICT-ND" H 3150 2950 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=BZT52C4V7-13FDICT-ND\">Link</a>" H 3150 2950 60  0001 C CNN "Link"
F 6 "Diodes Incorporated" H 3150 2950 50  0001 C CNN "Manufacturer"
F 7 "BZT52C4V7-13-F" H 3150 2950 50  0001 C CNN "Manufacturer PN"
F 8 "DIODE ZENER 4.7V 500MW SOD123" H 3150 2950 50  0001 C CNN "Description"
	1    3150 2950
	0    1    1    0   
$EndComp
Wire Wire Line
	3150 3150 3150 3250
$Comp
L FlexFox80-rescue:GND_L1-ARDF-2-Band-MiniTx-rescue #PWR0104
U 1 1 5C78B16C
P 1450 7500
F 0 "#PWR0104" H 1450 7250 50  0001 C CNN
F 1 "GND_L1" H 1450 7350 50  0000 C CNN
F 2 "" H 1450 7500 50  0000 C CNN
F 3 "" H 1450 7500 50  0000 C CNN
	1    1450 7500
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L0-ARDF-2-Band-MiniTx-rescue #PWR0105
U 1 1 5C78B9DA
P 1750 7500
F 0 "#PWR0105" H 1750 7250 50  0001 C CNN
F 1 "GND_L0" H 1750 7350 50  0000 C CNN
F 2 "" H 1750 7500 50  0000 C CNN
F 3 "" H 1750 7500 50  0000 C CNN
	1    1750 7500
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 5C78BCFE
P 1450 7500
F 0 "#FLG0101" H 1450 7595 50  0001 C CNN
F 1 "PWR_FLAG" H 1450 7680 50  0000 C CNN
F 2 "" H 1450 7500 50  0000 C CNN
F 3 "" H 1450 7500 50  0000 C CNN
	1    1450 7500
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 5C78BE63
P 1750 7500
F 0 "#FLG0102" H 1750 7595 50  0001 C CNN
F 1 "PWR_FLAG" H 1750 7680 50  0000 C CNN
F 2 "" H 1750 7500 50  0000 C CNN
F 3 "" H 1750 7500 50  0000 C CNN
	1    1750 7500
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0156
U 1 1 5CB334EC
P 8350 2350
F 0 "#PWR0156" H 8350 2100 50  0001 C CNN
F 1 "GND_L8" H 8350 2200 50  0001 C CNN
F 2 "" H 8350 2350 50  0000 C CNN
F 3 "" H 8350 2350 50  0000 C CNN
	1    8350 2350
	0    1    1    0   
$EndComp
$Comp
L FlexFox80-rescue:TVS-ARDF-2-Band-MiniTx-rescue D104
U 1 1 5CB36EAB
P 8650 2350
F 0 "D104" H 8600 2600 50  0000 C CNN
F 1 "5V" H 8600 2500 50  0000 C CNN
F 2 "Capacitors_SMD:C_0603" H 8650 2350 50  0001 C CNN
F 3 "" H 8650 2350 50  0001 C CNN
F 4 "CG0603MLC-05LECT-ND" H 8650 2350 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=CG0603MLC-05LECT-ND\">Link</a>" H 8650 2350 60  0001 C CNN "Link"
F 6 "Bourns Inc." H 8650 2350 50  0001 C CNN "Manufacturer"
F 7 "CG0603MLC-05LE" H 8650 2350 50  0001 C CNN "Manufacturer PN"
F 8 "VARISTOR 0603" H 8650 2350 50  0001 C CNN "Description"
	1    8650 2350
	-1   0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:INDUCTOR_CT-ARDF-2-Band-MiniTx-rescue L104
U 1 1 5CB66258
P 4700 3400
F 0 "L104" V 4850 3400 50  0000 C CNN
F 1 "9T #24 Bifilar T50-43" V 4850 3300 50  0001 C CNN
F 2 "Inductors:Toroid_T-50A_TH_BF_CT_Vertical" H 4700 3400 50  0001 C CNN
F 3 "" H 4700 3400 50  0000 C CNN
F 4 "FT50A-43 (Amidon)" H 4700 3400 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"http://www.amidoncorp.com/ft-50A-43/\">Link</a>" H 4700 3400 60  0001 C CNN "Link"
F 6 "TH" V 4700 3400 50  0001 C CNN "Manufacturer"
F 7 "TH" V 4700 3400 50  0001 C CNN "Manufacturer PN"
F 8 "TH" V 4700 3400 50  0001 C CNN "Description"
	1    4700 3400
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4350 3400 4400 3400
Wire Wire Line
	4700 3500 4700 3600
Wire Wire Line
	5000 3400 5450 3400
Wire Wire Line
	9150 2350 8950 2350
Connection ~ 8950 2350
Text Notes 6000 3950 0    39   ~ 0
L106 & L108:\n20T (46 cm) #24 T60-2 70% Coverage
Text Notes 4900 2950 0    39   ~ 0
L104:\n9T (24 cm) #24 Bifilar FT-50A-43 or\n6T #24 Bifilar FT-50-77
Wire Wire Line
	7700 3250 7700 3400
Connection ~ 7700 3400
$Comp
L FlexFox80-rescue:GND_L2-ARDF-2-Band-MiniTx-rescue #PWR0108
U 1 1 5CE8DCED
P 2050 7550
F 0 "#PWR0108" H 2050 7300 50  0001 C CNN
F 1 "GND_L2" H 2050 7400 50  0000 C CNN
F 2 "" H 2050 7550 50  0000 C CNN
F 3 "" H 2050 7550 50  0000 C CNN
	1    2050 7550
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L3-ARDF-2-Band-MiniTx-rescue #PWR0112
U 1 1 5CE8DEB4
P 2350 7550
F 0 "#PWR0112" H 2350 7300 50  0001 C CNN
F 1 "GND_L3" H 2350 7400 50  0000 C CNN
F 2 "" H 2350 7550 50  0000 C CNN
F 3 "" H 2350 7550 50  0000 C CNN
	1    2350 7550
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0103
U 1 1 5CE8E00C
P 2050 7550
F 0 "#FLG0103" H 2050 7645 50  0001 C CNN
F 1 "PWR_FLAG" H 2050 7730 50  0000 C CNN
F 2 "" H 2050 7550 50  0000 C CNN
F 3 "" H 2050 7550 50  0000 C CNN
	1    2050 7550
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0104
U 1 1 5CE8E164
P 2350 7550
F 0 "#FLG0104" H 2350 7645 50  0001 C CNN
F 1 "PWR_FLAG" H 2350 7730 50  0000 C CNN
F 2 "" H 2350 7550 50  0000 C CNN
F 3 "" H 2350 7550 50  0000 C CNN
	1    2350 7550
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0120
U 1 1 5A7DF174
P 3150 4250
F 0 "#PWR0120" H 3150 4000 50  0001 C CNN
F 1 "GND_L8" H 3150 4100 50  0001 C CNN
F 2 "" H 3150 4250 50  0000 C CNN
F 3 "" H 3150 4250 50  0000 C CNN
	1    3150 4250
	1    0    0    -1  
$EndComp
NoConn ~ 3450 4050
Wire Wire Line
	4350 2700 4450 2700
Wire Wire Line
	4350 2700 4350 3400
Wire Wire Line
	6500 3400 6500 3500
Wire Wire Line
	6500 3400 6850 3400
Wire Wire Line
	7500 3400 7700 3400
Wire Wire Line
	7500 3400 7500 3500
Wire Wire Line
	5800 3400 5900 3400
Wire Wire Line
	5800 3400 5800 3500
Wire Wire Line
	850  7450 950  7450
Wire Wire Line
	850  7350 850  7450
Wire Wire Line
	3150 3250 3250 3250
Wire Wire Line
	3150 3250 3150 3600
Wire Wire Line
	3150 3600 3150 3650
Wire Wire Line
	2650 3850 2850 3850
Wire Wire Line
	3950 3850 4400 3850
Wire Wire Line
	6850 3400 6900 3400
Wire Wire Line
	8950 2350 8950 2900
$Comp
L Connector:DIN-5_180degree J101
U 1 1 61BDEA6F
P 8500 3000
F 0 "J101" H 8650 3250 50  0000 C CNN
F 1 "DIN-5_180degree" H 8500 2634 50  0001 C CNN
F 2 "Connect:SDF-50J" H 8500 3000 50  0001 C CNN
F 3 "https://www.cuidevices.com/product/resource/sdf-j.pdf" H 8500 3000 50  0001 C CNN
F 4 "CP-7050-ND" H 8500 3000 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=CP-7050-ND\">Link</a>" H 8500 3000 50  0001 C CNN "Link"
F 6 "TH" H 8500 3000 50  0001 C CNN "Manufacturer"
F 7 "TH" H 8500 3000 50  0001 C CNN "Manufacturer PN"
F 8 "TH" H 8500 3000 50  0001 C CNN "Description"
	1    8500 3000
	1    0    0    -1  
$EndComp
NoConn ~ 8200 3000
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0145
U 1 1 61C04BAD
P 8500 2700
F 0 "#PWR0145" H 8500 2450 50  0001 C CNN
F 1 "GND_L8" H 8500 2550 50  0001 C CNN
F 2 "" H 8500 2700 50  0000 C CNN
F 3 "" H 8500 2700 50  0000 C CNN
	1    8500 2700
	-1   0    0    1   
$EndComp
Wire Wire Line
	8950 3400 8950 3000
Wire Wire Line
	8950 3000 8800 3000
Wire Wire Line
	7700 3400 8950 3400
Wire Wire Line
	8800 2900 8950 2900
NoConn ~ 8200 2900
Wire Wire Line
	8950 2100 9000 2100
Wire Wire Line
	8950 2100 8950 2350
$Comp
L Device:D_Schottky_ALT D101
U 1 1 607D3A52
P 9150 2100
F 0 "D101" H 9150 2317 50  0000 C CNN
F 1 "Dual Schottky" H 9150 2226 50  0001 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-363-0.65" H 9150 2100 50  0001 C CNN
F 3 "" H 9150 2100 50  0001 C CNN
F 4 "MBD54DWT1GOSCT-ND" H 9150 2100 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=MBD54DWT1GOSCT-ND\">Link</a>" H 9150 2100 50  0001 C CNN "Link"
F 6 "ON Semiconductor" H 9150 2100 50  0001 C CNN "Manufacturer"
F 7 "MBD54DWT1G" H 9150 2100 50  0001 C CNN "Manufacturer PN"
F 8 "DIODE ARRAY SCHOTTKY 30V SC88" H 9150 2100 50  0001 C CNN "Description"
	1    9150 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	9300 2100 9550 2100
$Comp
L Device:C_Small C122
U 1 1 607F36EA
P 5550 3400
F 0 "C122" V 5650 3300 50  0000 L CNN
F 1 "470nF" V 5400 3300 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 5550 3400 50  0001 C CNN
F 3 "" H 5550 3400 50  0001 C CNN
F 4 "478-9924-1-ND" H 5550 3400 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-9924-1-ND\">Link</a>" H 5550 3400 60  0001 C CNN "Link"
F 6 "AVX Corporation" V 5550 3400 50  0001 C CNN "Manufacturer"
F 7 "08051C474K4Z2A" V 5550 3400 50  0001 C CNN "Manufacturer PN"
F 8 "CAP CER 0.47UF 100V X7R 0805" V 5550 3400 50  0001 C CNN "Description"
	1    5550 3400
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C131
U 1 1 60A9B90E
P 7500 3600
F 0 "C131" H 7510 3670 50  0000 L CNN
F 1 "1100pF" H 7510 3520 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 7500 3600 50  0001 C CNN
F 3 "" H 7500 3600 50  0000 C CNN
F 4 "399-14552-1-ND" H 7500 3600 50  0001 C CNN "Digi-Key Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-14552-1-ND\">Link</a>" H 7500 3600 60  0001 C CNN "Link"
F 6 "KEMET" H 7500 3600 50  0001 C CNN "Manufacturer"
F 7 "C0805C112J5GAC7800" H 7500 3600 50  0001 C CNN "Manufacturer PN"
F 8 "CAP CER 1100PF 50V NP0 0805" H 7500 3600 50  0001 C CNN "Description"
	1    7500 3600
	1    0    0    -1  
$EndComp
$EndSCHEMATC
