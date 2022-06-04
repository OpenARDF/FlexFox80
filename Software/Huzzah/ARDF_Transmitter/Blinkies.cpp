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

#include "Blinkies.h"
#include <Arduino.h>
#include <user_interface.h>
#include "esp8266.h"

Blinkies::Blinkies()
{
  pinMode(RED_LED, OUTPUT);       /* Allow the red LED to be controlled */
  pinMode(BLUE_LED, OUTPUT);      /* Initialize the BUILTIN_LED pin as an output */
  digitalWrite(RED_LED, HIGH);    /* Turn off red LED */
  digitalWrite(BLUE_LED, HIGH);   /* Turn off blue LED */
}

void Blinkies::setLEDs(LEDPattern pattern, bool leds_enabled)
{
  if ((!leds_enabled) || (pattern == LEDS_OFF))
  {
    digitalWrite(BLUE_LED, HIGH);    /* Turn off blue LED */
    digitalWrite(RED_LED, HIGH);     /* Turn off red LED */
  }
  else if (pattern == RED_LED_ONLY)
  {
    digitalWrite(BLUE_LED, HIGH);    /* Turn on blue LED */
    digitalWrite(RED_LED, LOW);     /* Turn on red LED */
  }
  else if (pattern == BLUE_LED_ONLY)
  {
    digitalWrite(BLUE_LED, LOW);    /* Turn on blue LED */
    digitalWrite(RED_LED, HIGH);     /* Turn on red LED */
  }
  else if (pattern == RED_BLUE_TOGETHER)
  {
    digitalWrite(BLUE_LED, LOW);    /* Turn on blue LED */
    digitalWrite(RED_LED, LOW);     /* Turn on red LED */
  }
}

bool Blinkies::blinkLEDs(int blinkPeriodMillis, LEDPattern pattern, bool leds_enabled)
{
  unsigned long blinkTimeSeconds;

  blinkTimeSeconds = millis() / blinkPeriodMillis;
  unsigned int debounceCounts = max(3, (1500 / blinkPeriodMillis));

  if (holdTime_ != blinkTimeSeconds)
  {
    holdTime_ = blinkTimeSeconds;
    toggle_ = !toggle_;

    if ((progButtonPressed_) || (pattern == LEDS_ON))
    {
      digitalWrite(BLUE_LED, LOW);    /* Turn on blue LED */
      digitalWrite(RED_LED, LOW);     /* Turn on red LED */
    }
    else if (!leds_enabled || (pattern == LEDS_OFF))
    {
      digitalWrite(BLUE_LED, HIGH);   /* Turn off blue LED */
      digitalWrite(RED_LED, HIGH);    /* Turn off red LED */
    }
    else
    {
      if (pattern == RED_BLUE_ALTERNATING)
      {
        digitalWrite(BLUE_LED, toggle_); /* Blink blue LED */
        digitalWrite(RED_LED, !toggle_); /* Blink red LED */
        if (toggle_)
        {
          pinMode(RED_LED, INPUT);    /* Allow GPIO0 to be read */
          if (!digitalRead(RED_LED))
          {
            debounceProgButton_--;
            if (!debounceProgButton_)
            {
              progButtonPressed_ = true;
              debounceProgButton_ = debounceCounts;
            }
          }
          else
          {
            debounceProgButton_ = debounceCounts;
          }

          pinMode(RED_LED, OUTPUT);
        }
      }
      else if (pattern == BLUE_LED_ONLY)
      {
        digitalWrite(BLUE_LED, toggle_); /* Blink blue LED */

        digitalWrite(RED_LED, LOW);     /* Turn on red LED */
        pinMode(RED_LED, INPUT);        /* Allow GPIO0 to be read */
        if (!digitalRead(RED_LED))
        {
          debounceProgButton_--;
          if (!debounceProgButton_)
          {
            progButtonPressed_ = true;
            debounceProgButton_ = debounceCounts;
          }
        }
        else
        {
          debounceProgButton_ = debounceCounts;
        }

        pinMode(RED_LED, OUTPUT);
        digitalWrite(RED_LED, HIGH);    /* Turn off red LED */
      }
      else if (pattern == RED_LED_ONLY)
      {
        digitalWrite(RED_LED, toggle_);  /* Blink red LED */
        digitalWrite(BLUE_LED, HIGH);   /* Turn off blue LED */

        if (!toggle_)
        {
          pinMode(RED_LED, INPUT);    /* Allow GPIO0 to be read */
          if (!digitalRead(RED_LED))
          {
            debounceProgButton_--;
            if (!debounceProgButton_)
            {
              progButtonPressed_ = true;
              debounceProgButton_ = debounceCounts;
            }
          }
          else
          {
            debounceProgButton_ = debounceCounts;
          }

          pinMode(RED_LED, OUTPUT);
        }
      }
      else if (pattern == RED_BLUE_TOGETHER)
      {
        digitalWrite(BLUE_LED, toggle_); /* Blink blue LED */
        digitalWrite(RED_LED, toggle_); /* Blink red LED */
        if (!toggle_)
        {
          pinMode(RED_LED, INPUT);    /* Allow GPIO0 to be read */
          if (!digitalRead(RED_LED))
          {
            debounceProgButton_--;
            if (!debounceProgButton_)
            {
              progButtonPressed_ = true;
              debounceProgButton_ = debounceCounts;
            }
          }
          else
          {
            debounceProgButton_ = debounceCounts;
          }

          pinMode(RED_LED, OUTPUT);
        }
      }
    }
  }

  return progButtonPressed_;
}