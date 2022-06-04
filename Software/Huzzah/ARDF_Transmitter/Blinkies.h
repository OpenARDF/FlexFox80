/**********************************************************************************************
    Copyright © 2022 Digital Confections LLC

    Permission is hereby granted, free of charge, to any person obtaining a copy of
    this software and associated documentation files (the "Software"), to deal in the
    Software without restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so, subject to the
    following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
    FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
    OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

**********************************************************************************************/

#ifndef _BLINKIES_H_
#define _BLINKIES_H_

#include <ESP8266WiFi.h>
#include "Transmitter.h"

typedef enum
{
  RED_LED_ONLY,
  BLUE_LED_ONLY,
  RED_BLUE_TOGETHER,
  RED_BLUE_ALTERNATING,
  LEDS_ON,
  LEDS_OFF
} LEDPattern;


class Blinkies {
  public:
    ~Blinkies() {
    }

    Blinkies();
    bool blinkLEDs(int blinkPeriodMillis, LEDPattern pattern, bool leds_enabled);
    void setLEDs(LEDPattern pattern, bool leds_enabled);

  private:

    unsigned long holdTime_ = 0;
    bool toggle_ = 0;
    bool progButtonPressed_ = false;
    int debounceProgButton_ = 0;
};



#endif  /*_BLINKIES_H_ */
