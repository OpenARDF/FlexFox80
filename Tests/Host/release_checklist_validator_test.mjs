#!/usr/bin/env node

/*
 *  MIT License
 *
 *  Copyright (c) 2026 DigitalConfections
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
 * Characterize release-checklist phase, evidence, skip, and metadata enforcement.
 */

import { mkdtempSync, readFileSync, rmSync, writeFileSync } from "node:fs";
import { tmpdir } from "node:os";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { spawnSync } from "node:child_process";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..", "..");
const validator = join(repoRoot, "scripts", "check-release-checklist.mjs");
const templatePath = join(repoRoot, "Docs", "Software", "release-checklist-template.json");
const temporaryRoot = mkdtempSync(join(tmpdir(), "flexfox-release-checklist-"));
const testPath = join(temporaryRoot, "release-checklist.json");

function expect(condition, name) {
  if (!condition) {
    process.stderr.write(`FAIL ${name}\n`);
    process.exitCode = 1;
    return;
  }
  process.stdout.write(`PASS ${name}\n`);
}

function run(path, phase) {
  return spawnSync(process.execPath, [validator, "--file", path, "--phase", phase], {
    encoding: "utf8",
  });
}

function write(checklist) {
  writeFileSync(testPath, `${JSON.stringify(checklist, null, 2)}\n`);
}

try {
  const templateResult = run(templatePath, "template");
  expect(templateResult.status === 0, "template_structure_passes");

  const checklist = JSON.parse(readFileSync(templatePath, "utf8"));
  checklist.release = "v1.0.0";
  checklist.sourceCommit = "0123456789abcdef0123456789abcdef01234567";
  checklist.avrVersion = "0.201";
  checklist.espVersion = "2.1";

  write(checklist);
  expect(run(testPath, "candidate").status === 1, "pending_candidate_is_rejected");

  for (const item of checklist.items) {
    item.status = "done";
    item.evidence = `evidence for ${item.id}`;
  }
  write(checklist);
  expect(run(testPath, "candidate").status === 0, "candidate_with_evidence_passes");
  expect(run(testPath, "release").status === 0, "release_with_evidence_passes");
  expect(run(testPath, "final").status === 0, "final_with_evidence_passes");

  const skipped = structuredClone(checklist);
  skipped.items[0].status = "skipped";
  skipped.items[0].evidence = "";
  write(skipped);
  expect(run(testPath, "candidate").status === 1, "unattributed_skip_is_rejected");

  skipped.items[0].skipReason = "Explicitly deferred for this release";
  skipped.items[0].skipRequestedBy = "release owner";
  write(skipped);
  expect(run(testPath, "candidate").status === 0, "attributed_skip_passes");

  const wrongHardware = structuredClone(checklist);
  wrongHardware.hardwareRevision = "prototype";
  write(wrongHardware);
  expect(run(testPath, "candidate").status === 1, "unsupported_hardware_is_rejected");

  const duplicate = structuredClone(checklist);
  duplicate.items.push(structuredClone(duplicate.items[0]));
  write(duplicate);
  expect(run(testPath, "candidate").status === 1, "duplicate_item_is_rejected");
} finally {
  rmSync(temporaryRoot, { recursive: true, force: true });
}

if (process.exitCode) {
  process.exit(process.exitCode);
}
