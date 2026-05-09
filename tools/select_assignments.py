#!/usr/bin/env python3
"""Compute the autograder matrix for a student push.

Reads `workspace/assignment.txt` and a list of paths changed in the push,
emits a JSON array `[{id, folder, source}, ...]` of assignments that need
re-grading. `source` is one of: "changed" (folder touched), "manifest"
(assignment.txt itself changed), "force" (force-all flag), "first-push"
(no `before` SHA available).

Manifest format — one entry per line:

    HW1 HW1mj
    HW2 HW2mj  HW2_main.c   # optional 3rd column = explicit main file

Blank lines and `#` comments are ignored.

CLI:
    select_assignments.py --manifest <path> --workspace <dir>
                          --before <sha> --head <sha>
                          [--force-all] [--changed-from-stdin]
                          [--output github|json]

`--changed-from-stdin` reads newline-separated paths from stdin instead of
running `git diff` (used by tests and by the workflow when it has already
collected the diff).

`--output github` prints `matrix=<json>` for $GITHUB_OUTPUT consumption.

Run with `--self-test` to execute the in-file unit tests (stdlib only).
"""
from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, Sequence

ZERO_SHA = "0000000000000000000000000000000000000000"


@dataclass(frozen=True)
class ManifestEntry:
    id: str
    folder: str
    main: str | None  # explicit main file, optional

    def as_dict(self, source: str) -> dict:
        d = {"id": self.id, "folder": self.folder, "source": source}
        if self.main:
            d["main"] = self.main
        return d


def parse_manifest(text: str) -> list[ManifestEntry]:
    entries: list[ManifestEntry] = []
    for raw in text.splitlines():
        line = raw.split("#", 1)[0].strip()
        if not line:
            continue
        parts = line.split()
        if len(parts) < 2:
            raise ValueError(f"manifest line needs at least 'ID FOLDER': {raw!r}")
        entry_id, folder, *rest = parts
        main = rest[0] if rest else None
        entries.append(ManifestEntry(entry_id, folder, main))
    return entries


def select(
    entries: Sequence[ManifestEntry],
    changed: Iterable[str],
    workspace: str,
    manifest_path: str,
    force_all: bool,
    first_push: bool,
) -> list[dict]:
    if force_all or first_push:
        source = "force" if force_all else "first-push"
        return [e.as_dict(source) for e in entries]

    changed_set = set(changed)
    if manifest_path in changed_set:
        return [e.as_dict("manifest") for e in entries]

    workspace_prefix = workspace.rstrip("/") + "/"
    selected: list[dict] = []
    for e in entries:
        folder_prefix = f"{workspace_prefix}{e.folder}/"
        if any(p == folder_prefix.rstrip("/") or p.startswith(folder_prefix) for p in changed_set):
            selected.append(e.as_dict("changed"))
    return selected


def git_diff(before: str, head: str, repo: Path) -> list[str]:
    out = subprocess.check_output(
        ["git", "-C", str(repo), "diff", "--name-only", f"{before}..{head}"],
        text=True,
    )
    return [line for line in out.splitlines() if line.strip()]


def main(argv: list[str] | None = None) -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--manifest", default="workspace/assignment.txt")
    p.add_argument("--workspace", default="workspace")
    p.add_argument("--before", default="")
    p.add_argument("--head", default="HEAD")
    p.add_argument("--repo", default=".")
    p.add_argument("--force-all", action="store_true")
    p.add_argument("--changed-from-stdin", action="store_true")
    p.add_argument("--output", choices=("json", "github"), default="json")
    p.add_argument("--self-test", action="store_true")
    args = p.parse_args(argv)

    if args.self_test:
        return run_self_tests()

    manifest_text = Path(args.manifest).read_text()
    entries = parse_manifest(manifest_text)

    first_push = (not args.before) or args.before == ZERO_SHA
    if args.changed_from_stdin:
        changed = [line.strip() for line in sys.stdin if line.strip()]
    elif first_push or args.force_all:
        changed = []
    else:
        changed = git_diff(args.before, args.head, Path(args.repo))

    matrix = select(
        entries,
        changed=changed,
        workspace=args.workspace,
        manifest_path=args.manifest,
        force_all=args.force_all,
        first_push=first_push,
    )

    payload = json.dumps({"include": matrix})
    if args.output == "github":
        print(f"matrix={payload}")
        print(f"any={'true' if matrix else 'false'}")
    else:
        print(payload)
    return 0


# ---------------------------------------------------------------------------
# Self-tests
# ---------------------------------------------------------------------------

def run_self_tests() -> int:
    failures: list[str] = []

    def check(name: str, condition: bool, detail: str = "") -> None:
        if not condition:
            failures.append(f"{name}: {detail}")

    entries = parse_manifest(
        """
        # comment line
        HW1 HW1mj
        HW2 HW2mj  HW2_main.c
        Lab5 Lab5
        """
    )
    check("parse count", len(entries) == 3, str(entries))
    check("parse main override", entries[1].main == "HW2_main.c")
    check("parse no main", entries[0].main is None)

    base = dict(entries=entries, workspace="workspace",
                manifest_path="workspace/assignment.txt")

    # No changes → empty matrix.
    m = select(**base, changed=[], force_all=False, first_push=False)
    check("no changes -> empty", m == [], str(m))

    # Single folder change.
    m = select(**base, changed=["workspace/HW1mj/HW1mj_main.c"],
               force_all=False, first_push=False)
    check("single folder", [e["id"] for e in m] == ["HW1"], str(m))
    check("source=changed", m and m[0]["source"] == "changed")

    # Multi folder change.
    m = select(**base,
               changed=["workspace/HW1mj/x.c", "workspace/Lab5/y.c"],
               force_all=False, first_push=False)
    check("multi folder", sorted(e["id"] for e in m) == ["HW1", "Lab5"], str(m))

    # Manifest itself changed → all.
    m = select(**base, changed=["workspace/assignment.txt"],
               force_all=False, first_push=False)
    check("manifest changed -> all",
          [e["id"] for e in m] == ["HW1", "HW2", "Lab5"], str(m))
    check("source=manifest", m and m[0]["source"] == "manifest")

    # Force all.
    m = select(**base, changed=[], force_all=True, first_push=False)
    check("force-all", len(m) == 3 and m[0]["source"] == "force")

    # First push.
    m = select(**base, changed=[], force_all=False, first_push=True)
    check("first-push", len(m) == 3 and m[0]["source"] == "first-push")

    # Path that points at a non-manifested folder is ignored.
    m = select(**base, changed=["workspace/Garbage/x.c"],
               force_all=False, first_push=False)
    check("unknown folder ignored", m == [])

    # Path that's literally the folder (no trailing slash) still matches.
    m = select(**base, changed=["workspace/HW1mj"],
               force_all=False, first_push=False)
    check("exact folder match", [e["id"] for e in m] == ["HW1"], str(m))

    if failures:
        print("FAILED:")
        for f in failures:
            print("  -", f)
        return 1
    print(f"all {12} self-tests passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
