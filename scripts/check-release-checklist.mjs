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
 * Enforce evidence-bearing FlexFox release checklist gates adapted from SignalSlinger.
 */

import { readFileSync } from "node:fs";
import process from "node:process";

const candidateItems = [
  "branch-announced",
  "working-tree-reviewed",
  "release-channel",
  "release-label-and-firmware-pair",
  "hardware-revision",
  "source-commit-frozen",
  "full-repository-check",
  "deterministic-avr-build",
  "deterministic-esp-build",
  "artifact-manifest",
  "package-validation",
  "hardware-checklist",
  "live-version-report",
  "rollback-package",
  "release-notes",
];

const releaseItems = [
  ...candidateItems,
  "integration-approved",
  "release-branch-verified",
  "release-approved",
];

const finalItems = [
  ...releaseItems,
  "annotated-tag",
  "release-record-created",
  "release-assets-archived",
  "archived-release-verified",
];

const requiredByPhase = {
  template: finalItems,
  candidate: candidateItems,
  release: releaseItems,
  final: finalItems,
};

function usage() {
  process.stderr.write(
    "Usage: node ./scripts/check-release-checklist.mjs --file <checklist.json> " +
      "--phase <template|candidate|release|final>\n",
  );
}

function parseArgs(argv) {
  const args = {};

  for (let index = 2; index < argv.length; index += 1) {
    const arg = argv[index];

    if (arg === "--file") {
      args.file = argv[index + 1];
      index += 1;
    } else if (arg === "--phase") {
      args.phase = argv[index + 1];
      index += 1;
    } else if (arg === "--help" || arg === "-h") {
      args.help = true;
    } else {
      throw new Error(`Unknown argument: ${arg}`);
    }
  }

  return args;
}

function isNonEmptyString(value) {
  return typeof value === "string" && value.trim().length > 0;
}

function itemIsComplete(item) {
  if (item.status === "done") {
    return isNonEmptyString(item.evidence);
  }

  if (item.status === "skipped") {
    return isNonEmptyString(item.skipReason) && isNonEmptyString(item.skipRequestedBy);
  }

  return false;
}

function validateMetadata(checklist, templateMode, failures) {
  if (checklist.repository !== "OpenARDF/FlexFox80") {
    failures.push("repository: expected OpenARDF/FlexFox80");
  }

  if (!["stable", "prerelease"].includes(checklist.channel)) {
    failures.push("channel: expected stable or prerelease");
  }

  if (checklist.hardwareRevision !== "Ver 2.1 (Mar 2022)") {
    failures.push("hardwareRevision: expected the sole supported target Ver 2.1 (Mar 2022)");
  }

  for (const name of ["release", "sourceCommit", "hardwareRevision", "avrVersion", "espVersion"]) {
    if (!isNonEmptyString(checklist[name]) && !(templateMode && name !== "release")) {
      failures.push(`${name}: missing release metadata`);
    }
  }

  if (!templateMode) {
    if (/X/i.test(checklist.release) || /X/i.test(checklist.avrVersion) || /X/i.test(checklist.espVersion)) {
      failures.push("release versions: replace all template placeholders");
    }

    if (isNonEmptyString(checklist.sourceCommit) && !/^[0-9a-f]{40}$/.test(checklist.sourceCommit)) {
      failures.push("sourceCommit: expected the full 40-character lowercase Git commit ID");
    }
  }
}

try {
  const args = parseArgs(process.argv);

  if (args.help) {
    usage();
    process.exit(0);
  }

  if (!args.file || !args.phase) {
    usage();
    process.exit(2);
  }

  const requiredIds = requiredByPhase[args.phase];
  if (!requiredIds) {
    throw new Error(`Unknown phase '${args.phase}'. Expected: ${Object.keys(requiredByPhase).join(", ")}`);
  }

  const checklist = JSON.parse(readFileSync(args.file, "utf8"));
  if (!Array.isArray(checklist.items)) {
    throw new Error("Checklist must contain an 'items' array.");
  }

  const templateMode = args.phase === "template";
  const failures = [];
  validateMetadata(checklist, templateMode, failures);

  const itemsById = new Map();
  for (const item of checklist.items) {
    if (!isNonEmptyString(item.id)) {
      failures.push("item: missing id");
      continue;
    }
    if (itemsById.has(item.id)) {
      failures.push(`${item.id}: duplicate checklist item`);
    }
    if (!isNonEmptyString(item.description)) {
      failures.push(`${item.id}: missing description`);
    }
    itemsById.set(item.id, item);
  }

  for (const id of requiredIds) {
    const item = itemsById.get(id);
    if (!item) {
      failures.push(`${id}: missing from checklist`);
      continue;
    }

    if (templateMode) {
      if (item.status !== "pending" || item.evidence !== "") {
        failures.push(`${id}: template items must start pending with empty evidence`);
      }
    } else if (!itemIsComplete(item)) {
      failures.push(
        `${id}: mark done with evidence, or skipped with skipReason and skipRequestedBy`,
      );
    }
  }

  if (failures.length) {
    process.stderr.write(`Release checklist is invalid for phase '${args.phase}':\n`);
    for (const failure of failures) {
      process.stderr.write(`- ${failure}\n`);
    }
    process.exit(1);
  }

  process.stdout.write(
    `PASS release checklist phase '${args.phase}' (${requiredIds.length} items checked)\n`,
  );
} catch (error) {
  process.stderr.write(`${error instanceof Error ? error.message : String(error)}\n`);
  process.exit(2);
}
