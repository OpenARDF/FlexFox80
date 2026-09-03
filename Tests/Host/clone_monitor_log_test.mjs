import assert from "node:assert/strict";

import {
  cloneMonitorDisconnectIsExpected,
  cloneMonitorLogFileName,
  cloneMonitorLogRecord,
  cloneMonitorMessageShouldBeCaptured,
  cloneMonitorMessageType,
} from "../../scripts/lib/flexfox-clone-monitor-log.mjs";

assert.equal(cloneMonitorMessageType("CDIAG,Tx_12345678,release"), "CDIAG");
assert.equal(cloneMonitorMessageShouldBeCaptured("CDIAG,Tx_12345678,release"), true);
assert.equal(cloneMonitorMessageShouldBeCaptured("SUE,file-receive: file data timed out"), true);
assert.equal(cloneMonitorMessageShouldBeCaptured("SLAVE,0"), true);
assert.equal(cloneMonitorMessageShouldBeCaptured("SW_VERSIONS,2.34,0.210"), true);
assert.equal(cloneMonitorMessageShouldBeCaptured("FDAT,EVENT_NAME,example"), false);
assert.equal(cloneMonitorMessageShouldBeCaptured("SYNC,1788436034"), false);
assert.equal(cloneMonitorDisconnectIsExpected(true, true), true);
assert.equal(cloneMonitorDisconnectIsExpected(true, false), false);
assert.equal(cloneMonitorDisconnectIsExpected(false, true), false);

const date = new Date("2026-09-03T14:15:16.123Z");
assert.equal(cloneMonitorLogFileName(date), "clone-monitor-2026-09-03T14-15-16-123Z.jsonl");
assert.deepEqual(
  JSON.parse(cloneMonitorLogRecord("message", { message: "SLAVE,0" }, date)),
  {
    recordedAt: "2026-09-03T14:15:16.123Z",
    kind: "message",
    message: "SLAVE,0",
  },
);

console.log("PASS clone monitor logs retain diagnostics without event data or sync noise");
