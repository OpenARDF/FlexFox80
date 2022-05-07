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
 * ad5245.h
 *
 * AD5245: 256-Position I2C®-Compatible Digital Potentiometer
 * http://www.analog.com/media/en/technical-documentation/data-sheets/AD5245.pdf
 *
 * The AD5245 provides a compact 2.9 mm × 3 mm packaged solution for 256-position 
 * adjustment applications. These devices perform the same electronic adjustment 
 * function as mechanical potentiometers or variable resistors, with enhanced resolution, 
 * solid-state reliability, and superior low temperature coefficient performance.
 *
 */ 


#ifndef AD5245_H_
#define AD5245_H_

#include "defs.h"

/**
   Set the AD5345 potentiometer to the value passed in setting.
*/
void ad5245_set_potentiometer(uint8_t setting);


#endif /* AD5245_H_ */