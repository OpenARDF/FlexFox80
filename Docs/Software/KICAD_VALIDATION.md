# KiCad validation

`just kicad-check` runs schematic ERC and board DRC with KiCad 9.0.4 and compares the normalized violation inventories with [the reviewed baseline](../../scripts/kicad-validation-baseline.json).

The imported legacy design currently has 13 ERC errors, 301 ERC warnings, 39 DRC errors, and 443 DRC warnings. KiCad also warns that the legacy zone-fill strategy is converted on a best-effort basis. These findings are explicit legacy waivers, not clean-check claims. The check pins the KiCad version, severity counts, normalized violation-inventory fingerprints, and allowed stderr. Any added, removed, or changed finding and any other warning or error fails. KiCad nondeterministically selects representative objects for some legacy multiple-net-name findings, so the fingerprint uses each finding's rule, severity, and description while preserving duplicate counts.

Do not update the baseline merely to make a collector pass. A baseline change requires a focused review of both old and new reports, a documented reason for every changed finding, and the normal `Development1-KiCad` topic-branch workflow. New KiCad work should reduce the baseline where practical and must never add an unwaived warning.

The repository has no separate SPICE model or simulation gate at present. If a model or simulation is added under `KiCad/`, add its deterministic check to `just kicad-check` before promoting that work.
