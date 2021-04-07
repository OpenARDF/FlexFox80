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
L FlexFox80-rescue:ATMEGA328P-A-ARDF-2-Band-MiniTx-rescue U303
U 1 1 5797AFB0
P 6500 5450
F 0 "U303" H 6500 6850 60  0000 C CNB
F 1 "ATMEGA328P" H 6500 6750 60  0000 C CNB
F 2 "Housings_QFP:TQFP-32_7x7mm_Pitch0.8mm" H 6500 5450 50  0001 C CIN
F 3 "https://www.microchip.com/wwwproducts/en/ATmega328P" H 6500 5450 50  0001 C CNN
F 4 "ATMEGA328P-AURCT-ND" H 6500 5450 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=ATMEGA328P-AURCT-ND\">Link</a>" H 6500 5450 60  0001 C CNN "Link"
	1    6500 5450
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0118
U 1 1 5797CE10
P 8500 3800
F 0 "#PWR0118" H 8500 3550 50  0001 C CNN
F 1 "GNDD" H 8500 3650 50  0000 C CNN
F 2 "" H 8500 3800 50  0000 C CNN
F 3 "" H 8500 3800 50  0000 C CNN
	1    8500 3800
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0119
U 1 1 5797D116
P 5550 6750
F 0 "#PWR0119" H 5550 6500 50  0001 C CNN
F 1 "GNDD" H 5550 6600 50  0000 C CNN
F 2 "" H 5550 6750 50  0000 C CNN
F 3 "" H 5550 6750 50  0000 C CNN
	1    5550 6750
	1    0    0    -1  
$EndComp
$Comp
L power:GNDA #PWR0120
U 1 1 5797D770
P 5300 6750
F 0 "#PWR0120" H 5300 6500 50  0001 C CNN
F 1 "GNDA" H 5300 6600 50  0000 C CNN
F 2 "" H 5300 6750 50  0000 C CNN
F 3 "" H 5300 6750 50  0000 C CNN
	1    5300 6750
	1    0    0    -1  
$EndComp
Text Label 7500 4350 0    39   ~ 0
PB0
Entry Wire Line
	7600 4350 7700 4450
Text Label 7500 4450 0    39   ~ 0
PB1
Text Label 7500 4550 0    39   ~ 0
PB2
Text Label 7500 4650 0    39   ~ 0
PB3
Text Label 7500 4750 0    39   ~ 0
PB4
Text Label 7500 4850 0    39   ~ 0
PB5
Text Label 7500 4950 0    39   ~ 0
PB6
Text Label 7500 5050 0    39   ~ 0
PB7
Text Label 7500 5200 0    39   ~ 0
PC0
Text Label 7500 5300 0    39   ~ 0
PC1
Text Label 7500 5400 0    39   ~ 0
PC2
Text Label 7500 5500 0    39   ~ 0
PC3
Text Label 7500 5600 0    39   ~ 0
SDA
Text Label 7500 5700 0    39   ~ 0
SCL
Text Label 7500 5800 0    39   ~ 0
~RESET~
Text Label 7500 5950 0    39   ~ 0
PD0
Text Label 7500 6050 0    39   ~ 0
PD1
Text Label 7500 6150 0    39   ~ 0
PD2
Text Label 7500 6250 0    39   ~ 0
PD3
Text Label 7500 6350 0    39   ~ 0
PD4
Text Label 7500 6450 0    39   ~ 0
PD5
Text Label 7500 6550 0    39   ~ 0
PD6
Text Label 7500 6650 0    39   ~ 0
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
	7600 5200 7700 5300
Entry Wire Line
	7600 5300 7700 5400
Entry Wire Line
	7600 5400 7700 5500
Entry Wire Line
	7600 5500 7700 5600
Entry Wire Line
	7600 5600 7700 5700
Entry Wire Line
	7600 5700 7700 5800
Entry Wire Line
	7600 5800 7700 5900
Entry Wire Line
	7600 5950 7700 6050
Entry Wire Line
	7600 6050 7700 6150
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
	7700 4750 7800 4650
Entry Wire Line
	7700 4850 7800 4750
Text Label 7800 4650 0    39   ~ 0
SCL
Text Label 7800 4750 0    39   ~ 0
SDA
Entry Wire Line
	7700 5400 7800 5300
Text Label 7800 5200 0    39   ~ 0
PC2
Entry Wire Line
	7700 5650 7800 5550
Text Label 7800 4150 0    39   ~ 0
PC3
Entry Wire Line
	7700 4650 7800 4550
Text Label 7800 4450 0    39   ~ 0
PB5
Text Label 5600 5700 2    39   ~ 0
ADC6
Text Label 5600 5800 2    39   ~ 0
ADC7
Entry Wire Line
	7700 6650 7800 6550
Text Label 7800 6350 0    39   ~ 0
PB0
Entry Wire Line
	7700 5750 7800 5650
Text Label 7800 5650 0    39   ~ 0
PD5
Entry Wire Line
	7700 6450 7800 6350
Text Label 7800 6150 0    39   ~ 0
PD7
Entry Wire Line
	7700 5850 7800 5750
Entry Wire Line
	7700 5950 7800 5850
Entry Wire Line
	7700 6050 7800 5950
Entry Wire Line
	7700 6150 7800 6050
Text Label 7800 5750 0    39   ~ 0
PD0
Text Label 7800 5400 0    39   ~ 0
PD4
Text Label 7800 6050 0    39   ~ 0
PD3
Text Label 7800 5950 0    39   ~ 0
PD2
Entry Wire Line
	7700 6750 7800 6650
Text Label 7800 6650 0    39   ~ 0
PB7
Text Label 7800 4550 0    39   ~ 0
PB1
Entry Wire Line
	7700 6850 7800 6750
Text Label 7800 6750 0    39   ~ 0
PB6
Entry Wire Line
	7700 5200 7800 5100
Text GLabel 10250 4650 2    39   Output ~ 0
DI_SCL
Text GLabel 10250 4750 2    39   Output ~ 0
DI_SDA
Entry Wire Line
	7700 6250 7800 6150
Text Label 7800 5850 0    39   ~ 0
PD1
$Comp
L special:SI5351 U304
U 1 1 57ABD6C6
P 10650 3550
F 0 "U304" H 10650 4050 60  0000 C CNB
F 1 "SI5351A" H 10650 3550 60  0000 C CNB
F 2 "Housings_SSOP:MSOP-10_3x3mm_Pitch0.5mm_Handsolder" H 10650 3550 50  0001 C CNN
F 3 "http://www.silabs.com/Support%20Documents/TechnicalDocs/Si5351-B.pdf" H 10650 3550 50  0001 C CNN
F 4 "336-2399-5-ND" H 10650 3550 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=336-2399-5-ND\">Link</a>" H 10650 3550 60  0001 C CNN "Link"
	1    10650 3550
	1    0    0    -1  
$EndComp
Text Label 7800 5100 0    39   ~ 0
~RESET~
$Comp
L Device:R R305
U 1 1 57AD3D94
P 10150 5100
F 0 "R305" V 10230 5100 50  0000 C CNN
F 1 "10k" V 10150 5100 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 10080 5100 50  0001 C CNN
F 3 "http://www.yageo.com/documents/recent/PYu-RC_Group_51_RoHS_L_04.pdf" H 10150 5100 50  0001 C CNN
F 4 "311-10.0KCRCT-ND" H 10150 5100 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0KCRCT-ND\">Link</a>" H 10150 5100 60  0001 C CNN "Link"
	1    10150 5100
	0    1    1    0   
$EndComp
Text GLabel 10000 4950 2    39   Input ~ 0
~RESET~
$Comp
L FlexFox80-rescue:CONN_02X03-ARDF-2-Band-MiniTx-rescue P301
U 1 1 57AD5B1A
P 3300 5500
F 0 "P301" H 3300 5700 50  0000 C CNN
F 1 "ISP/PDI" H 3300 5300 50  0000 C CNN
F 2 "Pin_Headers:SM_Pin_Header_Straight_2x03" H 3300 4300 50  0001 C CNN
F 3 "http://katalog.we-online.de/em/datasheet/6100xx21121.pdf" H 3300 4300 50  0001 C CNN
F 4 "609-3487-1-ND" H 3300 5500 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=609-3487-1-ND\">Link</a>" H 3300 5500 60  0001 C CNN "Link"
	1    3300 5500
	1    0    0    -1  
$EndComp
Text GLabel 2950 5700 0    39   Output ~ 0
~RESET~
$Comp
L power:GNDD #PWR0121
U 1 1 57AD5D10
P 3650 5700
F 0 "#PWR0121" H 3650 5450 50  0001 C CNN
F 1 "GNDD" H 3650 5550 50  0000 C CNN
F 2 "" H 3650 5700 50  0000 C CNN
F 3 "" H 3650 5700 50  0000 C CNN
	1    3650 5700
	1    0    0    -1  
$EndComp
Text GLabel 3700 5500 2    39   Input ~ 0
MOSI
Entry Wire Line
	7700 4350 7800 4250
Text Label 7800 4250 0    39   ~ 0
PB3
Text GLabel 7950 4250 2    39   Output ~ 0
MOSI
Text GLabel 2900 5300 0    39   Input ~ 0
MISO
Entry Wire Line
	7700 4450 7800 4350
Text Label 7800 4350 0    39   ~ 0
PB4
Text GLabel 7950 4350 2    39   Output ~ 0
MISO
Entry Wire Line
	7700 4550 7800 4450
Text GLabel 7950 4450 2    39   Output ~ 0
SCK
Text GLabel 2900 5500 0    39   Input ~ 0
SCK
Entry Wire Line
	7700 6350 7800 6250
Text Label 7800 6250 0    39   ~ 0
PD6
Entry Wire Line
	7700 5500 7800 5400
Entry Wire Line
	7700 4250 7800 4150
Text Label 7800 5300 0    39   ~ 0
PC0
Text Label 7800 5550 0    39   ~ 0
PC1
Entry Wire Line
	7700 5300 7800 5200
Text Label 7800 6550 0    39   ~ 0
PB2
NoConn ~ 10000 3550
$Comp
L Device:C C310
U 1 1 57B42379
P 9750 3050
F 0 "C310" V 9600 2900 50  0000 L CNN
F 1 "470nF" V 9900 2950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 9788 2900 50  0001 C CNN
F 3 "" H 9750 3050 50  0001 C CNN
F 4 "478-9924-1-ND" H 9750 3050 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-9924-1-ND\">Link</a>" H 9750 3050 60  0001 C CNN "Link"
	1    9750 3050
	0    1    1    0   
$EndComp
$Comp
L power:GNDD #PWR0122
U 1 1 57B42657
P 9500 3100
F 0 "#PWR0122" H 9500 2850 50  0001 C CNN
F 1 "GNDD" H 9500 2950 50  0000 C CNN
F 2 "" H 9500 3100 50  0000 C CNN
F 3 "" H 9500 3100 50  0000 C CNN
	1    9500 3100
	1    0    0    -1  
$EndComp
$Comp
L Device:C C312
U 1 1 57B42FC2
P 11900 3850
F 0 "C312" H 11925 3950 50  0000 L CNN
F 1 "470nF" H 11925 3750 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 11938 3700 50  0001 C CNN
F 3 "" H 11900 3850 50  0001 C CNN
F 4 "478-9924-1-ND" H 11900 3850 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-9924-1-ND\">Link</a>" H 11900 3850 60  0001 C CNN "Link"
	1    11900 3850
	-1   0    0    1   
$EndComp
$Comp
L power:GNDD #PWR0123
U 1 1 57B4BF2C
P 4950 4150
F 0 "#PWR0123" H 4950 3900 50  0001 C CNN
F 1 "GNDD" H 4950 4000 50  0000 C CNN
F 2 "" H 4950 4150 50  0000 C CNN
F 3 "" H 4950 4150 50  0000 C CNN
	1    4950 4150
	0    1    1    0   
$EndComp
$Comp
L power:GNDD #PWR0124
U 1 1 57B4C04E
P 4800 4450
F 0 "#PWR0124" H 4800 4200 50  0001 C CNN
F 1 "GNDD" H 4800 4300 50  0000 C CNN
F 2 "" H 4800 4450 50  0000 C CNN
F 3 "" H 4800 4450 50  0000 C CNN
	1    4800 4450
	0    1    -1   0   
$EndComp
$Comp
L Device:C_Small C308
U 1 1 57B51AE4
P 8500 3600
F 0 "C308" H 8510 3670 50  0000 L CNN
F 1 "100nF" H 8510 3520 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 8500 3600 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 8500 3600 50  0001 C CNN
F 4 "478-1395-1-ND" H 8500 3600 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 8500 3600 60  0001 C CNN "Link"
	1    8500 3600
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C306
U 1 1 57AE8E4C
P 5000 4650
F 0 "C306" V 4900 4450 50  0000 L CNN
F 1 "100nF" V 5100 4700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5000 4650 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 5000 4650 50  0001 C CNN
F 4 "478-1395-1-ND" H 5000 4650 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 5000 4650 60  0001 C CNN "Link"
	1    5000 4650
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C307
U 1 1 57AE90C4
P 5050 4450
F 0 "C307" V 4950 4450 50  0000 L CNN
F 1 "100nF" V 5150 4200 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5050 4450 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 5050 4450 50  0001 C CNN
F 4 "478-1395-1-ND" H 5050 4450 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 5050 4450 60  0001 C CNN "Link"
	1    5050 4450
	0    -1   1    0   
$EndComp
Text Notes 6100 3800 0    79   ~ 16
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
L power:GNDA #PWR0126
U 1 1 57B0FFE1
P 11800 10300
F 0 "#PWR0126" H 11800 10050 50  0001 C CNN
F 1 "GNDA" H 11800 10150 50  0000 C CNN
F 2 "" H 11800 10300 50  0000 C CNN
F 3 "" H 11800 10300 50  0000 C CNN
	1    11800 10300
	1    0    0    -1  
$EndComp
$Comp
L power:GNDA #PWR0127
U 1 1 57B43438
P 5550 5250
F 0 "#PWR0127" H 5550 5000 50  0001 C CNN
F 1 "GNDA" H 5550 5100 50  0000 C CNN
F 2 "" H 5550 5250 50  0000 C CNN
F 3 "" H 5550 5250 50  0000 C CNN
	1    5550 5250
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C304
U 1 1 57B43F7B
P 5550 5150
F 0 "C304" H 5600 5050 50  0000 L CNN
F 1 "10uF" H 5650 5200 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5550 5150 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 5550 5150 50  0001 C CNN
F 4 "490-5523-1-ND" H 5550 5150 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=490-5523-1-ND\">Link</a>" H 5550 5150 60  0001 C CNN "Link"
	1    5550 5150
	-1   0    0    1   
$EndComp
Text Label 3550 5500 0    39   ~ 0
PB3
Text Label 3050 5400 2    39   ~ 0
PB4
Text Label 3050 5500 2    39   ~ 0
PB5
Text Notes 9550 2700 0    79   ~ 16
SI5351 Clock
$Comp
L FlexFox80-rescue:CONN_01X01-ARDF-2-Band-MiniTx-rescue P302
U 1 1 57BD33AA
P 10200 3050
F 0 "P302" H 10200 3150 50  0000 C CNN
F 1 "Thru" H 10300 3050 50  0001 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01" H 10200 3050 50  0001 C CNN
F 3 "" H 10200 3050 50  0000 C CNN
F 4 "np" H 10200 3050 60  0001 C CNN "Part No."
F 5 "np" H 10200 3050 60  0001 C CNN "Link"
	1    10200 3050
	1    0    0    -1  
$EndComp
Text Notes 10600 4550 0    39   ~ 8
I2C pull-ups can be provided by uC \nport pin pull-ups on SCL and SDA.
$Comp
L special:ASTX-H11 X301
U 1 1 58755171
P 9200 3600
F 0 "X301" H 9200 3950 61  0000 C CNB
F 1 "ASTX-H11-25.MHZ" H 9200 3600 61  0000 C CNB
F 2 "Crystals:ASTX-H11" H 9200 3600 60  0001 C CNN
F 3 "" H 9200 3600 60  0000 C CNN
F 4 "535-12039-1-ND" H 9200 3600 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=535-12039-1-ND\">Link</a>" H 9200 3600 60  0001 C CNN "Link"
	1    9200 3600
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:TEST_1P-ARDF-2-Band-MiniTx-rescue W301
U 1 1 587A8789
P 2950 6950
F 0 "W301" V 2950 7220 50  0000 C CNN
F 1 "TEST_1P" H 2950 7150 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Square-SMD-Pad_Small" H 3150 6950 50  0001 C CNN
F 3 "" H 3150 6950 50  0000 C CNN
F 4 "np" H 2950 6950 60  0001 C CNN "Part No."
F 5 "np" H 2950 6950 60  0001 C CNN "Link"
	1    2950 6950
	0    -1   1    0   
$EndComp
$Comp
L FlexFox80-rescue:TEST_1P-ARDF-2-Band-MiniTx-rescue W302
U 1 1 587A8A8E
P 2950 7350
F 0 "W302" V 2950 7620 50  0000 C CNN
F 1 "TEST_1P" H 2950 7550 50  0001 C CNN
F 2 "Measurement_Points:Measurement_Point_Square-SMD-Pad_Small" H 3150 7350 50  0001 C CNN
F 3 "" H 3150 7350 50  0000 C CNN
F 4 "np" H 2950 7350 60  0001 C CNN "Part No."
F 5 "np" H 2950 7350 60  0001 C CNN "Link"
	1    2950 7350
	0    -1   1    0   
$EndComp
Text Notes 2150 6350 0    79   ~ 16
Real-Time Clock
Text Notes 12350 9500 0    118  ~ 24
PCB: Flex Fox 80 ARDF Transmitter  (p. 3/3)
Text GLabel 8800 5950 3    39   Output ~ 0
DI_TXD0
$Comp
L special:DS3231 U301
U 1 1 591E6034
P 3350 7150
F 0 "U301" H 3550 7650 60  0000 C CNB
F 1 "DS3231" H 3600 7550 60  0000 C CNB
F 2 "SMD_Packages:SO16_Handsolder" H 3350 7150 60  0001 C CNN
F 3 "" H 3350 7150 60  0000 C CNN
F 4 "DS3231S#T&RCT-ND" H 3350 7150 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=DS3231S%23T%26RCT-ND\">Link</a>" H 3350 7150 60  0001 C CNN "Link"
	1    3350 7150
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0128
U 1 1 591E61CB
P 3350 7700
F 0 "#PWR0128" H 3350 7450 50  0001 C CNN
F 1 "GNDD" H 3350 7550 50  0000 C CNN
F 2 "" H 3350 7700 50  0000 C CNN
F 3 "" H 3350 7700 50  0000 C CNN
	1    3350 7700
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0129
U 1 1 591E902A
P 4050 7000
F 0 "#PWR0129" H 4050 6750 50  0001 C CNN
F 1 "GNDD" H 4050 6850 50  0000 C CNN
F 2 "" H 4050 7000 50  0000 C CNN
F 3 "" H 4050 7000 50  0000 C CNN
	1    4050 7000
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C301
U 1 1 591EA9E9
P 3200 6600
F 0 "C301" V 3100 6450 50  0000 L CNN
F 1 "1uF" V 3300 6550 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 3200 6600 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 3200 6600 50  0001 C CNN
F 4 "311-1365-1-ND" H 3200 6600 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-1365-1-ND\">Link</a>" H 3200 6600 60  0001 C CNN "Link"
	1    3200 6600
	0    1    1    0   
$EndComp
$Comp
L power:GNDD #PWR0130
U 1 1 591EABD4
P 3100 6600
F 0 "#PWR0130" H 3100 6350 50  0001 C CNN
F 1 "GNDD" H 3100 6450 50  0000 C CNN
F 2 "" H 3100 6600 50  0000 C CNN
F 3 "" H 3100 6600 50  0000 C CNN
	1    3100 6600
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
L FlexFox80-rescue:Vproc-ARDF-2-Band-MiniTx-rescue #PWR0133
U 1 1 591CAC09
P 5850 3450
F 0 "#PWR0133" H 5850 3300 50  0001 C CNN
F 1 "Vproc" H 5850 3600 50  0000 C CNN
F 2 "" H 5850 3450 50  0000 C CNN
F 3 "" H 5850 3450 50  0000 C CNN
	1    5850 3450
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0134
U 1 1 591CB758
P 5650 3850
F 0 "#FLG0134" H 5650 3945 50  0001 C CNN
F 1 "PWR_FLAG" H 5650 4030 50  0000 C CNN
F 2 "" H 5650 3850 50  0000 C CNN
F 3 "" H 5650 3850 50  0000 C CNN
	1    5650 3850
	1    0    0    -1  
$EndComp
Text Notes 5700 3200 0    59   ~ 0
Power to Processor & RTC Only
Wire Wire Line
	10000 3000 10000 3050
Wire Wire Line
	5600 6550 5550 6550
Wire Wire Line
	5150 4450 5350 4450
Wire Wire Line
	11900 3650 11300 3650
Wire Wire Line
	5550 4950 5600 4950
Wire Wire Line
	5600 6650 5550 6650
Wire Wire Line
	5550 6550 5550 6650
Connection ~ 5550 6650
Wire Wire Line
	5600 6450 5300 6450
Wire Wire Line
	5300 6450 5300 6750
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
	7600 5400 7500 5400
Wire Wire Line
	7600 5500 7500 5500
Wire Wire Line
	7600 5600 7500 5600
Wire Wire Line
	7600 5700 7500 5700
Wire Wire Line
	7600 5800 7500 5800
Wire Wire Line
	7600 6050 7500 6050
Wire Wire Line
	7600 6150 7500 6150
Wire Wire Line
	7500 6250 7600 6250
Wire Wire Line
	7600 6350 7500 6350
Wire Wire Line
	7600 6450 7500 6450
Wire Wire Line
	7600 6550 7500 6550
Wire Wire Line
	7600 6650 7500 6650
Wire Wire Line
	11400 3450 11300 3450
Wire Wire Line
	7950 4550 7800 4550
Wire Wire Line
	7800 5100 9450 5100
Wire Wire Line
	10400 5000 10400 5100
Wire Wire Line
	3550 5600 3650 5600
Wire Wire Line
	3650 5600 3650 5700
Wire Wire Line
	3550 5400 3650 5400
Wire Wire Line
	3650 5400 3650 5350
Wire Wire Line
	7950 4250 7800 4250
Wire Wire Line
	7800 4350 7950 4350
Wire Wire Line
	7800 4450 7950 4450
Wire Wire Line
	3050 5300 3050 5400
Wire Wire Line
	2900 5300 3050 5300
Wire Wire Line
	3050 5600 3050 5700
Wire Wire Line
	3050 5700 2950 5700
Wire Wire Line
	10000 4950 9950 4950
Wire Wire Line
	5100 4650 5350 4650
Wire Wire Line
	10000 3450 9700 3450
Wire Wire Line
	9900 3050 10000 3050
Connection ~ 10000 3050
Wire Wire Line
	11900 3550 11900 3650
Wire Wire Line
	9950 4950 9950 5100
Connection ~ 9950 5100
Wire Wire Line
	5350 4350 5600 4350
Connection ~ 5650 3850
Wire Wire Line
	5000 4150 4950 4150
Wire Wire Line
	8500 3400 8500 3450
Wire Wire Line
	8500 3450 8700 3450
Wire Wire Line
	8500 3700 8500 3750
Wire Wire Line
	8500 3750 8700 3750
Connection ~ 5350 4350
Wire Wire Line
	5550 4900 5550 4950
Wire Wire Line
	7600 5200 7500 5200
Wire Wire Line
	7600 5300 7500 5300
Wire Wire Line
	9900 3750 10000 3750
Wire Wire Line
	9800 3650 10000 3650
Wire Wire Line
	9600 3050 9500 3050
Wire Wire Line
	9500 3050 9500 3100
Connection ~ 8500 3450
Connection ~ 8500 3750
Wire Wire Line
	10400 5100 10300 5100
Wire Wire Line
	7800 6250 7950 6250
Wire Wire Line
	7600 5950 7500 5950
Wire Wire Line
	2900 5500 3050 5500
Wire Wire Line
	3700 5500 3550 5500
Wire Wire Line
	10400 5300 10350 5300
Connection ~ 10400 5100
Wire Wire Line
	7800 5750 9200 5750
Wire Wire Line
	7800 5850 8200 5850
Wire Wire Line
	5400 5700 5600 5700
Wire Wire Line
	3150 7600 3150 7650
Wire Wire Line
	3150 7650 3200 7650
Wire Wire Line
	3550 7650 3550 7600
Wire Wire Line
	3200 7600 3200 7650
Connection ~ 3200 7650
Wire Wire Line
	3250 7600 3250 7650
Connection ~ 3250 7650
Wire Wire Line
	3300 7600 3300 7650
Connection ~ 3300 7650
Wire Wire Line
	3350 7600 3350 7650
Wire Wire Line
	3400 7600 3400 7650
Connection ~ 3400 7650
Wire Wire Line
	3450 7600 3450 7650
Connection ~ 3450 7650
Wire Wire Line
	3500 7600 3500 7650
Connection ~ 3500 7650
Connection ~ 3350 7650
Wire Wire Line
	3350 6500 3350 6600
Connection ~ 3350 6600
Wire Wire Line
	7950 5950 7800 5950
Wire Wire Line
	7800 6050 7950 6050
Wire Wire Line
	7950 5200 7800 5200
Wire Wire Line
	7950 6150 7800 6150
Wire Wire Line
	11350 10300 11350 10200
Wire Wire Line
	11100 10200 11350 10200
Wire Wire Line
	11550 9850 11550 10050
Wire Wire Line
	5400 5800 5600 5800
Wire Wire Line
	7950 4150 7800 4150
Connection ~ 5350 3850
Wire Wire Line
	5850 3850 5850 3450
Wire Wire Line
	7950 5300 7800 5300
Wire Wire Line
	7950 5550 7800 5550
Text GLabel 5400 5800 0    39   Input ~ 0
TX_BATTERY_VOLTAGE
Text GLabel 5400 5700 0    39   Input ~ 0
TX_PA_VOLTAGE
Text GLabel 7950 6350 2    39   Output ~ 0
VHF_ENABLE
Wire Wire Line
	7950 6350 7800 6350
Wire Wire Line
	7950 5400 7800 5400
Wire Wire Line
	7950 6550 7800 6550
Text GLabel 7950 6650 2    39   Output ~ 0
MAIN_PWR_ENABLE
Wire Wire Line
	7950 6650 7800 6650
Text GLabel 7950 6750 2    39   Output ~ 0
TX_FINAL_VOLTAGE_ENABLE
Wire Wire Line
	7950 6750 7800 6750
Text GLabel 3750 7250 2    39   Output ~ 0
RTC_SQW
Text GLabel 7950 5950 2    39   Input ~ 0
RTC_SQW
Text GLabel 7950 6050 2    39   Input ~ 0
~ANT_CONNECT_INT~
Text GLabel 8050 5600 2    39   Input ~ 0
2M_ANTENNA_DETECT
Text GLabel 4950 3300 1    39   Input ~ 0
VBAT_UNINTERRUPTED
Wire Wire Line
	9800 4650 9800 3650
Connection ~ 9800 4650
Wire Wire Line
	9900 4750 9900 3750
Connection ~ 9900 4750
$Comp
L Device:R R304
U 1 1 594E78F8
P 8350 5850
F 0 "R304" V 8430 5850 50  0000 C CNN
F 1 "10" V 8350 5850 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8280 5850 50  0001 C CNN
F 3 "" H 8350 5850 50  0001 C CNN
F 4 "311-10.0CRCT-ND" H 8350 5850 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0CRCT-ND\">Link</a>" H 8350 5850 60  0001 C CNN "Link"
	1    8350 5850
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C305
U 1 1 5959ECE9
P 5100 4150
F 0 "C305" V 5200 4200 50  0000 L CNN
F 1 "10uF" V 5200 3950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5100 4150 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 5100 4150 50  0001 C CNN
F 4 "490-5523-1-ND" H 5100 4150 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=490-5523-1-ND\">Link</a>" H 5100 4150 60  0001 C CNN "Link"
	1    5100 4150
	0    -1   -1   0   
$EndComp
$Comp
L power:GNDD #PWR0135
U 1 1 595A2417
P 4800 4650
F 0 "#PWR0135" H 4800 4400 50  0001 C CNN
F 1 "GNDD" H 4800 4500 50  0000 C CNN
F 2 "" H 4800 4650 50  0000 C CNN
F 3 "" H 4800 4650 50  0000 C CNN
	1    4800 4650
	0    1    1    0   
$EndComp
Wire Wire Line
	4800 4650 4900 4650
Wire Wire Line
	4800 4450 4950 4450
Wire Wire Line
	5200 4150 5350 4150
Wire Wire Line
	12050 10200 12050 10300
Connection ~ 11550 10200
Wire Wire Line
	11800 10300 11800 10200
Connection ~ 11800 10200
$Comp
L power:GNDD #PWR0136
U 1 1 595A7BF6
P 11900 4000
F 0 "#PWR0136" H 11900 3750 50  0001 C CNN
F 1 "GNDD" H 11900 3850 50  0000 C CNN
F 2 "" H 11900 4000 50  0000 C CNN
F 3 "" H 11900 4000 50  0000 C CNN
	1    11900 4000
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0137
U 1 1 595A7E0D
P 11600 3550
F 0 "#PWR0137" H 11600 3300 50  0001 C CNN
F 1 "GNDD" H 11600 3400 50  0000 C CNN
F 2 "" H 11600 3550 50  0000 C CNN
F 3 "" H 11600 3550 50  0000 C CNN
	1    11600 3550
	0    -1   -1   0   
$EndComp
Connection ~ 11900 3650
Wire Wire Line
	11600 3550 11300 3550
$Comp
L FlexFox80-rescue:Vproc-ARDF-2-Band-MiniTx-rescue #PWR0138
U 1 1 59E2AD5F
P 10400 5000
F 0 "#PWR0138" H 10400 4850 50  0001 C CNN
F 1 "Vproc" H 10400 5150 50  0000 C CNN
F 2 "" H 10400 5000 50  0000 C CNN
F 3 "" H 10400 5000 50  0000 C CNN
	1    10400 5000
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
L FlexFox80-rescue:CONN_01X03-ARDF-2-Band-MiniTx-rescue TP301
U 1 1 5A0894A0
P 9350 4200
F 0 "TP301" V 9450 3900 50  0000 C CNN
F 1 "I2C" V 9450 4200 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 9350 4200 50  0001 C CNN
F 3 "" H 9350 4200 50  0001 C CNN
F 4 "np" H 9350 4200 60  0001 C CNN "Part No."
F 5 "np" H 9350 4200 60  0001 C CNN "Link"
	1    9350 4200
	0    1    -1   0   
$EndComp
$Comp
L power:GNDD #PWR0139
U 1 1 5A0895E4
P 9450 4400
F 0 "#PWR0139" H 9450 4150 50  0001 C CNN
F 1 "GNDD" H 9450 4250 50  0000 C CNN
F 2 "" H 9450 4400 50  0000 C CNN
F 3 "" H 9450 4400 50  0000 C CNN
	1    9450 4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	9250 4400 9250 4650
Connection ~ 9250 4650
Wire Wire Line
	9350 4400 9350 4750
Connection ~ 9350 4750
Wire Wire Line
	11300 2650 11300 3350
Wire Wire Line
	11400 2650 11400 3450
$Comp
L Device:R R307
U 1 1 5A6C903F
P 10350 4350
F 0 "R307" V 10430 4350 50  0000 C CNN
F 1 "10k" V 10350 4350 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 10280 4350 50  0001 C CNN
F 3 "http://www.yageo.com/documents/recent/PYu-RC_Group_51_RoHS_L_04.pdf" H 10350 4350 50  0001 C CNN
F 4 "311-10.0KCRCT-ND" H 10350 4350 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0KCRCT-ND\">Link</a>" H 10350 4350 60  0001 C CNN "Link"
	1    10350 4350
	0    1    1    0   
$EndComp
$Comp
L Device:R R306
U 1 1 5A6C9183
P 10350 4150
F 0 "R306" V 10430 4150 50  0000 C CNN
F 1 "10k" V 10350 4150 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 10280 4150 50  0001 C CNN
F 3 "http://www.yageo.com/documents/recent/PYu-RC_Group_51_RoHS_L_04.pdf" H 10350 4150 50  0001 C CNN
F 4 "311-10.0KCRCT-ND" H 10350 4150 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-10.0KCRCT-ND\">Link</a>" H 10350 4150 60  0001 C CNN "Link"
	1    10350 4150
	0    1    1    0   
$EndComp
Wire Wire Line
	10500 4150 10600 4150
Wire Wire Line
	10600 4150 10600 4250
Wire Wire Line
	10600 4350 10500 4350
Wire Wire Line
	10700 4250 10600 4250
Connection ~ 10600 4250
Wire Wire Line
	10200 4350 10100 4350
Wire Wire Line
	10100 4350 10100 4750
Connection ~ 10100 4750
Wire Wire Line
	10200 4150 10000 4150
Wire Wire Line
	10000 4150 10000 4650
Connection ~ 10000 4650
$Comp
L FlexFox80-rescue:Vproc-ARDF-2-Band-MiniTx-rescue #PWR0140
U 1 1 5AB7DABB
P 3350 6500
F 0 "#PWR0140" H 3350 6350 50  0001 C CNN
F 1 "Vproc" H 3350 6650 50  0000 C CNN
F 2 "" H 3350 6500 50  0000 C CNN
F 3 "" H 3350 6500 50  0000 C CNN
	1    3350 6500
	1    0    0    -1  
$EndComp
Text GLabel 11400 2650 1    39   Output ~ 0
CLK1
Text GLabel 11300 2650 1    39   Output ~ 0
CLK0
Text GLabel 7950 4550 2    39   Output ~ 0
HF_ENABLE
$Comp
L FlexFox80-rescue:Battery_Single_Cell-ARDF-2-Band-MiniTx-rescue BT301
U 1 1 5BCD438E
P 3900 7000
F 0 "BT301" H 4000 6900 50  0000 L CNN
F 1 "CR1220" H 4000 6975 50  0000 L CNN
F 2 "Oddities:BC501SM_CR1220_BAT_Handsolder" V 3900 7040 50  0001 C CNN
F 3 "" V 3900 7040 50  0000 C CNN
F 4 "BC501SM-ND" H 3900 7000 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=BC501SM-ND\">Link</a>" H 3900 7000 60  0001 C CNN "Link"
	1    3900 7000
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:+3V3_L8-ARDF-2-Band-MiniTx-rescue #PWR0141
U 1 1 5BCD52AA
P 10000 3000
F 0 "#PWR0141" H 10000 2850 50  0001 C CNN
F 1 "+3V3_L8" H 10000 3140 50  0000 C CNN
F 2 "" H 10000 3000 50  0000 C CNN
F 3 "" H 10000 3000 50  0000 C CNN
	1    10000 3000
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:+3V3_L8-ARDF-2-Band-MiniTx-rescue #PWR0142
U 1 1 5BCD5593
P 8500 3400
F 0 "#PWR0142" H 8500 3250 50  0001 C CNN
F 1 "+3V3_L8" H 8500 3540 50  0000 C CNN
F 2 "" H 8500 3400 50  0000 C CNN
F 3 "" H 8500 3400 50  0000 C CNN
	1    8500 3400
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:+3V3_L8-ARDF-2-Band-MiniTx-rescue #PWR0143
U 1 1 5BCD568A
P 10700 4250
F 0 "#PWR0143" H 10700 4100 50  0001 C CNN
F 1 "+3V3_L8" H 10700 4390 50  0000 C CNN
F 2 "" H 10700 4250 50  0000 C CNN
F 3 "" H 10700 4250 50  0000 C CNN
	1    10700 4250
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:+3V3_L8-ARDF-2-Band-MiniTx-rescue #PWR0144
U 1 1 5BCD5E43
P 11900 3550
F 0 "#PWR0144" H 11900 3400 50  0001 C CNN
F 1 "+3V3_L8" H 11900 3690 50  0000 C CNN
F 2 "" H 11900 3550 50  0000 C CNN
F 3 "" H 11900 3550 50  0000 C CNN
	1    11900 3550
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:CONN_01X06-ARDF-2-Band-MiniTx-rescue P303
U 1 1 5BCD63B2
P 10550 6000
F 0 "P303" H 10550 6350 50  0000 C CNN
F 1 "Header" V 10650 6000 50  0001 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06" H 10550 6000 50  0001 C CNN
F 3 "" H 10550 6000 50  0001 C CNN
F 4 "np" H 10550 6000 60  0001 C CNN "Part No."
F 5 "np" H 10550 6000 60  0001 C CNN "Link"
	1    10550 6000
	1    0    0    1   
$EndComp
$Comp
L power:GNDD #PWR0145
U 1 1 5BCD6506
P 10150 6350
F 0 "#PWR0145" H 10150 6100 50  0001 C CNN
F 1 "GNDD" H 10150 6200 50  0000 C CNN
F 2 "" H 10150 6350 50  0000 C CNN
F 3 "" H 10150 6350 50  0000 C CNN
	1    10150 6350
	1    0    0    -1  
$EndComp
NoConn ~ 10350 6150
$Comp
L Device:C_Small C309
U 1 1 5BCD6BBE
P 9450 5550
F 0 "C309" H 9460 5620 50  0000 L CNN
F 1 "100nF" H 9460 5470 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 9450 5550 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 9450 5550 50  0001 C CNN
F 4 "478-1395-1-ND" H 9450 5550 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 9450 5550 60  0001 C CNN "Link"
	1    9450 5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	9450 5450 9450 5100
Connection ~ 9450 5100
Wire Wire Line
	9450 5750 9450 5650
Wire Wire Line
	8500 5850 8800 5850
Wire Wire Line
	8800 5950 8800 5850
Connection ~ 8800 5850
Wire Wire Line
	10150 6350 10150 6250
Wire Wire Line
	10150 6250 10350 6250
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
L power:GNDD #PWR0146
U 1 1 5BCBB750
P 12100 6650
F 0 "#PWR0146" H 12100 6400 50  0001 C CNN
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
L power:GNDD #PWR0149
U 1 1 5BCBC48D
P 12450 6400
F 0 "#PWR0149" H 12450 6150 50  0001 C CNN
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
L power:GNDD #PWR0151
U 1 1 5BCBE48F
P 14750 7300
F 0 "#PWR0151" H 14750 7050 50  0001 C CNN
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
L power:GNDD #PWR0154
U 1 1 5BCC1FE2
P 11250 7950
F 0 "#PWR0154" H 11250 7700 50  0001 C CNN
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
L power:GNDD #PWR0156
U 1 1 5BCC2F26
P 10850 7300
F 0 "#PWR0156" H 10850 7050 50  0001 C CNN
F 1 "GNDD" H 10850 7150 50  0000 C CNN
F 2 "" H 10850 7300 50  0000 C CNN
F 3 "" H 10850 7300 50  0000 C CNN
	1    10850 7300
	0    1    1    0   
$EndComp
Wire Wire Line
	9450 7650 9450 5950
Wire Wire Line
	9200 5950 9450 5950
Wire Wire Line
	9200 5750 9200 5950
Connection ~ 9450 5950
Text Notes 11100 8200 2    39   ~ 0
Resetting the WiFi module must also disable\nWiFi UART communication with the processor.\nThis allows the processor to block powerup\ngarbage sent by the WiFi board.
Text GLabel 7950 6250 2    39   Output ~ 0
~WIFI_RESET~
Wire Wire Line
	11650 7750 13500 7750
Text GLabel 7950 6150 2    39   Output ~ 0
WIFI_ENABLE
Text GLabel 15150 7600 0    39   Input ~ 0
WIFI_ENABLE
Wire Wire Line
	15200 7600 15150 7600
Connection ~ 15200 7200
$Comp
L FlexFox80-rescue:MBD54DWT1G_Schottky_Ind-ARDF-2-Band-MiniTx-rescue D303
U 1 1 5BF19980
P 10150 5300
F 0 "D303" H 10150 5200 50  0000 C CNN
F 1 "MBD54DWT1G_Schottky_Ind" H 10175 5575 50  0001 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-363_Handsolder" H 10150 5300 50  0001 C CNN
F 3 "https://www.onsemi.com/pub/Collateral/MBD54DWT1-D.PDF" H 10150 5300 50  0001 C CNN
F 4 "MBD54DWT1GOSCT-ND" H 10150 5300 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=MBD54DWT1GOSCT-ND\">Link</a>" H 10150 5300 60  0001 C CNN "Link"
	1    10150 5300
	1    0    0    -1  
$EndComp
Text Notes 10650 6000 0    39   ~ 0
FTDI Cable Connector
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
Wire Wire Line
	9450 5750 10350 5750
$Comp
L FlexFox80-rescue:Vproc-ARDF-2-Band-MiniTx-rescue #PWR0158
U 1 1 5C60DF91
P 3650 5350
F 0 "#PWR0158" H 3650 5200 50  0001 C CNN
F 1 "Vproc" H 3650 5500 50  0000 C CNN
F 2 "" H 3650 5350 50  0000 C CNN
F 3 "" H 3650 5350 50  0000 C CNN
	1    3650 5350
	1    0    0    -1  
$EndComp
Text GLabel 7950 5200 2    39   Input ~ 0
TX_+12_VOLTAGE
Wire Wire Line
	7800 4650 8300 4650
Wire Wire Line
	7800 4750 8400 4750
Wire Wire Line
	9150 4650 9250 4650
Wire Wire Line
	9150 4750 9350 4750
$Comp
L power:GNDD #PWR0159
U 1 1 5C75CA25
P 8850 4950
F 0 "#PWR0159" H 8850 4700 50  0001 C CNN
F 1 "GNDD" H 8850 4800 50  0001 C CNN
F 2 "" H 8850 4950 50  0000 C CNN
F 3 "" H 8850 4950 50  0000 C CNN
	1    8850 4950
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:+3V3_L8-ARDF-2-Band-MiniTx-rescue #PWR0160
U 1 1 5C75CAFF
P 8850 4100
F 0 "#PWR0160" H 8850 3950 50  0001 C CNN
F 1 "+3V3_L8" H 8850 4240 50  0000 C CNN
F 2 "" H 8850 4100 50  0000 C CNN
F 3 "" H 8850 4100 50  0000 C CNN
	1    8850 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	8850 4100 8850 4150
Wire Wire Line
	8550 4550 8550 4300
Wire Wire Line
	8550 4300 8850 4300
Connection ~ 8850 4300
$Comp
L FlexFox80-rescue:SN74CB3Q3305-ARDF-2-Band-MiniTx-rescue U307
U 1 1 5C75D7C5
P 8850 4650
F 0 "U307" H 9000 4900 61  0000 C CNB
F 1 "SN74CB3Q3305" H 9250 4400 61  0000 C CNB
F 2 "Housings_SSOP:TSSOP-8_3x3mm_Pitch0.65mm_Handsolder" H 8850 4250 50  0001 C CNN
F 3 "" H 8850 4700 50  0001 C CNN
F 4 "296-16809-1-ND" H 8850 4650 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=296-16809-1-ND\">Link</a>" H 8850 4650 60  0001 C CNN "Link"
	1    8850 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 4300 9150 4550
Text GLabel 8300 4800 3    39   Output ~ 0
RTC_SCL
Text GLabel 8400 4800 3    39   Output ~ 0
RTC_SDA
Wire Wire Line
	8300 4800 8300 4650
Connection ~ 8300 4650
Wire Wire Line
	8400 4800 8400 4750
Connection ~ 8400 4750
$Comp
L Device:C_Small C316
U 1 1 5C7604ED
P 8650 4150
F 0 "C316" V 8550 4050 50  0000 L CNN
F 1 "100nF" V 8750 4050 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 8650 4150 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 8650 4150 50  0001 C CNN
F 4 "478-1395-1-ND" H 8650 4150 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 8650 4150 60  0001 C CNN "Link"
	1    8650 4150
	0    1    1    0   
$EndComp
Wire Wire Line
	8750 4150 8850 4150
Connection ~ 8850 4150
$Comp
L power:GNDD #PWR0161
U 1 1 5C7609F5
P 8550 4150
F 0 "#PWR0161" H 8550 3900 50  0001 C CNN
F 1 "GNDD" H 8550 4000 50  0001 C CNN
F 2 "" H 8550 4150 50  0000 C CNN
F 3 "" H 8550 4150 50  0000 C CNN
	1    8550 4150
	0    1    1    0   
$EndComp
Text GLabel 2950 7100 0    39   Input ~ 0
RTC_SCL
Text GLabel 2950 7200 0    39   Input ~ 0
RTC_SDA
Connection ~ 5350 4450
Connection ~ 5350 4150
Text GLabel 7950 6550 2    39   Output ~ 0
3V3_PWR_ENABLE
Connection ~ 5350 4650
$Comp
L Device:C_Small C302
U 1 1 5C78AA39
P 5000 4850
F 0 "C302" V 4900 4650 50  0000 L CNN
F 1 "100nF" V 5100 4900 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5000 4850 50  0001 C CNN
F 3 "http://www.kemet.com/Lists/ProductCatalog/Attachments/53/KEM_C1002_X7R_SMD.pdf" H 5000 4850 50  0001 C CNN
F 4 "478-1395-1-ND" H 5000 4850 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 5000 4850 60  0001 C CNN "Link"
	1    5000 4850
	0    1    1    0   
$EndComp
Wire Wire Line
	5350 4850 5100 4850
$Comp
L power:GNDD #PWR0162
U 1 1 5C78AA40
P 4800 4850
F 0 "#PWR0162" H 4800 4600 50  0001 C CNN
F 1 "GNDD" H 4800 4700 50  0000 C CNN
F 2 "" H 4800 4850 50  0000 C CNN
F 3 "" H 4800 4850 50  0000 C CNN
	1    4800 4850
	0    1    1    0   
$EndComp
Wire Wire Line
	4800 4850 4900 4850
$Comp
L power:PWR_FLAG #FLG0163
U 1 1 5C78D99A
P 5550 4900
F 0 "#FLG0163" H 5550 4995 50  0001 C CNN
F 1 "PWR_FLAG" H 5550 5080 50  0000 C CNN
F 2 "" H 5550 4900 50  0000 C CNN
F 3 "" H 5550 4900 50  0000 C CNN
	1    5550 4900
	1    0    0    -1  
$EndComp
Connection ~ 5550 4950
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
Text GLabel 8050 5350 2    39   Input ~ 0
80M_ANTENNA_DETECT
Wire Wire Line
	7950 5650 7800 5650
Wire Wire Line
	8050 5350 7950 5350
Wire Wire Line
	7950 5300 7950 5350
Connection ~ 7950 5350
Wire Wire Line
	8050 5600 7950 5600
Wire Wire Line
	7950 5550 7950 5600
Connection ~ 7950 5600
Text GLabel 11300 3750 2    39   Output ~ 0
CLK2
Text GLabel 7950 4150 2    39   Output ~ 0
VHF_CLK_SEL
$Comp
L FlexFox80-rescue:D_Schottky_x2_KCom_Dual-ARDF-2-Band-MiniTx-rescue D301
U 1 1 5CE37D9A
P 4650 3500
F 0 "D301" H 4650 3650 50  0000 C CNN
F 1 "D_Schottky_x2_KCom_Dual" H 4650 3600 50  0001 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-363_Handsolder" H 4650 3500 50  0001 C CNN
F 3 "" H 4650 3500 50  0000 C CNN
F 4 "BAT54CDW-FDICT-ND" H 4650 3500 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=BAT54CDW-FDICT-ND\">Link</a>" H 4650 3500 60  0001 C CNN "Link"
	1    4650 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 3650 4650 3850
Wire Wire Line
	4650 3850 5350 3850
Text Notes 12600 5400 0    79   ~ 16
WiFi Module
NoConn ~ 13400 7150
NoConn ~ 12100 7050
NoConn ~ 10350 6050
NoConn ~ 12700 6050
Wire Wire Line
	3300 6600 3350 6600
$Comp
L Device:Jumper_NC_Small J307
U 1 1 5F83B487
P 9950 7650
F 0 "J307" H 9950 7750 50  0000 C CNN
F 1 "Jumper_NC_Small" H 9960 7590 50  0001 C CNN
F 2 "Wire_Connections_Bridges:Solder-Jumper-NC-SMD-Pad_Small" H 9950 7650 50  0001 C CNN
F 3 "" H 9950 7650 50  0000 C CNN
F 4 "np" H 9950 7650 60  0001 C CNN "Part No."
F 5 "np" H 9950 7650 60  0001 C CNN "Link"
	1    9950 7650
	1    0    0    -1  
$EndComp
Wire Wire Line
	10450 7650 10050 7650
Wire Wire Line
	9850 7650 9450 7650
$Comp
L Device:C C314
U 1 1 5F83F111
P 2650 3650
F 0 "C314" H 2400 3750 50  0000 L CNN
F 1 "2.2uF" H 2750 3700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2688 3500 50  0001 C CNN
F 3 "" H 2650 3650 50  0001 C CNN
F 4 "1276-1763-1-ND" H 2650 3650 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-1763-1-ND\">Link</a>" H 2650 3650 60  0001 C CNN "Link"
	1    2650 3650
	-1   0    0    1   
$EndComp
$Comp
L power:GNDD #PWR0167
U 1 1 5F83F117
P 2650 3800
F 0 "#PWR0167" H 2650 3550 50  0001 C CNN
F 1 "GNDD" H 2650 3650 50  0000 C CNN
F 2 "" H 2650 3800 50  0000 C CNN
F 3 "" H 2650 3800 50  0000 C CNN
	1    2650 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 3500 2650 3500
$Comp
L FlexFox80-rescue:MIC5219-ARDF-2-Band-MiniTx-rescue U308
U 1 1 5F83F121
P 3400 3550
F 0 "U308" H 3400 3850 61  0000 C CNB
F 1 "MIC5219-3.3BM5" H 3400 3750 61  0000 C CNB
F 2 "TO_SOT_Packages_SMD:SOT-23-5_Handsolder" H 3400 3650 50  0001 C CIN
F 3 "" H 3400 3550 50  0000 C CNN
F 4 "576-1281-1-ND" H 3400 3550 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=576-1281-1-ND\">Link</a>" H 3400 3550 60  0001 C CNN "Link"
	1    3400 3550
	1    0    0    -1  
$EndComp
Connection ~ 2650 3500
Wire Wire Line
	2950 3650 3000 3650
$Comp
L power:GNDD #PWR0168
U 1 1 5F83F139
P 3400 3850
F 0 "#PWR0168" H 3400 3600 50  0001 C CNN
F 1 "GNDD" H 3400 3700 50  0000 C CNN
F 2 "" H 3400 3850 50  0000 C CNN
F 3 "" H 3400 3850 50  0000 C CNN
	1    3400 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 3500 4100 3500
$Comp
L power:GNDD #PWR0169
U 1 1 5F83F14F
P 4100 3800
F 0 "#PWR0169" H 4100 3550 50  0001 C CNN
F 1 "GNDD" H 4100 3650 50  0000 C CNN
F 2 "" H 4100 3800 50  0000 C CNN
F 3 "" H 4100 3800 50  0000 C CNN
	1    4100 3800
	1    0    0    -1  
$EndComp
Connection ~ 4100 3500
$Comp
L Device:C C317
U 1 1 5F83F15F
P 4100 3650
F 0 "C317" H 3850 3750 50  0000 L CNN
F 1 "2.2uF" H 4150 3750 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4138 3500 50  0001 C CNN
F 3 "" H 4100 3650 50  0001 C CNN
F 4 "1276-1763-1-ND" H 4100 3650 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-1763-1-ND\">Link</a>" H 4100 3650 60  0001 C CNN "Link"
	1    4100 3650
	-1   0    0    1   
$EndComp
Wire Wire Line
	2950 3650 2950 3500
Connection ~ 2950 3500
Text GLabel 2050 3500 0    39   Input ~ 0
AUX_BAT_DIRECT
NoConn ~ 3800 3650
NoConn ~ 14350 6900
$Comp
L FlexFox80-rescue:ZENER-ARDF-2-Band-MiniTx-rescue D302
U 1 1 5F84898E
P 2250 3500
F 0 "D302" H 2250 3600 50  0000 C CNN
F 1 "4.7V" H 2250 3400 50  0000 C CNN
F 2 "Diodes_SMD:SOD-123_Handsolder" H 2250 3500 50  0001 C CNN
F 3 "" H 2250 3500 50  0000 C CNN
F 4 "BZT52C4V7-13FDICT-ND" H 2250 3500 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=BZT52C4V7-13FDICT-ND\">Link</a>" H 2250 3500 60  0001 C CNN "Link"
	1    2250 3500
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0170
U 1 1 5F84CE0D
P 2650 3500
F 0 "#FLG0170" H 2650 3595 50  0001 C CNN
F 1 "PWR_FLAG" H 2650 3680 50  0000 C CNN
F 2 "" H 2650 3500 50  0000 C CNN
F 3 "" H 2650 3500 50  0000 C CNN
	1    2650 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 3500 4950 3500
Wire Wire Line
	4950 3500 4950 3300
Wire Wire Line
	5350 3850 5350 4150
Wire Wire Line
	5550 6650 5550 6750
Wire Wire Line
	10000 3050 10000 3350
Wire Wire Line
	9950 5100 10000 5100
Wire Wire Line
	9950 5100 9950 5300
Wire Wire Line
	5650 3850 5850 3850
Wire Wire Line
	5350 4350 5350 4450
Wire Wire Line
	8500 3450 8500 3500
Wire Wire Line
	8500 3750 8500 3800
Wire Wire Line
	10400 5100 10400 5300
Wire Wire Line
	3200 7650 3250 7650
Wire Wire Line
	3250 7650 3300 7650
Wire Wire Line
	3300 7650 3350 7650
Wire Wire Line
	3400 7650 3450 7650
Wire Wire Line
	3450 7650 3500 7650
Wire Wire Line
	3500 7650 3550 7650
Wire Wire Line
	3350 7650 3350 7700
Wire Wire Line
	3350 7650 3400 7650
Wire Wire Line
	3350 6600 3350 6700
Wire Wire Line
	5350 3850 5650 3850
Wire Wire Line
	9800 4650 10000 4650
Wire Wire Line
	9900 4750 10100 4750
Wire Wire Line
	11550 10200 11800 10200
Wire Wire Line
	11550 10200 11550 10300
Wire Wire Line
	11800 10200 12050 10200
Wire Wire Line
	11900 3650 11900 3700
Wire Wire Line
	11550 10050 11550 10200
Wire Wire Line
	9250 4650 9800 4650
Wire Wire Line
	9350 4750 9900 4750
Wire Wire Line
	10600 4250 10600 4350
Wire Wire Line
	10100 4750 10250 4750
Wire Wire Line
	10000 4650 10250 4650
Wire Wire Line
	9450 5100 9950 5100
Wire Wire Line
	8800 5850 10350 5850
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
	9450 5950 10350 5950
Wire Wire Line
	15200 7200 15200 7600
Wire Wire Line
	11350 10200 11550 10200
Wire Wire Line
	8850 4300 8850 4350
Wire Wire Line
	8850 4300 9150 4300
Wire Wire Line
	8300 4650 8550 4650
Wire Wire Line
	8400 4750 8550 4750
Wire Wire Line
	8850 4150 8850 4300
Wire Wire Line
	5350 4450 5600 4450
Wire Wire Line
	5350 4450 5350 4650
Wire Wire Line
	5350 4150 5350 4350
Wire Wire Line
	5350 4650 5600 4650
Wire Wire Line
	5350 4650 5350 4850
Wire Wire Line
	5550 4950 5550 5050
Wire Wire Line
	13500 7750 13800 7750
Wire Wire Line
	7950 5350 7950 5400
Wire Wire Line
	7950 5600 7950 5650
Wire Wire Line
	2650 3500 2950 3500
Wire Wire Line
	4100 3500 4400 3500
Wire Wire Line
	2950 3500 3000 3500
$Comp
L MCU_Microchip_ATmega:ATmega1284P-AU U?
U 1 1 6081BBDB
P 1350 8000
F 0 "U?" H 1350 5911 50  0000 C CNN
F 1 "ATmega1284P-AU" H 1350 5820 50  0000 C CNN
F 2 "Package_QFP:TQFP-44_10x10mm_P0.8mm" H 1350 8000 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8272-8-bit-AVR-microcontroller-ATmega164A_PA-324A_PA-644A_PA-1284_P_datasheet.pdf" H 1350 8000 50  0001 C CNN
	1    1350 8000
	1    0    0    -1  
$EndComp
Wire Bus Line
	7700 4150 7700 6850
$EndSCHEMATC
