#!/usr/bin/env python3
"""Render grader JSON results into student-facing markdown + JSON.

Inputs (CLI):
    --results-dir <dir>    directory containing <slot>.json (one per graded
                           run) and optional <slot>.log files.
    --manifest <path>      workspace/assignment.txt — used to render the
                           cumulative index even for assignments not graded
                           this run.
    --report-dir <dir>     output directory inside the student repo
                           (e.g. autograder/). Per-assignment files are
                           updated in place; entries not in this run are
                           preserved verbatim.
    --commit <sha>         commit SHA of the run (recorded into every
                           emitted JSON snapshot and history line so a
                           reader can trace a given result back to the
                           exact source).
    --run-url <url>        link to the GitHub Actions run (optional).
    --repo-url <url>       base URL of the student repo, e.g.
                           https://github.com/owner/repo. When set, the
                           rendered report turns each folder name into a
                           link to that folder at the run's commit
                           (`<repo-url>/tree/<sha>/<workspace>/<folder>`)
                           and each commit SHA into a link to the commit
                           (`<repo-url>/commit/<sha>`). Optional.
    --workspace <dir>      workspace prefix used when building folder URLs
                           (defaults to `workspace`).
    --step-summary <path>  if set, also write a Markdown summary suitable
                           for $GITHUB_STEP_SUMMARY.

The manifest may contain duplicate IDs (e.g. two `HW1` rows pointing at
different folders). Each row produces an independent grader run keyed by
its `slot` (unique per row, see select_assignments.py). For an ID with
multiple rows we emit:

  - one detail file per row:    <report-dir>/<slot>.md / .json
  - an aggregate <id>.md         (per-folder rows + total pass/fail)
  - an aggregate <id>.json       (machine-readable summary of all rows)
  - history/<slot>.jsonl         (one JSONL per row, recording commit + folder)

When an ID has only one manifest row, slot == id, so the layout collapses
to the historical single-file form.

Each <slot>.json read here is the format emitted by AutomaticGrader
--report-json:
    {
      "assignment": "HW1",
      "passed": 6, "failed": 1,
      "checks": [{"name": "...", "status": "pass|fail",
                  "messages": ["..."]}, ...]
    }

The grader does not know its own slot/folder/commit; the workflow is
expected to drop a sidecar `<slot>.meta.json` next to the result with at
least `{"id": ..., "folder": ..., "slot": ...}`. Without the sidecar we
fall back to filename-stem heuristics (treating "<id>__<rest>" as a slot
where everything after the "__" is a folder slug, and a bare "<id>" as a
single-row entry).
"""
from __future__ import annotations

import argparse
import datetime as dt
import json
import sys
from collections import defaultdict
from pathlib import Path

HISTORY_LIMIT = 50
STATUS_EMOJI = {"pass": "✅", "fail": "❌", "skip": "⚪"}


def _commit_url(repo_url: str, commit: str) -> str | None:
    if not repo_url or not commit or commit == "—":
        return None
    return f"{repo_url.rstrip('/')}/commit/{commit}"


def _folder_url(repo_url: str, commit: str, workspace: str, folder: str) -> str | None:
    if not repo_url or not commit or commit == "—" or not folder or folder == "—":
        return None
    base = repo_url.rstrip("/")
    ws = (workspace or "").strip("/")
    path = f"{ws}/{folder}".lstrip("/") if ws else folder
    return f"{base}/tree/{commit}/{path}"


def _md_link(text: str, url: str | None) -> str:
    return f"[{text}]({url})" if url else text


def _commit_md(repo_url: str, commit: str) -> str:
    """Render a commit SHA as `short` in markdown, hyperlinked when possible."""
    if not commit or commit == "—":
        return "—"
    short = commit[:8]
    url = _commit_url(repo_url, commit)
    return _md_link(f"`{short}`", url)


def _folder_md(repo_url: str, commit: str, workspace: str, folder: str) -> str:
    """Render a folder name as `folder` in markdown, hyperlinked when possible."""
    if not folder or folder == "—":
        return "`—`"
    url = _folder_url(repo_url, commit, workspace, folder)
    return _md_link(f"`{folder}`", url)


def _now_iso() -> str:
    return dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def _now_short() -> str:
    return dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%MZ")


def load_manifest(path: Path) -> list[tuple[str, str]]:
    """Return list of (id, folder) — preserves duplicates and order."""
    out: list[tuple[str, str]] = []
    for raw in path.read_text().splitlines():
        line = raw.split("#", 1)[0].strip()
        if not line:
            continue
        parts = line.split()
        if len(parts) >= 2:
            out.append((parts[0], parts[1]))
    return out


def render_assignment_md(report: dict, log_text: str | None,
                         repo_url: str = "", workspace: str = "") -> str:
    """Detail page for a single graded run (one slot)."""
    aid = report["assignment"]
    folder = report.get("folder", "—")
    commit = report.get("commit", "—")
    passed = report.get("passed", 0)
    failed = report.get("failed", 0)
    total = passed + failed
    badge = "✅" if failed == 0 and total > 0 else ("❌" if failed > 0 else "⚪")
    folder_md = _folder_md(repo_url, commit, workspace, folder)
    commit_md = _commit_md(repo_url, commit)
    title_folder = "" if folder in ("—", "", None) else f" — {folder_md}"
    lines = [
        f"# {aid}{title_folder} — {badge} {passed}/{total} passed",
        "",
        f"_Folder:_ {folder_md}  _Commit:_ {commit_md}  "
        f"_Graded:_ {report.get('timestamp', '—')}",
        "",
        "| Check | Status | Detail |",
        "|---|---|---|",
    ]
    for c in report.get("checks", []):
        emoji = STATUS_EMOJI.get(c.get("status"), "?")
        msgs = c.get("messages") or []
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


def render_aggregate_md(aid: str, runs: list[dict], commit: str,
                        repo_url: str = "", workspace: str = "") -> str:
    """Aggregate page for an ID that has multiple manifest rows.

    The folder cell links to that folder at the commit it was graded
    against (each row may have its own commit, since unchanged siblings
    keep their original commit across partial regrades). The header
    commit links to the most recent run's commit."""
    total_passed = sum(r.get("passed", 0) for r in runs)
    total_failed = sum(r.get("failed", 0) for r in runs)
    total = total_passed + total_failed
    badge = "✅" if total_failed == 0 and total > 0 else ("❌" if total_failed > 0 else "⚪")
    lines = [
        f"# {aid} (aggregate) — {badge} {total_passed}/{total} passed",
        "",
        f"_Commit:_ {_commit_md(repo_url, commit)}  _Submissions:_ {len(runs)}  "
        f"_Last update:_ {_now_short()}",
        "",
        "| Folder | Status | Passed / Total | Commit | Slot | Detail |",
        "|---|---|---|---|---|---|",
    ]
    for r in runs:
        passed = r.get("passed", 0)
        failed = r.get("failed", 0)
        sub_total = passed + failed
        status = "pass" if failed == 0 and sub_total > 0 else (
            "fail" if failed > 0 else "skip")
        emoji = STATUS_EMOJI.get(status, "⚪")
        slot = r.get("slot", aid)
        folder = r.get("folder", "—")
        sub_commit = r.get("commit", commit)
        lines.append(
            f"| {_folder_md(repo_url, sub_commit, workspace, folder)} "
            f"| {emoji} {status.upper()} | {passed}/{sub_total} "
            f"| {_commit_md(repo_url, sub_commit)} "
            f"| `{slot}` | [{slot}.md]({slot}.md) |"
        )
    lines.append("")
    lines.append(
        "Each row is an independent grading run against the named folder at "
        "the listed commit — click the folder to open the source tree at that "
        "commit, or the commit SHA to view the commit itself. Per-run history "
        "lives in `history/<slot>.jsonl`."
    )
    return "\n".join(lines) + "\n"


def render_index_md(rows: list[dict], commit: str, run_url: str | None,
                    repo_url: str = "", workspace: str = "") -> str:
    lines = [
        "# Autograder report",
        "",
        f"_Last update: {_now_short()}_  "
        f"_Commit: {_commit_md(repo_url, commit)}_"
        + (f"  _[Run]({run_url})_" if run_url else ""),
        "",
        "| Assignment | Folder(s) | Status | Passed / Total | Last graded | Detail |",
        "|---|---|---|---|---|---|",
    ]
    for r in rows:
        emoji = STATUS_EMOJI.get(r["status"], "⚪")
        passed = r.get("passed", 0)
        total = r.get("total", 0)
        last = r.get("last_commit", "—")
        folders = r.get("folders") or [r.get("folder", "—")]
        # Each folder links to that folder at the commit it was last graded
        # against. When per-folder commit info is available (multi-row IDs
        # that came through the aggregator), prefer it; otherwise fall back
        # to the row-level last_commit.
        per_folder_commits = r.get("folder_commits") or {}
        folder_cell = ", ".join(
            _folder_md(repo_url, per_folder_commits.get(f, last), workspace, f)
            for f in folders
        )
        link = r.get("detail_link") or "—"
        lines.append(
            f"| {r['id']} | {folder_cell} | {emoji} {r['status'].upper()} "
            f"| {passed}/{total} | {_commit_md(repo_url, last)} | {link} |"
        )
    lines.append("")
    lines.append(
        "Per-run detail and full logs are in `<slot>.md`. When a single ID "
        "has multiple manifest rows, the aggregate `<id>.md` lists each row. "
        "Per-run history (with commit SHA + folder) lives under "
        "`history/<slot>.jsonl`."
    )
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
        folder = r.get("folder", "")
        head = f"### {r['assignment']}"
        if folder:
            head += f" (`{folder}`)"
        head += f" — {badge} {passed}/{total} passed"
        parts.append(head)
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
        existing = [ln for ln in history_path.read_text().splitlines() if ln]
    existing.append(json.dumps(entry, sort_keys=True))
    if len(existing) > HISTORY_LIMIT:
        existing = existing[-HISTORY_LIMIT:]
    history_path.write_text("\n".join(existing) + "\n")


def _read_meta_sidecar(results_dir: Path, slot: str) -> dict:
    meta_path = results_dir / f"{slot}.meta.json"
    if meta_path.exists():
        try:
            return json.loads(meta_path.read_text())
        except json.JSONDecodeError:
            pass
    return {}


def _slot_to_id_folder(slot: str, manifest: list[tuple[str, str]]) -> tuple[str, str]:
    """Best-effort fallback when no sidecar is present."""
    if "__" in slot:
        aid, _ = slot.split("__", 1)
        for mid, mfolder in manifest:
            if mid == aid:
                return aid, mfolder
        return aid, "—"
    for mid, mfolder in manifest:
        if mid == slot:
            return mid, mfolder
    return slot, "—"


def _load_existing_aggregate(report_dir: Path, aid: str) -> dict | None:
    p = report_dir / f"{aid}.json"
    if not p.exists():
        return None
    try:
        return json.loads(p.read_text())
    except json.JSONDecodeError:
        return None


def _build_index_rows(manifest: list[tuple[str, str]],
                      report_dir: Path,
                      fresh_by_id: dict[str, list[dict]]) -> list[dict]:
    """One row per *unique* assignment ID, in manifest order."""
    seen_ids: list[str] = []
    folders_by_id: dict[str, list[str]] = defaultdict(list)
    for mid, mfolder in manifest:
        if mid not in folders_by_id:
            seen_ids.append(mid)
        folders_by_id[mid].append(mfolder)

    rows: list[dict] = []
    for aid in seen_ids:
        manifest_rows = [m for m in manifest if m[0] == aid]
        is_multi = len(manifest_rows) > 1
        if aid in fresh_by_id:
            runs = fresh_by_id[aid]
            folder_commits: dict[str, str] = {}
            if is_multi:
                # Prefer the aggregate JSON we just wrote (it merges fresh
                # runs over any prior per-slot results we have on disk), so
                # the index reflects every folder, not only the ones graded
                # this push.
                agg = _load_existing_aggregate(report_dir, aid) or {}
                passed = agg.get("passed", sum(r.get("passed", 0) for r in runs))
                failed = agg.get("failed", sum(r.get("failed", 0) for r in runs))
                folders = agg.get("folders") or [m[1] for m in manifest_rows]
                for sub in agg.get("runs", []):
                    f = sub.get("folder")
                    c = sub.get("commit")
                    if f and c:
                        folder_commits[f] = c
                detail_link = (
                    f"[{aid}.md]({aid}.md)"
                    if (report_dir / f"{aid}.md").exists() else "—"
                )
            else:
                passed = sum(r.get("passed", 0) for r in runs)
                failed = sum(r.get("failed", 0) for r in runs)
                folders = [r.get("folder", "—") for r in runs]
                for r in runs:
                    f = r.get("folder")
                    c = r.get("commit")
                    if f and c:
                        folder_commits[f] = c
                slot = runs[0].get("slot", aid)
                detail_link = (
                    f"[{slot}.md]({slot}.md)"
                    if (report_dir / f"{slot}.md").exists() else "—"
                )
            total = passed + failed
            status = "pass" if failed == 0 and total > 0 else (
                "fail" if failed > 0 else "skip")
            commit = next((r.get("commit", "—") for r in runs if r.get("commit")), "—")
        else:
            folder_commits = {}
            agg = _load_existing_aggregate(report_dir, aid)
            if agg:
                passed = agg.get("passed", 0)
                failed = agg.get("failed", 0)
                total = passed + failed
                status = "pass" if failed == 0 and total > 0 else (
                    "fail" if failed > 0 else "skip")
                commit = agg.get("commit", "—")
                folders = agg.get("folders") or folders_by_id[aid]
                for sub in agg.get("runs", []):
                    f = sub.get("folder")
                    c = sub.get("commit")
                    if f and c:
                        folder_commits[f] = c
                detail_link = (
                    f"[{aid}.md]({aid}.md)"
                    if (report_dir / f"{aid}.md").exists()
                    else "—"
                )
            else:
                passed = failed = total = 0
                status = "skip"
                commit = "—"
                folders = folders_by_id[aid]
                detail_link = "—"
        rows.append({
            "id": aid,
            "folders": folders,
            "status": status,
            "passed": passed,
            "total": total,
            "last_commit": commit,
            "folder_commits": folder_commits,
            "detail_link": detail_link,
        })
    return rows


def main(argv: list[str] | None = None) -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--results-dir", required=True, type=Path)
    p.add_argument("--manifest", required=True, type=Path)
    p.add_argument("--report-dir", required=True, type=Path)
    p.add_argument("--commit", default="")
    p.add_argument("--run-url", default="")
    p.add_argument("--repo-url", default="")
    p.add_argument("--workspace", default="workspace")
    p.add_argument("--step-summary", default="")
    args = p.parse_args(argv)

    args.report_dir.mkdir(parents=True, exist_ok=True)
    (args.report_dir / "history").mkdir(parents=True, exist_ok=True)

    manifest = load_manifest(args.manifest)

    # Group fresh results by assignment id.
    fresh_by_id: dict[str, list[dict]] = defaultdict(list)
    summary_reports: list[dict] = []

    for json_file in sorted(args.results_dir.glob("*.json")):
        if json_file.name.endswith(".meta.json"):
            continue
        try:
            data = json.loads(json_file.read_text())
        except json.JSONDecodeError as e:
            print(f"warning: bad JSON {json_file}: {e}", file=sys.stderr)
            continue

        slot = json_file.stem
        meta = _read_meta_sidecar(args.results_dir, slot)
        aid = meta.get("id") or data.get("assignment") or slot
        # If the sidecar disagrees with the grader's own assignment field,
        # trust the workflow (it knows the manifest mapping).
        folder = meta.get("folder")
        if not folder:
            _, folder = _slot_to_id_folder(slot, manifest)
        slot = meta.get("slot") or slot

        data["assignment"] = aid
        data["slot"] = slot
        data["folder"] = folder
        data["commit"] = args.commit or "—"
        data["timestamp"] = _now_iso()

        log_file = args.results_dir / f"{slot}.log"
        log_text = log_file.read_text() if log_file.exists() else None

        # Per-slot detail files.
        (args.report_dir / f"{slot}.md").write_text(
            render_assignment_md(data, log_text,
                                 repo_url=args.repo_url,
                                 workspace=args.workspace)
        )
        (args.report_dir / f"{slot}.json").write_text(
            json.dumps(data, indent=2, sort_keys=True) + "\n"
        )

        passed = data.get("passed", 0)
        failed = data.get("failed", 0)
        total = passed + failed
        status = "pass" if failed == 0 and total > 0 else (
            "fail" if failed > 0 else "skip")

        # Per-slot history. Each record carries commit + folder so the result
        # can always be traced back to the exact source tree that produced it.
        append_history(args.report_dir / "history" / f"{slot}.jsonl", {
            "commit": args.commit or "",
            "timestamp": data["timestamp"],
            "assignment": aid,
            "slot": slot,
            "folder": folder,
            "passed": passed,
            "failed": failed,
            "status": status,
        })

        fresh_by_id[aid].append(data)
        summary_reports.append(data)

    # Aggregate any ID that has more than one fresh row this run.
    for aid, runs in fresh_by_id.items():
        # Count manifest rows for this id; if >1, always emit aggregate even
        # when only one of the rows ran this push (so the aggregate page
        # doesn't disappear after a partial regrade).
        manifest_rows_for_id = [m for m in manifest if m[0] == aid]
        existing_agg = _load_existing_aggregate(args.report_dir, aid) or {}
        if len(manifest_rows_for_id) > 1:
            # Merge fresh runs over any prior per-slot results we have on disk.
            merged: dict[str, dict] = {}
            for sub in existing_agg.get("runs", []):
                merged[sub.get("slot", sub.get("folder", ""))] = sub
            for r in runs:
                merged[r["slot"]] = {
                    "slot": r["slot"],
                    "folder": r["folder"],
                    "commit": r["commit"],
                    "timestamp": r["timestamp"],
                    "passed": r.get("passed", 0),
                    "failed": r.get("failed", 0),
                }
            ordered: list[dict] = []
            for _, mfolder in manifest_rows_for_id:
                for sub in merged.values():
                    if sub.get("folder") == mfolder and sub not in ordered:
                        ordered.append(sub)
            for sub in merged.values():
                if sub not in ordered:
                    ordered.append(sub)

            (args.report_dir / f"{aid}.md").write_text(
                render_aggregate_md(aid, ordered, args.commit or "—",
                                    repo_url=args.repo_url,
                                    workspace=args.workspace)
            )
            agg_passed = sum(s.get("passed", 0) for s in ordered)
            agg_failed = sum(s.get("failed", 0) for s in ordered)
            (args.report_dir / f"{aid}.json").write_text(
                json.dumps({
                    "assignment": aid,
                    "commit": args.commit or "",
                    "timestamp": _now_iso(),
                    "passed": agg_passed,
                    "failed": agg_failed,
                    "folders": [f for _, f in manifest_rows_for_id],
                    "runs": ordered,
                }, indent=2, sort_keys=True) + "\n"
            )
        else:
            # Single-row id: <slot>.{md,json} already written above. If slot
            # != id, mirror to <id>.{md,json} for back-compat with existing
            # links into the report directory.
            r = runs[0]
            if r["slot"] != aid:
                (args.report_dir / f"{aid}.md").write_text(
                    (args.report_dir / f"{r['slot']}.md").read_text()
                )
                (args.report_dir / f"{aid}.json").write_text(
                    (args.report_dir / f"{r['slot']}.json").read_text()
                )

    rows = _build_index_rows(manifest, args.report_dir, fresh_by_id)
    (args.report_dir / "index.md").write_text(
        render_index_md(rows, args.commit or "—", args.run_url or None,
                        repo_url=args.repo_url, workspace=args.workspace)
    )

    if args.step_summary:
        Path(args.step_summary).parent.mkdir(parents=True, exist_ok=True)
        with open(args.step_summary, "a") as f:
            f.write(render_step_summary(summary_reports))

    any_failed = any(r.get("failed", 0) > 0 for r in summary_reports)
    return 1 if any_failed else 0


if __name__ == "__main__":
    sys.exit(main())
