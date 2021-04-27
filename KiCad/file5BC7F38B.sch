EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr B 17000 11000
encoding utf-8
Sheet 3 4
Title "ARDF 2-Band Mini Transmitter"
Date "2020-10-16"
Rev "P2.1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L power:GNDD #PWR0173
U 1 1 5797CE10
P 9700 3400
F 0 "#PWR0173" H 9700 3150 50  0001 C CNN
F 1 "GNDD" H 9600 3400 50  0000 C CNN
F 2 "" H 9700 3400 50  0000 C CNN
F 3 "" H 9700 3400 50  0000 C CNN
	1    9700 3400
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0175
U 1 1 5797D116
P 5350 6900
F 0 "#PWR0175" H 5350 6650 50  0001 C CNN
F 1 "GNDD" H 5350 6750 50  0000 C CNN
F 2 "" H 5350 6900 50  0000 C CNN
F 3 "" H 5350 6900 50  0000 C CNN
	1    5350 6900
	1    0    0    -1  
$EndComp
$Comp
L power:GNDA #PWR0176
U 1 1 5797D770
P 5100 6800
F 0 "#PWR0176" H 5100 6550 50  0001 C CNN
F 1 "GNDA" H 5100 6650 50  0000 C CNN
F 2 "" H 5100 6800 50  0000 C CNN
F 3 "" H 5100 6800 50  0000 C CNN
	1    5100 6800
	1    0    0    -1  
$EndComp
Text Label 7500 4350 0    39   ~ 0
PB0
Entry Wire Line
	7600 4350 7700 4450
Text Label 7500 4450 0    39   ~ 0
PB1
Text Label 7500 4550 0    39   ~ 0
INT2
Text Label 7500 4650 0    39   ~ 0
PB3
Text Label 7500 4750 0    39   ~ 0
PB4
Text Label 7500 4850 0    39   ~ 0
MOSI
Text Label 7500 4950 0    39   ~ 0
MISO
Text Label 7500 5050 0    39   ~ 0
SCK
Text Label 7500 5750 0    39   ~ 0
PC5
Text Label 7500 5650 0    39   ~ 0
PC4
Text Label 7500 5450 0    39   ~ 0
PC2
Text Label 7500 5550 0    39   ~ 0
PC3
Text Label 7500 5350 0    39   ~ 0
SDA
Text Label 7500 5250 0    39   ~ 0
SCL
Text Label 7500 6150 0    39   ~ 0
RXD0
Text Label 7500 6250 0    39   ~ 0
TXD0
Text Label 7500 6350 0    39   ~ 0
RXD1
Text Label 7500 6450 0    39   ~ 0
TXD1
Text Label 7500 6550 0    39   ~ 0
PD4
Text Label 7500 6650 0    39   ~ 0
PD5
Text Label 7500 6750 0    39   ~ 0
PD6
Text Label 7500 6850 0    39   ~ 0
PD7
Entry Wire Line
	7600 4450 7700 4550
Entry Wire Line
	7600 4550 7700 4650
Entry Wire Line
	7600 4650 7700 4750
Entry Wire Line
	7600 4750 7700 4850
Entry Wire Line
	7600 4850 7700 4950
Entry Wire Line
	7600 4950 7700 5050
Entry Wire Line
	7600 5050 7700 5150
Entry Wire Line
	7600 5250 7700 5350
Entry Wire Line
	7600 5350 7700 5450
Entry Wire Line
	7600 5450 7700 5550
Entry Wire Line
	7600 5550 7700 5650
Entry Wire Line
	7600 5650 7700 5750
Entry Wire Line
	7600 5750 7700 5850
Entry Wire Line
	7600 5850 7700 5950
Entry Wire Line
	7600 6150 7700 6250
Entry Wire Line
	7600 6250 7700 6350
Entry Wire Line
	7600 6350 7700 6450
Entry Wire Line
	7600 6450 7700 6550
Entry Wire Line
	7600 6550 7700 6650
Entry Wire Line
	7600 6650 7700 6750
Entry Wire Line
	7600 6750 7700 6850
Entry Wire Line
	7600 6850 7700 6950
Entry Wire Line
	7700 4750 7800 4650
Entry Wire Line
	7700 4850 7800 4750
Entry Wire Line
	7700 5450 7800 5350
Entry Wire Line
	7700 5650 7800 5550
Entry Wire Line
	7700 4650 7800 4550
Text Label 7800 5350 0    39   ~ 0
SDA
Text Label 7500 4050 0    39   ~ 0
ADC6
Text Label 7500 4150 0    39   ~ 0
ADC7
Entry Wire Line
	7700 6750 7800 6650
Text Label 7800 6550 0    39   ~ 0
PD4
Entry Wire Line
	7700 5750 7800 5650
Text Label 7800 5650 0    39   ~ 0
PC4
Entry Wire Line
	7700 6650 7800 6550
Entry Wire Line
	7700 5850 7800 5750
Entry Wire Line
	7700 5950 7800 5850
Entry Wire Line
	7700 6250 7800 6150
Entry Wire Line
	7700 6350 7800 6250
Text Label 7800 5750 0    39   ~ 0
PC5
Text Label 7800 5450 0    39   ~ 0
PC2
Text Label 7800 4550 0    39   ~ 0
INT2
Entry Wire Line
	7700 6850 7800 6750
Text Label 7800 6750 0    39   ~ 0
PD6
Entry Wire Line
	7700 6950 7800 6850
Text Label 7800 6850 0    39   ~ 0
PD7
Text GLabel 11450 4250 2    39   Output ~ 0
DI_SCL
Text GLabel 11450 4350 2    39   Output ~ 0
DI_SDA
Entry Wire Line
	7700 6450 7800 6350
Text Label 7800 5850 0    39   ~ 0
PC6
$Comp
L special:SI5351 U304
U 1 1 57ABD6C6
P 11850 3150
F 0 "U304" H 11850 3650 60  0000 C CNB
F 1 "SI5351A" H 11850 3150 60  0000 C CNB
F 2 "Housings_SSOP:MSOP-10_3x3mm_Pitch0.5mm_Handsolder" H 11850 3150 50  0001 C CNN
F 3 "http://www.silabs.com/Support%20Documents/TechnicalDocs/Si5351-B.pdf" H 11850 3150 50  0001 C CNN
F 4 "336-2399-5-ND" H 11850 3150 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=336-2399-5-ND\">Link</a>" H 11850 3150 60  0001 C CNN "Link"
	1    11850 3150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R305
U 1 1 57AD3D94
P 2100 8350
F 0 "R305" V 2180 8350 50  0000 C CNN
F 1 "10k" V 2100 8350 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 2030 8350 50  0001 C CNN
F 3 "http://www.yageo.com/documents/recent/PYu-RC_Group_51_RoHS_L_04.pdf" H 2100 8350 50  0001 C CNN
F 4 "311-10.0KCRCT-ND" H 2100 8350 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0KCRCT-ND\">Link</a>" H 2100 8350 60  0001 C CNN "Link"
	1    2100 8350
	0    -1   -1   0   
$EndComp
Text GLabel 2250 8500 0    39   Input ~ 0
~RESET~
$Comp
L FlexFox80-rescue:CONN_02X03-ARDF-2-Band-MiniTx-rescue P301
U 1 1 57AD5B1A
P 4100 5600
F 0 "P301" H 4100 5800 50  0000 C CNN
F 1 "ISP/PDI" H 4100 5400 50  0000 C CNN
F 2 "Pin_Headers:SM_Pin_Header_Straight_2x03" H 4100 4400 50  0001 C CNN
F 3 "http://katalog.we-online.de/em/datasheet/6100xx21121.pdf" H 4100 4400 50  0001 C CNN
F 4 "609-3487-1-ND" H 4100 5600 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=609-3487-1-ND\">Link</a>" H 4100 5600 60  0001 C CNN "Link"
	1    4100 5600
	1    0    0    -1  
$EndComp
Text GLabel 3750 5800 0    39   Output ~ 0
~RESET~
$Comp
L power:GNDD #PWR0121
U 1 1 57AD5D10
P 4450 5800
F 0 "#PWR0121" H 4450 5550 50  0001 C CNN
F 1 "GNDD" H 4450 5650 50  0000 C CNN
F 2 "" H 4450 5800 50  0000 C CNN
F 3 "" H 4450 5800 50  0000 C CNN
	1    4450 5800
	1    0    0    -1  
$EndComp
Text GLabel 4500 5600 2    39   Input ~ 0
MOSI
Text GLabel 7950 4850 2    39   Output ~ 0
MOSI
Text GLabel 3700 5400 0    39   Input ~ 0
MISO
Entry Wire Line
	7700 4450 7800 4350
Text Label 7800 5250 0    39   ~ 0
SCL
Text GLabel 7950 4950 2    39   Output ~ 0
MISO
Entry Wire Line
	7700 4550 7800 4450
Text GLabel 7950 5050 2    39   Output ~ 0
SCK
Text GLabel 3700 5600 0    39   Input ~ 0
SCK
Entry Wire Line
	7700 6550 7800 6450
Entry Wire Line
	7700 5550 7800 5450
Entry Wire Line
	7700 4250 7800 4150
Text Label 7800 5550 0    39   ~ 0
PC3
Entry Wire Line
	7700 5350 7800 5250
Text Label 7800 6650 0    39   ~ 0
PD5
NoConn ~ 11200 3150
$Comp
L Device:C C310
U 1 1 57B42379
P 10950 2650
F 0 "C310" V 10800 2500 50  0000 L CNN
F 1 "470nF" V 11100 2550 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 10988 2500 50  0001 C CNN
F 3 "" H 10950 2650 50  0001 C CNN
F 4 "478-9924-1-ND" H 10950 2650 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-9924-1-ND\">Link</a>" H 10950 2650 60  0001 C CNN "Link"
	1    10950 2650
	0    1    1    0   
$EndComp
$Comp
L power:GNDD #PWR0122
U 1 1 57B42657
P 10700 2700
F 0 "#PWR0122" H 10700 2450 50  0001 C CNN
F 1 "GNDD" H 10700 2550 50  0000 C CNN
F 2 "" H 10700 2700 50  0000 C CNN
F 3 "" H 10700 2700 50  0000 C CNN
	1    10700 2700
	1    0    0    -1  
$EndComp
$Comp
L Device:C C312
U 1 1 57B42FC2
P 13100 3450
F 0 "C312" H 13125 3550 50  0000 L CNN
F 1 "470nF" H 13125 3350 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 13138 3300 50  0001 C CNN
F 3 "" H 13100 3450 50  0001 C CNN
F 4 "478-9924-1-ND" H 13100 3450 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-9924-1-ND\">Link</a>" H 13100 3450 60  0001 C CNN "Link"
	1    13100 3450
	-1   0    0    1   
$EndComp
$Comp
L power:GNDD #PWR0177
U 1 1 57B4BF2C
P 4750 4150
F 0 "#PWR0177" H 4750 3900 50  0001 C CNN
F 1 "GNDD" H 4750 4000 50  0000 C CNN
F 2 "" H 4750 4150 50  0000 C CNN
F 3 "" H 4750 4150 50  0000 C CNN
	1    4750 4150
	0    1    1    0   
$EndComp
$Comp
L power:GNDD #PWR0178
U 1 1 57B4C04E
P 4600 4450
F 0 "#PWR0178" H 4600 4200 50  0001 C CNN
F 1 "GNDD" H 4600 4300 50  0000 C CNN
F 2 "" H 4600 4450 50  0000 C CNN
F 3 "" H 4600 4450 50  0000 C CNN
	1    4600 4450
	0    1    -1   0   
$EndComp
$Comp
L Device:C_Small C308
U 1 1 57B51AE4
P 9700 3200
F 0 "C308" H 9710 3270 50  0000 L CNN
F 1 "100nF" H 9710 3120 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 9700 3200 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 9700 3200 50  0001 C CNN
F 4 "478-1395-1-ND" H 9700 3200 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 9700 3200 60  0001 C CNN "Link"
	1    9700 3200
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C306
U 1 1 57AE8E4C
P 4800 4650
F 0 "C306" V 4700 4450 50  0000 L CNN
F 1 "100nF" V 4900 4700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4800 4650 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 4800 4650 50  0001 C CNN
F 4 "478-1395-1-ND" H 4800 4650 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 4800 4650 60  0001 C CNN "Link"
	1    4800 4650
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C307
U 1 1 57AE90C4
P 4850 4450
F 0 "C307" V 4750 4450 50  0000 L CNN
F 1 "100nF" V 4950 4200 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4850 4450 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 4850 4450 50  0001 C CNN
F 4 "478-1395-1-ND" H 4850 4450 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 4850 4450 60  0001 C CNN "Link"
	1    4850 4450
	0    -1   1    0   
$EndComp
Text Notes 6150 2550 0    79   ~ 16
Microcontroller
$Comp
L FlexFox80-rescue:GNDIF-ARDF-2-Band-MiniTx-rescue #PWR0125
U 1 1 57B0E34C
P 12050 10300
F 0 "#PWR0125" H 12050 10050 50  0001 C CNN
F 1 "GNDIF" H 12050 10150 50  0000 C CNN
F 2 "" H 12050 10300 50  0000 C CNN
F 3 "" H 12050 10300 50  0000 C CNN
	1    12050 10300
	1    0    0    -1  
$EndComp
$Comp
L power:GNDA #PWR0179
U 1 1 57B0FFE1
P 11800 10300
F 0 "#PWR0179" H 11800 10050 50  0001 C CNN
F 1 "GNDA" H 11800 10150 50  0000 C CNN
F 2 "" H 11800 10300 50  0000 C CNN
F 3 "" H 11800 10300 50  0000 C CNN
	1    11800 10300
	1    0    0    -1  
$EndComp
$Comp
L power:GNDA #PWR0127
U 1 1 57B43438
P 5450 5100
F 0 "#PWR0127" H 5450 4850 50  0001 C CNN
F 1 "GNDA" H 5450 4950 50  0000 C CNN
F 2 "" H 5450 5100 50  0000 C CNN
F 3 "" H 5450 5100 50  0000 C CNN
	1    5450 5100
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C304
U 1 1 57B43F7B
P 5450 5000
F 0 "C304" H 5500 4900 50  0000 L CNN
F 1 "10uF" H 5550 5050 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5450 5000 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 5450 5000 50  0001 C CNN
F 4 "490-5523-1-ND" H 5450 5000 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=490-5523-1-ND\">Link</a>" H 5450 5000 60  0001 C CNN "Link"
	1    5450 5000
	-1   0    0    1   
$EndComp
Text Label 4350 5600 0    39   ~ 0
PB3
Text Label 3850 5500 2    39   ~ 0
PB4
Text Label 3850 5600 2    39   ~ 0
PB5
Text Notes 10750 2300 0    79   ~ 16
SI5351 Clock
$Comp
L FlexFox80-rescue:CONN_01X01-ARDF-2-Band-MiniTx-rescue P302
U 1 1 57BD33AA
P 11400 2650
F 0 "P302" H 11400 2750 50  0000 C CNN
F 1 "Thru" H 11500 2650 50  0001 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01" H 11400 2650 50  0001 C CNN
F 3 "" H 11400 2650 50  0000 C CNN
F 4 "np" H 11400 2650 60  0001 C CNN "Part No."
F 5 "np" H 11400 2650 60  0001 C CNN "Link"
	1    11400 2650
	1    0    0    -1  
$EndComp
Text Notes 11800 4150 0    39   ~ 8
I2C pull-ups can be provided by uC \nport pin pull-ups on SCL and SDA.
$Comp
L special:ASTX-H11 X301
U 1 1 58755171
P 10400 3200
F 0 "X301" H 10400 3550 61  0000 C CNB
F 1 "ASTX-H11-25.MHZ" H 10400 3200 61  0000 C CNB
F 2 "Crystals:ASTX-H11" H 10400 3200 60  0001 C CNN
F 3 "" H 10400 3200 60  0000 C CNN
F 4 "535-12039-1-ND" H 10400 3200 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=535-12039-1-ND\">Link</a>" H 10400 3200 60  0001 C CNN "Link"
	1    10400 3200
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:TEST_1P-ARDF-2-Band-MiniTx-rescue W301
U 1 1 587A8789
P 1750 5450
F 0 "W301" V 1750 5720 50  0000 C CNN
F 1 "TEST_1P" H 1750 5650 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Square-SMD-Pad_Small" H 1950 5450 50  0001 C CNN
F 3 "" H 1950 5450 50  0000 C CNN
F 4 "np" H 1750 5450 60  0001 C CNN "Part No."
F 5 "np" H 1750 5450 60  0001 C CNN "Link"
	1    1750 5450
	0    -1   1    0   
$EndComp
$Comp
L FlexFox80-rescue:TEST_1P-ARDF-2-Band-MiniTx-rescue W302
U 1 1 587A8A8E
P 1750 5850
F 0 "W302" V 1750 6120 50  0000 C CNN
F 1 "TEST_1P" H 1750 6050 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Square-SMD-Pad_Small" H 1950 5850 50  0001 C CNN
F 3 "" H 1950 5850 50  0000 C CNN
F 4 "np" H 1750 5850 60  0001 C CNN "Part No."
F 5 "np" H 1750 5850 60  0001 C CNN "Link"
	1    1750 5850
	0    -1   1    0   
$EndComp
Text Notes 1650 4700 0    79   ~ 16
Real-Time Clock
Text Notes 12350 9500 0    118  ~ 24
PCB: Flex Fox 80 ARDF Transmitter  (p. 3/3)
$Comp
L special:DS3231 U301
U 1 1 591E6034
P 2150 5650
F 0 "U301" H 2350 6150 60  0000 C CNB
F 1 "DS3231" H 2400 6050 60  0000 C CNB
F 2 "SMD_Packages:SO16_Handsolder" H 2150 5650 60  0001 C CNN
F 3 "" H 2150 5650 60  0000 C CNN
F 4 "DS3231S#T&RCT-ND" H 2150 5650 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=DS3231S%23T%26RCT-ND\">Link</a>" H 2150 5650 60  0001 C CNN "Link"
	1    2150 5650
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0128
U 1 1 591E61CB
P 2150 6200
F 0 "#PWR0128" H 2150 5950 50  0001 C CNN
F 1 "GNDD" H 2150 6050 50  0000 C CNN
F 2 "" H 2150 6200 50  0000 C CNN
F 3 "" H 2150 6200 50  0000 C CNN
	1    2150 6200
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0129
U 1 1 591E902A
P 2850 5500
F 0 "#PWR0129" H 2850 5250 50  0001 C CNN
F 1 "GNDD" H 2850 5350 50  0000 C CNN
F 2 "" H 2850 5500 50  0000 C CNN
F 3 "" H 2850 5500 50  0000 C CNN
	1    2850 5500
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C301
U 1 1 591EA9E9
P 2000 5100
F 0 "C301" V 1900 4950 50  0000 L CNN
F 1 "1uF" V 2100 5050 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2000 5100 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 2000 5100 50  0001 C CNN
F 4 "311-1365-1-ND" H 2000 5100 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-1365-1-ND\">Link</a>" H 2000 5100 60  0001 C CNN "Link"
	1    2000 5100
	0    1    1    0   
$EndComp
$Comp
L power:GNDD #PWR0180
U 1 1 591EABD4
P 1900 5100
F 0 "#PWR0180" H 1900 4850 50  0001 C CNN
F 1 "GNDD" H 1900 4950 50  0000 C CNN
F 2 "" H 1900 5100 50  0000 C CNN
F 3 "" H 1900 5100 50  0000 C CNN
	1    1900 5100
	0    1    -1   0   
$EndComp
$Comp
L power:GND #PWR0131
U 1 1 59203AA4
P 11350 10300
F 0 "#PWR0131" H 11350 10050 50  0001 C CNN
F 1 "GND" H 11350 10150 50  0000 C CNN
F 2 "" H 11350 10300 50  0000 C CNN
F 3 "" H 11350 10300 50  0000 C CNN
	1    11350 10300
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0132
U 1 1 59203F23
P 11550 10300
F 0 "#PWR0132" H 11550 10050 50  0001 C CNN
F 1 "GNDD" H 11550 10150 50  0000 C CNN
F 2 "" H 11550 10300 50  0000 C CNN
F 3 "" H 11550 10300 50  0000 C CNN
	1    11550 10300
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:Vproc-ARDF-2-Band-MiniTx-rescue #PWR0181
U 1 1 591CAC09
P 5150 3450
F 0 "#PWR0181" H 5150 3300 50  0001 C CNN
F 1 "Vproc" H 5150 3600 50  0000 C CNN
F 2 "" H 5150 3450 50  0000 C CNN
F 3 "" H 5150 3450 50  0000 C CNN
	1    5150 3450
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0134
U 1 1 591CB758
P 4750 3850
F 0 "#FLG0134" H 4750 3945 50  0001 C CNN
F 1 "PWR_FLAG" H 4750 4030 50  0000 C CNN
F 2 "" H 4750 3850 50  0000 C CNN
F 3 "" H 4750 3850 50  0000 C CNN
	1    4750 3850
	1    0    0    -1  
$EndComp
Text Notes 5300 3500 1    59   ~ 0
Power to Processor & RTC Only
Wire Wire Line
	11200 2600 11200 2650
Wire Wire Line
	5600 6700 5350 6700
Wire Wire Line
	13100 3250 12500 3250
Wire Wire Line
	5600 6800 5350 6800
Wire Wire Line
	5350 6700 5350 6800
Connection ~ 5350 6800
Wire Wire Line
	5100 6500 5100 6800
Wire Wire Line
	7500 4350 7600 4350
Wire Wire Line
	7600 4450 7500 4450
Wire Wire Line
	7600 4550 7500 4550
Wire Wire Line
	7600 4650 7500 4650
Wire Wire Line
	7600 4750 7500 4750
Wire Wire Line
	7600 4850 7500 4850
Wire Wire Line
	7600 4950 7500 4950
Wire Wire Line
	7600 5050 7500 5050
Wire Wire Line
	7600 5450 7500 5450
Wire Wire Line
	7600 5550 7500 5550
Wire Wire Line
	7600 5650 7500 5650
Wire Wire Line
	7600 5750 7500 5750
Wire Wire Line
	7600 5850 7500 5850
Wire Wire Line
	7600 6250 7500 6250
Wire Wire Line
	7600 6350 7500 6350
Wire Wire Line
	7500 6450 7600 6450
Wire Wire Line
	7600 6550 7500 6550
Wire Wire Line
	7600 6650 7500 6650
Wire Wire Line
	7600 6750 7500 6750
Wire Wire Line
	7600 6850 7500 6850
Wire Wire Line
	12600 3050 12500 3050
Wire Wire Line
	4350 5700 4450 5700
Wire Wire Line
	4450 5700 4450 5800
Wire Wire Line
	4350 5500 4450 5500
Wire Wire Line
	4450 5500 4450 5450
Wire Wire Line
	3850 5400 3850 5500
Wire Wire Line
	3700 5400 3850 5400
Wire Wire Line
	3850 5700 3850 5800
Wire Wire Line
	3850 5800 3750 5800
Wire Wire Line
	2250 8500 2300 8500
Wire Wire Line
	4900 4650 5150 4650
Wire Wire Line
	11200 3050 10900 3050
Wire Wire Line
	11100 2650 11200 2650
Connection ~ 11200 2650
Wire Wire Line
	13100 3150 13100 3250
Wire Wire Line
	2300 8500 2300 8350
Connection ~ 2300 8350
Wire Wire Line
	4800 4150 4750 4150
Wire Wire Line
	9700 3000 9700 3050
Wire Wire Line
	9700 3050 9900 3050
Wire Wire Line
	9700 3300 9700 3350
Wire Wire Line
	9700 3350 9900 3350
Connection ~ 5150 4250
Wire Wire Line
	7600 5250 7500 5250
Wire Wire Line
	7600 5350 7500 5350
Wire Wire Line
	11100 3350 11200 3350
Wire Wire Line
	11000 3250 11200 3250
Wire Wire Line
	10800 2650 10700 2650
Wire Wire Line
	10700 2650 10700 2700
Connection ~ 9700 3050
Connection ~ 9700 3350
Wire Wire Line
	1850 8350 1950 8350
Wire Wire Line
	7600 6150 7500 6150
Wire Wire Line
	3700 5600 3850 5600
Wire Wire Line
	4500 5600 4350 5600
Wire Wire Line
	1850 8150 1900 8150
Wire Wire Line
	1950 6100 1950 6150
Wire Wire Line
	1950 6150 2000 6150
Wire Wire Line
	2350 6150 2350 6100
Wire Wire Line
	2000 6100 2000 6150
Connection ~ 2000 6150
Wire Wire Line
	2050 6100 2050 6150
Connection ~ 2050 6150
Wire Wire Line
	2100 6100 2100 6150
Connection ~ 2100 6150
Wire Wire Line
	2150 6100 2150 6150
Wire Wire Line
	2200 6100 2200 6150
Connection ~ 2200 6150
Wire Wire Line
	2250 6100 2250 6150
Connection ~ 2250 6150
Wire Wire Line
	2300 6100 2300 6150
Connection ~ 2300 6150
Connection ~ 2150 6150
Wire Wire Line
	2150 5000 2150 5100
Connection ~ 2150 5100
Wire Wire Line
	7800 6250 7950 6250
Wire Wire Line
	11350 10300 11350 10200
Wire Wire Line
	11100 10200 11350 10200
Wire Wire Line
	11550 9850 11550 10050
Text GLabel 7950 3950 2    39   Input ~ 0
TX_BATTERY_VOLTAGE
Text GLabel 7950 4050 2    39   Input ~ 0
TX_PA_VOLTAGE
Text GLabel 7950 6750 2    39   Output ~ 0
MAIN_PWR_ENABLE
Text GLabel 7950 6850 2    39   Output ~ 0
TX_FINAL_VOLTAGE_ENABLE
Text GLabel 2550 5750 2    39   Output ~ 0
RTC_SQW
Text GLabel 7950 4550 2    39   Input ~ 0
RTC_SQW
Text GLabel 7950 5750 2    39   Input ~ 0
~ANT_CONNECT_INT~
Wire Wire Line
	11000 4250 11000 3250
Connection ~ 11000 4250
Wire Wire Line
	11100 4350 11100 3350
Connection ~ 11100 4350
$Comp
L Device:R R304
U 1 1 594E78F8
P 8100 6250
F 0 "R304" V 8180 6250 50  0000 C CNN
F 1 "10" V 8100 6250 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8030 6250 50  0001 C CNN
F 3 "" H 8100 6250 50  0001 C CNN
F 4 "311-10.0CRCT-ND" H 8100 6250 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0CRCT-ND\">Link</a>" H 8100 6250 60  0001 C CNN "Link"
	1    8100 6250
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C305
U 1 1 5959ECE9
P 4900 4150
F 0 "C305" V 5000 4200 50  0000 L CNN
F 1 "10uF" V 5000 3950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4900 4150 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 4900 4150 50  0001 C CNN
F 4 "490-5523-1-ND" H 4900 4150 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=490-5523-1-ND\">Link</a>" H 4900 4150 60  0001 C CNN "Link"
	1    4900 4150
	0    -1   -1   0   
$EndComp
$Comp
L power:GNDD #PWR0135
U 1 1 595A2417
P 4600 4650
F 0 "#PWR0135" H 4600 4400 50  0001 C CNN
F 1 "GNDD" H 4600 4500 50  0000 C CNN
F 2 "" H 4600 4650 50  0000 C CNN
F 3 "" H 4600 4650 50  0000 C CNN
	1    4600 4650
	0    1    1    0   
$EndComp
Wire Wire Line
	4600 4650 4700 4650
Wire Wire Line
	4600 4450 4750 4450
Wire Wire Line
	12050 10200 12050 10300
Connection ~ 11550 10200
Wire Wire Line
	11800 10300 11800 10200
Connection ~ 11800 10200
$Comp
L power:GNDD #PWR0182
U 1 1 595A7BF6
P 13100 3600
F 0 "#PWR0182" H 13100 3350 50  0001 C CNN
F 1 "GNDD" H 13100 3450 50  0000 C CNN
F 2 "" H 13100 3600 50  0000 C CNN
F 3 "" H 13100 3600 50  0000 C CNN
	1    13100 3600
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0137
U 1 1 595A7E0D
P 12800 3150
F 0 "#PWR0137" H 12800 2900 50  0001 C CNN
F 1 "GNDD" H 12800 3000 50  0000 C CNN
F 2 "" H 12800 3150 50  0000 C CNN
F 3 "" H 12800 3150 50  0000 C CNN
	1    12800 3150
	0    -1   -1   0   
$EndComp
Connection ~ 13100 3250
Wire Wire Line
	12800 3150 12500 3150
$Comp
L FlexFox80-rescue:Vproc-ARDF-2-Band-MiniTx-rescue #PWR0138
U 1 1 59E2AD5F
P 1850 8000
F 0 "#PWR0138" H 1850 7850 50  0001 C CNN
F 1 "Vproc" H 1850 8150 50  0000 C CNN
F 2 "" H 1850 8000 50  0000 C CNN
F 3 "" H 1850 8000 50  0000 C CNN
	1    1850 8000
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:CONN_01X01-ARDF-2-Band-MiniTx-rescue W307
U 1 1 59FF6C44
P 11800 9850
F 0 "W307" H 11800 9950 50  0000 C CNN
F 1 "CONN_01X01" V 11900 9850 50  0001 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01" H 11800 9850 50  0001 C CNN
F 3 "" H 11800 9850 50  0001 C CNN
F 4 "np" H 11800 9850 60  0001 C CNN "Part No."
F 5 "np" H 11800 9850 60  0001 C CNN "Link"
	1    11800 9850
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:CONN_01X01-ARDF-2-Band-MiniTx-rescue W308
U 1 1 59FF6D51
P 11800 10050
F 0 "W308" H 11800 10150 50  0000 C CNN
F 1 "CONN_01X01" V 11900 10050 50  0001 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01" H 11800 10050 50  0001 C CNN
F 3 "" H 11800 10050 50  0001 C CNN
F 4 "np" H 11800 10050 60  0001 C CNN "Part No."
F 5 "np" H 11800 10050 60  0001 C CNN "Link"
	1    11800 10050
	1    0    0    -1  
$EndComp
Wire Wire Line
	11600 9850 11550 9850
Wire Wire Line
	11600 10050 11550 10050
Connection ~ 11550 10050
$Comp
L power:GNDD #PWR0139
U 1 1 5A0895E4
P 10650 4000
F 0 "#PWR0139" H 10650 3750 50  0001 C CNN
F 1 "GNDD" H 10650 3850 50  0000 C CNN
F 2 "" H 10650 4000 50  0000 C CNN
F 3 "" H 10650 4000 50  0000 C CNN
	1    10650 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	10450 4000 10450 4250
Connection ~ 10450 4250
Wire Wire Line
	10550 4000 10550 4350
Connection ~ 10550 4350
Wire Wire Line
	12500 2250 12500 2950
Wire Wire Line
	12600 2250 12600 3050
$Comp
L Device:R R307
U 1 1 5A6C903F
P 11550 3950
F 0 "R307" V 11630 3950 50  0000 C CNN
F 1 "10k" V 11550 3950 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 11480 3950 50  0001 C CNN
F 3 "http://www.yageo.com/documents/recent/PYu-RC_Group_51_RoHS_L_04.pdf" H 11550 3950 50  0001 C CNN
F 4 "311-10.0KCRCT-ND" H 11550 3950 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0KCRCT-ND\">Link</a>" H 11550 3950 60  0001 C CNN "Link"
	1    11550 3950
	0    1    1    0   
$EndComp
$Comp
L Device:R R306
U 1 1 5A6C9183
P 11550 3750
F 0 "R306" V 11630 3750 50  0000 C CNN
F 1 "10k" V 11550 3750 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 11480 3750 50  0001 C CNN
F 3 "http://www.yageo.com/documents/recent/PYu-RC_Group_51_RoHS_L_04.pdf" H 11550 3750 50  0001 C CNN
F 4 "311-10.0KCRCT-ND" H 11550 3750 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0KCRCT-ND\">Link</a>" H 11550 3750 60  0001 C CNN "Link"
	1    11550 3750
	0    1    1    0   
$EndComp
Wire Wire Line
	11700 3750 11800 3750
Wire Wire Line
	11800 3750 11800 3850
Wire Wire Line
	11800 3950 11700 3950
Wire Wire Line
	11900 3850 11800 3850
Connection ~ 11800 3850
Wire Wire Line
	11400 3950 11300 3950
Wire Wire Line
	11300 3950 11300 4350
Connection ~ 11300 4350
Wire Wire Line
	11400 3750 11200 3750
Wire Wire Line
	11200 3750 11200 4250
Connection ~ 11200 4250
$Comp
L FlexFox80-rescue:Vproc-ARDF-2-Band-MiniTx-rescue #PWR0183
U 1 1 5AB7DABB
P 2150 5000
F 0 "#PWR0183" H 2150 4850 50  0001 C CNN
F 1 "Vproc" H 2150 5150 50  0000 C CNN
F 2 "" H 2150 5000 50  0000 C CNN
F 3 "" H 2150 5000 50  0000 C CNN
	1    2150 5000
	1    0    0    -1  
$EndComp
Text GLabel 12600 2250 1    39   Output ~ 0
CLK1
Text GLabel 12500 2250 1    39   Output ~ 0
CLK0
Text GLabel 7950 6550 2    39   Output ~ 0
HF_ENABLE
$Comp
L FlexFox80-rescue:Battery_Single_Cell-ARDF-2-Band-MiniTx-rescue BT301
U 1 1 5BCD438E
P 2700 5500
F 0 "BT301" H 2800 5400 50  0000 L CNN
F 1 "CR1220" H 2800 5475 50  0000 L CNN
F 2 "Oddities:BC501SM_CR1220_BAT_Handsolder" V 2700 5540 50  0001 C CNN
F 3 "" V 2700 5540 50  0000 C CNN
F 4 "BC501SM-ND" H 2700 5500 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=BC501SM-ND\">Link</a>" H 2700 5500 60  0001 C CNN "Link"
	1    2700 5500
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:+3V3_L8-ARDF-2-Band-MiniTx-rescue #PWR0141
U 1 1 5BCD52AA
P 11200 2600
F 0 "#PWR0141" H 11200 2450 50  0001 C CNN
F 1 "+3V3_L8" H 11200 2740 50  0000 C CNN
F 2 "" H 11200 2600 50  0000 C CNN
F 3 "" H 11200 2600 50  0000 C CNN
	1    11200 2600
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:+3V3_L8-ARDF-2-Band-MiniTx-rescue #PWR0142
U 1 1 5BCD5593
P 9700 3000
F 0 "#PWR0142" H 9700 2850 50  0001 C CNN
F 1 "+3V3_L8" H 9700 3140 50  0000 C CNN
F 2 "" H 9700 3000 50  0000 C CNN
F 3 "" H 9700 3000 50  0000 C CNN
	1    9700 3000
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:+3V3_L8-ARDF-2-Band-MiniTx-rescue #PWR0143
U 1 1 5BCD568A
P 11900 3850
F 0 "#PWR0143" H 11900 3700 50  0001 C CNN
F 1 "+3V3_L8" H 11900 3990 50  0000 C CNN
F 2 "" H 11900 3850 50  0000 C CNN
F 3 "" H 11900 3850 50  0000 C CNN
	1    11900 3850
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:+3V3_L8-ARDF-2-Band-MiniTx-rescue #PWR0184
U 1 1 5BCD5E43
P 13100 3150
F 0 "#PWR0184" H 13100 3000 50  0001 C CNN
F 1 "+3V3_L8" H 13100 3290 50  0000 C CNN
F 2 "" H 13100 3150 50  0000 C CNN
F 3 "" H 13100 3150 50  0000 C CNN
	1    13100 3150
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0185
U 1 1 5BCD6506
P 3100 8250
F 0 "#PWR0185" H 3100 8000 50  0001 C CNN
F 1 "GNDD" V 3100 8100 50  0000 C CNN
F 2 "" H 3100 8250 50  0000 C CNN
F 3 "" H 3100 8250 50  0000 C CNN
	1    3100 8250
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C309
U 1 1 5BCD6BBE
P 2550 8350
F 0 "C309" V 2750 8250 50  0000 L CNN
F 1 "100nF" V 2650 8250 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2550 8350 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 2550 8350 50  0001 C CNN
F 4 "478-1395-1-ND" H 2550 8350 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 2550 8350 60  0001 C CNN "Link"
	1    2550 8350
	0    -1   -1   0   
$EndComp
$Comp
L special:HUZZAH_ESP8266_Breakout MOD301
U 1 1 5BCBB394
P 12750 7100
F 0 "MOD301" H 12750 7150 50  0000 C CNB
F 1 "HUZZAH_ESP8266_Breakout" H 12750 7700 50  0000 C CNB
F 2 "Oddities:Adafruit_HUZZAH_ESP8266_Breakout_SM" H 12750 7100 50  0001 C CNN
F 3 "" H 12750 7100 50  0000 C CNN
F 4 "1528-1223-ND" H 12750 7100 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1528-1223-ND\">Link</a>" H 12750 7100 60  0001 C CNN "Link"
	1    12750 7100
	-1   0    0    1   
$EndComp
NoConn ~ 13400 6850
NoConn ~ 13400 7350
NoConn ~ 12100 6750
NoConn ~ 12100 6850
$Comp
L power:GNDD #PWR0186
U 1 1 5BCBB750
P 12100 6650
F 0 "#PWR0186" H 12100 6400 50  0001 C CNN
F 1 "GNDD" H 12100 6500 50  0000 C CNN
F 2 "" H 12100 6650 50  0000 C CNN
F 3 "" H 12100 6650 50  0000 C CNN
	1    12100 6650
	0    1    1    0   
$EndComp
$Comp
L power:GNDD #PWR0147
U 1 1 5BCBB7E2
P 13400 6650
F 0 "#PWR0147" H 13400 6400 50  0001 C CNN
F 1 "GNDD" H 13400 6500 50  0000 C CNN
F 2 "" H 13400 6650 50  0000 C CNN
F 3 "" H 13400 6650 50  0000 C CNN
	1    13400 6650
	0    -1   1    0   
$EndComp
$Comp
L FlexFox80-rescue:CONN_01X06-ARDF-2-Band-MiniTx-rescue P304
U 1 1 5BCBB8A3
P 12750 5850
F 0 "P304" H 12750 6200 50  0000 C CNN
F 1 "Header" V 12850 5850 50  0001 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06" H 12750 5850 50  0001 C CNN
F 3 "" H 12750 5850 50  0001 C CNN
F 4 "np" H 12750 5850 60  0001 C CNN "Part No."
F 5 "np" H 12750 5850 60  0001 C CNN "Link"
	1    12750 5850
	0    -1   -1   0   
$EndComp
Wire Wire Line
	12500 6050 12500 6400
Wire Wire Line
	12600 6450 12600 6050
Wire Wire Line
	12800 6450 12800 6050
Wire Wire Line
	12900 6450 12900 6050
Wire Wire Line
	13000 6450 13000 6050
$Comp
L power:PWR_FLAG #FLG0148
U 1 1 5BCBC2DC
P 12700 6450
F 0 "#FLG0148" H 12700 6545 50  0001 C CNN
F 1 "PWR_FLAG" H 12700 6630 50  0000 C CNN
F 2 "" H 12700 6450 50  0000 C CNN
F 3 "" H 12700 6450 50  0000 C CNN
	1    12700 6450
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0187
U 1 1 5BCBC48D
P 12450 6400
F 0 "#PWR0187" H 12450 6150 50  0001 C CNN
F 1 "GNDD" H 12450 6250 50  0000 C CNN
F 2 "" H 12450 6400 50  0000 C CNN
F 3 "" H 12450 6400 50  0000 C CNN
	1    12450 6400
	0    1    1    0   
$EndComp
Wire Wire Line
	12450 6400 12500 6400
Connection ~ 12500 6400
$Comp
L Device:C C313
U 1 1 5BCBD2A6
P 14000 6950
F 0 "C313" H 13750 6850 50  0000 L CNN
F 1 "2.2uF" H 14025 6850 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 14038 6800 50  0001 C CNN
F 3 "" H 14000 6950 50  0001 C CNN
F 4 "1276-1763-1-ND" H 14000 6950 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-1763-1-ND\">Link</a>" H 14000 6950 60  0001 C CNN "Link"
	1    14000 6950
	-1   0    0    1   
$EndComp
$Comp
L power:GNDD #PWR0150
U 1 1 5BCBD2AC
P 14000 7100
F 0 "#PWR0150" H 14000 6850 50  0001 C CNN
F 1 "GNDD" H 14000 6950 50  0000 C CNN
F 2 "" H 14000 7100 50  0000 C CNN
F 3 "" H 14000 7100 50  0000 C CNN
	1    14000 7100
	1    0    0    -1  
$EndComp
Wire Wire Line
	14000 6800 14000 6750
Wire Wire Line
	13400 6750 14000 6750
$Comp
L FlexFox80-rescue:MIC5219-ARDF-2-Band-MiniTx-rescue U306
U 1 1 5BCBD8D5
P 14750 6800
F 0 "U306" H 14750 7100 61  0000 C CNB
F 1 "MIC5219-3.3BM5" H 14750 7000 61  0000 C CNB
F 2 "TO_SOT_Packages_SMD:SOT-23-5_Handsolder" H 14750 6900 50  0001 C CIN
F 3 "" H 14750 6800 50  0000 C CNN
F 4 "576-1281-1-ND" H 14750 6800 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=576-1281-1-ND\">Link</a>" H 14750 6800 60  0001 C CNN "Link"
	1    14750 6800
	-1   0    0    -1  
$EndComp
Connection ~ 14000 6750
$Comp
L power:GNDD #PWR0188
U 1 1 5BCBE48F
P 14750 7300
F 0 "#PWR0188" H 14750 7050 50  0001 C CNN
F 1 "GNDD" H 14750 7150 50  0000 C CNN
F 2 "" H 14750 7300 50  0000 C CNN
F 3 "" H 14750 7300 50  0000 C CNN
	1    14750 7300
	1    0    0    -1  
$EndComp
$Comp
L Device:R R310
U 1 1 5BCBE564
P 15000 7200
F 0 "R310" V 15080 7200 50  0000 C CNN
F 1 "10k" V 15000 7200 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 14930 7200 50  0001 C CNN
F 3 "" H 15000 7200 50  0000 C CNN
F 4 "311-10.0KCRCT-ND" H 15000 7200 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0KCRCT-ND\">Link</a>" H 15000 7200 60  0001 C CNN "Link"
	1    15000 7200
	0    1    1    0   
$EndComp
Wire Wire Line
	14750 7100 14750 7200
Wire Wire Line
	14850 7200 14750 7200
Connection ~ 14750 7200
Wire Wire Line
	15150 6900 15200 6900
Wire Wire Line
	15200 6900 15200 7200
Wire Wire Line
	15200 7200 15150 7200
Wire Wire Line
	15450 6650 15450 6750
Wire Wire Line
	15450 6750 15150 6750
$Comp
L power:GNDD #PWR0152
U 1 1 5BCBED7E
P 15450 7100
F 0 "#PWR0152" H 15450 6850 50  0001 C CNN
F 1 "GNDD" H 15450 6950 50  0000 C CNN
F 2 "" H 15450 7100 50  0000 C CNN
F 3 "" H 15450 7100 50  0000 C CNN
	1    15450 7100
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0153
U 1 1 5BCBF043
P 15450 6650
F 0 "#PWR0153" H 15450 6500 50  0001 C CNN
F 1 "+12V" H 15450 6790 50  0000 C CNN
F 2 "" H 15450 6650 50  0000 C CNN
F 3 "" H 15450 6650 50  0000 C CNN
	1    15450 6650
	1    0    0    -1  
$EndComp
Connection ~ 15450 6750
Text Notes 13450 6250 0    39   ~ 0
Remove pull-up resistor R7 on\nAdafruit HUZZAH ESP8266\nbreakout board if needed to\ndisable I2C pull-up
$Comp
L FlexFox80-rescue:TEST_1P-ARDF-2-Band-MiniTx-rescue W313
U 1 1 5BCBF833
P 13400 6950
F 0 "W313" V 13400 7220 50  0000 C CNN
F 1 "TEST_1P" H 13400 7150 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Square-SMD-Pad_Small" H 13600 6950 50  0001 C CNN
F 3 "" H 13600 6950 50  0000 C CNN
F 4 "np" H 13400 6950 60  0001 C CNN "Part No."
F 5 "np" H 13400 6950 60  0001 C CNN "Link"
	1    13400 6950
	0    1    -1   0   
$EndComp
$Comp
L FlexFox80-rescue:TEST_1P-ARDF-2-Band-MiniTx-rescue W314
U 1 1 5BCBFAA2
P 13400 7050
F 0 "W314" V 13400 7320 50  0000 C CNN
F 1 "TEST_1P" H 13400 7250 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Square-SMD-Pad_Small" H 13600 7050 50  0001 C CNN
F 3 "" H 13600 7050 50  0000 C CNN
F 4 "np" H 13400 7050 60  0001 C CNN "Part No."
F 5 "np" H 13400 7050 60  0001 C CNN "Link"
	1    13400 7050
	0    1    -1   0   
$EndComp
$Comp
L FlexFox80-rescue:TEST_1P-ARDF-2-Band-MiniTx-rescue W315
U 1 1 5BCBFB6C
P 13400 7250
F 0 "W315" V 13400 7520 50  0000 C CNN
F 1 "TEST_1P" H 13400 7450 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Square-SMD-Pad_Small" H 13600 7250 50  0001 C CNN
F 3 "" H 13600 7250 50  0000 C CNN
F 4 "np" H 13400 7250 60  0001 C CNN "Part No."
F 5 "np" H 13400 7250 60  0001 C CNN "Link"
	1    13400 7250
	0    1    -1   0   
$EndComp
Text GLabel 13800 7750 2    39   Input ~ 0
~WIFI_RESET~
Wire Wire Line
	13500 7550 13400 7550
$Comp
L FlexFox80-rescue:TEST_1P-ARDF-2-Band-MiniTx-rescue W316
U 1 1 5BCBFFE7
P 13400 7450
F 0 "W316" V 13400 7720 50  0000 C CNN
F 1 "TEST_1P" H 13400 7650 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Square-SMD-Pad_Small" H 13600 7450 50  0001 C CNN
F 3 "" H 13600 7450 50  0000 C CNN
F 4 "np" H 13400 7450 60  0001 C CNN "Part No."
F 5 "np" H 13400 7450 60  0001 C CNN "Link"
	1    13400 7450
	0    1    -1   0   
$EndComp
$Comp
L FlexFox80-rescue:TEST_1P-ARDF-2-Band-MiniTx-rescue W309
U 1 1 5BCC10A9
P 12100 6950
F 0 "W309" V 12100 7220 50  0000 C CNN
F 1 "TEST_1P" H 12100 7150 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Square-SMD-Pad_Small" H 12300 6950 50  0001 C CNN
F 3 "" H 12300 6950 50  0000 C CNN
F 4 "np" H 12100 6950 60  0001 C CNN "Part No."
F 5 "np" H 12100 6950 60  0001 C CNN "Link"
	1    12100 6950
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:TEST_1P-ARDF-2-Band-MiniTx-rescue W310
U 1 1 5BCC11A2
P 12100 7150
F 0 "W310" V 12100 7420 50  0000 C CNN
F 1 "TEST_1P" H 12100 7350 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Square-SMD-Pad_Small" H 12300 7150 50  0001 C CNN
F 3 "" H 12300 7150 50  0000 C CNN
F 4 "np" H 12100 7150 60  0001 C CNN "Part No."
F 5 "np" H 12100 7150 60  0001 C CNN "Link"
	1    12100 7150
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:TEST_1P-ARDF-2-Band-MiniTx-rescue W311
U 1 1 5BCC1265
P 12100 7250
F 0 "W311" V 12100 7520 50  0000 C CNN
F 1 "TEST_1P" H 12100 7450 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Square-SMD-Pad_Small" H 12300 7250 50  0001 C CNN
F 3 "" H 12300 7250 50  0000 C CNN
F 4 "np" H 12100 7250 60  0001 C CNN "Part No."
F 5 "np" H 12100 7250 60  0001 C CNN "Link"
	1    12100 7250
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:TEST_1P-ARDF-2-Band-MiniTx-rescue W312
U 1 1 5BCC1327
P 12100 7350
F 0 "W312" V 12100 7620 50  0000 C CNN
F 1 "TEST_1P" H 12100 7550 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Square-SMD-Pad_Small" H 12300 7350 50  0001 C CNN
F 3 "" H 12300 7350 50  0000 C CNN
F 4 "np" H 12100 7350 60  0001 C CNN "Part No."
F 5 "np" H 12100 7350 60  0001 C CNN "Link"
	1    12100 7350
	0    -1   -1   0   
$EndComp
Text GLabel 12000 7450 0    39   Input ~ 0
DI_TXD0
Wire Wire Line
	12000 7450 12100 7450
$Comp
L FlexFox80-rescue:74HC1G08-ARDF-2-Band-MiniTx-rescue U305
U 1 1 5BCC1DC3
P 11050 7650
F 0 "U305" H 11050 7700 61  0000 C CNB
F 1 "74HC1G08" H 11050 7600 61  0000 C CNB
F 2 "TO_SOT_Packages_SMD:SOT-23-5_Handsolder" H 11050 7650 50  0001 C CNN
F 3 "" H 11050 7650 50  0001 C CNN
F 4 "MC74HC1G08DTT1GOSCT-ND" H 11050 7650 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=MC74HC1G08DTT1GOSCT-ND\">Link</a>" H 11050 7650 60  0001 C CNN "Link"
	1    11050 7650
	-1   0    0    -1  
$EndComp
Wire Wire Line
	12100 7550 11650 7550
$Comp
L power:GNDD #PWR0189
U 1 1 5BCC1FE2
P 11250 7950
F 0 "#PWR0189" H 11250 7700 50  0001 C CNN
F 1 "GNDD" H 11250 7800 50  0000 C CNN
F 2 "" H 11250 7950 50  0000 C CNN
F 3 "" H 11250 7950 50  0000 C CNN
	1    11250 7950
	1    0    0    -1  
$EndComp
Wire Wire Line
	11250 7950 11250 7850
$Comp
L FlexFox80-rescue:+3V3_L8-ARDF-2-Band-MiniTx-rescue #PWR0155
U 1 1 5BCC23A8
P 11250 7200
F 0 "#PWR0155" H 11250 7050 50  0001 C CNN
F 1 "+3V3_L8" H 11250 7340 50  0000 C CNN
F 2 "" H 11250 7200 50  0000 C CNN
F 3 "" H 11250 7200 50  0000 C CNN
	1    11250 7200
	1    0    0    -1  
$EndComp
Wire Wire Line
	11250 7200 11250 7300
$Comp
L Device:C C311
U 1 1 5BCC29B8
P 11000 7300
F 0 "C311" H 11050 7400 50  0000 L CNN
F 1 "100nF" H 11025 7200 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 11038 7150 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 11000 7300 50  0001 C CNN
F 4 "478-1395-1-ND" H 11000 7300 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 11000 7300 60  0001 C CNN "Link"
	1    11000 7300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	11150 7300 11250 7300
Connection ~ 11250 7300
$Comp
L power:GNDD #PWR0190
U 1 1 5BCC2F26
P 10850 7300
F 0 "#PWR0190" H 10850 7050 50  0001 C CNN
F 1 "GNDD" H 10850 7150 50  0000 C CNN
F 2 "" H 10850 7300 50  0000 C CNN
F 3 "" H 10850 7300 50  0000 C CNN
	1    10850 7300
	0    1    1    0   
$EndComp
Text Notes 11100 8200 2    39   ~ 0
Resetting the WiFi module must also disable\nWiFi UART communication with the processor.\nThis allows the processor to block powerup\ngarbage sent by the WiFi board.
Text GLabel 7950 5950 2    39   Output ~ 0
~WIFI_RESET~
Wire Wire Line
	11650 7750 13500 7750
Text GLabel 7950 5850 2    39   Output ~ 0
WIFI_ENABLE
Text GLabel 15150 7600 0    39   Input ~ 0
WIFI_ENABLE
Wire Wire Line
	15200 7600 15150 7600
Connection ~ 15200 7200
$Comp
L FlexFox80-rescue:MBD54DWT1G_Schottky_Ind-ARDF-2-Band-MiniTx-rescue D303
U 1 1 5BF19980
P 2100 8150
F 0 "D303" H 2100 8050 50  0000 C CNN
F 1 "MBD54DWT1G_Schottky_Ind" H 2125 8425 50  0001 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-363_Handsolder" H 2100 8150 50  0001 C CNN
F 3 "https://www.onsemi.com/pub/Collateral/MBD54DWT1-D.PDF" H 2100 8150 50  0001 C CNN
F 4 "MBD54DWT1GOSCT-ND" H 2100 8150 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=MBD54DWT1GOSCT-ND\">Link</a>" H 2100 8150 60  0001 C CNN "Link"
	1    2100 8150
	-1   0    0    1   
$EndComp
Text Notes 12550 5650 0    39   ~ 0
Programming
Text Notes 12450 5750 0    39   ~ 0
FTDI Cable Connector
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0157
U 1 1 5C1313AA
P 11100 10300
F 0 "#PWR0157" H 11100 10050 50  0001 C CNN
F 1 "GND_L8" H 11100 10150 50  0000 C CNN
F 2 "" H 11100 10300 50  0000 C CNN
F 3 "" H 11100 10300 50  0000 C CNN
	1    11100 10300
	1    0    0    -1  
$EndComp
Wire Wire Line
	11100 10300 11100 10200
Connection ~ 11350 10200
$Comp
L FlexFox80-rescue:Vproc-ARDF-2-Band-MiniTx-rescue #PWR0158
U 1 1 5C60DF91
P 4450 5450
F 0 "#PWR0158" H 4450 5300 50  0001 C CNN
F 1 "Vproc" H 4450 5600 50  0000 C CNN
F 2 "" H 4450 5450 50  0000 C CNN
F 3 "" H 4450 5450 50  0000 C CNN
	1    4450 5450
	1    0    0    -1  
$EndComp
Text GLabel 7950 3850 2    39   Input ~ 0
TX_+12_VOLTAGE
Wire Wire Line
	10350 4250 10450 4250
Wire Wire Line
	10350 4350 10550 4350
$Comp
L power:GNDD #PWR0159
U 1 1 5C75CA25
P 10050 4550
F 0 "#PWR0159" H 10050 4300 50  0001 C CNN
F 1 "GNDD" H 10050 4400 50  0001 C CNN
F 2 "" H 10050 4550 50  0000 C CNN
F 3 "" H 10050 4550 50  0000 C CNN
	1    10050 4550
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:+3V3_L8-ARDF-2-Band-MiniTx-rescue #PWR0191
U 1 1 5C75CAFF
P 10050 3700
F 0 "#PWR0191" H 10050 3550 50  0001 C CNN
F 1 "+3V3_L8" H 10050 3840 50  0000 C CNN
F 2 "" H 10050 3700 50  0000 C CNN
F 3 "" H 10050 3700 50  0000 C CNN
	1    10050 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	10050 3700 10050 3750
Wire Wire Line
	9750 4150 9750 3900
Wire Wire Line
	9750 3900 10050 3900
Connection ~ 10050 3900
$Comp
L FlexFox80-rescue:SN74CB3Q3305-ARDF-2-Band-MiniTx-rescue U307
U 1 1 5C75D7C5
P 10050 4250
F 0 "U307" H 10200 4500 61  0000 C CNB
F 1 "SN74CB3Q3305" H 10450 4000 61  0000 C CNB
F 2 "Housings_SSOP:TSSOP-8_3x3mm_Pitch0.65mm_Handsolder" H 10050 3850 50  0001 C CNN
F 3 "" H 10050 4300 50  0001 C CNN
F 4 "296-16809-1-ND" H 10050 4250 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=296-16809-1-ND\">Link</a>" H 10050 4250 60  0001 C CNN "Link"
	1    10050 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	10350 3900 10350 4150
Text GLabel 9500 4550 3    39   Output ~ 0
RTC_SCL
Text GLabel 9600 4550 3    39   Output ~ 0
RTC_SDA
$Comp
L Device:C_Small C316
U 1 1 5C7604ED
P 9850 3750
F 0 "C316" V 9750 3650 50  0000 L CNN
F 1 "100nF" V 9950 3650 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 9850 3750 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 9850 3750 50  0001 C CNN
F 4 "478-1395-1-ND" H 9850 3750 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 9850 3750 60  0001 C CNN "Link"
	1    9850 3750
	0    1    1    0   
$EndComp
Wire Wire Line
	9950 3750 10050 3750
Connection ~ 10050 3750
$Comp
L power:GNDD #PWR0161
U 1 1 5C7609F5
P 9750 3750
F 0 "#PWR0161" H 9750 3500 50  0001 C CNN
F 1 "GNDD" H 9750 3600 50  0001 C CNN
F 2 "" H 9750 3750 50  0000 C CNN
F 3 "" H 9750 3750 50  0000 C CNN
	1    9750 3750
	0    1    1    0   
$EndComp
Text GLabel 1750 5600 0    39   Input ~ 0
RTC_SCL
Text GLabel 1750 5700 0    39   Input ~ 0
RTC_SDA
Connection ~ 5150 4350
Text GLabel 7950 6650 2    39   Output ~ 0
3V3_PWR_ENABLE
Connection ~ 5150 4550
$Comp
L Device:C_Small C302
U 1 1 5C78AA39
P 4800 4850
F 0 "C302" V 4700 4650 50  0000 L CNN
F 1 "100nF" V 4900 4900 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4800 4850 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 4800 4850 50  0001 C CNN
F 4 "478-1395-1-ND" H 4800 4850 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 4800 4850 60  0001 C CNN "Link"
	1    4800 4850
	0    1    1    0   
$EndComp
Wire Wire Line
	5150 4850 4900 4850
$Comp
L power:GNDD #PWR0162
U 1 1 5C78AA40
P 4600 4850
F 0 "#PWR0162" H 4600 4600 50  0001 C CNN
F 1 "GNDD" H 4600 4700 50  0000 C CNN
F 2 "" H 4600 4850 50  0000 C CNN
F 3 "" H 4600 4850 50  0000 C CNN
	1    4600 4850
	0    1    1    0   
$EndComp
Wire Wire Line
	4600 4850 4700 4850
$Comp
L power:PWR_FLAG #FLG0163
U 1 1 5C78D99A
P 5450 4850
F 0 "#FLG0163" H 5450 4945 50  0001 C CNN
F 1 "PWR_FLAG" H 5450 5030 50  0000 C CNN
F 2 "" H 5450 4850 50  0000 C CNN
F 3 "" H 5450 4850 50  0000 C CNN
	1    5450 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	13500 7550 13500 7750
Connection ~ 13500 7750
$Comp
L Device:C C315
U 1 1 5C9ED27C
P 15450 6950
F 0 "C315" H 15200 6850 50  0000 L CNN
F 1 "2.2uF" H 15475 6850 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 15488 6800 50  0001 C CNN
F 3 "" H 15450 6950 50  0001 C CNN
F 4 "1276-1763-1-ND" H 15450 6950 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-1763-1-ND\">Link</a>" H 15450 6950 60  0001 C CNN "Link"
	1    15450 6950
	-1   0    0    1   
$EndComp
Text GLabel 8350 3700 2    39   Input ~ 0
80M_ANTENNA_DETECT
Wire Wire Line
	8350 3700 8250 3700
Wire Wire Line
	8250 3650 8250 3700
Connection ~ 8250 3700
Text GLabel 12500 3350 2    39   Output ~ 0
CLK2
Wire Wire Line
	4450 3500 4450 3850
Wire Wire Line
	4450 3850 4750 3850
Text Notes 12600 5400 0    79   ~ 16
WiFi Module
NoConn ~ 13400 7150
NoConn ~ 12100 7050
NoConn ~ 12700 6050
Wire Wire Line
	2100 5100 2150 5100
$Comp
L Device:C C314
U 1 1 5F83F111
P 2450 3650
F 0 "C314" H 2200 3750 50  0000 L CNN
F 1 "2.2uF" H 2550 3700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2488 3500 50  0001 C CNN
F 3 "" H 2450 3650 50  0001 C CNN
F 4 "1276-1763-1-ND" H 2450 3650 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-1763-1-ND\">Link</a>" H 2450 3650 60  0001 C CNN "Link"
	1    2450 3650
	-1   0    0    1   
$EndComp
$Comp
L power:GNDD #PWR0167
U 1 1 5F83F117
P 2450 3800
F 0 "#PWR0167" H 2450 3550 50  0001 C CNN
F 1 "GNDD" H 2450 3650 50  0000 C CNN
F 2 "" H 2450 3800 50  0000 C CNN
F 3 "" H 2450 3800 50  0000 C CNN
	1    2450 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2250 3500 2450 3500
$Comp
L FlexFox80-rescue:MIC5219-ARDF-2-Band-MiniTx-rescue U308
U 1 1 5F83F121
P 3200 3550
F 0 "U308" H 3200 3850 61  0000 C CNB
F 1 "MIC5219-3.3BM5" H 3200 3750 61  0000 C CNB
F 2 "TO_SOT_Packages_SMD:SOT-23-5_Handsolder" H 3200 3650 50  0001 C CIN
F 3 "" H 3200 3550 50  0000 C CNN
F 4 "576-1281-1-ND" H 3200 3550 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=576-1281-1-ND\">Link</a>" H 3200 3550 60  0001 C CNN "Link"
	1    3200 3550
	1    0    0    -1  
$EndComp
Connection ~ 2450 3500
Wire Wire Line
	2750 3650 2800 3650
$Comp
L power:GNDD #PWR0168
U 1 1 5F83F139
P 3200 3850
F 0 "#PWR0168" H 3200 3600 50  0001 C CNN
F 1 "GNDD" H 3200 3700 50  0000 C CNN
F 2 "" H 3200 3850 50  0000 C CNN
F 3 "" H 3200 3850 50  0000 C CNN
	1    3200 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 3500 3900 3500
$Comp
L power:GNDD #PWR0169
U 1 1 5F83F14F
P 3900 3800
F 0 "#PWR0169" H 3900 3550 50  0001 C CNN
F 1 "GNDD" H 3900 3650 50  0000 C CNN
F 2 "" H 3900 3800 50  0000 C CNN
F 3 "" H 3900 3800 50  0000 C CNN
	1    3900 3800
	1    0    0    -1  
$EndComp
Connection ~ 3900 3500
$Comp
L Device:C C317
U 1 1 5F83F15F
P 3900 3650
F 0 "C317" H 3650 3750 50  0000 L CNN
F 1 "2.2uF" H 3950 3750 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 3938 3500 50  0001 C CNN
F 3 "" H 3900 3650 50  0001 C CNN
F 4 "1276-1763-1-ND" H 3900 3650 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-1763-1-ND\">Link</a>" H 3900 3650 60  0001 C CNN "Link"
	1    3900 3650
	-1   0    0    1   
$EndComp
Wire Wire Line
	2750 3650 2750 3500
Connection ~ 2750 3500
Text GLabel 1850 3500 0    39   Input ~ 0
EXTERNAL_3.2V-12V_SUPPLY
NoConn ~ 3600 3650
NoConn ~ 14350 6900
$Comp
L FlexFox80-rescue:ZENER-ARDF-2-Band-MiniTx-rescue D302
U 1 1 5F84898E
P 2050 3500
F 0 "D302" H 2050 3600 50  0000 C CNN
F 1 "1.7V" H 2050 3400 50  0000 C CNN
F 2 "Diodes_SMD:SOD-123_Handsolder" H 2050 3500 50  0001 C CNN
F 3 "" H 2050 3500 50  0000 C CNN
F 4 "BZT52C4V7-13FDICT-ND" H 2050 3500 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=BZT52C4V7-13FDICT-ND\">Link</a>" H 2050 3500 60  0001 C CNN "Link"
	1    2050 3500
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0170
U 1 1 5F84CE0D
P 2450 3500
F 0 "#FLG0170" H 2450 3595 50  0001 C CNN
F 1 "PWR_FLAG" H 2450 3680 50  0000 C CNN
F 2 "" H 2450 3500 50  0000 C CNN
F 3 "" H 2450 3500 50  0000 C CNN
	1    2450 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5350 6800 5350 6900
Wire Wire Line
	11200 2650 11200 2950
Wire Wire Line
	2300 8350 2250 8350
Wire Wire Line
	2300 8350 2300 8150
Wire Wire Line
	5150 4250 5150 4350
Wire Wire Line
	9700 3050 9700 3100
Wire Wire Line
	9700 3350 9700 3400
Wire Wire Line
	1850 8350 1850 8150
Wire Wire Line
	2000 6150 2050 6150
Wire Wire Line
	2050 6150 2100 6150
Wire Wire Line
	2100 6150 2150 6150
Wire Wire Line
	2200 6150 2250 6150
Wire Wire Line
	2250 6150 2300 6150
Wire Wire Line
	2300 6150 2350 6150
Wire Wire Line
	2150 6150 2150 6200
Wire Wire Line
	2150 6150 2200 6150
Wire Wire Line
	2150 5100 2150 5200
Wire Wire Line
	11000 4250 11200 4250
Wire Wire Line
	11100 4350 11300 4350
Wire Wire Line
	11550 10200 11800 10200
Wire Wire Line
	11550 10200 11550 10300
Wire Wire Line
	11800 10200 12050 10200
Wire Wire Line
	13100 3250 13100 3300
Wire Wire Line
	11550 10050 11550 10200
Wire Wire Line
	10450 4250 11000 4250
Wire Wire Line
	10550 4350 11100 4350
Wire Wire Line
	11800 3850 11800 3950
Wire Wire Line
	11300 4350 11450 4350
Wire Wire Line
	11200 4250 11450 4250
Wire Wire Line
	2450 8350 2300 8350
Wire Wire Line
	12500 6400 12500 6450
Wire Wire Line
	14000 6750 14350 6750
Wire Wire Line
	14750 7200 14750 7300
Wire Wire Line
	15450 6750 15450 6800
Wire Wire Line
	11250 7300 11250 7450
Wire Wire Line
	15200 7200 15200 7600
Wire Wire Line
	11350 10200 11550 10200
Wire Wire Line
	10050 3900 10050 3950
Wire Wire Line
	10050 3900 10350 3900
Wire Wire Line
	9500 4250 9750 4250
Wire Wire Line
	9600 4350 9750 4350
Wire Wire Line
	10050 3750 10050 3900
Wire Wire Line
	5150 4350 5150 4450
Wire Wire Line
	5150 4550 5150 4650
Wire Wire Line
	13500 7750 13800 7750
Wire Wire Line
	8250 3700 8250 3750
Wire Wire Line
	2450 3500 2750 3500
Wire Wire Line
	3900 3500 4450 3500
Wire Wire Line
	2750 3500 2800 3500
$Comp
L MCU_Microchip_ATmega:ATmega1284P-AU U302
U 1 1 6081BBDB
P 6550 5150
F 0 "U302" H 6550 7400 60  0000 C CNB
F 1 "ATmega1284P-AU" H 6600 7250 60  0000 C CNB
F 2 "Package_QFP:TQFP-44_10x10mm_P0.8mm" H 6550 5150 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8272-8-bit-AVR-microcontroller-ATmega164A_PA-324A_PA-644A_PA-1284_P_datasheet.pdf" H 6550 5150 50  0001 C CNN
	1    6550 5150
	1    0    0    -1  
$EndComp
Connection ~ 4750 3850
Wire Wire Line
	4750 3850 5150 3850
Connection ~ 5150 4450
Wire Wire Line
	5150 4450 5150 4550
Entry Wire Line
	7600 3450 7700 3550
Entry Wire Line
	7600 3550 7700 3650
Entry Wire Line
	7600 3650 7700 3750
Entry Wire Line
	7600 3750 7700 3850
Entry Wire Line
	7600 3850 7700 3950
Entry Wire Line
	7600 3950 7700 4050
Entry Wire Line
	7600 4050 7700 4150
Entry Wire Line
	7600 4150 7700 4250
Entry Wire Line
	7700 3550 7800 3450
Entry Wire Line
	7700 3650 7800 3550
Entry Wire Line
	7700 3750 7800 3650
Entry Wire Line
	7700 3850 7800 3750
Entry Wire Line
	7700 3950 7800 3850
Entry Wire Line
	7700 4050 7800 3950
Entry Wire Line
	7700 4150 7800 4050
Wire Wire Line
	7600 3450 7500 3450
Wire Wire Line
	7500 3550 7600 3550
Wire Wire Line
	7500 3650 7600 3650
Wire Wire Line
	7500 3750 7600 3750
Wire Wire Line
	7500 3850 7600 3850
Wire Wire Line
	7500 3950 7600 3950
Wire Wire Line
	7500 4050 7600 4050
Wire Wire Line
	7500 4150 7600 4150
Wire Wire Line
	5600 6600 5350 6600
Wire Wire Line
	5350 6600 5350 6700
Connection ~ 5350 6700
Wire Wire Line
	5150 3450 5150 3850
Connection ~ 5150 3850
Wire Wire Line
	5150 3850 5150 4150
Connection ~ 5150 4150
Wire Wire Line
	5150 4150 5150 4250
Connection ~ 5150 4650
Wire Wire Line
	5150 4650 5150 4850
Wire Wire Line
	4950 4450 5150 4450
Wire Wire Line
	5000 4150 5150 4150
Wire Wire Line
	5100 6500 5600 6500
Wire Wire Line
	5150 4350 5600 4350
Wire Wire Line
	5150 4550 5600 4550
Wire Wire Line
	5150 4250 5600 4250
Wire Wire Line
	5150 4450 5600 4450
Wire Wire Line
	5450 4900 5450 4850
Wire Wire Line
	7800 4550 7950 4550
Wire Wire Line
	7800 6450 7950 6450
Wire Wire Line
	7800 4150 7950 4150
Wire Wire Line
	7800 6650 7950 6650
Wire Wire Line
	7800 6750 7950 6750
Wire Wire Line
	7800 6850 7950 6850
Wire Wire Line
	5600 4850 5450 4850
Connection ~ 5450 4850
Text GLabel 5550 5550 0    39   Input ~ 0
~RESET~
Wire Wire Line
	5550 5550 5600 5550
Entry Wire Line
	7600 5950 7700 6050
Wire Wire Line
	7600 5950 7500 5950
Entry Wire Line
	7700 4950 7800 4850
Wire Wire Line
	7950 4850 7800 4850
Entry Wire Line
	7700 5050 7800 4950
Entry Wire Line
	7700 5150 7800 5050
Wire Wire Line
	7800 4950 7950 4950
Text Label 7800 4850 0    39   ~ 0
MOSI
Text Label 7800 4950 0    39   ~ 0
MISO
Wire Wire Line
	7800 5050 7950 5050
Text Label 7800 5050 0    39   ~ 0
SCK
Wire Wire Line
	7950 3950 7800 3950
Wire Wire Line
	7950 4050 7800 4050
Text GLabel 7950 4150 2    39   Input ~ 0
PROC_HT_AUDIO_IN
Wire Wire Line
	7950 3850 7800 3850
Text Label 7500 3950 0    39   ~ 0
ADC5
Entry Wire Line
	7700 6050 7800 5950
NoConn ~ 7800 4750
Text Label 7500 3850 0    39   ~ 0
ADC4
Text Label 7800 4150 0    39   ~ 0
ADC7
Text Label 7800 4050 0    39   ~ 0
ADC6
Text Label 7800 3950 0    39   ~ 0
ADC5
Text Label 7800 3850 0    39   ~ 0
ADC4
Text Label 7800 6150 0    39   ~ 0
RXD0
Text Label 7800 6250 0    39   ~ 0
TXD0
Text Label 7800 6350 0    39   ~ 0
RXD1
Text Label 7800 6450 0    39   ~ 0
TXD1
Wire Wire Line
	7800 6150 8400 6150
Wire Wire Line
	9250 6350 9250 7650
Wire Wire Line
	7800 6350 9250 6350
Text GLabel 7950 6450 2    39   Output ~ 0
DI_TXD0
Wire Wire Line
	9250 7650 10450 7650
Text Label 7500 5850 0    39   ~ 0
PC6
Text Label 7500 5950 0    39   ~ 0
PC7
Wire Wire Line
	7800 5750 7950 5750
Wire Wire Line
	7800 5850 7950 5850
Wire Wire Line
	7800 5950 7950 5950
Wire Wire Line
	7800 5250 8900 5250
Wire Wire Line
	8900 5250 8900 4250
Wire Wire Line
	7800 5350 9000 5350
Wire Wire Line
	9000 5350 9000 4350
Wire Wire Line
	9000 4350 9250 4350
Wire Wire Line
	8900 4250 9150 4250
Wire Wire Line
	9500 4550 9500 4250
Connection ~ 9500 4250
Wire Wire Line
	9600 4550 9600 4350
Connection ~ 9600 4350
Text Label 7800 5950 0    39   ~ 0
PC7
Text Label 7800 4350 0    39   ~ 0
PB0
Text Label 7800 4450 0    39   ~ 0
PB1
Text Label 7800 4650 0    39   ~ 0
PB3
Text Label 7800 4750 0    39   ~ 0
PB4
Text Label 7500 3450 0    39   ~ 0
PA0
Text Label 7500 3550 0    39   ~ 0
PA1
Text Label 7500 3650 0    39   ~ 0
PA2
Text GLabel 7950 4350 2    39   Output ~ 0
PROC_PTT_OUT
Wire Wire Line
	7800 4350 7950 4350
Text GLabel 7950 4650 2    39   Output ~ 0
PROC_TONE_OUT
Wire Wire Line
	7800 4650 7950 4650
$Comp
L Device:Resonator_Small Y301
U 1 1 617DD630
P 5400 5850
F 0 "Y301" V 5200 5750 50  0000 C CNN
F 1 "20 MHz" V 5600 5950 50  0000 C CNN
F 2 "" H 5375 5850 50  0001 C CNN
F 3 "~" H 5375 5850 50  0001 C CNN
F 4 "CSTNE20M0V53Z000R0" H 5400 5850 50  0001 C CNN "Part No."
	1    5400 5850
	0    1    1    0   
$EndComp
Wire Wire Line
	5500 5750 5600 5750
$Comp
L power:GNDD #PWR0301
U 1 1 61834E63
P 5200 5850
F 0 "#PWR0301" H 5200 5600 50  0001 C CNN
F 1 "GNDD" H 5200 5700 50  0000 C CNN
F 2 "" H 5200 5850 50  0000 C CNN
F 3 "" H 5200 5850 50  0000 C CNN
	1    5200 5850
	0    1    1    0   
$EndComp
Wire Wire Line
	5500 5950 5600 5950
$Comp
L Connector:DB9_Female J301
U 1 1 619A55E4
P 3400 8550
F 0 "J301" H 3580 8596 50  0000 L CNN
F 1 "DB9_Female" H 3580 8505 50  0000 L CNN
F 2 "" H 3400 8550 50  0001 C CNN
F 3 " ~" H 3400 8550 50  0001 C CNN
	1    3400 8550
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 8350 2650 8350
Wire Wire Line
	1850 8000 1850 8150
Connection ~ 1850 8150
Text GLabel 8400 6150 2    39   Input ~ 0
RXD_TO_FTDI
Text GLabel 8400 6250 2    39   Output ~ 0
TXD_TO_FTDI
Wire Wire Line
	8250 6250 8400 6250
Text GLabel 3100 8650 0    39   Output ~ 0
RXD_TO_FTDI
Text GLabel 3100 8850 0    39   Input ~ 0
TXD_TO_FTDI
NoConn ~ 7800 4450
$Comp
L Device:LED_ALT D304
U 1 1 61B9E05F
P 10450 5550
F 0 "D304" H 10450 5450 50  0000 C CNN
F 1 "GREEN" H 10500 5650 50  0000 C CNN
F 2 "" H 10450 5550 50  0001 C CNN
F 3 "~" H 10450 5550 50  0001 C CNN
	1    10450 5550
	-1   0    0    1   
$EndComp
$Comp
L Device:LED_ALT D305
U 1 1 61BA0B72
P 10450 5850
F 0 "D305" H 10450 5950 50  0000 C CNN
F 1 "RED" H 10500 5750 50  0000 C CNN
F 2 "" H 10450 5850 50  0001 C CNN
F 3 "~" H 10450 5850 50  0001 C CNN
	1    10450 5850
	-1   0    0    1   
$EndComp
$Comp
L Switch:SPST_small SW301
U 1 1 61BA254A
P 10250 5300
F 0 "SW301" H 10250 5540 50  0000 C CNN
F 1 "Sync" H 10250 5449 50  0000 C CNN
F 2 "" H 10250 5300 50  0000 C CNN
F 3 "" H 10250 5300 50  0000 C CNN
	1    10250 5300
	1    0    0    -1  
$EndComp
NoConn ~ 3100 8750
$Comp
L Device:R R301
U 1 1 61C7B255
P 10150 5550
F 0 "R301" V 10230 5550 50  0000 C CNN
F 1 "470" V 10150 5550 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 10080 5550 50  0001 C CNN
F 3 "http://www.yageo.com/documents/recent/PYu-RC_Group_51_RoHS_L_04.pdf" H 10150 5550 50  0001 C CNN
F 4 "311-10.0KCRCT-ND" H 10150 5550 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0KCRCT-ND\">Link</a>" H 10150 5550 60  0001 C CNN "Link"
	1    10150 5550
	0    1    1    0   
$EndComp
$Comp
L Device:R R302
U 1 1 61C8FCFC
P 10150 5850
F 0 "R302" V 10230 5850 50  0000 C CNN
F 1 "470" V 10150 5850 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 10080 5850 50  0001 C CNN
F 3 "http://www.yageo.com/documents/recent/PYu-RC_Group_51_RoHS_L_04.pdf" H 10150 5850 50  0001 C CNN
F 4 "311-10.0KCRCT-ND" H 10150 5850 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0KCRCT-ND\">Link</a>" H 10150 5850 60  0001 C CNN "Link"
	1    10150 5850
	0    1    1    0   
$EndComp
$Comp
L power:GNDD #PWR?
U 1 1 61CAFDE2
P 10500 5300
F 0 "#PWR?" H 10500 5050 50  0001 C CNN
F 1 "GNDD" H 10500 5150 50  0000 C CNN
F 2 "" H 10500 5300 50  0000 C CNN
F 3 "" H 10500 5300 50  0000 C CNN
	1    10500 5300
	0    -1   -1   0   
$EndComp
$Comp
L power:GNDD #PWR?
U 1 1 61CB2436
P 10600 5550
F 0 "#PWR?" H 10600 5300 50  0001 C CNN
F 1 "GNDD" H 10600 5400 50  0000 C CNN
F 2 "" H 10600 5550 50  0000 C CNN
F 3 "" H 10600 5550 50  0000 C CNN
	1    10600 5550
	0    -1   -1   0   
$EndComp
$Comp
L power:GNDD #PWR?
U 1 1 61CB3557
P 10600 5850
F 0 "#PWR?" H 10600 5600 50  0001 C CNN
F 1 "GNDD" H 10600 5700 50  0000 C CNN
F 2 "" H 10600 5850 50  0000 C CNN
F 3 "" H 10600 5850 50  0000 C CNN
	1    10600 5850
	0    -1   -1   0   
$EndComp
Wire Wire Line
	9700 5300 10000 5300
Wire Wire Line
	7800 6550 7950 6550
Wire Wire Line
	7800 5450 9700 5450
Wire Wire Line
	9700 5450 9700 5300
Wire Wire Line
	7800 5550 10000 5550
Wire Wire Line
	9700 5650 9700 5850
Wire Wire Line
	7800 5650 9700 5650
Wire Wire Line
	9700 5850 10000 5850
Wire Wire Line
	7800 3650 8250 3650
Wire Wire Line
	7800 3750 8250 3750
Text Label 7800 3750 0    39   ~ 0
ADC3
Text Label 7500 3750 0    39   ~ 0
ADC3
Text Label 7800 3650 0    39   ~ 0
PA2
NoConn ~ 7800 3450
NoConn ~ 7800 3550
$Comp
L power:GNDD #PWR?
U 1 1 60878579
P 9450 4000
F 0 "#PWR?" H 9450 3750 50  0001 C CNN
F 1 "GNDD" H 9600 3850 50  0001 C CNN
F 2 "" H 9450 4000 50  0000 C CNN
F 3 "" H 9450 4000 50  0000 C CNN
	1    9450 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 4000 9150 4250
Wire Wire Line
	9250 4000 9250 4350
Connection ~ 9150 4250
Wire Wire Line
	9150 4250 9500 4250
Connection ~ 9250 4350
Wire Wire Line
	9250 4350 9600 4350
$Comp
L FlexFox80-rescue:Vproc-ARDF-2-Band-MiniTx-rescue #PWR?
U 1 1 608A7865
P 9350 4000
F 0 "#PWR?" H 9350 3850 50  0001 C CNN
F 1 "Vproc" H 9350 4150 50  0000 C CNN
F 2 "" H 9350 4000 50  0000 C CNN
F 3 "" H 9350 4000 50  0000 C CNN
	1    9350 4000
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 TP302
U 1 1 608ADA8E
P 9350 3800
F 0 "TP302" V 9550 3650 50  0000 L CNN
F 1 "I2C0" V 9450 3650 50  0000 L CNN
F 2 "" H 9350 3800 50  0001 C CNN
F 3 "~" H 9350 3800 50  0001 C CNN
	1    9350 3800
	0    1    -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x03 TP301
U 1 1 608CC2B9
P 10550 3800
F 0 "TP301" V 10750 3700 50  0000 L CNN
F 1 "I2C1" V 10650 3700 50  0000 L CNN
F 2 "" H 10550 3800 50  0001 C CNN
F 3 "~" H 10550 3800 50  0001 C CNN
	1    10550 3800
	0    1    -1   0   
$EndComp
Text GLabel 2350 9450 0    39   Output ~ 0
PROC_HT_AUDIO_IN
$Comp
L Device:C_Small C?
U 1 1 60859858
P 2800 9450
F 0 "C?" V 2700 9450 50  0000 L CNN
F 1 "100nF" V 2900 9200 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2800 9450 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 2800 9450 50  0001 C CNN
F 4 "478-1395-1-ND" H 2800 9450 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 2800 9450 60  0001 C CNN "Link"
	1    2800 9450
	0    -1   1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 6085EDC6
P 2550 9700
F 0 "R?" V 2630 9700 50  0000 C CNN
F 1 "100k" V 2550 9700 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 2480 9700 50  0001 C CNN
F 3 "http://www.yageo.com/documents/recent/PYu-RC_Group_51_RoHS_L_04.pdf" H 2550 9700 50  0001 C CNN
F 4 "311-10.0KCRCT-ND" H 2550 9700 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0KCRCT-ND\">Link</a>" H 2550 9700 60  0001 C CNN "Link"
	1    2550 9700
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 6085BA3A
P 2550 9200
F 0 "R?" V 2630 9200 50  0000 C CNN
F 1 "1M" V 2550 9200 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 2480 9200 50  0001 C CNN
F 3 "http://www.yageo.com/documents/recent/PYu-RC_Group_51_RoHS_L_04.pdf" H 2550 9200 50  0001 C CNN
F 4 "311-10.0KCRCT-ND" H 2550 9200 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0KCRCT-ND\">Link</a>" H 2550 9200 60  0001 C CNN "Link"
	1    2550 9200
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR?
U 1 1 608626B6
P 2550 9850
F 0 "#PWR?" H 2550 9600 50  0001 C CNN
F 1 "GNDD" H 2550 9700 50  0000 C CNN
F 2 "" H 2550 9850 50  0000 C CNN
F 3 "" H 2550 9850 50  0000 C CNN
	1    2550 9850
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:Vproc-ARDF-2-Band-MiniTx-rescue #PWR?
U 1 1 6087887D
P 2550 9050
F 0 "#PWR?" H 2550 8900 50  0001 C CNN
F 1 "Vproc" H 2550 9200 50  0000 C CNN
F 2 "" H 2550 9050 50  0000 C CNN
F 3 "" H 2550 9050 50  0000 C CNN
	1    2550 9050
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 9450 2550 9450
Wire Wire Line
	2550 9350 2550 9450
Connection ~ 2550 9450
Wire Wire Line
	2550 9450 2350 9450
Wire Wire Line
	2550 9550 2550 9450
$Comp
L Transistor_BJT:PN2222A Q?
U 1 1 6092F0A9
P 1600 8950
F 0 "Q?" H 1790 8996 50  0000 L CNN
F 1 "PN2222A" H 1790 8905 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 1800 8875 50  0001 L CIN
F 3 "https://www.onsemi.com/pub/Collateral/PN2222-D.PDF" H 1600 8950 50  0001 L CNN
	1    1600 8950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 6093322E
P 1250 8950
F 0 "R?" V 1330 8950 50  0000 C CNN
F 1 "10k" V 1250 8950 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 1180 8950 50  0001 C CNN
F 3 "http://www.yageo.com/documents/recent/PYu-RC_Group_51_RoHS_L_04.pdf" H 1250 8950 50  0001 C CNN
F 4 "311-10.0KCRCT-ND" H 1250 8950 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0KCRCT-ND\">Link</a>" H 1250 8950 60  0001 C CNN "Link"
	1    1250 8950
	0    -1   -1   0   
$EndComp
$Comp
L power:GNDD #PWR?
U 1 1 609522C8
P 1700 9150
F 0 "#PWR?" H 1700 8900 50  0001 C CNN
F 1 "GNDD" H 1700 9000 50  0000 C CNN
F 2 "" H 1700 9150 50  0000 C CNN
F 3 "" H 1700 9150 50  0000 C CNN
	1    1700 9150
	-1   0    0    -1  
$EndComp
Text GLabel 1100 8950 0    39   Input ~ 0
PROC_PTT_OUT
Wire Wire Line
	2950 8150 3100 8150
Wire Wire Line
	2900 9450 3000 9450
Wire Wire Line
	3000 9450 3000 8950
Wire Wire Line
	3000 8950 3100 8950
Wire Wire Line
	1700 8750 1700 8650
Wire Wire Line
	1700 8650 2500 8650
Wire Wire Line
	2500 8650 2500 8550
Wire Wire Line
	2500 8550 3100 8550
Connection ~ 1700 8650
Wire Wire Line
	2950 7250 2950 7150
$Comp
L Jumper:SolderJumper_2_Open SJ?
U 1 1 60B1099A
P 2950 7000
F 0 "SJ?" H 2950 7205 50  0000 C CNN
F 1 "SolderJumper_2_Open" H 2950 7114 50  0000 C CNN
F 2 "" H 2950 7000 50  0001 C CNN
F 3 "~" H 2950 7000 50  0001 C CNN
	1    2950 7000
	0    -1   -1   0   
$EndComp
Connection ~ 2950 7700
Wire Wire Line
	2950 7550 2950 7700
$Comp
L Device:R R?
U 1 1 60AF77BF
P 2950 7400
F 0 "R?" V 3030 7400 50  0000 C CNN
F 1 "2.2k" V 2950 7400 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 2880 7400 50  0001 C CNN
F 3 "http://www.yageo.com/documents/recent/PYu-RC_Group_51_RoHS_L_04.pdf" H 2950 7400 50  0001 C CNN
F 4 "311-10.0KCRCT-ND" H 2950 7400 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0KCRCT-ND\">Link</a>" H 2950 7400 60  0001 C CNN "Link"
	1    2950 7400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2950 7700 2950 8150
Wire Wire Line
	2800 7700 2950 7700
$Comp
L Device:C_Small C?
U 1 1 609DD1AF
P 2700 7700
F 0 "C?" V 2600 7700 50  0000 L CNN
F 1 "100nF" V 2800 7450 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2700 7700 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 2700 7700 50  0001 C CNN
F 4 "478-1395-1-ND" H 2700 7700 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 2700 7700 60  0001 C CNN "Link"
	1    2700 7700
	0    -1   1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 609DA886
P 2450 7400
F 0 "R?" V 2530 7400 50  0000 C CNN
F 1 "100k" V 2450 7400 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 2380 7400 50  0001 C CNN
F 3 "http://www.yageo.com/documents/recent/PYu-RC_Group_51_RoHS_L_04.pdf" H 2450 7400 50  0001 C CNN
F 4 "311-10.0KCRCT-ND" H 2450 7400 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0KCRCT-ND\">Link</a>" H 2450 7400 60  0001 C CNN "Link"
	1    2450 7400
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR?
U 1 1 609D79E7
P 2450 7850
F 0 "#PWR?" H 2450 7600 50  0001 C CNN
F 1 "GNDD" H 2450 7700 50  0000 C CNN
F 2 "" H 2450 7850 50  0000 C CNN
F 3 "" H 2450 7850 50  0000 C CNN
	1    2450 7850
	-1   0    0    -1  
$EndComp
$Comp
L Device:R_POT RV?
U 1 1 609BC9EF
P 2450 7700
F 0 "RV?" H 2381 7746 50  0000 R CNN
F 1 "R_POT" H 2381 7655 50  0000 R CNN
F 2 "" H 2450 7700 50  0001 C CNN
F 3 "~" H 2450 7700 50  0001 C CNN
	1    2450 7700
	1    0    0    -1  
$EndComp
Text GLabel 2450 7250 1    39   Input ~ 0
PROC_TONE_OUT
Text Notes 1400 7500 0    79   ~ 16
HT Interface
$Comp
L Device:C C?
U 1 1 60F38BA0
P 3900 2250
F 0 "C?" H 3650 2150 50  0000 L CNN
F 1 "2.2uF" H 3925 2150 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 3938 2100 50  0001 C CNN
F 3 "" H 3900 2250 50  0001 C CNN
F 4 "1276-1763-1-ND" H 3900 2250 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-1763-1-ND\">Link</a>" H 3900 2250 60  0001 C CNN "Link"
	1    3900 2250
	1    0    0    1   
$EndComp
$Comp
L power:GNDD #PWR?
U 1 1 60F38BAA
P 3900 2400
F 0 "#PWR?" H 3900 2150 50  0001 C CNN
F 1 "GNDD" H 3900 2250 50  0000 C CNN
F 2 "" H 3900 2400 50  0000 C CNN
F 3 "" H 3900 2400 50  0000 C CNN
	1    3900 2400
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3900 2100 3900 2050
Wire Wire Line
	4200 2050 3900 2050
$Comp
L FlexFox80-rescue:MIC5219-ARDF-2-Band-MiniTx-rescue U?
U 1 1 60F38BB8
P 3150 2100
F 0 "U?" H 3150 2400 61  0000 C CNB
F 1 "MIC5219-3.3BM5" H 3150 2300 61  0000 C CNB
F 2 "TO_SOT_Packages_SMD:SOT-23-5_Handsolder" H 3150 2200 50  0001 C CIN
F 3 "" H 3150 2100 50  0000 C CNN
F 4 "576-1281-1-ND" H 3150 2100 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=576-1281-1-ND\">Link</a>" H 3150 2100 60  0001 C CNN "Link"
	1    3150 2100
	1    0    0    -1  
$EndComp
Connection ~ 3900 2050
$Comp
L power:GNDD #PWR?
U 1 1 60F38BC3
P 3150 2600
F 0 "#PWR?" H 3150 2350 50  0001 C CNN
F 1 "GNDD" H 3150 2450 50  0000 C CNN
F 2 "" H 3150 2600 50  0000 C CNN
F 3 "" H 3150 2600 50  0000 C CNN
	1    3150 2600
	-1   0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 60F38BCF
P 2900 2500
F 0 "R?" V 2980 2500 50  0000 C CNN
F 1 "10k" V 2900 2500 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 2830 2500 50  0001 C CNN
F 3 "" H 2900 2500 50  0000 C CNN
F 4 "311-10.0KCRCT-ND" H 2900 2500 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0KCRCT-ND\">Link</a>" H 2900 2500 60  0001 C CNN "Link"
	1    2900 2500
	0    -1   1    0   
$EndComp
Wire Wire Line
	3150 2400 3150 2500
Wire Wire Line
	3050 2500 3150 2500
Connection ~ 3150 2500
Wire Wire Line
	2750 2200 2700 2200
Wire Wire Line
	2700 2200 2700 2500
Wire Wire Line
	2700 2500 2750 2500
Wire Wire Line
	2450 1950 2450 2050
Wire Wire Line
	2450 2050 2750 2050
$Comp
L power:GNDD #PWR?
U 1 1 60F38BE1
P 2450 2400
F 0 "#PWR?" H 2450 2150 50  0001 C CNN
F 1 "GNDD" H 2450 2250 50  0000 C CNN
F 2 "" H 2450 2400 50  0000 C CNN
F 3 "" H 2450 2400 50  0000 C CNN
	1    2450 2400
	-1   0    0    -1  
$EndComp
$Comp
L power:+12V #PWR?
U 1 1 60F38BEB
P 2450 1950
F 0 "#PWR?" H 2450 1800 50  0001 C CNN
F 1 "+12V" H 2450 2090 50  0000 C CNN
F 2 "" H 2450 1950 50  0000 C CNN
F 3 "" H 2450 1950 50  0000 C CNN
	1    2450 1950
	-1   0    0    -1  
$EndComp
Connection ~ 2450 2050
Connection ~ 2700 2500
$Comp
L Device:C C?
U 1 1 60F38BF9
P 2450 2250
F 0 "C?" H 2200 2150 50  0000 L CNN
F 1 "2.2uF" H 2475 2150 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2488 2100 50  0001 C CNN
F 3 "" H 2450 2250 50  0001 C CNN
F 4 "1276-1763-1-ND" H 2450 2250 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-1763-1-ND\">Link</a>" H 2450 2250 60  0001 C CNN "Link"
	1    2450 2250
	1    0    0    1   
$EndComp
NoConn ~ 3550 2200
Wire Wire Line
	3900 2050 3550 2050
Wire Wire Line
	3150 2500 3150 2600
Wire Wire Line
	2450 2050 2450 2100
Wire Wire Line
	2700 2500 2700 2900
Text GLabel 2600 2900 0    39   Input ~ 0
3V3_PWR_ENABLE
Wire Wire Line
	2700 2900 2600 2900
$Comp
L FlexFox80-rescue:+3V3_L8-ARDF-2-Band-MiniTx-rescue #PWR?
U 1 1 60FA2D37
P 4200 1950
F 0 "#PWR?" H 4200 1800 50  0001 C CNN
F 1 "+3V3_L8" H 4200 2090 50  0000 C CNN
F 2 "" H 4200 1950 50  0000 C CNN
F 3 "" H 4200 1950 50  0000 C CNN
	1    4200 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	4200 2050 4200 1950
$Comp
L FlexFox80-rescue:MBD54DWT1G_Schottky_Ind-ARDF-2-Band-MiniTx-rescue D?
U 2 1 6120E592
P 2900 8450
F 0 "D?" H 2900 8350 50  0000 C CNN
F 1 "MBD54DWT1GOSCT-ND" H 2925 8725 50  0001 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-363-0.65" H 2900 8450 50  0001 C CNN
F 3 "https://www.onsemi.com/pub/Collateral/MBD54DWT1-D.PDF" H 2900 8450 50  0001 C CNN
F 4 "MBD54DWT1GOSCT-ND" H 2900 8450 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=MBD54DWT1GOSCT-ND\">Link</a>" H 2900 8450 60  0001 C CNN "Link"
F 6 "ON Semiconductor" H 2900 8450 50  0001 C CNN "Manufacturer"
F 7 "MBD54DWT1G" H 2900 8450 50  0001 C CNN "Manufacturer PN"
F 8 "DIODE ARRAY SCHOTTKY 30V SC88" H 2900 8450 50  0001 C CNN "Description"
	2    2900 8450
	-1   0    0    1   
$EndComp
$Comp
L FlexFox80-rescue:Vproc-ARDF-2-Band-MiniTx-rescue #PWR0302
U 1 1 61A2E79F
P 2700 8450
F 0 "#PWR0302" H 2700 8300 50  0001 C CNN
F 1 "Vproc" V 2700 8650 50  0000 C CNN
F 2 "" H 2700 8450 50  0000 C CNN
F 3 "" H 2700 8450 50  0000 C CNN
	1    2700 8450
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1000 8650 1000 6550
Wire Wire Line
	1000 6550 2950 6550
Wire Wire Line
	2950 6550 2950 6850
Wire Wire Line
	1000 8650 1700 8650
$Comp
L Diode:ESD9B5.0ST5G D?
U 1 1 612F5B15
P 3750 9800
F 0 "D?" V 3704 9879 50  0000 L CNN
F 1 "MBD54DWT1GOSCT-ND" V 3795 9879 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-363-0.65" H 3750 9800 50  0001 C CNN
F 3 "https://www.onsemi.com/pub/Collateral/MBD54DWT1-D.PDF" H 3750 9800 50  0001 C CNN
	1    3750 9800
	0    1    1    0   
$EndComp
$Comp
L power:GNDD #PWR?
U 1 1 612F8482
P 3750 9950
F 0 "#PWR?" H 3750 9700 50  0001 C CNN
F 1 "GNDD" H 3750 9800 50  0000 C CNN
F 2 "" H 3750 9950 50  0000 C CNN
F 3 "" H 3750 9950 50  0000 C CNN
	1    3750 9950
	1    0    0    -1  
$EndComp
$Comp
L Diode:ESD9B5.0ST5G D?
U 1 1 61314FD3
P 4100 9800
F 0 "D?" V 4054 9879 50  0000 L CNN
F 1 "MBD54DWT1GOSCT-ND" V 4145 9879 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-363-0.65" H 4100 9800 50  0001 C CNN
F 3 "https://www.onsemi.com/pub/Collateral/MBD54DWT1-D.PDF" H 4100 9800 50  0001 C CNN
	1    4100 9800
	0    1    1    0   
$EndComp
$Comp
L power:GNDD #PWR?
U 1 1 61314FDD
P 4100 9950
F 0 "#PWR?" H 4100 9700 50  0001 C CNN
F 1 "GNDD" H 4100 9800 50  0000 C CNN
F 2 "" H 4100 9950 50  0000 C CNN
F 3 "" H 4100 9950 50  0000 C CNN
	1    4100 9950
	1    0    0    -1  
$EndComp
Text GLabel 4150 9500 2    39   Input ~ 0
RXD_TO_FTDI
Text GLabel 3700 9500 0    39   Input ~ 0
TXD_TO_FTDI
Wire Wire Line
	3750 9650 3750 9500
Wire Wire Line
	3750 9500 3700 9500
Wire Wire Line
	4100 9650 4100 9500
Wire Wire Line
	4100 9500 4150 9500
Wire Bus Line
	7700 3250 7700 7050
$EndSCHEMATC
