EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 4
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
L FlexFox80-rescue:LTC3600-ARDF-2-Band-MiniTx-rescue U203
U 1 1 58B77FB4
P 7550 2500
F 0 "U203" H 7450 2500 61  0000 C CNB
F 1 "LTC3600" H 7600 2400 61  0000 C CNB
F 2 "SMD_Packages:SSOP-12_Pad_Handsolder" H 7550 2500 50  0001 C CNN
F 3 "" H 7550 2500 50  0000 C CNN
F 4 "LTC3600EMSE#PBF-ND" H 7550 2500 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=LTC3600EMSE%23PBF-ND\">Link</a>" H 7550 2500 60  0001 C CNN "Link"
	1    7550 2500
	1    0    0    -1  
$EndComp
NoConn ~ 7650 1950
Text Notes 7050 6950 0    118  ~ 24
PCB: Flex Fox 80 ARDF Transmitter  (p. 2/3)
$Comp
L Device:C_Small C213
U 1 1 58B7FEBD
P 8700 3000
F 0 "C213" H 8500 3100 50  0000 L CNN
F 1 "1uF" H 8550 2900 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 8700 3000 50  0001 C CNN
F 3 "" H 8700 3000 50  0000 C CNN
F 4 "311-1365-1-ND" H 8700 3000 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-1365-1-ND\">Link</a>" H 8700 3000 60  0001 C CNN "Link"
	1    8700 3000
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR082
U 1 1 58B7FF36
P 8700 3100
F 0 "#PWR082" H 8700 2850 50  0001 C CNN
F 1 "GND_L8" H 8700 2950 50  0000 C CNN
F 2 "" H 8700 3100 50  0000 C CNN
F 3 "" H 8700 3100 50  0000 C CNN
	1    8700 3100
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR083
U 1 1 58B80A53
P 7550 3050
F 0 "#PWR083" H 7550 2800 50  0001 C CNN
F 1 "GND_L8" H 7550 2900 50  0000 C CNN
F 2 "" H 7550 3050 50  0000 C CNN
F 3 "" H 7550 3050 50  0000 C CNN
	1    7550 3050
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C211
U 1 1 58B80AF3
P 8300 2550
F 0 "C211" V 8400 2500 50  0000 L CNN
F 1 "100nF" V 8200 2500 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 8300 2550 50  0001 C CNN
F 3 "" H 8300 2550 50  0000 C CNN
F 4 "478-1395-1-ND" H 8300 2550 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 8300 2550 60  0001 C CNN "Link"
	1    8300 2550
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:INDUCTOR_SMALL-ARDF-2-Band-MiniTx-rescue L202
U 1 1 58B80C57
P 8850 2650
F 0 "L202" H 8850 2750 50  0000 C CNN
F 1 "6.8uH" H 8850 2600 50  0000 C CNN
F 2 "Inductors:CDR7D28MN" H 8850 2650 50  0001 C CNN
F 3 "http://www.bourns.com/docs/Product-Datasheets/SRP7028A.pdf" H 8850 2650 50  0001 C CNN
F 4 "308-1328-1-ND" H 8850 2650 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=308-1328-1-ND\">Link</a>" H 8850 2650 60  0001 C CNN "Link"
	1    8850 2650
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR084
U 1 1 58B81116
P 9200 2950
F 0 "#PWR084" H 9200 2700 50  0001 C CNN
F 1 "GND_L8" H 9200 2800 50  0000 C CNN
F 2 "" H 9200 2950 50  0000 C CNN
F 3 "" H 9200 2950 50  0000 C CNN
	1    9200 2950
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C210
U 1 1 58B8121A
P 7250 1900
F 0 "C210" V 7150 1800 50  0000 L CNN
F 1 "22uF" V 7350 1850 50  0000 L CNN
F 2 "Capacitors_SMD:C_1210_HandSoldering" H 7250 1900 50  0001 C CNN
F 3 "" H 7250 1900 50  0000 C CNN
F 4 "1276-3373-1-ND" H 7250 1900 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-3373-1-ND\">Link</a>" H 7250 1900 60  0001 C CNN "Link"
	1    7250 1900
	0    1    1    0   
$EndComp
$Comp
L FlexFox80-rescue:DAC081C085-ARDF-2-Band-MiniTx-rescue U201
U 1 1 58B84991
P 3600 2250
F 0 "U201" H 3600 2700 61  0000 L CNB
F 1 "DAC081C085" H 3400 2600 61  0000 L CNB
F 2 "SMD_Packages:SSOP-8_Handsolder" H 3600 2250 50  0001 C CIN
F 3 "" H 3600 2250 50  0000 C CNN
F 4 "DAC081C085CIMM/NOPBCT-ND" H 3600 2250 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=DAC081C085CIMM%2FNOPBCT-ND\">Link</a>" H 3600 2250 60  0001 C CNN "Link"
	1    3600 2250
	1    0    0    -1  
$EndComp
Text GLabel 3150 2300 0    39   Input ~ 0
DI_SCL
Text GLabel 3150 2400 0    39   Input ~ 0
DI_SDA
$Comp
L FlexFox80-rescue:+3V3_L8-ARDF-2-Band-MiniTx-rescue #PWR085
U 1 1 58B86FD9
P 4350 1250
F 0 "#PWR085" H 4350 1100 50  0001 C CNN
F 1 "+3V3_L8" H 4350 1390 50  0000 C CNN
F 2 "" H 4350 1250 50  0000 C CNN
F 3 "" H 4350 1250 50  0000 C CNN
	1    4350 1250
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C201
U 1 1 58B87194
P 4000 1350
F 0 "C201" V 4100 1250 50  0000 L CNN
F 1 "10uF" V 3900 1250 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4000 1350 50  0001 C CNN
F 3 "" H 4000 1350 50  0000 C CNN
F 4 "490-5523-1-ND" H 4000 1350 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=490-5523-1-ND\">Link</a>" H 4000 1350 60  0001 C CNN "Link"
	1    4000 1350
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR086
U 1 1 58B87220
P 4350 2500
F 0 "#PWR086" H 4350 2250 50  0001 C CNN
F 1 "GND_L8" H 4350 2350 50  0001 C CNN
F 2 "" H 4350 2500 50  0000 C CNN
F 3 "" H 4350 2500 50  0000 C CNN
	1    4350 2500
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR087
U 1 1 58B88093
P 3800 1400
F 0 "#PWR087" H 3800 1150 50  0001 C CNN
F 1 "GND_L8" H 3800 1250 50  0000 C CNN
F 2 "" H 3800 1400 50  0000 C CNN
F 3 "" H 3800 1400 50  0000 C CNN
	1    3800 1400
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C203
U 1 1 58B880F2
P 4350 2400
F 0 "C203" H 4150 2350 50  0000 L CNN
F 1 "10uF" H 4100 2450 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4350 2400 50  0001 C CNN
F 3 "" H 4350 2400 50  0000 C CNN
F 4 "490-5523-1-ND" H 4350 2400 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=490-5523-1-ND\">Link</a>" H 4350 2400 60  0001 C CNN "Link"
	1    4350 2400
	-1   0    0    1   
$EndComp
NoConn ~ 3200 2100
NoConn ~ 3200 2200
$Comp
L Device:R R204
U 1 1 58B8A0BC
P 5250 1950
F 0 "R204" V 5330 1950 50  0000 C CNN
F 1 "536k" V 5250 1950 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5180 1950 50  0001 C CNN
F 3 "" H 5250 1950 50  0000 C CNN
F 4 "311-536KCRCT-ND" H 5250 1950 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-536KCRCT-ND\">Link</a>" H 5250 1950 60  0001 C CNN "Link"
	1    5250 1950
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R207
U 1 1 58B8A411
P 6000 2850
F 0 "R207" V 6080 2850 50  0000 C CNN
F 1 "100k" V 6000 2850 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5930 2850 50  0001 C CNN
F 3 "" H 6000 2850 50  0000 C CNN
F 4 "311-100KCRCT-ND" H 6000 2850 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-100KCRCT-ND\">Link</a>" H 6000 2850 60  0001 C CNN "Link"
	1    6000 2850
	0    1    1    0   
$EndComp
$Comp
L Device:R R206
U 1 1 58B8A49B
P 5500 2850
F 0 "R206" V 5580 2850 50  0000 C CNN
F 1 "46.4k" V 5500 2850 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5430 2850 50  0001 C CNN
F 3 "" H 5500 2850 50  0000 C CNN
F 4 "RMCF0805FT46K4CT-ND" H 5500 2850 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=RMCF0805FT46K4CT-ND\">Link</a>" H 5500 2850 60  0001 C CNN "Link"
	1    5500 2850
	0    1    1    0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR088
U 1 1 58B8A516
P 5900 2550
F 0 "#PWR088" H 5900 2300 50  0001 C CNN
F 1 "GND_L8" H 5900 2400 50  0000 C CNN
F 2 "" H 5900 2550 50  0000 C CNN
F 3 "" H 5900 2550 50  0000 C CNN
F 4 "311-100KCRCT-ND" H 5900 2550 60  0001 C CNN "Part No."
F 5 "https://www.digikey.com/products/en?keywords=311-100KCRCT-ND" H 5900 2550 60  0001 C CNN "Link"
	1    5900 2550
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR089
U 1 1 58B8B2D8
P 5350 2850
F 0 "#PWR089" H 5350 2600 50  0001 C CNN
F 1 "GND_L8" H 5350 2700 50  0001 C CNN
F 2 "" H 5350 2850 50  0000 C CNN
F 3 "" H 5350 2850 50  0000 C CNN
	1    5350 2850
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C205
U 1 1 58B8BFA7
P 5650 1750
F 0 "C205" V 5750 1650 50  0000 L CNN
F 1 "10uF" V 5550 1650 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5650 1750 50  0001 C CNN
F 3 "" H 5650 1750 50  0000 C CNN
F 4 "490-5523-1-ND" H 5650 1750 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=490-5523-1-ND\">Link</a>" H 5650 1750 60  0001 C CNN "Link"
	1    5650 1750
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR090
U 1 1 58B8BFAD
P 5550 1750
F 0 "#PWR090" H 5550 1500 50  0001 C CNN
F 1 "GND_L8" H 5550 1600 50  0001 C CNN
F 2 "" H 5550 1750 50  0001 C CNN
F 3 "" H 5550 1750 50  0000 C CNN
	1    5550 1750
	0    1    1    0   
$EndComp
$Comp
L Device:R R212
U 1 1 58B8D442
P 8900 1700
F 0 "R212" V 8800 1700 50  0000 C CNN
F 1 "162k" V 8900 1700 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8830 1700 50  0001 C CNN
F 3 "" H 8900 1700 50  0000 C CNN
F 4 "311-162KCRCT-ND" H 8900 1700 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-162KCRCT-ND\">Link</a>" H 8900 1700 60  0001 C CNN "Link"
	1    8900 1700
	0    1    1    0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR091
U 1 1 58B8D5B1
P 8300 1750
F 0 "#PWR091" H 8300 1500 50  0001 C CNN
F 1 "GND_L8" H 8300 1600 50  0000 C CNN
F 2 "" H 8300 1750 50  0000 C CNN
F 3 "" H 8300 1750 50  0000 C CNN
	1    8300 1750
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C212
U 1 1 58B8D65C
P 8700 1900
F 0 "C212" H 8800 1900 50  0000 L CNN
F 1 "100nF" H 8750 1800 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 8700 1900 50  0001 C CNN
F 3 "" H 8700 1900 50  0000 C CNN
F 4 "478-1395-1-ND" H 8700 1900 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 8700 1900 60  0001 C CNN "Link"
	1    8700 1900
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR092
U 1 1 58B8D9C8
P 8700 2000
F 0 "#PWR092" H 8700 1750 50  0001 C CNN
F 1 "GND_L8" H 8700 1850 50  0000 C CNN
F 2 "" H 8700 2000 50  0000 C CNN
F 3 "" H 8700 2000 50  0000 C CNN
	1    8700 2000
	1    0    0    -1  
$EndComp
Text GLabel 8700 1600 1    39   Output ~ 0
TX_PA_VOLTAGE
Text GLabel 6950 2850 3    39   Input ~ 0
TX_FINAL_VOLTAGE_ENABLE
$Comp
L power:+12V #PWR095
U 1 1 58BBB18B
P 7450 1700
F 0 "#PWR095" H 7450 1550 50  0001 C CNN
F 1 "+12V" H 7450 1840 50  0000 C CNN
F 2 "" H 7450 1700 50  0000 C CNN
F 3 "" H 7450 1700 50  0000 C CNN
	1    7450 1700
	1    0    0    -1  
$EndComp
$Comp
L Device:R R210
U 1 1 58C44704
P 6700 2700
F 0 "R210" V 6600 2700 50  0000 C CNN
F 1 "100k" V 6700 2700 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 6630 2700 50  0001 C CNN
F 3 "" H 6700 2700 50  0000 C CNN
F 4 "311-100KCRCT-ND" H 6700 2700 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-100KCRCT-ND\">Link</a>" H 6700 2700 60  0001 C CNN "Link"
	1    6700 2700
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR097
U 1 1 58C44803
P 6700 2850
F 0 "#PWR097" H 6700 2600 50  0001 C CNN
F 1 "GND_L8" H 6700 2700 50  0001 C CNN
F 2 "" H 6700 2850 50  0000 C CNN
F 3 "" H 6700 2850 50  0000 C CNN
	1    6700 2850
	1    0    0    -1  
$EndComp
Text Notes 6600 5150 0    79   ~ 16
External Battery\nConnector
Text Notes 4750 1000 0    118  ~ 24
High-Efficiency Variable Buck Power Regulator
$Comp
L FlexFox80-rescue:OPA171-ARDF-2-Band-MiniTx-rescue U202
U 1 1 59D417D4
P 6000 2200
F 0 "U202" H 6000 2450 61  0000 L CNB
F 1 "OPA171" H 6000 2350 61  0000 L CNB
F 2 "TO_SOT_Packages_SMD:SOT-23-5_Handsolder" H 6050 2400 50  0001 C CNN
F 3 "" H 6000 2450 50  0000 C CNN
F 4 "296-37698-1-ND" H 6000 2200 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=296-37698-1-ND\">Link</a>" H 6000 2200 60  0001 C CNN "Link"
	1    6000 2200
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0100
U 1 1 59D6B4C4
P 5800 5050
F 0 "#PWR0100" H 5800 4800 50  0001 C CNN
F 1 "GND_L8" H 5800 4900 50  0001 C CNN
F 2 "" H 5800 5050 50  0000 C CNN
F 3 "" H 5800 5050 50  0000 C CNN
	1    5800 5050
	0    1    1    0   
$EndComp
$Comp
L Device:CP1_Small C214
U 1 1 59D849C5
P 9200 2850
F 0 "C214" H 8900 2850 50  0000 L CNN
F 1 "47uF low ESR" H 9350 2850 50  0000 L CNN
F 2 "Capacitors_SMD:C_2917_7343_Metric_HandSoldering" H 9200 2850 50  0001 C CNN
F 3 "https://content.kemet.com/datasheets/KEM_T2009_T495.pdf" H 9200 2850 50  0001 C CNN
F 4 "399-3885-1-ND" H 9200 2850 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-3885-1-ND\">Link</a>" H 9200 2850 60  0001 C CNN "Link"
	1    9200 2850
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:CONN_01X02-ARDF-2-Band-MiniTx-rescue P203
U 1 1 59FCFE1A
P 6000 5100
F 0 "P203" H 6000 5250 50  0000 C CNN
F 1 "Power Pole" V 6150 5100 50  0001 C CNN
F 2 "" H 6000 5100 50  0001 C CNN
F 3 "" H 6000 5100 50  0001 C CNN
F 4 "2243-ASMPP30-1X2-RK-ND" H 6000 5100 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=2243-ASMPP30-1X2-RK-ND\">Link</a>" H 6000 5100 60  0001 C CNN "Link"
F 6 "TH" H 6000 5100 50  0001 C CNN "Manufacturer"
F 7 "TH" H 6000 5100 50  0001 C CNN "Manufacturer PN"
F 8 "TH" H 6000 5100 50  0001 C CNN "Description"
	1    6000 5100
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0163
U 1 1 5A747983
P 8100 2800
F 0 "#PWR0163" H 8100 2550 50  0001 C CNN
F 1 "GND_L8" H 8100 2650 50  0001 C CNN
F 2 "" H 8100 2800 50  0000 C CNN
F 3 "" H 8100 2800 50  0000 C CNN
	1    8100 2800
	0    -1   1    0   
$EndComp
$Comp
L FlexFox80-rescue:+2to+12V-ARDF-2-Band-MiniTx-rescue #PWR0102
U 1 1 5A74F9FF
P 9200 1400
AR Path="/5A74F9FF" Ref="#PWR0102"  Part="1" 
AR Path="/591BC960/5A74F9FF" Ref="#PWR0165"  Part="1" 
F 0 "#PWR0165" H 9200 1250 50  0001 C CNN
F 1 "+2to+12V" H 9200 1540 50  0000 C CNN
F 2 "" H 9200 1400 50  0000 C CNN
F 3 "" H 9200 1400 50  0000 C CNN
	1    9200 1400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R208
U 1 1 5A765C6C
P 6500 3450
F 0 "R208" V 6580 3450 50  0000 C CNN
F 1 "27k" V 6500 3450 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 6430 3450 50  0001 C CNN
F 3 "" H 6500 3450 50  0000 C CNN
F 4 "311-27.0KCRCT-ND" H 6500 3450 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-27.0KCRCT-ND\">Link</a>" H 6500 3450 60  0001 C CNN "Link"
	1    6500 3450
	-1   0    0    1   
$EndComp
$Comp
L Device:C_Small C207
U 1 1 5A767617
P 6500 3750
F 0 "C207" V 6600 3700 50  0000 L CNN
F 1 "270pF" V 6400 3700 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 6500 3750 50  0001 C CNN
F 3 "" H 6500 3750 50  0000 C CNN
F 4 "1276-2624-1-ND" H 6500 3750 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-2624-1-ND\">Link</a>" H 6500 3750 60  0001 C CNN "Link"
	1    6500 3750
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C208
U 1 1 5A76779A
P 6700 3450
F 0 "C208" V 6800 3400 50  0000 L CNN
F 1 "10pF" V 6600 3400 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 6700 3450 50  0001 C CNN
F 3 "" H 6700 3450 50  0000 C CNN
F 4 "478-10457-1-ND" H 6700 3450 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-10457-1-ND\">Link<a/>" H 6700 3450 60  0001 C CNN "Link"
	1    6700 3450
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0103
U 1 1 5A769729
P 6500 3900
F 0 "#PWR0103" H 6500 3650 50  0001 C CNN
F 1 "GND_L8" H 6500 3750 50  0001 C CNN
F 2 "" H 6500 3900 50  0000 C CNN
F 3 "" H 6500 3900 50  0000 C CNN
	1    6500 3900
	1    0    0    -1  
$EndComp
$Comp
L power:+12V #PWR0166
U 1 1 5A8ADFD2
P 5900 1550
F 0 "#PWR0166" H 5900 1400 50  0001 C CNN
F 1 "+12V" H 5900 1690 50  0000 C CNN
F 2 "" H 5900 1550 50  0000 C CNN
F 3 "" H 5900 1550 50  0000 C CNN
	1    5900 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	7000 2350 6500 2350
Wire Wire Line
	6850 2650 7000 2650
Wire Wire Line
	8100 2350 8500 2350
Wire Wire Line
	8500 2350 8500 2800
Wire Wire Line
	8500 3300 6850 3300
Connection ~ 8500 2800
Wire Wire Line
	8100 2500 8100 2450
Wire Wire Line
	8100 2450 8300 2450
Wire Wire Line
	8100 2650 8300 2650
Connection ~ 8300 2650
Wire Wire Line
	9200 1400 9200 1500
Wire Wire Line
	9200 2650 9100 2650
Wire Wire Line
	7450 1700 7450 1900
Wire Wire Line
	8100 2200 9200 2200
Connection ~ 9200 2650
Connection ~ 9200 2200
Wire Wire Line
	3200 2300 3150 2300
Wire Wire Line
	3200 2400 3150 2400
Wire Wire Line
	4350 1250 4350 1350
Wire Wire Line
	4100 1350 4200 1350
Connection ~ 4350 1350
Wire Wire Line
	5200 2100 5550 2100
Wire Wire Line
	3800 1350 3900 1350
Wire Wire Line
	4350 1900 4350 1950
Wire Wire Line
	4350 2200 4100 2200
Connection ~ 4350 2200
Wire Wire Line
	4100 2300 4200 2300
Wire Wire Line
	4200 2300 4200 1350
Connection ~ 4200 1350
Wire Wire Line
	5900 1550 5900 1750
Wire Wire Line
	5900 2550 5900 2500
Connection ~ 5550 2100
Wire Wire Line
	6150 2850 6350 2850
Wire Wire Line
	6350 2850 6350 2200
Connection ~ 6350 2200
Wire Wire Line
	5650 2850 5700 2850
Connection ~ 5700 2850
Wire Wire Line
	5750 1750 5900 1750
Connection ~ 5900 1750
Wire Wire Line
	5400 1950 5550 1950
Wire Wire Line
	5100 1950 4350 1950
Connection ~ 4350 1950
Wire Wire Line
	8300 1750 8300 1700
Wire Wire Line
	8300 1700 8350 1700
Wire Wire Line
	8650 1700 8700 1700
Wire Wire Line
	8700 1600 8700 1700
Connection ~ 8700 1700
Wire Wire Line
	9050 1700 9200 1700
Connection ~ 9200 1700
Wire Wire Line
	6700 2500 7000 2500
Wire Wire Line
	7350 1900 7450 1900
Connection ~ 7450 1900
Wire Wire Line
	5800 5150 5650 5150
Wire Wire Line
	8700 2800 8700 2900
Wire Wire Line
	8700 2800 8500 2800
Wire Wire Line
	6300 2200 6350 2200
Wire Wire Line
	4100 2100 4800 2100
Wire Wire Line
	5700 2300 5700 2850
Wire Wire Line
	6850 3300 6850 2650
Wire Wire Line
	6700 2500 6700 2550
Wire Wire Line
	6500 3650 6500 3600
Wire Wire Line
	6500 2350 6500 3250
Connection ~ 6500 3250
Wire Wire Line
	6500 3900 6500 3850
Wire Wire Line
	6950 2850 6950 2800
Wire Wire Line
	6950 2800 7000 2800
Connection ~ 9200 1500
Wire Wire Line
	6700 3250 6700 3350
Wire Wire Line
	6700 3900 6700 3550
Connection ~ 6500 3900
Wire Wire Line
	6700 3250 6500 3250
Wire Wire Line
	6700 3900 6500 3900
Wire Wire Line
	3800 1350 3800 1400
$Comp
L FlexFox80-rescue:INDUCTOR_SMALL-ARDF-2-Band-MiniTx-rescue L201
U 1 1 5BF17B46
P 4350 1650
F 0 "L201" H 4350 1750 50  0000 C CNN
F 1 "10uH" H 4350 1600 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 4350 1650 50  0001 C CNN
F 3 "" H 4350 1650 50  0000 C CNN
F 4 "445-6396-1-ND" H 4350 1650 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=445-6396-1-ND\">Link</a>" H 4350 1650 60  0001 C CNN "Link"
	1    4350 1650
	0    1    1    0   
$EndComp
NoConn ~ 6500 5050
NoConn ~ 6500 5150
$Comp
L FlexFox80-rescue:TEST_SCOPE-ARDF-2-Band-MiniTx-rescue W204
U 1 1 5C0F212A
P 9400 1500
F 0 "W204" V 9450 1750 50  0000 C CNN
F 1 "V4" V 9450 1550 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 9600 1500 50  0001 C CNN
F 3 "" H 9600 1500 50  0001 C CNN
F 4 "np" H 9400 1500 60  0001 C CNN "Part No."
F 5 "np" H 9400 1500 60  0001 C CNN "Link"
	1    9400 1500
	0    1    1    0   
$EndComp
Wire Wire Line
	9400 1500 9200 1500
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0107
U 1 1 5C0F26F3
P 9400 1600
F 0 "#PWR0107" H 9400 1350 50  0001 C CNN
F 1 "GND_L8" H 9400 1450 50  0001 C CNN
F 2 "" H 9400 1600 50  0000 C CNN
F 3 "" H 9400 1600 50  0000 C CNN
	1    9400 1600
	0    1    1    0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0110
U 1 1 5C131D21
P 6550 7550
F 0 "#PWR0110" H 6550 7300 50  0001 C CNN
F 1 "GND_L8" H 6550 7400 50  0000 C CNN
F 2 "" H 6550 7550 50  0000 C CNN
F 3 "" H 6550 7550 50  0000 C CNN
	1    6550 7550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0111
U 1 1 5C131DD9
P 6800 7550
F 0 "#PWR0111" H 6800 7300 50  0001 C CNN
F 1 "GND" H 6800 7400 50  0000 C CNN
F 2 "" H 6800 7550 50  0000 C CNN
F 3 "" H 6800 7550 50  0000 C CNN
	1    6800 7550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6550 7550 6550 7400
Wire Wire Line
	6550 7400 6700 7400
Wire Wire Line
	6800 7400 6800 7550
$Comp
L power:PWR_FLAG #FLG0112
U 1 1 5C13203A
P 6700 7300
F 0 "#FLG0112" H 6700 7395 50  0001 C CNN
F 1 "PWR_FLAG" H 6700 7480 50  0000 C CNN
F 2 "" H 6700 7300 50  0000 C CNN
F 3 "" H 6700 7300 50  0000 C CNN
	1    6700 7300
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 7300 6700 7400
Connection ~ 6700 7400
$Comp
L FlexFox80-rescue:CONN_01X01-ARDF-2-Band-MiniTx-rescue J203
U 1 1 5C64C637
P 9400 2200
F 0 "J203" H 9400 2300 50  0000 C CNN
F 1 "V4" H 9500 2200 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01_circle_pad" H 9400 2200 50  0001 C CNN
F 3 "" H 9400 2200 50  0001 C CNN
F 4 "np" H 9400 2200 60  0001 C CNN "Part No."
F 5 "np" H 9400 2200 60  0001 C CNN "Link"
	1    9400 2200
	1    0    0    -1  
$EndComp
$Comp
L Device:R R203
U 1 1 5C6E394D
P 5050 2100
F 0 "R203" V 4950 2100 50  0000 C CNN
F 1 "27k" V 5050 2100 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 4980 2100 50  0001 C CNN
F 3 "" H 5050 2100 50  0000 C CNN
F 4 "311-27.0KCRCT-ND" H 5050 2100 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-27.0KCRCT-ND\">Link</a>" H 5050 2100 60  0001 C CNN "Link"
	1    5050 2100
	0    1    1    0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0113
U 1 1 58B811D0
P 7150 1900
F 0 "#PWR0113" H 7150 1650 50  0001 C CNN
F 1 "GND_L8" H 7150 1750 50  0001 C CNN
F 2 "" H 7150 1900 50  0000 C CNN
F 3 "" H 7150 1900 50  0000 C CNN
	1    7150 1900
	0    1    1    0   
$EndComp
$Comp
L Device:R R205
U 1 1 5C7585A8
P 5650 5650
F 0 "R205" V 5730 5650 50  0000 C CNN
F 1 "162k" V 5650 5650 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5580 5650 50  0001 C CNN
F 3 "" H 5650 5650 50  0000 C CNN
F 4 "311-162KCRCT-ND" H 5650 5650 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-162KCRCT-ND\">Link</a>" H 5650 5650 60  0001 C CNN "Link"
	1    5650 5650
	1    0    0    -1  
$EndComp
$Comp
L Device:R R213
U 1 1 5C7585B0
P 5650 6050
F 0 "R213" V 5730 6050 50  0000 C CNN
F 1 "10k" V 5650 6050 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5580 6050 50  0001 C CNN
F 3 "" H 5650 6050 50  0000 C CNN
F 4 "P12.4KCCT-ND" H 5650 6050 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=P12.4KCCT-ND\">Link</a>" H 5650 6050 60  0001 C CNN "Link"
	1    5650 6050
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0114
U 1 1 5C7585B6
P 5650 6300
F 0 "#PWR0114" H 5650 6050 50  0001 C CNN
F 1 "GND_L8" H 5650 6150 50  0000 C CNN
F 2 "" H 5650 6300 50  0000 C CNN
F 3 "" H 5650 6300 50  0000 C CNN
	1    5650 6300
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C204
U 1 1 5C7585BE
P 5400 6000
F 0 "C204" V 5500 5950 50  0000 L CNN
F 1 "100nF" V 5300 5950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 5400 6000 50  0001 C CNN
F 3 "" H 5400 6000 50  0000 C CNN
F 4 "478-1395-1-ND" H 5400 6000 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 5400 6000 60  0001 C CNN "Link"
	1    5400 6000
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0115
U 1 1 5C7585C4
P 5400 6100
F 0 "#PWR0115" H 5400 5850 50  0001 C CNN
F 1 "GND_L8" H 5400 5950 50  0000 C CNN
F 2 "" H 5400 6100 50  0000 C CNN
F 3 "" H 5400 6100 50  0000 C CNN
	1    5400 6100
	1    0    0    -1  
$EndComp
Text GLabel 4300 4500 2    39   Output ~ 0
TX_+12_VOLTAGE
Wire Wire Line
	5650 5800 5650 5850
Wire Wire Line
	5650 6300 5650 6200
Wire Wire Line
	5400 5850 5400 5900
Wire Wire Line
	5800 5850 5650 5850
Connection ~ 5650 5850
$Comp
L Device:R R211
U 1 1 5C7752BE
P 8500 1700
F 0 "R211" V 8580 1700 50  0000 C CNN
F 1 "12.4k" V 8500 1700 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8430 1700 50  0001 C CNN
F 3 "" H 8500 1700 50  0000 C CNN
F 4 "P12.4KCCT-ND" H 8500 1700 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=P12.4KCCT-ND\">Link</a>" H 8500 1700 60  0001 C CNN "Link"
	1    8500 1700
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5650 5500 5650 5150
Connection ~ 5650 5150
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0171
U 1 1 5F83A1A7
P 4100 2400
F 0 "#PWR0171" H 4100 2150 50  0001 C CNN
F 1 "GND_L8" H 4100 2250 50  0001 C CNN
F 2 "" H 4100 2400 50  0000 C CNN
F 3 "" H 4100 2400 50  0000 C CNN
	1    4100 2400
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:TEST_SCOPE-ARDF-2-Band-MiniTx-rescue W205
U 1 1 5F84AD75
P 4800 2250
F 0 "W205" H 4700 2450 50  0000 C CNN
F 1 "Vset" H 4850 2550 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 5000 2250 50  0001 C CNN
F 3 "" H 5000 2250 50  0001 C CNN
F 4 "np" H 4800 2250 60  0001 C CNN "Part No."
F 5 "np" H 4800 2250 60  0001 C CNN "Link"
	1    4800 2250
	-1   0    0    1   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR0172
U 1 1 5F84AE20
P 4700 2250
F 0 "#PWR0172" H 4700 2000 50  0001 C CNN
F 1 "GND_L8" H 4700 2100 50  0001 C CNN
F 2 "" H 4700 2250 50  0000 C CNN
F 3 "" H 4700 2250 50  0000 C CNN
	1    4700 2250
	-1   0    0    1   
$EndComp
Wire Wire Line
	4800 2250 4800 2100
Wire Wire Line
	5550 1950 5550 2100
Connection ~ 4800 2100
Wire Wire Line
	8500 2800 8500 3300
Wire Wire Line
	8300 2650 8600 2650
Wire Wire Line
	9200 2650 9200 2750
Wire Wire Line
	9200 2200 9200 2650
Wire Wire Line
	4350 1350 4350 1400
Wire Wire Line
	4350 2200 4350 2300
Wire Wire Line
	4200 1350 4350 1350
Wire Wire Line
	5550 2100 5700 2100
Wire Wire Line
	6350 2200 7000 2200
Wire Wire Line
	5700 2850 5850 2850
Wire Wire Line
	5900 1750 5900 1900
Wire Wire Line
	4350 1950 4350 2200
Wire Wire Line
	8700 1700 8750 1700
Wire Wire Line
	8700 1700 8700 1800
Wire Wire Line
	9200 1700 9200 2200
Wire Wire Line
	7450 1900 7450 1950
Wire Wire Line
	6500 3250 6500 3300
Wire Wire Line
	9200 1500 9200 1700
Wire Wire Line
	6700 7400 6800 7400
Wire Wire Line
	5650 5850 5650 5900
Wire Wire Line
	5650 5850 5400 5850
Wire Wire Line
	5650 5150 5500 5150
Wire Wire Line
	4800 2100 4900 2100
$Comp
L Power_Management:MIC2196 U?
U 1 1 60CD0D96
P 4100 6550
F 0 "U?" H 3700 6300 61  0000 C CNB
F 1 "MIC2196" H 3700 6200 61  0000 C CNB
F 2 "" H 4100 6550 50  0000 C CNN
F 3 "" H 4100 6550 50  0000 C CNN
	1    4100 6550
	-1   0    0    -1  
$EndComp
$Comp
L Transistor_FET:DMP4025LSD Q?
U 1 1 60CD34F7
P 1950 5700
F 0 "Q?" H 1400 5950 61  0000 L CNB
F 1 "DMP4025LSD" H 1300 5850 61  0000 L CNB
F 2 "SO-8" H 2238 5609 50  0001 L CIN
F 3 "" H 1950 5700 50  0000 L CNN
	1    1950 5700
	1    0    0    1   
$EndComp
$Comp
L Transistor_FET:DMP4025LSD Q?
U 2 1 60CD520C
P 3100 4650
F 0 "Q?" V 3400 4700 61  0000 L CNB
F 1 "DMP4025LSD" V 3500 4700 61  0000 L CNB
F 2 "SO-8" H 3388 4559 50  0001 L CIN
F 3 "" H 3100 4650 50  0000 L CNN
	2    3100 4650
	0    1    1    0   
$EndComp
$Comp
L Transistor_FET:SI4884BDY Q?
U 1 1 60D0C948
P 3050 5900
F 0 "Q?" V 2850 5650 61  0000 R CNB
F 1 "AO4576" V 2750 5700 61  0000 R CNB
F 2 "SO-8" H 3150 5500 50  0001 C CIN
F 3 "" V 3050 5900 50  0001 L CNN
	1    3050 5900
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C?
U 1 1 60D0FCA1
P 4350 5950
F 0 "C?" V 4250 5850 50  0000 L CNN
F 1 "47uF" V 4450 5900 50  0000 L CNN
F 2 "Capacitors_SMD:C_1210_HandSoldering" H 4350 5950 50  0001 C CNN
F 3 "" H 4350 5950 50  0000 C CNN
F 4 "1276-3373-1-ND" H 4350 5950 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-3373-1-ND\">Link</a>" H 4350 5950 60  0001 C CNN "Link"
	1    4350 5950
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C?
U 1 1 60D1101B
P 3850 5950
F 0 "C?" V 3750 5850 50  0000 L CNN
F 1 "10uF" V 3950 5900 50  0000 L CNN
F 2 "Capacitors_SMD:C_1210_HandSoldering" H 3850 5950 50  0001 C CNN
F 3 "" H 3850 5950 50  0000 C CNN
F 4 "1276-3373-1-ND" H 3850 5950 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=1276-3373-1-ND\">Link</a>" H 3850 5950 60  0001 C CNN "Link"
	1    3850 5950
	0    1    1    0   
$EndComp
$Comp
L FlexFox80-rescue:INDUCTOR_SMALL-ARDF-2-Band-MiniTx-rescue L?
U 1 1 60D12815
P 3500 5450
F 0 "L?" H 3500 5550 50  0000 C CNN
F 1 "6.8uH" H 3500 5400 50  0000 C CNN
F 2 "Inductors:CDR7D28MN" H 3500 5450 50  0001 C CNN
F 3 "http://www.bourns.com/docs/Product-Datasheets/SRP7028A.pdf" H 3500 5450 50  0001 C CNN
F 4 "308-1328-1-ND" H 3500 5450 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=308-1328-1-ND\">Link</a>" H 3500 5450 60  0001 C CNN "Link"
	1    3500 5450
	1    0    0    -1  
$EndComp
$Comp
L Transistor_Array:MUN5211DW1T1G Q?
U 1 1 60D5AF1D
P 2950 3850
F 0 "Q?" H 3200 3900 61  0000 L CNB
F 1 "MUN5211" H 3200 3800 61  0000 L CNB
F 2 "SO-8" H 2950 3850 50  0001 L CNN
F 3 "" H 2950 3850 50  0000 L CNN
	1    2950 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 60D63427
P 2850 4350
F 0 "R?" V 2930 4350 50  0000 C CNN
F 1 "100k" V 2850 4350 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 2780 4350 50  0001 C CNN
F 3 "" H 2850 4350 50  0000 C CNN
F 4 "311-100KCRCT-ND" H 2850 4350 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-100KCRCT-ND\">Link</a>" H 2850 4350 60  0001 C CNN "Link"
	1    2850 4350
	0    1    1    0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR?
U 1 1 60D6904D
P 3050 3850
F 0 "#PWR?" H 3050 3600 50  0001 C CNN
F 1 "GND_L8" H 3050 3700 50  0001 C CNN
F 2 "" H 3050 3850 50  0000 C CNN
F 3 "" H 3050 3850 50  0000 C CNN
	1    3050 3850
	1    0    0    1   
$EndComp
Wire Wire Line
	3050 4050 3050 4150
Wire Wire Line
	3000 4350 3050 4350
Connection ~ 3050 4350
Wire Wire Line
	3050 4350 3050 4450
Wire Wire Line
	2900 4750 2600 4750
Wire Wire Line
	2600 4750 2600 4350
Wire Wire Line
	2600 4350 2700 4350
$Comp
L Device:C_Small C?
U 1 1 60D7C59C
P 3250 4350
F 0 "C?" V 3350 4300 50  0000 L CNN
F 1 "100nF" V 3150 4300 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 3250 4350 50  0001 C CNN
F 3 "" H 3250 4350 50  0000 C CNN
F 4 "478-1395-1-ND" H 3250 4350 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 3250 4350 60  0001 C CNN "Link"
	1    3250 4350
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR?
U 1 1 60D7E90E
P 3350 4350
F 0 "#PWR?" H 3350 4100 50  0001 C CNN
F 1 "GND_L8" H 3350 4200 50  0001 C CNN
F 2 "" H 3350 4350 50  0000 C CNN
F 3 "" H 3350 4350 50  0000 C CNN
	1    3350 4350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3150 4350 3050 4350
Wire Wire Line
	3300 4750 3400 4750
Wire Wire Line
	3400 4750 3400 4850
Wire Wire Line
	3400 4850 3300 4850
Wire Wire Line
	2850 5600 2850 5450
Wire Wire Line
	2850 5450 2950 5450
Wire Wire Line
	2950 5600 2950 5450
Connection ~ 2950 5450
Wire Wire Line
	2950 5450 3050 5450
Wire Wire Line
	3050 5600 3050 5450
Connection ~ 3050 5450
Wire Wire Line
	3050 5450 3150 5450
Wire Wire Line
	3150 5450 3150 5600
Connection ~ 3150 5450
Wire Wire Line
	3150 5450 3250 5450
Wire Wire Line
	2850 6200 2850 6300
Wire Wire Line
	2850 6300 2950 6300
Wire Wire Line
	3050 6300 3050 6200
Wire Wire Line
	2950 6200 2950 6300
Connection ~ 2950 6300
Wire Wire Line
	2950 6300 3050 6300
Wire Wire Line
	1750 5650 1550 5650
Wire Wire Line
	1550 5650 1550 4150
Wire Wire Line
	1550 4150 3050 4150
Connection ~ 3050 4150
Wire Wire Line
	3050 4150 3050 4350
Wire Wire Line
	2050 5500 2050 5450
Wire Wire Line
	2050 4750 2600 4750
Connection ~ 2600 4750
Connection ~ 2850 5450
Wire Wire Line
	2400 5450 2050 5450
Connection ~ 2050 5450
Wire Wire Line
	2050 5450 2050 4950
Wire Wire Line
	4100 6150 4100 5950
Wire Wire Line
	4100 5450 3750 5450
Wire Wire Line
	3950 5950 4100 5950
Connection ~ 4100 5950
Wire Wire Line
	4100 5950 4100 5450
Wire Wire Line
	4250 5950 4100 5950
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR?
U 1 1 60E200F4
P 4450 5950
F 0 "#PWR?" H 4450 5700 50  0001 C CNN
F 1 "GND_L8" H 4450 5800 50  0001 C CNN
F 2 "" H 4450 5950 50  0000 C CNN
F 3 "" H 4450 5950 50  0000 C CNN
	1    4450 5950
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR?
U 1 1 60E20F37
P 3750 5950
F 0 "#PWR?" H 3750 5700 50  0001 C CNN
F 1 "GND_L8" H 3750 5800 50  0001 C CNN
F 2 "" H 3750 5950 50  0000 C CNN
F 3 "" H 3750 5950 50  0000 C CNN
	1    3750 5950
	0    1    -1   0   
$EndComp
$Comp
L Device:R R?
U 1 1 60E2433B
P 2150 6200
F 0 "R?" V 2230 6200 50  0000 C CNN
F 1 "10k" V 2150 6200 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 2080 6200 50  0001 C CNN
F 3 "" H 2150 6200 50  0000 C CNN
F 4 "311-27.0KCRCT-ND" H 2150 6200 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-27.0KCRCT-ND\">Link</a>" H 2150 6200 60  0001 C CNN "Link"
	1    2150 6200
	-1   0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 60E2697C
P 2150 6800
F 0 "R?" V 2230 6800 50  0000 C CNN
F 1 "1.1k" V 2150 6800 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 2080 6800 50  0001 C CNN
F 3 "" H 2150 6800 50  0000 C CNN
F 4 "311-27.0KCRCT-ND" H 2150 6800 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-27.0KCRCT-ND\">Link</a>" H 2150 6800 60  0001 C CNN "Link"
	1    2150 6800
	-1   0    0    1   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR?
U 1 1 60E276BC
P 2150 6950
F 0 "#PWR?" H 2150 6700 50  0001 C CNN
F 1 "GND_L8" H 2150 6800 50  0001 C CNN
F 2 "" H 2150 6950 50  0000 C CNN
F 3 "" H 2150 6950 50  0000 C CNN
	1    2150 6950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 6400 3150 6400
Wire Wire Line
	3150 6400 3150 6200
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR?
U 1 1 60E3B67E
P 4100 6950
F 0 "#PWR?" H 4100 6700 50  0001 C CNN
F 1 "GND_L8" H 4100 6800 50  0001 C CNN
F 2 "" H 4100 6950 50  0000 C CNN
F 3 "" H 4100 6950 50  0000 C CNN
	1    4100 6950
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 60E3C6D7
P 3550 6900
F 0 "C?" H 3350 7000 50  0000 L CNN
F 1 "1uF" H 3400 6800 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 3550 6900 50  0001 C CNN
F 3 "" H 3550 6900 50  0000 C CNN
F 4 "311-1365-1-ND" H 3550 6900 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-1365-1-ND\">Link</a>" H 3550 6900 60  0001 C CNN "Link"
	1    3550 6900
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 6800 3550 6700
Wire Wire Line
	3550 6700 3650 6700
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR?
U 1 1 60E4844E
P 3550 7000
F 0 "#PWR?" H 3550 6750 50  0001 C CNN
F 1 "GND_L8" H 3550 6850 50  0001 C CNN
F 2 "" H 3550 7000 50  0000 C CNN
F 3 "" H 3550 7000 50  0000 C CNN
	1    3550 7000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 5900 2150 5950
Wire Wire Line
	2050 5900 2050 5950
Wire Wire Line
	2050 5950 2150 5950
Connection ~ 2150 5950
Wire Wire Line
	2150 5950 2150 6050
Wire Wire Line
	2150 6650 2150 6500
Connection ~ 2150 6500
Wire Wire Line
	2150 6500 2150 6350
$Comp
L Device:R R?
U 1 1 60E711E7
P 2950 6850
F 0 "R?" V 3030 6850 50  0000 C CNN
F 1 "0.01 0.5W" V 2850 6850 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 2880 6850 50  0001 C CNN
F 3 "" H 2950 6850 50  0000 C CNN
F 4 "311-27.0KCRCT-ND" H 2950 6850 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-27.0KCRCT-ND\">Link</a>" H 2950 6850 60  0001 C CNN "Link"
	1    2950 6850
	-1   0    0    1   
$EndComp
Wire Wire Line
	3650 6600 2950 6600
Wire Wire Line
	2950 6600 2950 6300
Wire Wire Line
	2950 6700 2950 6600
Connection ~ 2950 6600
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR?
U 1 1 60E8B864
P 2950 7000
F 0 "#PWR?" H 2950 6750 50  0001 C CNN
F 1 "GND_L8" H 2950 6850 50  0001 C CNN
F 2 "" H 2950 7000 50  0000 C CNN
F 3 "" H 2950 7000 50  0000 C CNN
	1    2950 7000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2150 6500 3650 6500
$Comp
L Device:CP1_Small C?
U 1 1 60EA3D71
P 1850 4750
F 0 "C?" V 1600 4650 50  0000 L CNN
F 1 "150uF low ESR" V 1700 4350 50  0000 L CNN
F 2 "Capacitors_SMD:C_2917_7343_Metric_HandSoldering" H 1850 4750 50  0001 C CNN
F 3 "https://content.kemet.com/datasheets/KEM_T2009_T495.pdf" H 1850 4750 50  0001 C CNN
F 4 "399-3885-1-ND" H 1850 4750 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-3885-1-ND\">Link</a>" H 1850 4750 60  0001 C CNN "Link"
	1    1850 4750
	0    1    1    0   
$EndComp
$Comp
L Device:CP1_Small C?
U 1 1 60EA8041
P 1850 4950
F 0 "C?" V 2000 4850 50  0000 L CNN
F 1 "150uF low ESR" V 2100 4550 50  0000 L CNN
F 2 "Capacitors_SMD:C_2917_7343_Metric_HandSoldering" H 1850 4950 50  0001 C CNN
F 3 "https://content.kemet.com/datasheets/KEM_T2009_T495.pdf" H 1850 4950 50  0001 C CNN
F 4 "399-3885-1-ND" H 1850 4950 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=399-3885-1-ND\">Link</a>" H 1850 4950 60  0001 C CNN "Link"
	1    1850 4950
	0    1    1    0   
$EndComp
Wire Wire Line
	1950 4750 2050 4750
Connection ~ 2050 4750
Wire Wire Line
	1950 4950 2050 4950
Connection ~ 2050 4950
Wire Wire Line
	2050 4950 2050 4750
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR?
U 1 1 60EBE635
P 1750 4750
F 0 "#PWR?" H 1750 4500 50  0001 C CNN
F 1 "GND_L8" H 1750 4600 50  0001 C CNN
F 2 "" H 1750 4750 50  0000 C CNN
F 3 "" H 1750 4750 50  0000 C CNN
	1    1750 4750
	0    1    1    0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR?
U 1 1 60EC02B8
P 1750 4950
F 0 "#PWR?" H 1750 4700 50  0001 C CNN
F 1 "GND_L8" H 1750 4800 50  0001 C CNN
F 2 "" H 1750 4950 50  0000 C CNN
F 3 "" H 1750 4950 50  0000 C CNN
	1    1750 4950
	0    1    1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 60EC4929
P 4700 6700
F 0 "R?" V 4780 6700 50  0000 C CNN
F 1 "10k" V 4700 6700 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 4630 6700 50  0001 C CNN
F 3 "" H 4700 6700 50  0000 C CNN
F 4 "311-100KCRCT-ND" H 4700 6700 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-100KCRCT-ND\">Link</a>" H 4700 6700 60  0001 C CNN "Link"
	1    4700 6700
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C?
U 1 1 60EC599D
P 4950 6700
F 0 "C?" V 5050 6600 50  0000 L CNN
F 1 "10nF" V 4850 6600 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4950 6700 50  0001 C CNN
F 3 "" H 4950 6700 50  0000 C CNN
F 4 "490-5523-1-ND" H 4950 6700 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=490-5523-1-ND\">Link</a>" H 4950 6700 60  0001 C CNN "Link"
	1    4950 6700
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR?
U 1 1 60EC6C77
P 5050 6700
F 0 "#PWR?" H 5050 6450 50  0001 C CNN
F 1 "GND_L8" H 5050 6550 50  0001 C CNN
F 2 "" H 5050 6700 50  0000 C CNN
F 3 "" H 5050 6700 50  0000 C CNN
	1    5050 6700
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R?
U 1 1 60EC84A1
P 4850 6350
F 0 "R?" V 4930 6350 50  0000 C CNN
F 1 "10k" V 4850 6350 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 4780 6350 50  0001 C CNN
F 3 "" H 4850 6350 50  0000 C CNN
F 4 "311-100KCRCT-ND" H 4850 6350 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-100KCRCT-ND\">Link</a>" H 4850 6350 60  0001 C CNN "Link"
	1    4850 6350
	0    1    1    0   
$EndComp
Wire Wire Line
	4550 6550 4650 6550
Wire Wire Line
	4650 6550 4650 6350
Wire Wire Line
	4650 6350 4700 6350
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR?
U 1 1 60ED569F
P 5000 6350
F 0 "#PWR?" H 5000 6100 50  0001 C CNN
F 1 "GND_L8" H 5000 6200 50  0001 C CNN
F 2 "" H 5000 6350 50  0000 C CNN
F 3 "" H 5000 6350 50  0000 C CNN
	1    5000 6350
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR?
U 1 1 60ED6DBC
P 3050 3650
F 0 "#PWR?" H 3050 3400 50  0001 C CNN
F 1 "GND_L8" H 3050 3500 50  0001 C CNN
F 2 "" H 3050 3650 50  0000 C CNN
F 3 "" H 3050 3650 50  0000 C CNN
	1    3050 3650
	1    0    0    1   
$EndComp
Text GLabel 4250 5450 2    39   Output ~ 0
EXTERNAL_3.2V-12V_SUPPLY
Wire Wire Line
	4250 5450 4100 5450
Connection ~ 4100 5450
$Comp
L power:+12V #PWR?
U 1 1 60F00CB3
P 3650 4650
F 0 "#PWR?" H 3650 4500 50  0001 C CNN
F 1 "+12V" H 3650 4790 50  0000 C CNN
F 2 "" H 3650 4650 50  0000 C CNN
F 3 "" H 3650 4650 50  0000 C CNN
	1    3650 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 4650 3650 4750
Wire Wire Line
	3650 4750 3400 4750
Connection ~ 3400 4750
Text Notes 950  3500 0    118  ~ 24
High-Efficiency Boost Power Regulator
Wire Wire Line
	5200 5150 4100 5150
Wire Wire Line
	4100 5150 4100 5450
$Comp
L Device:R R201
U 1 1 58B957B6
P 4050 4750
F 0 "R201" V 4130 4750 50  0000 C CNN
F 1 "162k" V 4050 4750 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 3980 4750 50  0001 C CNN
F 3 "" H 4050 4750 50  0000 C CNN
F 4 "311-162KCRCT-ND" H 4050 4750 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-162KCRCT-ND\">Link</a>" H 4050 4750 60  0001 C CNN "Link"
	1    4050 4750
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R202
U 1 1 58B957BC
P 4450 4750
F 0 "R202" V 4530 4750 50  0000 C CNN
F 1 "49.9k" V 4450 4750 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 4380 4750 50  0001 C CNN
F 3 "" H 4450 4750 50  0000 C CNN
F 4 "RMCF0805FT49K9CT-ND" H 4450 4750 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=RMCF0805FT49K9CT-ND\">Link</a>" H 4450 4750 60  0001 C CNN "Link"
	1    4450 4750
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR093
U 1 1 58B957C2
P 4700 4750
F 0 "#PWR093" H 4700 4500 50  0001 C CNN
F 1 "GND_L8" H 4700 4600 50  0000 C CNN
F 2 "" H 4700 4750 50  0000 C CNN
F 3 "" H 4700 4750 50  0000 C CNN
	1    4700 4750
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C202
U 1 1 58B957C8
P 4400 5000
F 0 "C202" V 4500 4950 50  0000 L CNN
F 1 "100nF" V 4300 4950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4400 5000 50  0001 C CNN
F 3 "" H 4400 5000 50  0000 C CNN
F 4 "478-1395-1-ND" H 4400 5000 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 4400 5000 60  0001 C CNN "Link"
	1    4400 5000
	0    -1   -1   0   
$EndComp
$Comp
L FlexFox80-rescue:GND_L8-ARDF-2-Band-MiniTx-rescue #PWR094
U 1 1 58B957CE
P 4500 5000
F 0 "#PWR094" H 4500 4750 50  0001 C CNN
F 1 "GND_L8" H 4500 4850 50  0000 C CNN
F 2 "" H 4500 5000 50  0000 C CNN
F 3 "" H 4500 5000 50  0000 C CNN
	1    4500 5000
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4200 4750 4250 4750
Wire Wire Line
	4700 4750 4600 4750
Wire Wire Line
	4300 5000 4250 5000
Wire Wire Line
	4250 5000 4250 4750
Connection ~ 4250 4750
Wire Wire Line
	4250 4750 4300 4750
Wire Wire Line
	3650 4750 3900 4750
Text GLabel 5800 5850 2    39   Output ~ 0
TX_BATTERY_VOLTAGE
Connection ~ 3650 4750
Wire Wire Line
	4300 4500 4250 4500
Wire Wire Line
	4250 4500 4250 4750
Text GLabel 4700 6150 2    39   Input ~ 0
BOOST_ENABLE
Wire Wire Line
	4700 6150 4650 6150
Wire Wire Line
	4650 6150 4650 6350
Connection ~ 4650 6350
Text Notes 3550 5800 0    39   ~ 0
Close to pin 6
Text Notes 1300 6600 0    39   ~ 0
+11.5V => 15k & 1.8k\n+12.5V => 10k & 1.1k\n+12.8V => 14k & 1.5k\n+13.6V => 15k & 1.5k
Text GLabel 2700 3850 0    39   Input ~ 0
BOOST_ENABLE
$Comp
L Device:D_Schottky_ALT D?
U 1 1 607D94BA
P 2550 5450
F 0 "D?" H 2550 5667 50  0000 C CNN
F 1 "B530" H 2550 5576 50  0000 C CNN
F 2 "Diodes_SMD:DO-214AB_Handsoldering" H 2550 5450 50  0001 C CNN
F 3 "" H 2550 5450 50  0001 C CNN
F 4 "B530C-FDICT-ND" H 2550 5450 50  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=B530C-FDICT-ND\">Link</a>" H 2550 5450 50  0001 C CNN "Link"
F 6 "Diodes Incorporated" H 2550 5450 50  0001 C CNN "Manufacturer"
F 7 "B530C-13-F" H 2550 5450 50  0001 C CNN "Manufacturer PN"
F 8 "DIODE SCHOTTKY 30V 5A SMC" H 2550 5450 50  0001 C CNN "Description"
	1    2550 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 5450 2850 5450
$Comp
L Device:D_Schottky_ALT D?
U 1 1 6086B1F1
P 5350 5150
F 0 "D?" H 5350 5367 50  0000 C CNN
F 1 "B530" H 5350 5276 50  0000 C CNN
F 2 "Diodes_SMD:DO-214AB_Handsoldering" H 5350 5150 50  0001 C CNN
F 3 "" H 5350 5150 50  0001 C CNN
F 4 "B530C-FDICT-ND" H 5350 5150 50  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=B530C-FDICT-ND\">Link</a>" H 5350 5150 50  0001 C CNN "Link"
F 6 "Diodes Incorporated" H 5350 5150 50  0001 C CNN "Manufacturer"
F 7 "B530C-13-F" H 5350 5150 50  0001 C CNN "Manufacturer PN"
F 8 "DIODE SCHOTTKY 30V 5A SMC" H 5350 5150 50  0001 C CNN "Description"
	1    5350 5150
	1    0    0    -1  
$EndComp
$Comp
L FlexFox80-rescue:CONN_01X02-ARDF-2-Band-MiniTx-rescue P?
U 1 1 607F80D4
P 6300 5100
F 0 "P?" H 6300 5250 50  0000 C CNN
F 1 "Power Pole" V 6450 5100 50  0000 C CNN
F 2 "" H 6300 5100 50  0001 C CNN
F 3 "" H 6300 5100 50  0001 C CNN
F 4 "2243-ASMPP30-1X2-RK-ND" H 6300 5100 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=2243-ASMPP30-1X2-RK-ND\">Link</a>" H 6300 5100 60  0001 C CNN "Link"
F 6 "TH" H 6300 5100 50  0001 C CNN "Manufacturer"
F 7 "TH" H 6300 5100 50  0001 C CNN "Manufacturer PN"
F 8 "TH" H 6300 5100 50  0001 C CNN "Description"
	1    6300 5100
	-1   0    0    -1  
$EndComp
$EndSCHEMATC
