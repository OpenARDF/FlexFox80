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
 * Verify license and module-purpose headers on project-owned production firmware.
 */

import { readFileSync, readdirSync } from "node:fs";
import { dirname, join, relative, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const avrRoot = join(repoRoot, "Software", "AVR128DA48", "FlexFox80");
const espRoot = join(repoRoot, "Software", "Huzzah", "ARDF_Transmitter");

const externalAvrHeaders = new Set([
  "atmel_start_pins.h",
  "protected_io.h",
  "rstctrl.h",
  "system.h",
]);

const sourceFiles = [
  join(avrRoot, "defs.h"),
  join(avrRoot, "main.cpp"),
  ...readdirSync(join(avrRoot, "include"))
    .filter((name) => name.endsWith(".h") && !externalAvrHeaders.has(name))
    .map((name) => join(avrRoot, "include", name)),
  ...readdirSync(join(avrRoot, "src"))
    .filter((name) => name.endsWith(".cpp"))
    .map((name) => join(avrRoot, "src", name)),
  ...readdirSync(espRoot)
    .filter((name) => /\.(?:cpp|h|ino)$/.test(name))
    .map((name) => join(espRoot, name)),
].sort();

const licensePrefix = "/*\n *  MIT License\n *\n *  Copyright (c) ";
const licenseSuffix =
  "\n *\n *  Permission is hereby granted, free of charge, to any person obtaining a copy\n" +
  " *  of this software and associated documentation files (the \"Software\"), to deal\n" +
  " *  in the Software without restriction, including without limitation the rights\n" +
  " *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n" +
  " *  copies of the Software, and to permit persons to whom the Software is\n" +
  " *  furnished to do so, subject to the following conditions:\n" +
  " *\n" +
  " *  The above copyright notice and this permission notice shall be included in all\n" +
  " *  copies or substantial portions of the Software.\n" +
  " *\n" +
  " *  THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n" +
  " *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n" +
  " *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n" +
  " *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n" +
  " *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n" +
  " *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n" +
  " *  SOFTWARE.\n" +
  " */\n\n";

const failures = [];

for (const path of sourceFiles) {
  const source = readFileSync(path, "utf8");
  const displayPath = relative(repoRoot, path);

  if (!source.startsWith(licensePrefix)) {
    failures.push(`${displayPath}: missing standardized MIT header`);
    continue;
  }

  const copyright = source
    .slice(licensePrefix.length)
    .match(/^(\d{4}(?:-2026)?) (?:DigitalConfections|Digital Confections LLC)/);
  if (!copyright) {
    failures.push(`${displayPath}: missing dated DigitalConfections copyright holder`);
  }

  const licenseEnd = source.indexOf(licenseSuffix, licensePrefix.length);
  if (licenseEnd < 0) {
    failures.push(`${displayPath}: MIT permission text differs from the project standard`);
    continue;
  }

  const moduleHeader = source.slice(licenseEnd + licenseSuffix.length);
  if (!moduleHeader.startsWith("/*\n * ")) {
    failures.push(`${displayPath}: missing module-purpose header after license`);
  }
}

if (failures.length) {
  process.stderr.write(`${failures.join("\n")}\n`);
  process.exit(1);
}

process.stdout.write(`PASS ${sourceFiles.length} project-owned firmware files have standardized headers\n`);
