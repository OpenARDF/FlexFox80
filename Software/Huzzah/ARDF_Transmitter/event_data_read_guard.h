/*
 *  MIT License
 *
 *  Copyright (c) 2026 Digital Confections LLC
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

/*
 * Dependency-free reset and bounds helpers for ESP8266 event-file reads.
 */

#pragma once

static inline bool eventReadCountWithinBounds(int count, int capacity)
{
  return count > 0 && capacity > 0 && count <= capacity;
}

template <typename EventData>
static inline void resetEventDataForRead(EventData *data, int roleCapacity, int txCapacity)
{
  if (!data)
  {
    return;
  }

  data->tx_assignment = "";
  data->tx_role_name = "";
  data->tx_role_pwr = "";
  data->tx_role_freq = "";
  data->tx_assignment_is_default = false;
  data->event_name = "";
  data->event_file_version = "";
  data->event_band = "";
  data->event_antenna_port = "";
  data->event_callsign = "";
  data->event_callsign_speed = "";
  data->event_start_date_time = "";
  data->event_finish_date_time = "";
  data->event_modulation = "";
  data->event_number_of_tx_types = -1;

  for (int roleIndex = 0; roleIndex < roleCapacity; roleIndex++)
  {
    if (!data->role[roleIndex])
    {
      continue;
    }

    data->role[roleIndex]->rolename = "";
    data->role[roleIndex]->numberOfTxs = 0;
    data->role[roleIndex]->frequency = 0;
    data->role[roleIndex]->powerLevel_mW = 0;
    data->role[roleIndex]->code_speed = 0;
    data->role[roleIndex]->id_interval = 0;

    for (int txIndex = 0; txIndex < txCapacity; txIndex++)
    {
      if (!data->role[roleIndex]->tx[txIndex])
      {
        continue;
      }

      data->role[roleIndex]->tx[txIndex]->pattern = "";
      data->role[roleIndex]->tx[txIndex]->onTime = "";
      data->role[roleIndex]->tx[txIndex]->offTime = "";
      data->role[roleIndex]->tx[txIndex]->delayTime = "";
    }
  }
}
