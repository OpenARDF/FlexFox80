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
 * Transactional Fleet Soak activation, target provisioning, and exact cleanup.
 */

#include "FleetSoak.h"

#include <LittleFS.h>

#include "Event.h"
#include "fleet_soak.h"

namespace
{
const char *const readyStagingPath = "/fleet-soak.ready.tmp";
const char *const assignmentStagingPath = "/.fleet-soak-me.tmp";
const char *const assignmentBackupPath = "/.fleet-soak-me.bak";

bool setError(String *error, const String& message)
{
  if (error)
  {
    *error = message;
  }
  return false;
}

bool eventHasOwnedLayout(const String& path, int expectedIndex)
{
  File file = LittleFS.open(path, "r");
  if (!file)
  {
    return false;
  }

  const int sequence = expectedIndex + 1;
  const String expectedVersion = String(EVENT_FILE_VERSION) + ", \"" + FLEET_SOAK_EVENT_VERSION + "\"";
  const String expectedNamePrefix = String("EVENT_NAME, \"Fleet Soak ") +
                                    (sequence < 10 ? "0" : "") + String(sequence) + " " +
                                    (expectedIndex % 2 ? "Classic " : "Sprint ");
  const char *const expectedCounts[] = {
    "TYPE1_TX_COUNT, 5",
    "TYPE2_TX_COUNT, 5",
    "TYPE3_TX_COUNT, 1",
    "TYPE4_TX_COUNT, 1"
  };
  const char *const expectedFrequencies[] = {
    "TYPE1_FREQ, 3520000",
    "TYPE2_FREQ, 3560000",
    "TYPE3_FREQ, 3540000",
    "TYPE4_FREQ, 3600000"
  };
  bool foundVersion = false;
  bool foundName = false;
  bool foundRoleCount = false;
  bool foundCounts[4] = {false, false, false, false};
  bool foundFrequencies[4] = {false, false, false, false};
  int lines = 0;
  while (file.available() && lines++ < MAXIMUM_NUMBER_OF_EVENT_FILE_LINES)
  {
    yield();
    String line = file.readStringUntil('\n');
    line.trim();
    foundVersion = foundVersion || line.equals(expectedVersion);
    foundName = foundName || line.startsWith(expectedNamePrefix);
    foundRoleCount = foundRoleCount || line.equals("EVENT_NUMBER_OF_TX_TYPES, 4");
    for (size_t role = 0; role < 4; role++)
    {
      foundCounts[role] = foundCounts[role] || line.equals(expectedCounts[role]);
      foundFrequencies[role] = foundFrequencies[role] || line.equals(expectedFrequencies[role]);
    }
  }
  file.close();
  if (!foundVersion || !foundName || !foundRoleCount)
  {
    return false;
  }
  for (size_t role = 0; role < 4; role++)
  {
    if (!foundCounts[role] || !foundFrequencies[role])
    {
      return false;
    }
  }
  return true;
}

String readStoredAssignment(const char *path)
{
  File file = LittleFS.open(path, "r");
  if (!file)
  {
    return "";
  }
  String assignment;
  int lines = 0;
  const String prefix = String(TX_ASSIGNMENT) + ",";
  while (file.available() && lines++ < MAXIMUM_NUMBER_OF_ME_FILE_LINES)
  {
    String line = file.readStringUntil('\n');
    line.trim();
    if (line.startsWith(prefix))
    {
      assignment = line.substring(prefix.length());
      assignment.trim();
      break;
    }
  }
  file.close();
  return fleetSoakAssignmentIsValid(assignment.c_str()) ? assignment : String("");
}

bool writeReadyMarker(String *error)
{
  /* The marker is the scheduler's commit point and is always written last. */
  LittleFS.remove(readyStagingPath);
  File file = LittleFS.open(readyStagingPath, "w");
  if (!file)
  {
    return setError(error, "Could not create Fleet Soak ready marker");
  }
  file.println(FLEET_SOAK_READY_VALUE);
  file.close();
  LittleFS.remove(FLEET_SOAK_READY_PATH);
  if (!LittleFS.rename(readyStagingPath, FLEET_SOAK_READY_PATH))
  {
    LittleFS.remove(readyStagingPath);
    return setError(error, "Could not commit Fleet Soak ready marker");
  }
  return true;
}

bool writeAssignmentFile(const char *path, const String& assignment, String *error)
{
  /*
   * Event::saveMeData() has no failure result or rollback path. Provisioning
   * needs both, so retain the established .me format but commit it by rename.
   */
  LittleFS.remove(assignmentStagingPath);
  File file = LittleFS.open(assignmentStagingPath, "w");
  if (!file)
  {
    return setError(error, "Could not stage Fleet Soak assignment");
  }
  file.println(EVENT_FILE_START);
  file.println(String(TX_ASSIGNMENT) + "," + assignment);
  file.println(String(TX_DESCRIPTIVE_NAME) + "," + fleetSoakAssignmentDescription(assignment));
  file.println(String(TX_ASSIGNMENT_IS_DEFAULT) + ",false");
  file.println(EVENT_FILE_END);
  file.close();

  LittleFS.remove(assignmentBackupPath);
  const bool hadAssignment = LittleFS.exists(path);
  if (hadAssignment && !LittleFS.rename(path, assignmentBackupPath))
  {
    LittleFS.remove(assignmentStagingPath);
    return setError(error, String("Could not preserve assignment ") + path);
  }
  if (!LittleFS.rename(assignmentStagingPath, path))
  {
    LittleFS.remove(assignmentStagingPath);
    if (hadAssignment)
    {
      LittleFS.rename(assignmentBackupPath, path);
    }
    return setError(error, String("Could not commit assignment ") + path);
  }
  LittleFS.remove(assignmentBackupPath);
  return true;
}

bool validateOwnedEventFileAt(const String& path, int expectedIndex, String *error)
{
  if (expectedIndex < 0 || expectedIndex >= FLEET_SOAK_EVENT_COUNT)
  {
    return setError(error, "Fleet Soak event index is invalid");
  }
  if (!LittleFS.exists(path) || !Event::validEventFile(path))
  {
    return setError(error, String("Invalid Fleet Soak event: ") + path);
  }
  /* Event supplies the mature framing/parser validation; this pass adds ownership checks. */
  if (!eventHasOwnedLayout(path, expectedIndex))
  {
    return setError(error, String("Fleet Soak ownership or role layout mismatch: ") + path);
  }
  return true;
}

void recordAssignmentEvidence(String *evidence, const String& assignment)
{
  if (!evidence || !assignment.length())
  {
    return;
  }
  if (!evidence->length())
  {
    *evidence = assignment;
  }
  else if (!evidence->equals(assignment))
  {
    *evidence = "mixed";
  }
}

bool requireFleetSoakSuiteCapacity(String *error)
{
  if (fleetSoakOrdinaryEventCount() + FLEET_SOAK_EVENT_COUNT <= MAXIMUM_NUMBER_OF_EVENTS)
  {
    return true;
  }
  return setError(error, String("Fleet Soak suite would exceed the ") +
                         MAXIMUM_NUMBER_OF_EVENTS + "-event capacity");
}
} // namespace

bool fleetSoakPageIsAvailable(bool isMaster)
{
  return isMaster && LittleFS.exists(FLEET_SOAK_PAGE_PATH);
}

bool fleetSoakSuiteIsReady(void)
{
  if (!LittleFS.exists(FLEET_SOAK_READY_PATH))
  {
    return false;
  }
  File marker = LittleFS.open(FLEET_SOAK_READY_PATH, "r");
  if (!marker)
  {
    return false;
  }
  String value = marker.readStringUntil('\n');
  marker.close();
  value.trim();
  if (!value.equals(FLEET_SOAK_READY_VALUE))
  {
    return false;
  }
  for (size_t index = 0; index < FLEET_SOAK_EVENT_COUNT; index++)
  {
    if (!LittleFS.exists(FLEET_SOAK_EVENT_PATHS[index]))
    {
      return false;
    }
  }
  return true;
}

void fleetSoakDeactivateSuite(void)
{
  LittleFS.remove(FLEET_SOAK_READY_PATH);
  LittleFS.remove(readyStagingPath);
}

int fleetSoakEventFileCount(void)
{
  int count = 0;
  Dir dir = LittleFS.openDir("/");
  while (dir.next())
  {
    yield();
    if (dir.fileName().endsWith(".event"))
    {
      count++;
    }
  }
  return count;
}

int fleetSoakReservedEventCount(void)
{
  int count = 0;
  for (size_t index = 0; index < FLEET_SOAK_EVENT_COUNT; index++)
  {
    if (LittleFS.exists(FLEET_SOAK_EVENT_PATHS[index]))
    {
      count++;
    }
  }
  return count;
}

int fleetSoakOrdinaryEventCount(void)
{
  int count = 0;
  Dir dir = LittleFS.openDir("/");
  while (dir.next())
  {
    yield();
    const String path = dir.fileName();
    if (path.endsWith(".event") && !fleetSoakIsReservedEventPath(path.c_str()))
    {
      count++;
    }
  }
  return count;
}

bool fleetSoakValidateOwnedEventFile(const String& path, String *error)
{
  const int index = fleetSoakEventIndex(path.c_str());
  if (index < 0)
  {
    return setError(error, "File is outside the Fleet Soak reserved namespace");
  }
  return validateOwnedEventFileAt(path, index, error);
}

bool fleetSoakActivateInstalledSuite(String *error)
{
  fleetSoakDeactivateSuite();
  if (!requireFleetSoakSuiteCapacity(error))
  {
    return false;
  }
  for (size_t index = 0; index < FLEET_SOAK_EVENT_COUNT; index++)
  {
    if (!fleetSoakValidateOwnedEventFile(FLEET_SOAK_EVENT_PATHS[index], error))
    {
      return false;
    }
  }
  return writeReadyMarker(error);
}

bool fleetSoakStageReceivedEvent(const String& receivedPath, const String& sourcePath, String *error)
{
  const int index = fleetSoakEventIndex(receivedPath.c_str());
  if (index < 0)
  {
    return setError(error, "Received file is outside the Fleet Soak reserved namespace");
  }
  if (!LittleFS.exists(sourcePath))
  {
    return setError(error, "Received Fleet Soak staging file is missing");
  }
  fleetSoakDeactivateSuite();
  LittleFS.remove(FLEET_SOAK_STAGING_PATHS[index]);
  if (!LittleFS.rename(sourcePath, FLEET_SOAK_STAGING_PATHS[index]))
  {
    return setError(error, String("Could not stage ") + receivedPath);
  }
  return true;
}

bool fleetSoakFinalizeStagedSuite(const String& assignment, String *error)
{
  if (!fleetSoakAssignmentIsValid(assignment.c_str()))
  {
    return setError(error, "Fleet Soak assignment is invalid");
  }
  if (!requireFleetSoakSuiteCapacity(error))
  {
    return false;
  }

  fleetSoakDeactivateSuite();
  /* Validate the complete inert staging set before changing any active event path. */
  for (size_t index = 0; index < FLEET_SOAK_EVENT_COUNT; index++)
  {
    if (!LittleFS.exists(FLEET_SOAK_STAGING_PATHS[index]))
    {
      return setError(error, String("Fleet Soak staging set is incomplete at file ") + String(index + 1));
    }
    if (!validateOwnedEventFileAt(FLEET_SOAK_STAGING_PATHS[index], (int)index, error))
    {
      return false;
    }
    if (LittleFS.exists(FLEET_SOAK_EVENT_PATHS[index]) &&
        !fleetSoakValidateOwnedEventFile(FLEET_SOAK_EVENT_PATHS[index], error))
    {
      return false;
    }
  }

  for (size_t index = 0; index < FLEET_SOAK_EVENT_COUNT; index++)
  {
    if (!writeAssignmentFile(FLEET_SOAK_ME_PATHS[index], assignment, error))
    {
      return false;
    }
  }
  for (size_t index = 0; index < FLEET_SOAK_EVENT_COUNT; index++)
  {
    LittleFS.remove(FLEET_SOAK_EVENT_PATHS[index]);
    if (!LittleFS.rename(FLEET_SOAK_STAGING_PATHS[index], FLEET_SOAK_EVENT_PATHS[index]))
    {
      return setError(error, String("Could not activate Fleet Soak file ") + String(index + 1));
    }
  }
  for (size_t index = 0; index < FLEET_SOAK_EVENT_COUNT; index++)
  {
    if (!fleetSoakValidateOwnedEventFile(FLEET_SOAK_EVENT_PATHS[index], error))
    {
      return false;
    }
  }
  /* A failed promotion remains inert because the ready marker is still absent. */
  return writeReadyMarker(error);
}

bool fleetSoakCleanupReservedEvents(int *removedCount, String *error)
{
  if (removedCount)
  {
    *removedCount = 0;
  }
  fleetSoakDeactivateSuite();

  /* Prove ownership of every existing reserved name before deleting any of them. */
  for (size_t index = 0; index < FLEET_SOAK_EVENT_COUNT; index++)
  {
    if (LittleFS.exists(FLEET_SOAK_EVENT_PATHS[index]) &&
        !fleetSoakValidateOwnedEventFile(FLEET_SOAK_EVENT_PATHS[index], error))
    {
      return false;
    }
  }
  for (size_t index = 0; index < FLEET_SOAK_EVENT_COUNT; index++)
  {
    /* Staging files are disposable; matching .me assignments are intentionally retained. */
    LittleFS.remove(FLEET_SOAK_STAGING_PATHS[index]);
    if (LittleFS.exists(FLEET_SOAK_EVENT_PATHS[index]))
    {
      if (!LittleFS.remove(FLEET_SOAK_EVENT_PATHS[index]))
      {
        return setError(error, String("Could not remove ") + FLEET_SOAK_EVENT_PATHS[index]);
      }
      if (removedCount)
      {
        (*removedCount)++;
      }
    }
  }
  return true;
}

String fleetSoakAssignmentDescription(const String& assignment)
{
  if (!fleetSoakAssignmentIsValid(assignment.c_str()))
  {
    return "Invalid Fleet Soak assignment";
  }
  const int role = assignment.substring(0, 1).toInt();
  const int slot = assignment.substring(2).toInt();
  if (role == 0)
  {
    return String("Fleet Soak 3.52 MHz slot ") + String(slot + 1);
  }
  if (role == 1)
  {
    return String("Fleet Soak 3.56 MHz slot ") + String(slot + 1);
  }
  return role == 2 ? String("Fleet Soak spectator 3.54 MHz") : String("Fleet Soak beacon 3.60 MHz");
}

void fleetSoakOrdinaryAssignmentEvidence(String *sprintAssignment, String *classicAssignment)
{
  if (sprintAssignment)
  {
    *sprintAssignment = "";
  }
  if (classicAssignment)
  {
    *classicAssignment = "";
  }
  Event event(false);
  Dir dir = LittleFS.openDir("/");
  while (dir.next())
  {
    yield();
    String path = dir.fileName();
    if (!path.endsWith(".event") || fleetSoakIsReservedEventPath(path.c_str()))
    {
      continue;
    }
    String lowerPath = path;
    lowerPath.toLowerCase();
    const bool isSprint = lowerPath.indexOf("sprint") >= 0;
    const bool isClassic = lowerPath.indexOf("classic") >= 0;
    if (!isSprint && !isClassic)
    {
      continue;
    }
    if (!event.readEventFile(path))
    {
      if (isSprint)
      {
        recordAssignmentEvidence(sprintAssignment, event.getTxAssignment());
      }
      if (isClassic)
      {
        recordAssignmentEvidence(classicAssignment, event.getTxAssignment());
      }
    }
  }
}

void fleetSoakStoredAssignmentEvidence(String *assignment)
{
  if (assignment)
  {
    *assignment = "";
  }
  for (size_t index = 0; index < FLEET_SOAK_EVENT_COUNT; index++)
  {
    if (!LittleFS.exists(FLEET_SOAK_ME_PATHS[index]))
    {
      continue;
    }
    recordAssignmentEvidence(assignment, readStoredAssignment(FLEET_SOAK_ME_PATHS[index]));
  }
}
