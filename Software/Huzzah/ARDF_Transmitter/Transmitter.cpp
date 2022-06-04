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

#include "Transmitter.h"


Transmitter::Transmitter(bool debug)
{
#ifdef TRANSMITTER_DEBUG_PRINTS_OVERRIDE
  debug = TRANSMITTER_DEBUG_PRINTS_OVERRIDE;
#endif
  debug_prints_enabled = debug;
}

bool Transmitter::parseStringData(String s)
{
  if (s.indexOf(',') < 0)
  {
    return ( true); /* invalid line found */
  }

  String settingID = s.substring(0, s.indexOf(','));
  String value = s.substring(s.indexOf(',') + 1, s.indexOf('\n'));

  if (value.charAt(0) == '"')
  {
    if (value.charAt(1) == '"') /* handle empty string */
    {
      value = "";
    }
    else /* remove quotes */
    {
      value = value.substring(1, value.length() - 2);
    }
  }

  return (setXmtrData(settingID, value));
}

bool Transmitter::setXmtrData(String id, String value)
{
  bool result = false;

  if (id.equalsIgnoreCase(MASTER_OR_CLONE_SETTING))
  {
    masterCloneSetting = value;
  }
  else
  {
    if (debug_prints_enabled)
    {
      Serial.println("Error in file: XmtrData = " + id + " Value = [" + value + "]");
    }

    result = true;
  }

  return ( result);
}
