#!/usr/bin/env python3
"""Render grader JSON results into student-facing markdown + JSON.

Inputs (CLI):
    --results-dir <dir>    directory containing <id>.json (one per graded
                           assignment) and optional <id>.log files.
    --manifest <path>      workspace/assignment.txt — used to render the
                           cumulative index even for assignments not graded
                           this run.
    --report-dir <dir>     output directory inside the student repo
                           (e.g. autograder/). Per-assignment files are
                           updated in place; entries not in this run are
                           preserved verbatim.
    --commit <sha>         commit SHA of the run (for index + history).
    --run-url <url>        link to the GitHub Actions run (optional).
    --step-summary <path>  if set, also write a Markdown summary suitable
                           for $GITHUB_STEP_SUMMARY.

Each <id>.json is the format emitted by AutomaticGrader --report-json:
    {
      "assignment": "HW1",
      "passed": 6, "failed": 1,
      "checks": [{"name": "...", "status": "pass|fail",
                  "messages": ["..."]}, ...]
    }
"""
from __future__ import annotations

import argparse
import datetime as dt
import json
import sys
from pathlib import Path

HISTORY_LIMIT = 50
STATUS_EMOJI = {"pass": "✅", "fail": "❌", "skip": "⚪"}


def _now_iso() -> str:
    return dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def _now_short() -> str:
    return dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%MZ")


def load_manifest(path: Path) -> list[tuple[str, str]]:
    out: list[tuple[str, str]] = []
    for raw in path.read_text().splitlines():
        line = raw.split("#", 1)[0].strip()
        if not line:
            continue
        parts = line.split()
        if len(parts) >= 2:
            out.append((parts[0], parts[1]))
    return out


def render_assignment_md(report: dict, log_text: str | None) -> str:
    aid = report["assignment"]
    passed = report.get("passed", 0)
    failed = report.get("failed", 0)
    total = passed + failed
    badge = "✅" if failed == 0 and total > 0 else ("❌" if failed > 0 else "⚪")
    lines = [
        f"# {aid} — {badge} {passed}/{total} passed",
        "",
        "| Check | Status | Detail |",
        "|---|---|---|",
    ]
    for c in report.get("checks", []):
        emoji = STATUS_EMOJI.get(c.get("status"), "?")
        msgs = c.get("messages") or []
        # Keep the first error/warn line as the visible detail.
        detail = ""
        for m in msgs:
            low = m.lower()
            if "[error]" in low or "[warning]" in low:
                detail = m.strip()
                break
        if not detail and msgs:
            detail = msgs[0].strip()
        detail = detail.replace("|", "\\|").replace("\n", " ")
        if len(detail) > 160:
            detail = detail[:157] + "..."
        lines.append(f"| `{c['name']}` | {emoji} {c['status'].upper()} | {detail or '—'} |")
    lines.append("")
    if log_text:
        lines.append("<details><summary>Full grader log</summary>")
        lines.append("")
        lines.append("```")
        lines.append(log_text.rstrip())
        lines.append("```")
        lines.append("")
        lines.append("</details>")
    return "\n".join(lines) + "\n"


def render_index_md(rows: list[dict], commit: str, run_url: str | None) -> str:
    lines = [
        "# Autograder report",
        "",
        f"_Last update: {_now_short()}_  "
        f"_Commit: `{commit[:8]}`_"
        + (f"  _[Run]({run_url})_" if run_url else ""),
        "",
        "| Assignment | Folder | Status | Passed / Total | Last graded | Detail |",
        "|---|---|---|---|---|---|",
    ]
    for r in rows:
        emoji = STATUS_EMOJI.get(r["status"], "⚪")
        passed = r.get("passed", 0)
        total = r.get("total", 0)
        last = r.get("last_commit", "—")
        last_short = last[:8] if last and last != "—" else "—"
        detail_link = f"[{r['id']}.md]({r['id']}.md)" if r.get("has_md") else "—"
        lines.append(
            f"| {r['id']} | `{r['folder']}` | {emoji} {r['status'].upper()} "
            f"| {passed}/{total} | `{last_short}` | {detail_link} |"
        )
    lines.append("")
    lines.append("Per-assignment details and full logs are in the matching "
                 "`<assignment>.md` files. Per-run history lives under "
                 "`history/<assignment>.jsonl`.")
    return "\n".join(lines) + "\n"


def render_step_summary(reports: list[dict]) -> str:
    parts: list[str] = []
    if not reports:
        return "_Autograder: no assignments selected for this run._\n"
    for r in reports:
        passed = r.get("passed", 0)
        failed = r.get("failed", 0)
        total = passed + failed
        badge = "✅" if failed == 0 and total > 0 else "❌"
        parts.append(f"### {r['assignment']} — {badge} {passed}/{total} passed")
        parts.append("")
        parts.append("| Check | Status | Detail |")
        parts.append("|---|---|---|")
        for c in r.get("checks", []):
            emoji = STATUS_EMOJI.get(c.get("status"), "?")
            detail = ""
            for m in c.get("messages") or []:
                low = m.lower()
                if "[error]" in low or "[warning]" in low:
                    detail = m.strip()
                    break
            detail = detail.replace("|", "\\|").replace("\n", " ")
            if len(detail) > 200:
                detail = detail[:197] + "..."
            parts.append(f"| `{c['name']}` | {emoji} | {detail or '—'} |")
        parts.append("")
    return "\n".join(parts) + "\n"


def append_history(history_path: Path, entry: dict) -> None:
    history_path.parent.mkdir(parents=True, exist_ok=True)
    existing: list[str] = []
    if history_path.exists():
        existing = history_path.read_text().splitlines()
    existing.append(json.dumps(entry, sort_keys=True))
    if len(existing) > HISTORY_LIMIT:
        existing = existing[-HISTORY_LIMIT:]
    history_path.write_text("\n".join(existing) + "\n")


def read_existing_index_state(report_dir: Path, manifest: list[tuple[str, str]]) -> dict[str, dict]:
    """Pick up prior status for assignments not graded this run."""
    state: dict[str, dict] = {}
    for aid, folder in manifest:
        json_path = report_dir / f"{aid}.json"
        md_path = report_dir / f"{aid}.md"
        if json_path.exists():
            try:
                data = json.loads(json_path.read_text())
                passed = data.get("passed", 0)
                failed = data.get("failed", 0)
                total = passed + failed
                status = "pass" if failed == 0 and total > 0 else ("fail" if failed > 0 else "skip")
                state[aid] = {
                    "id": aid, "folder": folder, "status": status,
                    "passed": passed, "total": total,
                    "last_commit": data.get("commit", "—"),
                    "has_md": md_path.exists(),
                }
                continue
            except Exception:
                pass
        state[aid] = {
            "id": aid, "folder": folder, "status": "skip",
            "passed": 0, "total": 0, "last_commit": "—", "has_md": False,
        }
    return state


def main(argv: list[str] | None = None) -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--results-dir", required=True, type=Path)
    p.add_argument("--manifest", required=True, type=Path)
    p.add_argument("--report-dir", required=True, type=Path)
    p.add_argument("--commit", default="")
    p.add_argument("--run-url", default="")
    p.add_argument("--step-summary", default="")
    args = p.parse_args(argv)

    args.report_dir.mkdir(parents=True, exist_ok=True)
    (args.report_dir / "history").mkdir(parents=True, exist_ok=True)

    manifest = load_manifest(args.manifest)
    state = read_existing_index_state(args.report_dir, manifest)

    reports: list[dict] = []
    for json_file in sorted(args.results_dir.glob("*.json")):
        try:
            data = json.loads(json_file.read_text())
        except json.JSONDecodeError as e:
            print(f"warning: bad JSON {json_file}: {e}", file=sys.stderr)
            continue
        aid = data.get("assignment") or json_file.stem
        data["assignment"] = aid
        reports.append(data)

        log_file = args.results_dir / f"{aid}.log"
        log_text = log_file.read_text() if log_file.exists() else None

        # Per-assignment markdown + JSON, committed back to student repo.
        (args.report_dir / f"{aid}.md").write_text(render_assignment_md(data, log_text))
        snapshot = dict(data)
        snapshot["commit"] = args.commit
        snapshot["timestamp"] = _now_iso()
        (args.report_dir / f"{aid}.json").write_text(
            json.dumps(snapshot, indent=2, sort_keys=True) + "\n"
        )

        passed = data.get("passed", 0)
        failed = data.get("failed", 0)
        total = passed + failed
        status = "pass" if failed == 0 and total > 0 else ("fail" if failed > 0 else "skip")
        append_history(args.report_dir / "history" / f"{aid}.jsonl", {
            "commit": args.commit,
            "timestamp": snapshot["timestamp"],
            "passed": passed,
            "failed": failed,
            "status": status,
        })

        # Update in-memory state for the index.
        folder = next((f for (i, f) in manifest if i == aid), aid)
        state[aid] = {
            "id": aid, "folder": folder, "status": status,
            "passed": passed, "total": total,
            "last_commit": args.commit or "—", "has_md": True,
        }

    # Render index in manifest order so the file diff stays stable.
    rows = [state[aid] for (aid, _) in manifest if aid in state]
    (args.report_dir / "index.md").write_text(
        render_index_md(rows, args.commit or "—", args.run_url or None)
    )

    if args.step_summary:
        Path(args.step_summary).parent.mkdir(parents=True, exist_ok=True)
        with open(args.step_summary, "a") as f:
            f.write(render_step_summary(reports))

    # Exit non-zero iff any graded assignment failed.
    any_failed = any(r.get("failed", 0) > 0 for r in reports)
    return 1 if any_failed else 0


if __name__ == "__main__":
    sys.exit(main())
