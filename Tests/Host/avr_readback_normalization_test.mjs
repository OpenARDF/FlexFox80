#!/usr/bin/env node

import assert from "node:assert/strict";
import { normalizeAvrdudeFlashReadback } from "../../scripts/lib/flexfox-avr-readback.mjs";

const trimmed = Buffer.from([0x01, 0x02, 0x03]);
const normalized = normalizeAvrdudeFlashReadback(trimmed, 8);
assert.deepEqual([...normalized], [0x01, 0x02, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff]);
assert.deepEqual([...trimmed], [0x01, 0x02, 0x03], "normalization must not alter raw evidence");

const complete = Buffer.from([0xaa, 0xbb, 0xcc]);
assert.deepEqual([...normalizeAvrdudeFlashReadback(complete, 3)], [...complete]);
assert.throws(() => normalizeAvrdudeFlashReadback(Buffer.alloc(4), 3), /target flash is 3 bytes/);
assert.throws(() => normalizeAvrdudeFlashReadback(new Uint8Array(2), 3), /must be a Buffer/);
assert.throws(() => normalizeAvrdudeFlashReadback(Buffer.alloc(0), 0), /positive integer/);

process.stdout.write("PASS avrdude trimmed flash readbacks normalize only their erased tail\n");
