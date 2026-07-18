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
 * Optional Fleet Soak filesystem operations and assignment evidence.
 */

#pragma once

#include <Arduino.h>

bool fleetSoakPageIsAvailable(bool isMaster);
bool fleetSoakSuiteIsReady(void);
void fleetSoakDeactivateSuite(void);
int fleetSoakEventFileCount(void);
int fleetSoakReservedEventCount(void);
int fleetSoakOrdinaryEventCount(void);
bool fleetSoakValidateOwnedEventFile(const String& path, String *error);
bool fleetSoakActivateInstalledSuite(String *error);
bool fleetSoakStageReceivedEvent(const String& receivedPath, const String& sourcePath, String *error);
bool fleetSoakFinalizeStagedSuite(const String& assignment, String *error);
bool fleetSoakCleanupReservedEvents(int *removedCount, String *error);
String fleetSoakAssignmentDescription(const String& assignment);
void fleetSoakOrdinaryAssignmentEvidence(String *sprintAssignment, String *classicAssignment);
void fleetSoakStoredAssignmentEvidence(String *assignment);
