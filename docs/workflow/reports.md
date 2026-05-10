# Reports & traceability

`tools/render_report.py` consumes the per-slot result JSON files and
emits a layered Markdown + JSON report inside the student repo. Every
rendered row is hyperlinked back to **the exact commit and folder it
was graded against** — so a TA can click `HW1mj` on the aggregate page
and open that folder's source tree at the SHA it ran against.

## Output layout

```
autograder/
├── index.md                       # top-level dashboard
├── HW1.md  /  HW1.json            # aggregate (when HW1 has multiple rows)
├── HW1__HW1mj.md  /  .json        # per-slot detail
├── HW1__HW1_revised.md  /  .json
├── HW2.md  /  HW2.json            # single-row detail (slot == id)
└── history/
    ├── HW1__HW1mj.jsonl           # one append-only line per run
    ├── HW1__HW1_revised.jsonl
    └── HW2.jsonl
```

| File | Contents |
|---|---|
| `index.md` | Status table — one row per **assignment ID**, with all folders listed and a link to the per-assignment detail. |
| `<id>.md` (aggregate) | When an ID has multiple manifest rows: one row per folder, with pass/total + commit link + slot. |
| `<slot>.md` | Per-run detail. Includes the assignment's check list, status emojis, error messages, and (if uploaded) the full grader log. |
| `<id>.json` (aggregate) | Machine-readable summary with `passed`, `failed`, `folders`, and per-row `runs` (each carrying its own `commit`, `folder`, `timestamp`). |
| `<slot>.json` | Per-run snapshot — exact JSON the grader emitted plus `commit`, `folder`, `slot`, `timestamp`. |
| `history/<slot>.jsonl` | Append-only ledger; each line records `{commit, folder, slot, assignment, passed, failed, status, timestamp}`. Capped at 50 entries per slot. |

## Per-folder, per-commit hyperlinks

Both folder names and commit SHAs in the rendered Markdown become
clickable links when `--repo-url` is passed:

```
| Folder                                                                                    | Status   | Passed / Total | Commit                                                                              |
|-------------------------------------------------------------------------------------------|----------|----------------|-------------------------------------------------------------------------------------|
| [`HW1mj`](https://github.com/student/se423/tree/beefc0de/workspace/HW1mj)                 | ✅ PASS  | 6/6            | [`beefc0de`](https://github.com/student/se423/commit/beefc0de)                      |
| [`HW1_revised`](https://github.com/student/se423/tree/cafef00d/workspace/HW1_revised)     | ✅ PASS  | 6/6            | [`cafef00d`](https://github.com/student/se423/commit/cafef00d)                      |
```

The folder link points at `tree/<sha>/<workspace>/<folder>` — i.e. the
source tree of that folder at the commit it was last graded against.
**Each row uses its own commit**, not the most-recent push, so the
links survive partial regrades.

## Partial-regrade behaviour

Suppose `HW1mj` is re-graded at commit `B` but `HW1_revised` is
unchanged from commit `A`. The aggregate JSON correctly preserves both:

```json
{
  "assignment": "HW1",
  "passed": 12,
  "failed": 0,
  "folders": ["HW1mj", "HW1_revised"],
  "runs": [
    { "slot": "HW1__HW1mj",      "folder": "HW1mj",      "commit": "bbbb...", "passed": 6 },
    { "slot": "HW1__HW1_revised", "folder": "HW1_revised", "commit": "aaaa...", "passed": 6 }
  ]
}
```

The history JSONL for the unchanged slot is **not** appended; only
`HW1__HW1mj.jsonl` grows by one line. This is what the Python test
suite at `tests/python/test_render_report.py` asserts.

## Step summary

The aggregate job also writes a per-run summary to `$GITHUB_STEP_SUMMARY`
so the Actions UI shows status without requiring the user to click into
the rendered Markdown:

```
### HW1 (`HW1mj`) — ✅ 6/6 passed

| Check               | Status | Detail |
|---------------------|--------|--------|
| `check_initialization` | ✅     | —      |
| `check_timer2`         | ✅     | —      |
```

## Renderer CLI

```bash
python3 tools/render_report.py \
    --results-dir  _results \
    --manifest     workspace/assignment.txt \
    --report-dir   autograder \
    --workspace    workspace \
    --commit       "$GITHUB_SHA" \
    --repo-url     "https://github.com/${GITHUB_REPOSITORY}" \
    --run-url      "$GITHUB_SERVER_URL/$GITHUB_REPOSITORY/actions/runs/$GITHUB_RUN_ID" \
    --step-summary "$GITHUB_STEP_SUMMARY"
```

The authoritative description of every CLI flag and every output file
lives in the module docstring of
[`tools/render_report.py`](../reference/python-tools.md#render_report) —
that's what `mkdocstrings` renders into the Python tools page.

## When `--repo-url` is omitted

Local renders (or older workflows that don't pass the flag) gracefully
fall back to plain-text folder names and commit SHAs:

```
| `HW1mj` | ✅ PASS | 6/6 | `cafef00d` |
```

Same content, no hyperlinks. The renderer is safe to run in any
environment.
