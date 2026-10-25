#!/usr/bin/env node

import { createHash } from "node:crypto";
import { existsSync } from "node:fs";
import { mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { spawnSync } from "node:child_process";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const baselinePath = join(repoRoot, "scripts", "kicad-validation-baseline.json");
const baseline = JSON.parse(await readFile(baselinePath, "utf8"));
const macKicadCli = "/Applications/KiCad/KiCad.app/Contents/MacOS/kicad-cli";
const kicadCli = process.env.KICAD_CLI || (existsSync(macKicadCli) ? macKicadCli : "kicad-cli");
const workDir = await mkdtemp(join(tmpdir(), "flexfox-kicad-check-"));

function normalizedDiagnosticLines(stderr) {
  return stderr
    .split(/\r?\n/)
    .map((line) => line.replace(/^\d{2}:\d{2}:\d{2}[^:]*:\s*/, "").trim())
    .filter(Boolean);
}

function fingerprint(violations) {
  // KiCad can choose a different representative item for the same legacy
  // multiple-net-name violation. Its stable identity excludes that item.
  const inventory = violations
    .map(({ type, severity, description }) => ({ type, severity, description }))
    .sort((left, right) => JSON.stringify(left).localeCompare(JSON.stringify(right)));
  return createHash("sha256")
    .update(JSON.stringify(inventory))
    .digest("hex");
}

function countSeverities(violations) {
  const counts = violations.reduce((result, violation) => {
    result[violation.severity] = (result[violation.severity] || 0) + 1;
    return result;
  }, {});
  return Object.fromEntries(Object.entries(counts).sort(([left], [right]) => left.localeCompare(right)));
}

function runCheck(name, args, outputPath, expectedDiagnostics) {
  const result = spawnSync(kicadCli, [...args, "--format", "json", "--output", outputPath], {
    cwd: repoRoot,
    encoding: "utf8",
    env: {
      ...process.env,
      ...(process.platform === "darwin" ? { FONTCONFIG_FILE: join(workDir, "fonts.conf") } : {}),
    },
  });
  if (result.error) throw new Error(`${name} could not start: ${result.error.message}`);
  if (result.status !== 0) {
    throw new Error(`${name} exited ${result.status}:\n${result.stderr || result.stdout}`);
  }
  const diagnostics = normalizedDiagnosticLines(result.stderr);
  if (JSON.stringify(diagnostics) !== JSON.stringify(expectedDiagnostics)) {
    throw new Error(`${name} emitted unwaived diagnostics:\n${diagnostics.join("\n") || "(none)"}`);
  }
}

try {
  if (process.platform === "darwin") {
    await writeFile(
      join(workDir, "fonts.conf"),
      "<?xml version=\"1.0\"?><!DOCTYPE fontconfig SYSTEM \"fonts.dtd\"><fontconfig><dir>/System/Library/Fonts</dir><cachedir>/tmp</cachedir></fontconfig>\n",
    );
  }

  const version = spawnSync(kicadCli, ["--version"], { encoding: "utf8" });
  if (version.error || version.status !== 0) {
    throw new Error(`kicad-cli version check failed: ${version.error?.message || version.stderr}`);
  }
  const actualVersion = version.stdout.trim();
  if (actualVersion !== baseline.kicadVersion) {
    throw new Error(`kicad-cli ${actualVersion} does not match baseline ${baseline.kicadVersion}`);
  }

  const ercPath = join(workDir, "erc.json");
  const drcPath = join(workDir, "drc.json");
  runCheck("ERC", ["sch", "erc", "KiCad/FlexFox80.kicad_sch"], ercPath, baseline.erc.allowedDiagnostics);
  runCheck("DRC", ["pcb", "drc", "KiCad/FlexFox80.kicad_pcb"], drcPath, baseline.drc.allowedDiagnostics);

  const erc = JSON.parse(await readFile(ercPath, "utf8"));
  const drc = JSON.parse(await readFile(drcPath, "utf8"));
  const ercViolations = erc.sheets.flatMap((sheet) => sheet.violations || []);
  const actual = {
    erc: { severities: countSeverities(ercViolations), fingerprint: fingerprint(ercViolations) },
    drc: {
      severities: countSeverities(drc.violations || []),
      unconnected: (drc.unconnected_items || []).length,
      fingerprint: fingerprint(drc.violations || []),
    },
  };

  if (process.argv.includes("--print-baseline")) {
    console.log(JSON.stringify(actual, null, 2));
  } else {
    for (const name of ["erc", "drc"]) {
      for (const field of ["severities", "unconnected", "fingerprint"]) {
        if (!(field in baseline[name])) continue;
        if (JSON.stringify(actual[name][field]) !== JSON.stringify(baseline[name][field])) {
          throw new Error(
            `${name.toUpperCase()} ${field} changed: expected ${JSON.stringify(baseline[name][field])}, got ${JSON.stringify(actual[name][field])}`,
          );
        }
      }
      console.log(`PASS ${name.toUpperCase()} matches reviewed legacy waiver: ${JSON.stringify(actual[name].severities)}`);
    }
  }
} finally {
  await rm(workDir, { recursive: true, force: true });
}
