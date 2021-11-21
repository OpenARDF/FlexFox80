/**********************************************************************************************
 * Copyright © 2017 Digital Confections LLC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of 
 * this software and associated documentation files (the "Software"), to deal in the 
 * Software without restriction, including without limitation the rights to use, copy, 
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, subject to the 
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all 
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 *
 **********************************************************************************************
 * dac081c085.h
 *
 * DAC081C085: 8-Bit Micro Power Digital-to-Analog Converter With An I2C-Compatible Interface
 * http://www.ti.com/lit/ds/symlink/dac081c081.pdf
 *
 * The DAC081C08x device is an 8-bit, single-channel, voltage-output digital-to-analog converter 
 * (DAC) that operates from a 2.7-V to 5.5-V supply. The output amplifier allows rail-to-rail 
 * output swing and has an 4.5-µsec settling time.
 *
 */ 

#define DAC081C_I2C_SLAVE_ADDR_A0 0x18
#define DAC081C_I2C_SLAVE_ADDR_A1 0x1A

#ifndef DAC081C085_H_
#define DAC081C085_H_

#include "i2c.h"
#include "defs.h"

/**
   Set the DAC to the value passed in setting.
*/
void dac081c_set_dac(uint8_t setting, uint8_t addr);

/**
   Get the current DAC setting.
*/
uint8_t dac081c_read_dac(uint8_t addr);


#endif /* DAC081C085_H_ */