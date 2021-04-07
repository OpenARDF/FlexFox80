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
L ARDF-2-Band-MiniTx-rescue:LTC3600 U203
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
Text Notes 7400 6950 0    118  ~ 24
PCB: Dual-Band Transmitter (p. 2/3)
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
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR082
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
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR083
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
L ARDF-2-Band-MiniTx-rescue:INDUCTOR_SMALL L202
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
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR084
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
L ARDF-2-Band-MiniTx-rescue:DAC081C085 U201
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
L ARDF-2-Band-MiniTx-rescue:+3V3_L8 #PWR085
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
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR086
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
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR087
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
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR088
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
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR089
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
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR090
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
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR091
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
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR092
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
$Comp
L Device:R R201
U 1 1 58B957B6
P 3700 6550
F 0 "R201" V 3780 6550 50  0000 C CNN
F 1 "162k" V 3700 6550 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 3630 6550 50  0001 C CNN
F 3 "" H 3700 6550 50  0000 C CNN
F 4 "311-162KCRCT-ND" H 3700 6550 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-162KCRCT-ND\">Link</a>" H 3700 6550 60  0001 C CNN "Link"
	1    3700 6550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R202
U 1 1 58B957BC
P 3700 6950
F 0 "R202" V 3780 6950 50  0000 C CNN
F 1 "49.9k" V 3700 6950 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 3630 6950 50  0001 C CNN
F 3 "" H 3700 6950 50  0000 C CNN
F 4 "RMCF0805FT49K9CT-ND" H 3700 6950 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=RMCF0805FT49K9CT-ND\">Link</a>" H 3700 6950 60  0001 C CNN "Link"
	1    3700 6950
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR093
U 1 1 58B957C2
P 3700 7200
F 0 "#PWR093" H 3700 6950 50  0001 C CNN
F 1 "GND_L8" H 3700 7050 50  0000 C CNN
F 2 "" H 3700 7200 50  0000 C CNN
F 3 "" H 3700 7200 50  0000 C CNN
	1    3700 7200
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C202
U 1 1 58B957C8
P 3450 6900
F 0 "C202" V 3550 6850 50  0000 L CNN
F 1 "100nF" V 3350 6850 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 3450 6900 50  0001 C CNN
F 3 "" H 3450 6900 50  0000 C CNN
F 4 "478-1395-1-ND" H 3450 6900 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 3450 6900 60  0001 C CNN "Link"
	1    3450 6900
	1    0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR094
U 1 1 58B957CE
P 3450 7000
F 0 "#PWR094" H 3450 6750 50  0001 C CNN
F 1 "GND_L8" H 3450 6850 50  0000 C CNN
F 2 "" H 3450 7000 50  0000 C CNN
F 3 "" H 3450 7000 50  0000 C CNN
	1    3450 7000
	1    0    0    -1  
$EndComp
Text GLabel 3850 6750 2    39   Output ~ 0
TX_BATTERY_VOLTAGE
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
L power:+12V #PWR096
U 1 1 58BBB432
P 3900 4450
F 0 "#PWR096" H 3900 4300 50  0001 C CNN
F 1 "+12V" H 3900 4590 50  0000 C CNN
F 2 "" H 3900 4450 50  0000 C CNN
F 3 "" H 3900 4450 50  0000 C CNN
	1    3900 4450
	1    0    0    -1  
$EndComp
$Comp
L Device:D_Schottky D201
U 1 1 58BBD5FE
P 3400 4900
AR Path="/591BC960/58BBD5FE" Ref="D201"  Part="1" 
AR Path="/58BBD5FE" Ref="D201"  Part="1" 
F 0 "D201" H 3400 5000 50  0000 C CNN
F 1 "B530" H 3400 4800 50  0000 C CNN
F 2 "Diodes_SMD:DO-214AB_Handsoldering" H 3400 5000 50  0001 C CNN
F 3 "" H 3400 4900 50  0000 C CNN
F 4 "B530C-FDICT-ND" H 3400 4900 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=B530C-FDICT-ND\">Link</a>" H 3400 4900 60  0001 C CNN "Link"
	1    3400 4900
	-1   0    0    -1  
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
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR097
U 1 1 58C44803
P 6700 2850
F 0 "#PWR097" H 6700 2600 50  0001 C CNN
F 1 "GND_L8" H 6700 2700 50  0001 C CNN
F 2 "" H 6700 2850 50  0000 C CNN
F 3 "" H 6700 2850 50  0000 C CNN
	1    6700 2850
	1    0    0    -1  
$EndComp
Text GLabel 3900 5850 2    39   Output ~ 0
VBAT_UNINTERRUPTED
$Comp
L power:PWR_FLAG #FLG098
U 1 1 59158B1C
P 3800 5850
F 0 "#FLG098" H 3800 5945 50  0001 C CNN
F 1 "PWR_FLAG" H 3800 6030 50  0000 C CNN
F 2 "" H 3800 5850 50  0000 C CNN
F 3 "" H 3800 5850 50  0000 C CNN
	1    3800 5850
	1    0    0    -1  
$EndComp
Text Notes 2250 4900 2    79   ~ 16
Auxiliary Battery\nConnector
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR099
U 1 1 5976D077
P 3050 5950
F 0 "#PWR099" H 3050 5700 50  0001 C CNN
F 1 "GND_L8" H 3050 5800 50  0001 C CNN
F 2 "" H 3050 5950 50  0000 C CNN
F 3 "" H 3050 5950 50  0000 C CNN
	1    3050 5950
	0    -1   -1   0   
$EndComp
Text Notes 5100 1200 0    118  ~ 24
High-Efficiency Buck Power Regulator
Text GLabel 3100 5750 2    39   Input ~ 0
MAIN_PWR_ENABLE
$Comp
L ARDF-2-Band-MiniTx-rescue:OPA171 U202
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
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR0100
U 1 1 59D6B4C4
P 3050 4800
F 0 "#PWR0100" H 3050 4550 50  0001 C CNN
F 1 "GND_L8" H 3050 4650 50  0001 C CNN
F 2 "" H 3050 4800 50  0000 C CNN
F 3 "" H 3050 4800 50  0000 C CNN
	1    3050 4800
	0    -1   1    0   
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
L ARDF-2-Band-MiniTx-rescue:CONN_01X02 P203
U 1 1 59FCFE1A
P 2850 4850
F 0 "P203" H 2850 5000 50  0000 C CNN
F 1 "AUX BATTERY" V 3000 4850 50  0000 C CNN
F 2 "Sockets:Phoenix_3.81mm_2pos" H 2850 4850 50  0001 C CNN
F 3 "" H 2850 4850 50  0001 C CNN
F 4 "277-1206-ND" H 2850 4850 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=277-1206-ND\">Link</a>" H 2850 4850 60  0001 C CNN "Link"
	1    2850 4850
	-1   0    0    -1  
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR0101
U 1 1 5A747983
P 8100 2800
F 0 "#PWR0101" H 8100 2550 50  0001 C CNN
F 1 "GND_L8" H 8100 2650 50  0001 C CNN
F 2 "" H 8100 2800 50  0000 C CNN
F 3 "" H 8100 2800 50  0000 C CNN
	1    8100 2800
	0    -1   1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:+2to+12V #PWR0102
U 1 1 5A74F9FF
P 9200 1400
F 0 "#PWR0102" H 9200 1250 50  0001 C CNN
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
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR0103
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
L power:PWR_FLAG #FLG0104
U 1 1 5A8A9F75
P 3800 4500
F 0 "#FLG0104" H 3800 4595 50  0001 C CNN
F 1 "PWR_FLAG" H 3800 4680 50  0000 C CNN
F 2 "" H 3800 4500 50  0000 C CNN
F 3 "" H 3800 4500 50  0000 C CNN
	1    3800 4500
	0    -1   -1   0   
$EndComp
$Comp
L power:+12V #PWR0105
U 1 1 5A8ADFD2
P 5900 1550
F 0 "#PWR0105" H 5900 1400 50  0001 C CNN
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
	8500 2350 8500 3300
Wire Wire Line
	8500 3300 6850 3300
Connection ~ 8500 2800
Wire Wire Line
	8100 2500 8100 2450
Wire Wire Line
	8100 2450 8300 2450
Wire Wire Line
	8100 2650 8600 2650
Connection ~ 8300 2650
Wire Wire Line
	9200 1400 9200 2750
Wire Wire Line
	9200 2650 9100 2650
Wire Wire Line
	7450 1700 7450 1950
Wire Wire Line
	8100 2200 9200 2200
Connection ~ 9200 2650
Connection ~ 9200 2200
Wire Wire Line
	3200 2300 3150 2300
Wire Wire Line
	3200 2400 3150 2400
Wire Wire Line
	4350 1250 4350 1400
Wire Wire Line
	4100 1350 4350 1350
Connection ~ 4350 1350
Wire Wire Line
	5200 2100 5700 2100
Wire Wire Line
	3800 1350 3900 1350
Wire Wire Line
	4350 1900 4350 2300
Wire Wire Line
	4350 2200 4100 2200
Connection ~ 4350 2200
Wire Wire Line
	4100 2300 4200 2300
Wire Wire Line
	4200 2300 4200 1350
Connection ~ 4200 1350
Wire Wire Line
	5900 1550 5900 1900
Wire Wire Line
	5900 2550 5900 2500
Connection ~ 5550 2100
Wire Wire Line
	6150 2850 6350 2850
Wire Wire Line
	6350 2850 6350 2200
Connection ~ 6350 2200
Wire Wire Line
	5650 2850 5850 2850
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
	8650 1700 8750 1700
Wire Wire Line
	8700 1600 8700 1800
Connection ~ 8700 1700
Wire Wire Line
	9050 1700 9200 1700
Connection ~ 9200 1700
Wire Wire Line
	3700 6700 3700 6800
Wire Wire Line
	3700 7200 3700 7100
Wire Wire Line
	3450 6800 3450 6750
Wire Wire Line
	3450 6750 3850 6750
Connection ~ 3700 6750
Wire Wire Line
	6700 2500 7000 2500
Wire Wire Line
	7350 1900 7450 1900
Connection ~ 7450 1900
Wire Wire Line
	3050 4900 3250 4900
Wire Wire Line
	8700 2800 8700 2900
Wire Wire Line
	3700 5850 3700 6400
Wire Wire Line
	3050 5850 3900 5850
Wire Wire Line
	8700 2800 8500 2800
Wire Wire Line
	6300 2200 7000 2200
Wire Wire Line
	4100 2100 4900 2100
Wire Wire Line
	5700 2300 5700 2850
Wire Wire Line
	6850 3300 6850 2650
Wire Wire Line
	6700 2500 6700 2550
Wire Wire Line
	6500 3650 6500 3600
Wire Wire Line
	6500 2350 6500 3300
Connection ~ 6500 3250
Wire Wire Line
	6500 3900 6500 3850
Wire Wire Line
	6950 2850 6950 2800
Wire Wire Line
	6950 2800 7000 2800
$Comp
L ARDF-2-Band-MiniTx-rescue:CONN_02X06 P202
U 1 1 5AAED373
P 2800 5700
F 0 "P202" H 2800 6050 50  0000 C CNN
F 1 "To Power Supply Board" V 3150 5750 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x06" H 2800 4500 50  0001 C CNN
F 3 "" H 2800 4500 50  0001 C CNN
F 4 "SAM1062-06-ND" H 2800 5700 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=SAM1062-06-ND\">Link</a>" H 2800 5700 60  0001 C CNN "Link"
	1    2800 5700
	-1   0    0    1   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR0106
U 1 1 5AAEF184
P 2550 5950
F 0 "#PWR0106" H 2550 5700 50  0001 C CNN
F 1 "GND_L8" H 2550 5800 50  0001 C CNN
F 2 "" H 2550 5950 50  0000 C CNN
F 3 "" H 2550 5950 50  0000 C CNN
	1    2550 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 5450 2550 5500
Wire Wire Line
	2550 5500 3050 5500
Wire Wire Line
	3050 5500 3050 5450
Wire Wire Line
	2550 5550 2550 5600
Wire Wire Line
	2550 5600 3050 5600
Wire Wire Line
	3050 5600 3050 5550
Wire Wire Line
	2550 5750 2550 5800
Wire Wire Line
	2550 5800 3050 5800
Wire Wire Line
	3050 5800 3050 5750
Wire Wire Line
	2550 5850 2550 5900
Wire Wire Line
	2550 5900 3050 5900
Wire Wire Line
	3050 5900 3050 5850
Wire Wire Line
	3050 5750 3100 5750
Connection ~ 3050 5450
Connection ~ 3050 5550
Connection ~ 3050 5750
Connection ~ 3050 5850
Connection ~ 9200 1500
Wire Wire Line
	3050 5450 4300 5450
Wire Wire Line
	3800 4500 3900 4500
Wire Wire Line
	3900 4450 3900 5450
Connection ~ 3900 4500
Wire Wire Line
	3550 4900 3900 4900
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
L ARDF-2-Band-MiniTx-rescue:INDUCTOR_SMALL L201
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
$Comp
L ARDF-2-Band-MiniTx-rescue:CONN_01X02 P201
U 1 1 5BF55297
P 2550 4850
F 0 "P201" H 2550 5000 50  0000 C CNN
F 1 "AUX BATTERY" V 2700 4850 50  0001 C CNN
F 2 "Oddities:Dummy_Empty_2" H 2550 4850 50  0001 C CNN
F 3 "" H 2550 4850 50  0001 C CNN
F 4 "277-1161-ND" H 2550 4850 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=277-1161-ND\">Link</a>" H 2550 4850 60  0001 C CNN "Link"
	1    2550 4850
	1    0    0    -1  
$EndComp
NoConn ~ 2350 4800
NoConn ~ 2350 4900
Wire Wire Line
	2550 5650 2550 5700
Wire Wire Line
	2550 5700 3050 5700
Wire Wire Line
	3050 5700 3050 5650
$Comp
L ARDF-2-Band-MiniTx-rescue:TEST_1P W201
U 1 1 5C09F516
P 3050 5550
F 0 "W201" V 2950 5800 50  0000 C CNN
F 1 "Vin" V 3050 5800 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_Round-SMD-Pad_Small" H 3250 5550 50  0001 C CNN
F 3 "" H 3250 5550 50  0001 C CNN
F 4 "np" H 3050 5550 60  0001 C CNN "Part No."
F 5 "np" H 3050 5550 60  0001 C CNN "Link"
	1    3050 5550
	0    1    -1   0   
$EndComp
Connection ~ 3700 6150
Connection ~ 3700 5850
Connection ~ 3800 5850
Text Notes 2300 5750 2    79   ~ 16
Power Supply Board\nConnector
$Comp
L ARDF-2-Band-MiniTx-rescue:TEST_SCOPE W204
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
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR0107
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
L ARDF-2-Band-MiniTx-rescue:TEST_SCOPE W203
U 1 1 5C0F3BFB
P 4050 4600
F 0 "W203" V 4000 4850 50  0000 C CNN
F 1 "+12V" V 4100 4900 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 4250 4600 50  0001 C CNN
F 3 "" H 4250 4600 50  0001 C CNN
F 4 "np" H 4050 4600 60  0001 C CNN "Part No."
F 5 "np" H 4050 4600 60  0001 C CNN "Link"
	1    4050 4600
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR0108
U 1 1 5C0F3C01
P 4050 4700
F 0 "#PWR0108" H 4050 4450 50  0001 C CNN
F 1 "GND_L8" H 4050 4550 50  0001 C CNN
F 2 "" H 4050 4700 50  0000 C CNN
F 3 "" H 4050 4700 50  0000 C CNN
	1    4050 4700
	0    1    1    0   
$EndComp
Wire Wire Line
	4050 4600 3900 4600
Connection ~ 3900 4600
$Comp
L ARDF-2-Band-MiniTx-rescue:TEST_SCOPE W202
U 1 1 5C0F5E5F
P 3850 6150
F 0 "W202" V 3900 6500 50  0000 C CNN
F 1 "Vbat" V 3750 6200 50  0000 C CNN
F 2 "Measurement_Points:Measurement_Point_ScopeProbe_Round-SMD-2sides-Pad_Small" H 4050 6150 50  0001 C CNN
F 3 "" H 4050 6150 50  0001 C CNN
F 4 "np" H 3850 6150 60  0001 C CNN "Part No."
F 5 "np" H 3850 6150 60  0001 C CNN "Link"
	1    3850 6150
	0    1    1    0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR0109
U 1 1 5C0F5E65
P 3850 6250
F 0 "#PWR0109" H 3850 6000 50  0001 C CNN
F 1 "GND_L8" H 3850 6100 50  0001 C CNN
F 2 "" H 3850 6250 50  0000 C CNN
F 3 "" H 3850 6250 50  0000 C CNN
	1    3850 6250
	0    1    1    0   
$EndComp
Wire Wire Line
	3850 6150 3700 6150
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR0110
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
	6550 7400 6800 7400
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
L ARDF-2-Band-MiniTx-rescue:CONN_01X01 J203
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
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR0113
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
P 4450 5450
F 0 "R205" V 4530 5450 50  0000 C CNN
F 1 "162k" V 4450 5450 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 4380 5450 50  0001 C CNN
F 3 "" H 4450 5450 50  0000 C CNN
F 4 "311-162KCRCT-ND" H 4450 5450 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=311-162KCRCT-ND\">Link</a>" H 4450 5450 60  0001 C CNN "Link"
	1    4450 5450
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R213
U 1 1 5C7585B0
P 4850 5450
F 0 "R213" V 4930 5450 50  0000 C CNN
F 1 "12.4k" V 4850 5450 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 4780 5450 50  0001 C CNN
F 3 "" H 4850 5450 50  0000 C CNN
F 4 "P12.4KCCT-ND" H 4850 5450 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=P12.4KCCT-ND\">Link</a>" H 4850 5450 60  0001 C CNN "Link"
	1    4850 5450
	0    -1   -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR0114
U 1 1 5C7585B6
P 5100 5450
F 0 "#PWR0114" H 5100 5200 50  0001 C CNN
F 1 "GND_L8" H 5100 5300 50  0000 C CNN
F 2 "" H 5100 5450 50  0000 C CNN
F 3 "" H 5100 5450 50  0000 C CNN
	1    5100 5450
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C204
U 1 1 5C7585BE
P 4800 5700
F 0 "C204" V 4900 5650 50  0000 L CNN
F 1 "100nF" V 4700 5650 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4800 5700 50  0001 C CNN
F 3 "" H 4800 5700 50  0000 C CNN
F 4 "478-1395-1-ND" H 4800 5700 60  0001 C CNN "Part No."
F 5 "<a href=\"https://www.digikey.com/products/en?keywords=478-1395-1-ND\">Link</a>" H 4800 5700 60  0001 C CNN "Link"
	1    4800 5700
	0    -1   -1   0   
$EndComp
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR0115
U 1 1 5C7585C4
P 4900 5700
F 0 "#PWR0115" H 4900 5450 50  0001 C CNN
F 1 "GND_L8" H 4900 5550 50  0000 C CNN
F 2 "" H 4900 5700 50  0000 C CNN
F 3 "" H 4900 5700 50  0000 C CNN
	1    4900 5700
	0    -1   -1   0   
$EndComp
Text GLabel 4650 5300 1    39   Output ~ 0
TX_+12_VOLTAGE
Wire Wire Line
	4600 5450 4700 5450
Wire Wire Line
	5100 5450 5000 5450
Wire Wire Line
	4650 5700 4700 5700
Wire Wire Line
	4650 5300 4650 5700
Connection ~ 4650 5450
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
Text Notes 3200 5950 0    39   ~ 0
Vbat <= 4.5V
Text GLabel 3150 5250 0    39   Output ~ 0
AUX_BAT_DIRECT
Wire Wire Line
	3150 5250 3200 5250
Wire Wire Line
	3200 5250 3200 4900
Connection ~ 3200 4900
$Comp
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR0116
U 1 1 5F83A1A7
P 4100 2400
F 0 "#PWR0116" H 4100 2150 50  0001 C CNN
F 1 "GND_L8" H 4100 2250 50  0001 C CNN
F 2 "" H 4100 2400 50  0000 C CNN
F 3 "" H 4100 2400 50  0000 C CNN
	1    4100 2400
	0    -1   -1   0   
$EndComp
Connection ~ 3900 5450
Connection ~ 3900 4900
$Comp
L ARDF-2-Band-MiniTx-rescue:TEST_SCOPE W205
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
L ARDF-2-Band-MiniTx-rescue:GND_L8 #PWR0117
U 1 1 5F84AE20
P 4700 2250
F 0 "#PWR0117" H 4700 2000 50  0001 C CNN
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
$EndSCHEMATC
