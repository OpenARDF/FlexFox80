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
#include "Event.h"
#include <LittleFS.h>
#include "Helpers.h"
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

extern ESP8266WebServer * g_http_server;

Event::Event(bool debug)
{

#ifdef EVENT_DEBUG_PRINTS_OVERRIDE
  debug = EVENT_DEBUG_PRINTS_OVERRIDE;
#endif
  debug_prints_enabled = debug;

  values_did_change = false;
  eventData = new EventDataStruct();

  for (int i = 0; i < MAXIMUM_NUMBER_OF_EVENT_TX_TYPES; i++)
  {
    eventData->role[i] = new RoleDataStruct();
    if (eventData->role[i] == NULL)
    {
      Serial.println("Error! Out of memory?");
    }

    for (int j = 0; j < MAXIMUM_NUMBER_OF_TXs_OF_A_TYPE; j++)
    {
      eventData->role[i]->tx[j] = new TxDataStruct();

      if (eventData->role[i]->tx[j] == NULL)
      {
        Serial.println("Error! Out of memory?");
      }
    }
  }
}

Event::~Event()
{

  for (int i = 0; i < MAXIMUM_NUMBER_OF_EVENT_TX_TYPES; i++)
  {
    for (int j = 0; j < MAXIMUM_NUMBER_OF_TXs_OF_A_TYPE; j++)
    {
      delete eventData->role[i]->tx[j];
    }

    delete eventData->role[i];
  }

  delete eventData;
}

bool Event::extractLineData(String s, EventLineData *result)
{
  if (s.indexOf(',') < 0)
  {
    if ((s.indexOf("EVENT_START") < 0) && (s.indexOf("EVENT_END") < 0))
    {
      return ( true); /* invalid line found */
    }
    else
    {
      result->id = "";
      result->value = s;
      return ( false); /* start or end found */
    }
  }

  String settingID = s.substring(0, s.indexOf(','));
  String value = s.substring(s.indexOf(',') + 1);
  value.trim();                   /*remove leading and trailing whitespace */

  if (value.charAt(0) == '"')
  {
    if (value.charAt(1) == '"') /* handle empty string */
    {
      value = "";
    }
    else                        /* remove quotes */
    {
      value = value.substring(1, value.length() - 1);
    }
  }

  result->id = settingID;
  result->value = value;

  return ( false);
}

bool Event::parseStringData(String s)
{
  EventLineData data;

  if (extractLineData(s, &data))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Error extracting data");
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    return ( true); /* flag error */
  }
  else if ((data.id).equals(""))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("No data found");
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    return ( false); /* no data but no error */
  }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (debug_prints_enabled)
  {
    Serial.println(String("Parsed: ") + s + " = " + data.id + " + " + data.value);
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
  return (setEventData(data.id, data.value));
}

/**
    Returns a boolean indicating if event a will commence sooner that b relative to the currentEpoch
*/
bool Event::isSoonerEvent(EventFileRef a, EventFileRef b, unsigned long currentEpoch)
{
  bool aIsEnabled = (currentEpoch < a.finishDateTimeEpoch) && (a.startDateTimeEpoch < a.finishDateTimeEpoch);
  bool bIsEnabled = (currentEpoch < b.finishDateTimeEpoch) && (b.startDateTimeEpoch < b.finishDateTimeEpoch);

  if (!aIsEnabled)
  {
    return (false); /* a is disabled */
  }

  if (!bIsEnabled)
  {
    return (true); /* b is disabled and a is not */
  }

  return (a.startDateTimeEpoch < b.startDateTimeEpoch);   /* a did/will start sooner than b */
}


bool Event::isNotDisabledEvent(unsigned long currentEpoch)
{
  bool isDisabled = convertTimeStringToEpoch(this->eventData->event_start_date_time) >= convertTimeStringToEpoch(this->eventData->event_finish_date_time);

  isDisabled = isDisabled || (convertTimeStringToEpoch(this->eventData->event_finish_date_time) <= currentEpoch);

  return (!isDisabled);
}


String Event::getTxDescriptiveName(String role_tx)          /* role_tx = "r:t" */
{
  String theName = "";
  int i = role_tx.indexOf(":");

  if (i < 1)
  {
    return ( String("Error(:)"));
  }

  int roleIndex = (role_tx.substring(0, i)).toInt();  /* r */
  int txIndex = (role_tx.substring(i + 1)).toInt();   /* t */

  //    if(debug_prints_enabled)
  //    {
  //    	Serial.println(String("role = " + String(roleIndex) + "; tx = " + String(txIndex)));
  //    }

  if (((roleIndex >= 0) && (roleIndex < this->eventData->event_number_of_tx_types)) && ((txIndex >= 0) && (txIndex < this->eventData->role[roleIndex]->numberOfTxs)))
  {
    int txsInRole = this->eventData->role[roleIndex]->numberOfTxs;

    theName = this->eventData->role[roleIndex]->rolename;

    if (txsInRole > 1)
    {
      theName = String(theName + " " + String(txIndex + 1));
    }

    theName = String(theName + " - " + this->eventData->role[roleIndex]->tx[txIndex]->pattern);
  }

  return (theName);
}

String Event::readMeFile(String path)
{
  if (path == NULL)
  {
    return ( "");
  }

  if (path.endsWith(".event"))
  {
    int dot = path.lastIndexOf(".event");
    path = path.substring(0, dot);
  }

  if (!path.endsWith(".me"))
  {
    path = path + ".me";
  }

  if (LittleFS.exists(path))
  {
    /* Create an object to hold the file data */
    File file = LittleFS.open(path, "r"); /* Open the file for reading */

    if (file)
    {
      yield();
      String s = file.readStringUntil('\n');
      int count = 0;

      while (s.length() && count++ < MAXIMUM_NUMBER_OF_ME_FILE_LINES)
      {
        this->parseStringData(s);
        s = file.readStringUntil('\n');
      }

      file.close();   /* Close the file */
    }
  }
  else
  {
    File file = LittleFS.open(path, "w"); /* Open the file for writing */

    if (file)
    {
      file.println(TX_ASSIGNMENT + String(",0:0"));
      file.println(String(TX_DESCRIPTIVE_NAME) + "," + getTxDescriptiveName("0:0"));
      file.println(TX_ASSIGNMENT_IS_DEFAULT + String(",true"));
      file.close();   /* Close the file */
    }

    //		if(debug_prints_enabled)
    //		{
    //			Serial.println(String("\tWrote file: ") + path);
    //		}

    this->eventData->tx_assignment = "0:0";
    this->eventData->tx_assignment_is_default = true;
  }

  return ( path);
}

bool Event::extractMeFileData(String path, EventFileRef *eventRef)
{
  bool fail = true;

  if (path == NULL)
  {
    return ( fail);
  }

  if (path.endsWith(".event"))
  {
    int dot = path.lastIndexOf(".event");
    path = path.substring(0, dot);
  }

  if (!path.endsWith(".me"))
  {
    path = path + ".me";
  }

  if (LittleFS.exists(path))
  {
    /* Create an object to hold the file data */
    File file = LittleFS.open(path, "r"); /* Open the file for reading */

    if (file)
    {
      yield();
      String s = file.readStringUntil('\n');
      int count = 0;
      int items = 0;

      EventLineData data;

      while (s.length() && (items < 4) && (count++ < MAXIMUM_NUMBER_OF_ME_FILE_LINES))
      {
        Event::extractLineData(s, &data);

        if (data.id.equalsIgnoreCase(TX_DESCRIPTIVE_NAME))
        {
          eventRef->role = data.value;
          fail = false;
          items++;
        }
        /* ignore TX_ASSIGNMENT "TX_ASSIGNMENT" / *Which role and time slot is assigned to this transmitter: "r:t" * / */

        yield();
        s = file.readStringUntil('\n');
      }

      file.close();   /* Close the file */
    }
  }
  else
  {
    File file = LittleFS.open(path, "w"); /* Open the file for writing */

    if (file)
    {
      file.println(TX_ASSIGNMENT + String(",0:0"));
      file.println(String(TX_DESCRIPTIVE_NAME) + ",Finish - MO");
      //      file.println(String(TX_ROLE_POWER) + ",1000");
      //      file.println(String(TX_ROLE_FREQ) + ",3550000");
      file.println(TX_ASSIGNMENT_IS_DEFAULT + String(",true"));
      file.close();   /* Close the file */
    }

    eventRef->role = "?";
    eventRef->freq = "?";
    fail = false;
  }

  return ( fail);
}

bool Event::validEventFile(String path)
{
  bool isValid;
  isValid = validEventFile(path, NULL);
  return (isValid);
}

bool Event::validEventFile(String path, String* filename)
{
  bool failure = false;
  bool startFound = false;
  bool endFound = false;
  int linesInFile = 0;
  EventLineData lineData;
  int checksum;

  if (LittleFS.exists(path))
  {
    /* Create an object to hold the file data */
    File file = LittleFS.open(path, "r"); /* Open the file for reading */

    if (file)
    {
      String s = String("START");

      while (s.length() && !startFound)
      {
        yield();
        s = file.readStringUntil('\n');
        s.trim();

        if (!extractLineData(s, &lineData))
        {
          startFound = s.equals(EVENT_FILE_START);
          if (lineData.id.equals(EVENT_FILE_NAME))
          {
            if (filename)
            {
              *filename = lineData.value;
            }
          }
        }
      }

      if (startFound)
      {
        linesInFile = 1;
        checksum += s.length(); /* Add length of EVENT_START */

        while (s.length() && (linesInFile++ <= MAXIMUM_NUMBER_OF_EVENT_FILE_LINES) && !endFound)
        {
          yield();
          s = file.readStringUntil('\n');
          s.trim();
          if (!endFound)
          {
            checksum += s.length();
            endFound = s.equals(EVENT_FILE_END);
          }
          else if (!extractLineData(s, &lineData))
          {
            if (lineData.id.equals(EVENT_FILE_CHECKSUM))
            {
              failure = (checksum != lineData.value.toInt());
            }
          }

          linesInFile++;
        }

        if (!failure)
        {
          failure = (!endFound || (linesInFile > MAXIMUM_NUMBER_OF_EVENT_FILE_LINES));
        }
      }
      else
      {
        failure = true;
      }

      file.close();   /* Close the file */
    }
    else
    {
      failure = true;
    }
  }
  else
  {
    failure = true;
  }

  return (!failure);
}

bool Event::readEventFile(String path)
{
  bool failure = false;
  bool startFound = false;
  bool endFound = false;
  int linesInFile = 0;

  if (LittleFS.exists(path))
  {
    this->eventData->tx_assignment = "";
    this->eventData->tx_assignment_is_default = "";
    this->eventData->event_file_version = "";
    this->eventData->event_band = "";
    this->eventData->event_antenna_port = "";
    this->eventData->event_callsign = "";
    this->eventData->event_callsign_speed = "";
    this->eventData->event_start_date_time = "";
    this->eventData->event_finish_date_time = "";
    this->eventData->event_modulation = "";
    this->eventData->event_number_of_tx_types = -1;
    this->myPath = path;

    /* Create an object to hold the file data */
    File file = LittleFS.open(path, "r"); /* Open the file for reading */

    if (file)
    {
      String s = String("START");

      while (s.length() && !startFound)
      {
        yield();
        s = file.readStringUntil('\n');
        s.trim();
        startFound = s.equals(EVENT_FILE_START);
      }

      if (startFound)
      {
        linesInFile = 1;

        while (s.length() && (linesInFile++ <= MAXIMUM_NUMBER_OF_EVENT_FILE_LINES) && !endFound)
        {
          yield();
          s = file.readStringUntil('\n');
          s.trim();
          this->parseStringData(s);
          endFound = s.equals(EVENT_FILE_END);
          linesInFile++;
        }

        failure = (!endFound || (linesInFile > MAXIMUM_NUMBER_OF_EVENT_FILE_LINES));
      }
      else
      {
        failure = true;
      }

      file.close();   /* Close the file */
    }
    else
    {
      failure = true;
    }

    getTxAssignment();
    values_did_change = false;
  }
  else
  {
    failure = true;
  }

  return ( failure);
}

void Event::dumpData(void)
{
  if (eventData == NULL)
  {
    return;
  }

  Serial.println("=====");
  Serial.println("Event name: " + eventData->event_name);
  Serial.println("File ver: " + eventData->event_file_version);
  Serial.println("Band: " + eventData->event_band);
  Serial.println("Call: " + eventData->event_callsign);
  Serial.println("Call WPM: " + eventData->event_callsign_speed);
  Serial.println("Start: " + eventData->event_start_date_time);
  Serial.println("Finish: " + eventData->event_finish_date_time);
  Serial.println("Mod: " + eventData->event_modulation);
  Serial.println("Types: " + String(eventData->event_number_of_tx_types));
  for (int i = 0; i < eventData->event_number_of_tx_types; i++)
  {
    Serial.println("  Name: " + eventData->role[i]->rolename);
    Serial.println("    No. txs: " + String(eventData->role[i]->numberOfTxs));
    Serial.println("    Freq: " + String(eventData->role[i]->frequency));
    Serial.println("    Pwr: " + String(eventData->role[i]->powerLevel_mW));
    Serial.println("    WPM: " + String(eventData->role[i]->code_speed));
    Serial.println("    ID int: " + String(eventData->role[i]->id_interval));

    for (int j = 0; j < eventData->role[i]->numberOfTxs; j++)
    {
      Serial.println("      Pattern: " + eventData->role[i]->tx[j]->pattern);
      Serial.println("      onTime: " + eventData->role[i]->tx[j]->onTime);
      Serial.println("      offTime: " + eventData->role[i]->tx[j]->offTime);
      Serial.println("      delayTime: " + eventData->role[i]->tx[j]->delayTime);
    }
  }
  Serial.println("=====");
}

/**
   Returns TRUE if this event contains valid data
*/
bool Event::validateEvent(void)
{
  bool success;

  success = (this->eventData != NULL);

  if (success)
  {
    success &= (this->eventData->event_name).length() > 0;
    success &= (this->eventData->event_file_version).length() > 0;
    success &= (this->eventData->event_band).length() > 0;
    /*success &= (this->eventData->event_callsign).length() > 2;*/
    success &= (this->eventData->event_callsign_speed).length() > 0;
    success &= (this->eventData->event_start_date_time).length() > 19;
    success &= (this->eventData->event_finish_date_time).length() > 19;
    success &= (this->eventData->event_modulation).length() > 0;
    success &= ((this->eventData->event_number_of_tx_types) > 0);

    for (int i = 0; i < this->eventData->event_number_of_tx_types; i++)
    {
      success &= (this->eventData->role[i]->rolename).length() > 0;
      success &= (this->eventData->role[i]->numberOfTxs) > 0;
      success &= (this->eventData->role[i]->frequency) > 0;
      success &= (this->eventData->role[i]->powerLevel_mW) > 0;
      success &= (this->eventData->role[i]->code_speed) > 0;
      /*success &= (this->eventData->role[i]->id_interval); */

      for (int j = 0; j < this->eventData->role[i]->numberOfTxs; j++)
      {
        success &= (this->eventData->role[i]->tx[j]->pattern).length() > 0;
        success &= (this->eventData->role[i]->tx[j]->onTime).length() > 0;
        success &= (this->eventData->role[i]->tx[j]->offTime).length() > 0;
        success &= (this->eventData->role[i]->tx[j]->delayTime).length() > 0;
      }
    }

    if (success)
    {
      String start = this->eventData->event_start_date_time;
      String finish = this->eventData->event_finish_date_time;
      success &= (start.indexOf(':') > 0);
      success &= (finish.indexOf(':') > 0);
    }
  }

  return ( success);
}

bool Event::writeEventFile(void)
{
  return (this->writeEventFile(this->myPath));
}

bool Event::writeEventFile(String path)
{
  bool failure = true;
  yield();

  if (this->eventData == NULL)
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.print("Not written: eventData is NULL: ");
      Serial.println(path);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    return ( true);
  }

  if (!validateEvent())
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.print("Not written: Event not valid: ");
      Event::dumpData();
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    return ( true);
  }

  if (this->values_did_change == false)
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.print("Not written: Event did not change: ");
      Serial.println(path);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    return ( false); /*nothing new to save, return no error */
  }
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  else if (debug_prints_enabled)
  {
    Serial.print("Writing Event changes to:");
    Serial.println(path);
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  if (path == NULL)
  {
    path = this->myPath;
  }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (debug_prints_enabled)
  {
    Serial.print("Writing file: ");
    Serial.println(path);
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  noInterrupts();

  File eventFile = LittleFS.open(path, "w");    /* Open the file for writing in LittleFS (create if it doesn't exist) */

  this->myPath = path;

  String typenum, txnum;

  if (eventFile)
  {
    eventFile.println(EVENT_FILE_START);
    eventFile.println(String(String(EVENT_NAME) + "," + this->eventData->event_name));
    eventFile.println(String(String(EVENT_FILE_VERSION) + "," + this->eventData->event_file_version));
    eventFile.println(String(String(EVENT_BAND) + "," + this->eventData->event_band));
    eventFile.println(String(String(EVENT_ANTENNA_PORT) + "," + this->eventData->event_antenna_port));
    eventFile.println(String(String(EVENT_CALLSIGN) + "," + this->eventData->event_callsign));
    eventFile.println(String(String(EVENT_CALLSIGN_SPEED) + "," + this->eventData->event_callsign_speed));
    eventFile.println(String(String(EVENT_START_DATE_TIME) + "," + this->eventData->event_start_date_time));
    eventFile.println(String(String(EVENT_FINISH_DATE_TIME) + "," + this->eventData->event_finish_date_time));
    eventFile.println(String(String(EVENT_MODULATION) + "," + this->eventData->event_modulation));
    eventFile.println(String(String(EVENT_NUMBER_OF_TX_TYPES) + "," + this->eventData->event_number_of_tx_types));

    for (int i = 0; i < this->eventData->event_number_of_tx_types; i++)
    {
      typenum = String("TYPE" + String(i + 1));
      eventFile.println(String(typenum + TYPE_NAME + "," + this->eventData->role[i]->rolename));
      eventFile.println(String(typenum + TYPE_TX_COUNT + "," + String(this->eventData->role[i]->numberOfTxs)));
      eventFile.println(String(typenum + TYPE_FREQ + "," + String(this->eventData->role[i]->frequency)));
      eventFile.println(String(typenum + TYPE_POWER_LEVEL + "," + String(this->eventData->role[i]->powerLevel_mW)));
      eventFile.println(String(typenum + TYPE_CODE_SPEED + "," + String(this->eventData->role[i]->code_speed)));
      eventFile.println(String(typenum + TYPE_ID_INTERVAL + "," + String(this->eventData->role[i]->id_interval)));

      for (int j = 0; j < this->eventData->role[i]->numberOfTxs; j++)
      {
        txnum = String("TX" + String(j + 1));
        eventFile.println(String(typenum + "_" + txnum + TYPE_TX_PATTERN + "," + this->eventData->role[i]->tx[j]->pattern));
        eventFile.println(String(typenum + "_" + txnum + TYPE_TX_ON_TIME + "," + this->eventData->role[i]->tx[j]->onTime));
        eventFile.println(String(typenum + "_" + txnum + TYPE_TX_OFF_TIME + "," + this->eventData->role[i]->tx[j]->offTime));
        eventFile.println(String(typenum + "_" + txnum + TYPE_TX_DELAY_TIME + "," + this->eventData->role[i]->tx[j]->delayTime));
      }
    }

    eventFile.println(EVENT_FILE_END);
    eventFile.close();  /* Close the file */
  }
  else
  {
    interrupts();
    return ( true);
  }

  interrupts();
  saveMeData("");

  failure = !validEventFile(path);
  this->values_did_change = failure;

  return ( failure);
}

void Event::saveMeData(String newAssignment)
{
  if (this->eventData == NULL)
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if ( debug_prints_enabled )
    {
      Serial.println("Me not saved: no event data");
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    return;
  }

  if ((this->eventData->tx_assignment.length() < 3) || (this->eventData->tx_assignment.indexOf(":") < 1))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if ( debug_prints_enabled )
    {
      Serial.println("Me not saved: assignment error");
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    return;
  }

  if (this->myPath.length() < 7)
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if ( debug_prints_enabled )
    {
      Serial.println("Me not saved: path error");
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    return;
  }

  String holdTxAssignment;
  String holdRoleName = this->eventData->tx_role_name;
  //  String holdRoleFrequency = this->eventData->tx_role_freq;

  if (newAssignment.indexOf(":") < 1)
  {
    holdTxAssignment = this->eventData->tx_assignment;
  }
  else
  {
    holdTxAssignment = newAssignment;
  }

  String path = readMeFile(this->myPath); /* reads file value into this->eventData->tx_assignment, and returns the path to the Me file */

  if ((!holdTxAssignment.equals(this->eventData->tx_assignment)) || (!holdRoleName.equals(this->eventData->tx_role_name)))
  {
    String role = holdTxAssignment.substring(0, holdTxAssignment.indexOf(":"));
    File file = LittleFS.open(path, "w"); /* Open the file for writing */

    if (file)
    {
      file.println(EVENT_FILE_START);
      file.println(String(TX_ASSIGNMENT) + "," + holdTxAssignment);
      file.println(String(TX_DESCRIPTIVE_NAME) + "," + getTxDescriptiveName(holdTxAssignment));
      //      file.println(String(TX_ROLE_POWER) + "," + String(this->getPowerlevelForRole(role.toInt())));
      //      file.println(String(TX_ROLE_FREQ) + "," + String(this->getFrequencyForRole(role.toInt())));
      file.println(String(TX_ASSIGNMENT_IS_DEFAULT) + ",false");
      file.println(EVENT_FILE_END);
      file.close();   /* Close the file */

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if ( debug_prints_enabled )
      {
        Serial.println(String("Me file written: ") + path);
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    }
  }

  this->eventData->tx_assignment = holdTxAssignment;  /* set tx_assignment to the latest value */
}

/*/////////////////////////////////////////////////////////////////////////////////////// */

bool Event::setTxAssignment(String role_slot)
{
  if (this->eventData == NULL)
  {
    return ( true);
  }
  role_slot.trim();
  int c = role_slot.indexOf(':');
  if (c < 1)
  {
    return ( true);
  }

  if (this->eventData->tx_assignment != role_slot)
  {
    String r = role_slot.substring(0, c - 1);
    this->eventData->tx_assignment = role_slot;
    this->eventData->tx_role_name = Event::getTxDescriptiveName(role_slot);
    this->eventData->tx_role_pwr = Event::getPowerlevelForRole(r.toInt());
    this->eventData->tx_role_freq = Event::getFrequencyForRole(r.toInt());
    this->values_did_change = true;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Set role: " + this->eventData->tx_assignment);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
  }

  return ( false);
}

String Event::getPath(void)
{
  String path = this->myPath;
  if (path.startsWith("/"))
  {
    path = path.substring(1);
  }

  return (path);
}

String Event::getTxAssignment(void)
{
  if ((this->eventData->tx_assignment.length() < 1) || (this->eventData->tx_assignment.indexOf(":") < 1))
  {
    readMeFile(this->myPath);
  }

  return ( this->eventData->tx_assignment);
}

bool Event::setTxFrequency(String frequency)
{
  if (this->eventData == NULL)
  {
    return ( true);
  }
  if (frequency == NULL)
  {
    return ( true);
  }

  if (!frequency.equals(this->eventData->tx_role_freq))
  {
    this->eventData->tx_role_freq = frequency;
    this->values_did_change = true;

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Set frequency: " + this->eventData->tx_role_freq);
    }
#endif // #if TRANSMITTER_COMPILE_DEBUG_PRINTS
  }

  return ( false);
}

String Event::getTxFrequency(void)
{
  if ((this->eventData->tx_assignment.length() < 1) || (this->eventData->tx_assignment.indexOf(":") < 1))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Reading ME file...");
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    readMeFile(this->myPath);
  }

  return ( this->eventData->tx_role_freq);
}

int Event::getTxRoleIndex(void)
{
  int result = -1;
  String assign = this->eventData->tx_assignment;

  if (assign.indexOf(":") < 1)
  {
    getTxAssignment();
  }
  int colon = assign.indexOf(":");
  if (colon < 1)
  {
    return ( result);
  }
  result = (assign.substring(0, colon)).toInt();  /* r */
  return ( result);
}

int Event::getTxSlotIndex(void)
{
  int result = -1;
  String assign = this->eventData->tx_assignment;

  if (assign.indexOf(":") < 1)
  {
    getTxAssignment();
  }
  int colon = assign.indexOf(":");
  if (colon < 1)
  {
    return ( result);
  }
  result = (assign.substring(colon + 1)).toInt(); /* t */
  return ( result);
}

TxDataType *Event::getTxData(int roleIndex, int txIndex)
{
  return ( eventData->role[roleIndex]->tx[txIndex]);
}

void Event::setEventName(String str)
{
  if (this->eventData == NULL)
  {
    return;
  }
  str.trim();

  if (this->eventData->event_name != str)
  {
    this->setEventData(EVENT_NAME, str);
    this->values_did_change = true;
  }
}

String Event::getEventName(void) const
{
  if (this->eventData == NULL)
  {
    return ( "");
  }
  return ( this->eventData->event_name);
}

void Event::setEventFileVersion(String str)
{
  if (this->eventData == NULL)
  {
    return;
  }
  str.trim();

  if (this->eventData->event_file_version != str)
  {
    this->setEventData(EVENT_FILE_VERSION, str);
    this->values_did_change = true;
  }
}

String Event::getEventFileVersion(void) const
{
  if (this->eventData == NULL)
  {
    return ( "");
  }
  return ( this->eventData->event_file_version);
}

void Event::setEventBand(String str)
{
  if (this->eventData == NULL)
  {
    return;
  }
  str.trim();

  if (this->eventData->event_band != str)
  {
    this->setEventData(EVENT_BAND, str);
    this->values_did_change = true;
  }
}

String Event::getEventBand(void) const
{
  if (this->eventData == NULL)
  {
    return ( "");
  }
  return ( this->eventData->event_band);
}

void Event::setCallsign(String str)
{
  if (this->eventData == NULL)
  {
    return;
  }
  str.trim();

  if (this->eventData->event_callsign != str)
  {
    this->setEventData(EVENT_CALLSIGN, str);
    this->values_did_change = true;
  }
}

String Event::getCallsign(void) const
{
  if (this->eventData == NULL)
  {
    return ( "");
  }
  return ( this->eventData->event_callsign);
}

void Event::setAntennaPort(String str)
{
  if (this->eventData == NULL)
  {
    return;
  }
  str.trim();

  if (this->eventData->event_antenna_port != str)
  {
    this->setEventData(EVENT_ANTENNA_PORT, str);
    this->values_did_change = true;
  }
}

String Event::getAntennaPort(void) const
{
  if (this->eventData == NULL)
  {
    return ( "");
  }
  return ( this->eventData->event_antenna_port);
}

void Event::setCallsignSpeed(String str)
{
  if (this->eventData == NULL)
  {
    return;
  }
  str.trim();

  if (this->eventData->event_callsign_speed != str)
  {
    this->setEventData(EVENT_CALLSIGN_SPEED, str);
    this->values_did_change = true;
  }
}

String Event::getCallsignSpeed(void) const
{
  if (this->eventData == NULL)
  {
    return ( "");
  }
  return ( this->eventData->event_callsign_speed);
}

/**
  Takes a string of format "yyyy-mm-ddThh:mm:ssZ" or containing an epoch and saves it to the event
*/
void Event::setEventStartDateTime(String str)
{
  if (this->eventData == NULL)
  {
    return;
  }
  str.trim();

  if (str.indexOf(':') < 0) // received epoch
  {
    int len = str.length();

    if (len < 10) // invalid epoch
    {
      return;
    }
    else if (len > 10) // milliseconds epoch
    {
      String epoch = str.substring(0, 10);
      str = convertEpochToTimeString(epoch.toInt());
    }
    else
    {
      str = convertEpochToTimeString(str.toInt());
    }
  }

  /* Lop off milliseconds if they exist */
  if ((str.length() > 20) && (str.lastIndexOf('.') > 0))
  {
    int index = str.lastIndexOf('.');
    str = str.substring(0, index);
    str = str + "Z";
  }

  /* Add seconds if they are missing */
  if (str.lastIndexOf(':') == str.indexOf(':'))   /* if time contains only one ':' */
  {
    int index = str.lastIndexOf(':') + 3;
    str = str.substring(0, index);
    str = str + ":00Z";
  }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (debug_prints_enabled)
  {
    Serial.println(String("Setting start = \"" + str + "\""));
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  if (!(this->eventData->event_start_date_time.equals(str)))
  {
    this->setEventData(EVENT_START_DATE_TIME, str);
    this->values_did_change = true;
  }
}

String Event::getEventStartDateTime(void) const
{
  if (this->eventData == NULL)
  {
    return ( "");
  }
  return ( this->eventData->event_start_date_time);
}

/**
  Takes a string of format "yyyy-mm-ddThh:mm:ssZ" or containing an epoch and saves it to the event
*/
void Event::setEventFinishDateTime(String str)
{
  if (this->eventData == NULL)
  {
    return;
  }
  str.trim();

  if (str.indexOf(':') < 0) // received epoch
  {
    int len = str.length();

    if (len < 10) // invalid epoch
    {
      return;
    }
    else if (len > 10) // milliseconds
    {
      String epoch = str.substring(0, 10);
      str = convertEpochToTimeString(epoch.toInt());
    }
    else
    {
      str = convertEpochToTimeString(str.toInt());
    }
  }

  /* Lop off milliseconds if they exist */
  if ((str.length() > 20) && (str.lastIndexOf('.') > 0))
  {
    int index = str.lastIndexOf('.');
    str = str.substring(0, index);
    str = str + "Z";
  }

  /* Add seconds if they are missing */
  if (str.lastIndexOf(':') == str.indexOf(':'))   /* if time contains only one ':' */
  {
    int index = str.lastIndexOf(':') + 3;
    str = str.substring(0, index);
    str = str + ":00Z";
  }

#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (debug_prints_enabled)
  {
    Serial.println(String("Setting finish = \"" + str + "\""));
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  if (!(this->eventData->event_finish_date_time.equals(str)))
  {
    this->setEventData(EVENT_FINISH_DATE_TIME, str);
    this->values_did_change = true;
  }
}

String Event::getEventFinishDateTime(void) const
{
  if (this->eventData == NULL)
  {
    return ( "");
  }
  return ( this->eventData->event_finish_date_time);
}

void Event::setEventModulation(String str)
{
  if (this->eventData == NULL)
  {
    return;
  }
  str.trim();

  if (this->eventData->event_modulation != str)
  {
    this->setEventData(EVENT_MODULATION, str);
    this->values_did_change = true;
  }
}

String Event::getEventModulation(void) const
{
  if (this->eventData == NULL)
  {
    return ( "");
  }
  return ( this->eventData->event_modulation);
}

void Event::setEventNumberOfTxTypes(int val)
{
  if (this->eventData == NULL)
  {
    return;
  }

  if (this->eventData->event_number_of_tx_types != val)
  {
    this->setEventData(EVENT_NUMBER_OF_TX_TYPES, val);
    this->values_did_change = true;
  }
}

void Event::setEventNumberOfTxTypes(String str)
{
  this->setEventNumberOfTxTypes(str.toInt());
  this->values_did_change = true;
}

int Event::getEventNumberOfTxTypes(void) const
{
  if (this->eventData == NULL)
  {
    return ( -1);
  }
  return ( this->eventData->event_number_of_tx_types);
}

bool Event::setRolename(int roleIndex, String str)
{
  if (this->eventData == NULL)
  {
    return ( true);
  }
  if (roleIndex < 0)
  {
    return ( true);
  }
  if (roleIndex >= this->eventData->event_number_of_tx_types)
  {
    return ( true);
  }
  this->eventData->role[roleIndex]->rolename = str;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (debug_prints_enabled)
  {
    Serial.println("Type" + String(roleIndex) + " role: " + str);
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  this->values_did_change = true;
  return ( false);
}

String Event::getRolename(int roleIndex) const
{
  if (this->eventData == NULL)
  {
    return ( "");
  }
  if (roleIndex < 0)
  {
    return ( "");
  }
  if (roleIndex >= this->eventData->event_number_of_tx_types)
  {
    return ( "");
  }
  return ( this->eventData->role[roleIndex]->rolename);
}

bool Event::setNumberOfTxsForRole(int roleIndex, String str)
{
  int num = str.toInt();

  if (num < 1)
  {
    return ( true);
  }
  if (this->eventData == NULL)
  {
    return ( true);
  }
  if (roleIndex < 0)
  {
    return ( true);
  }
  if (roleIndex >= this->eventData->event_number_of_tx_types)
  {
    return ( true);
  }

  this->eventData->role[roleIndex]->numberOfTxs = num;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (debug_prints_enabled)
  {
    Serial.println("Type" + String(roleIndex) + " No Txs: " + str);
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  this->values_did_change = true;
  return ( false);
}

int Event::getNumberOfTxsForRole(int roleIndex) const
{
  if (this->eventData == NULL)
  {
    return ( -1);
  }
  if (roleIndex < 0)
  {
    return ( -1);
  }
  if (roleIndex >= this->eventData->event_number_of_tx_types)
  {
    return ( -1);
  }
  return ( this->eventData->role[roleIndex]->numberOfTxs);
}

bool Event::setFrequencyForRole(int roleIndex, long freq)
{
  if (this->eventData == NULL)
  {
    return ( true);
  }
  if (roleIndex < 0)
  {
    return ( true);
  }
  if (roleIndex >= this->eventData->event_number_of_tx_types)
  {
    return ( true);
  }
  this->eventData->role[roleIndex]->frequency = freq;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (debug_prints_enabled)
  {
    Serial.println("Type" + String(roleIndex) + " freq: " + String(freq));
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  this->values_did_change = true;

  /*If the transmitter is currently set to this role then update its frequency too */
  if (roleIndex == this->getTxRoleIndex())
  {
    this->setTxFrequency(String(freq));
  }
  return ( false);
}

long Event::getFrequencyForRole(int roleIndex) const
{
  if (this->eventData == NULL)
  {
    return ( -1);
  }
  if (roleIndex < 0)
  {
    return ( -1);
  }
  if (roleIndex >= this->eventData->event_number_of_tx_types)
  {
    return ( -1);
  }
  return ( this->eventData->role[roleIndex]->frequency);
}

bool Event::setPowerlevelForRole(int roleIndex, String str)
{
  if (this->eventData == NULL)
  {
    return ( true);
  }
  if (roleIndex < 0)
  {
    return ( true);
  }
  if (roleIndex >= this->eventData->event_number_of_tx_types)
  {
    return ( true);
  }
  this->eventData->role[roleIndex]->powerLevel_mW = str.toInt();
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (debug_prints_enabled)
  {
    Serial.println("Type" + String(roleIndex) + " freq: " + str);
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  this->values_did_change = true;
  return ( false);
}

int Event::getPowerlevelForRole(int roleIndex) const
{
  if (this->eventData == NULL)
  {
    return ( -1);
  }
  if (roleIndex < 0)
  {
    return ( -1);
  }
  if (roleIndex >= this->eventData->event_number_of_tx_types)
  {
    return ( -1);
  }
  return ( this->eventData->role[roleIndex]->powerLevel_mW);
}

bool Event::setCodeSpeedForRole(int roleIndex, String str)
{
  if (this->eventData == NULL)
  {
    return ( true);
  }
  if (roleIndex < 0)
  {
    return ( true);
  }
  if (roleIndex >= this->eventData->event_number_of_tx_types)
  {
    return ( true);
  }
  this->eventData->role[roleIndex]->code_speed = str.toInt();
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (debug_prints_enabled)
  {
    Serial.println("Type" + String(roleIndex) + " freq: " + str);
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  this->values_did_change = true;
  return ( false);
}

int Event::getCodeSpeedForRole(int roleIndex) const
{
  if (this->eventData == NULL)
  {
    return ( -1);
  }
  if (roleIndex < 0)
  {
    return ( -1);
  }
  if (roleIndex >= this->eventData->event_number_of_tx_types)
  {
    return ( -1);
  }
  return ( this->eventData->role[roleIndex]->code_speed);
}

bool Event::setPatternForTx(int typeIndex, int txIndex, String str)
{
  if (this->eventData == NULL)
  {
    return ( true);
  }
  if (typeIndex < 0)
  {
    return ( true);
  }
  if (typeIndex >= this->eventData->event_number_of_tx_types)
  {
    return ( true);
  }
  if (txIndex >= this->eventData->role[typeIndex]->numberOfTxs)
  {
    return ( true);
  }
  this->eventData->role[typeIndex]->tx[txIndex]->pattern = str;
  this->values_did_change = true;
  return ( false);
}

String Event::getPatternForTx(int roleIndex, int txIndex) const
{
  if (this->eventData == NULL)
  {
    return ( "");
  }
  if (roleIndex < 0)
  {
    return ( "");
  }
  if (roleIndex >= this->eventData->event_number_of_tx_types)
  {
    return ( "");
  }
  if (txIndex >= this->eventData->role[roleIndex]->numberOfTxs)
  {
    return ( "");
  }
  return ( this->eventData->role[roleIndex]->tx[txIndex]->pattern);
}

bool Event::setIDIntervalForRole(int roleIndex, String str)
{
  if (this->eventData == NULL)
  {
    return ( true);
  }
  if (roleIndex < 0)
  {
    return ( true);
  }
  if (roleIndex >= this->eventData->event_number_of_tx_types)
  {
    return ( true);
  }
  this->eventData->role[roleIndex]->id_interval = str.toInt();
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
  if (debug_prints_enabled)
  {
    Serial.println("Type" + String(roleIndex) + " freq: " + str);
  }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

  this->values_did_change = true;
  return ( false);
}

int Event::getIDIntervalForRole(int roleIndex) const
{
  if (this->eventData == NULL)
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Event data null");
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    return ( -1);
  }

  if (roleIndex < 0)
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println(String("Roleindex invalid < :") + roleIndex);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    return ( -1);
  }

  if (roleIndex >= this->eventData->event_number_of_tx_types)
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println(String("Roleindex invalid > :") + roleIndex + ">=" + this->eventData->event_number_of_tx_types);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    return ( -1);
  }

  return ( this->eventData->role[roleIndex]->id_interval);
}

/*/////////////////////////////////////////////////////////////////////////////////// */

bool Event::setEventData(String id, int value)
{
  return ( this->setEventData(id, String(value)));
}

bool Event::setEventData(String id, String value)
{
  bool result = false;
  static String typeIndexStr = "";
  static int typeIndex = 0;
  static String txIndexStr = "";
  static int txIndex = 0;

  if (id.equalsIgnoreCase(TX_ASSIGNMENT))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Tx assignment: " + value);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    this->eventData->tx_assignment = value;
  }
  else if (id.equalsIgnoreCase(TX_DESCRIPTIVE_NAME))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Tx descr name: " + value);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    this->eventData->tx_role_name = value;
  }
  else if (id.equalsIgnoreCase(TX_ASSIGNMENT_IS_DEFAULT))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Tx is default: " + value);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    if (value.equalsIgnoreCase("TRUE") || value.equals("1"))
    {
      this->eventData->tx_assignment_is_default = true;
    }
    else
    {
      this->eventData->tx_assignment_is_default = false;
    }
  }
  else if (id.equalsIgnoreCase(EVENT_NAME))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Set event_name: " + value);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    this->eventData->event_name = value;
  }
  else if (id.equalsIgnoreCase(EVENT_FILE_VERSION))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("File ver: " + value);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    this->eventData->event_file_version = value;
  }
  else if (id.equalsIgnoreCase(EVENT_BAND))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Event band: " + value);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    this->eventData->event_band = value;
  }
  else if (id.equalsIgnoreCase(EVENT_CALLSIGN))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Event callsign: " + value);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    this->eventData->event_callsign = value;
  }
  else if (id.equalsIgnoreCase(EVENT_ANTENNA_PORT))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Event antenna: " + value);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    this->eventData->event_antenna_port = value;
  }
  else if (id.equalsIgnoreCase(EVENT_CALLSIGN_SPEED))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Event ID speed: " + value);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    this->eventData->event_callsign_speed = value;
  }
  else if (id.equalsIgnoreCase(EVENT_START_DATE_TIME))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Event start: " + value);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    this->eventData->event_start_date_time = value;
  }
  else if (id.equalsIgnoreCase(EVENT_FINISH_DATE_TIME))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Event finish: " + value);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    this->eventData->event_finish_date_time = value;
  }
  else if (id.equalsIgnoreCase(EVENT_MODULATION))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Event modulation: " + value);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    this->eventData->event_modulation = value;
  }
  else if (id.equalsIgnoreCase(EVENT_NUMBER_OF_TX_TYPES))
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Event tx types: " + value);
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    this->eventData->event_number_of_tx_types = value.toInt();
  }
  else if (id.endsWith(TYPE_TX_COUNT))
  {
    typeIndexStr = id.substring((id.indexOf("TYPE") + 4), id.indexOf("_"));
    typeIndex = typeIndexStr.toInt() - 1;
    if ((typeIndex >= 0) && (typeIndex < MAXIMUM_NUMBER_OF_EVENT_TX_TYPES))
    {
      this->eventData->role[typeIndex]->numberOfTxs = value.toInt();
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (debug_prints_enabled)
      {
        Serial.println("Type" + typeIndexStr + " count: " + value);
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    }
  }
  else if (id.endsWith(TYPE_NAME))
  {
    typeIndexStr = id.substring((id.indexOf("TYPE") + 4), id.indexOf("_"));
    typeIndex = typeIndexStr.toInt() - 1;
    if ((typeIndex >= 0) && (typeIndex < MAXIMUM_NUMBER_OF_EVENT_TX_TYPES))
    {
      this->eventData->role[typeIndex]->rolename = value;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (debug_prints_enabled)
      {
        Serial.println("Type" + typeIndexStr + " role: " + value);
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    }
  }
  else if (id.endsWith(TYPE_FREQ))
  {
    typeIndexStr = id.substring((id.indexOf("TYPE") + 4), id.indexOf("_"));
    typeIndex = typeIndexStr.toInt() - 1;
    if ((typeIndex >= 0) && (typeIndex < MAXIMUM_NUMBER_OF_EVENT_TX_TYPES))
    {
      this->eventData->role[typeIndex]->frequency = value.toInt();
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (debug_prints_enabled)
      {
        Serial.println("Type" + typeIndexStr + " freq: " + value);
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    }
  }
  else if (id.endsWith(TYPE_POWER_LEVEL))
  {
    typeIndexStr = id.substring((id.indexOf("TYPE") + 4), id.indexOf("_"));
    typeIndex = typeIndexStr.toInt() - 1;
    if ((typeIndex >= 0) && (typeIndex < MAXIMUM_NUMBER_OF_EVENT_TX_TYPES))
    {
      this->eventData->role[typeIndex]->powerLevel_mW = value.toInt();
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (debug_prints_enabled)
      {
        Serial.println("Type" + typeIndexStr + " power: " + value + " mW");
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    }
  }
  else if (id.endsWith(TYPE_ID_INTERVAL))
  {
    typeIndexStr = id.substring((id.indexOf("TYPE") + 4), id.indexOf("_"));
    typeIndex = typeIndexStr.toInt() - 1;
    if ((typeIndex >= 0) && (typeIndex < MAXIMUM_NUMBER_OF_EVENT_TX_TYPES))
    {
      this->eventData->role[typeIndex]->id_interval = value.toInt();
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (debug_prints_enabled)
      {
        Serial.println("Type" + typeIndexStr + " ID interval: " + value + " sec");
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    }
  }
  else if (id.endsWith(TYPE_CODE_SPEED))
  {
    typeIndexStr = id.substring((id.indexOf("TYPE") + 4), id.indexOf("_"));
    typeIndex = typeIndexStr.toInt() - 1;
    if ((typeIndex >= 0) && (typeIndex < MAXIMUM_NUMBER_OF_EVENT_TX_TYPES))
    {
      this->eventData->role[typeIndex]->code_speed = value.toInt();
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
      if (debug_prints_enabled)
      {
        Serial.println("Type" + typeIndexStr + " code speed: " + value + " WPM");
      }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
    }
  }
  else if (id.endsWith(TYPE_TX_PATTERN))
  {
    if ((typeIndex >= 0) && (typeIndex < MAXIMUM_NUMBER_OF_EVENT_TX_TYPES))
    {
      int i = id.indexOf("TX") + 2;
      txIndexStr = id.substring(i, id.indexOf("_", i));
      txIndex = txIndexStr.toInt() - 1;

      if ((txIndex >= 0) && (txIndex < MAXIMUM_NUMBER_OF_TXs_OF_A_TYPE))
      {
        this->eventData->role[typeIndex]->tx[txIndex]->pattern = value;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
        if (debug_prints_enabled)
        {
          Serial.println("Type" + typeIndexStr + "Tx" + txIndexStr + " pattern: " + value);
        }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
      }
    }
  }
  else if (id.endsWith(TYPE_TX_ON_TIME))
  {
    if ((typeIndex >= 0) && (typeIndex < MAXIMUM_NUMBER_OF_EVENT_TX_TYPES))
    {
      int i = id.indexOf("TX") + 2;
      txIndexStr = id.substring(i, id.indexOf("_", i));
      txIndex = txIndexStr.toInt() - 1;

      if ((txIndex >= 0) && (txIndex < MAXIMUM_NUMBER_OF_TXs_OF_A_TYPE))
      {
        this->eventData->role[typeIndex]->tx[txIndex]->onTime = value;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
        if (debug_prints_enabled)
        {
          Serial.println("Type" + typeIndexStr + "Tx" + txIndexStr + " on time: " + value + "sec");
        }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
      }
    }
  }
  else if (id.endsWith(TYPE_TX_OFF_TIME))
  {
    if ((typeIndex >= 0) && (typeIndex < MAXIMUM_NUMBER_OF_EVENT_TX_TYPES))
    {
      int i = id.indexOf("TX") + 2;
      txIndexStr = id.substring(i, id.indexOf("_", i));
      txIndex = txIndexStr.toInt() - 1;

      if ((txIndex >= 0) && (txIndex < MAXIMUM_NUMBER_OF_TXs_OF_A_TYPE))
      {
        this->eventData->role[typeIndex]->tx[txIndex]->offTime = value.toInt();
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
        if (debug_prints_enabled)
        {
          Serial.println("Type" + typeIndexStr + "Tx" + txIndexStr + " off time: " + value + "sec");
        }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
      }
    }
  }
  else if (id.endsWith(TYPE_TX_DELAY_TIME))
  {
    if ((typeIndex >= 0) && (typeIndex < MAXIMUM_NUMBER_OF_EVENT_TX_TYPES))
    {
      int i = id.indexOf("TX") + 2;
      txIndexStr = id.substring(i, id.indexOf("_", i));
      txIndex = txIndexStr.toInt() - 1;

      if ((txIndex >= 0) && (txIndex < MAXIMUM_NUMBER_OF_TXs_OF_A_TYPE))
      {
        this->eventData->role[typeIndex]->tx[txIndex]->delayTime = value;
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
        if (debug_prints_enabled)
        {
          Serial.println("Type" + typeIndexStr + "Tx" + txIndexStr + " delay time: " + value + "sec");
        }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS
      }
    }
  }
  else
  {
#if TRANSMITTER_COMPILE_DEBUG_PRINTS
    if (debug_prints_enabled)
    {
      Serial.println("Error in file: EventData = " + id + " Value =[" + value + "]");
    }
#endif // TRANSMITTER_COMPILE_DEBUG_PRINTS

    result = true;
  }

  return ( result);
}
