# Datasheet Handling SOP (No Repeat Downloads)

You must follow this workflow for any datasheet, reference manual, or PDF artifact.

## 1) Source of truth
- Primary source is local repository files, not vendor websites.
- Read `docs/datasheets/manifest.yaml` first.
- If an entry exists, treat `path` as canonical.

## 2) Zero-redundant-download rule
- Before any download attempt:
  1. Check whether `path` exists.
  2. If it exists, compute SHA-256 and compare to `sha256` in manifest.
  3. If checksum matches, DO NOT download.
- Download is allowed only if:
  - file is missing, or
  - checksum mismatch, or
  - no manifest entry exists.

## 3) On allowed download
- Download exactly once from `source_url`.
- Save only to canonical manifest path.
- Compute SHA-256.
- Update/create manifest entry with:
  - `id`, `vendor`, `path`, `revision`, `source_url`, `sha256`, `last_verified`.

## 4) Referencing in analysis/tasks
- Always cite local path and revision, never “latest online datasheet”.
- Include page anchors when relevant (example: `docs/datasheets/st/stm32f405-rev7.pdf#p=134`).
- If multiple revisions exist, explicitly state which revision is used.

## 5) Pre-extracted artifacts (preferred)
- If available, use:
  - `docs/datasheets/extracts/<id>/summary.md`
  - `docs/datasheets/extracts/<id>/tables/*.md`
- Prefer extracts for repeated queries; open PDF only when needed for verification.

## 6) Fail-safe behavior
- If manifest exists but `source_url` is dead:
  - continue using verified local copy if checksum matches.
  - report link failure, do not block progress.
- Never overwrite an existing PDF silently with a different revision.
  - Save new revision as a new filename and update manifest entry/revision.

## 7) Required output note (in every related task)
At completion, include:
- datasheet id(s) used
- revision(s)
- whether any download occurred
- whether manifest was updated
