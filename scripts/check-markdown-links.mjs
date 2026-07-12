#!/usr/bin/env node

import fs from "node:fs";
import path from "node:path";
import process from "node:process";

const repositoryRoot = process.cwd();
const documentationRoot = path.join(repositoryRoot, "Docs", "Software");
const markdownFiles = [];

function collectMarkdownFiles(directory) {
  for (const entry of fs.readdirSync(directory, { withFileTypes: true })) {
    const entryPath = path.join(directory, entry.name);
    if (entry.isDirectory()) {
      collectMarkdownFiles(entryPath);
    } else if (entry.isFile() && entry.name.endsWith(".md")) {
      markdownFiles.push(entryPath);
    }
  }
}

collectMarkdownFiles(documentationRoot);

const linkPattern = /!?\[[^\]]*\]\(([^)]+)\)/g;
const failures = [];
let checkedLinks = 0;

for (const markdownFile of markdownFiles) {
  const content = fs.readFileSync(markdownFile, "utf8");
  for (const match of content.matchAll(linkPattern)) {
    let target = match[1].trim();
    if (target.startsWith("<") && target.endsWith(">")) {
      target = target.slice(1, -1);
    }

    if (/^(?:[a-z]+:|#)/i.test(target)) {
      continue;
    }

    const pathWithoutAnchor = target.split("#", 1)[0];
    if (!pathWithoutAnchor) {
      continue;
    }

    checkedLinks += 1;
    let decodedPath;
    try {
      decodedPath = decodeURIComponent(pathWithoutAnchor);
    } catch {
      failures.push(`${path.relative(repositoryRoot, markdownFile)}: invalid encoded link ${target}`);
      continue;
    }

    const resolvedPath = path.resolve(path.dirname(markdownFile), decodedPath);
    if (!fs.existsSync(resolvedPath)) {
      failures.push(`${path.relative(repositoryRoot, markdownFile)}: missing ${target}`);
    }
  }
}

if (failures.length > 0) {
  for (const failure of failures) {
    console.error(failure);
  }
  process.exit(1);
}

console.log(`Checked ${checkedLinks} local links in ${markdownFiles.length} Markdown files.`);
