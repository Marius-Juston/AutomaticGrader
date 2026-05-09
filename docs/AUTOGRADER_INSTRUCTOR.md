# Autograder — instructor guide

How to set up, operate, and modify the GitHub Actions autograder for SE 423.
This guide is for course staff (TAs, professors). For the student-facing
guide, see `autograder/README.md` inside the base course repo.

## Overview

Two repositories cooperate:

| Repository                      | Owns                                                                                                                       |
|---------------------------------|----------------------------------------------------------------------------------------------------------------------------|
| **AutomaticGrader** (this repo) | The grader source, the **reusable workflow** (`.github/workflows/grade.reusable.yml`), the helper Python tools.            |
| **SE423 base course repo**      | A thin **caller workflow** (`.github/workflows/grade.yml`), `workspace/assignment.txt`, and the `autograder/` results dir. |

The base repo is the upstream every student pulls from. Adding the workflow
to the base repo is therefore the only "rollout" step needed — students inherit
it on their next pull.

## One-time setup

### 1. Tag the grader

Push the AutomaticGrader repo to GitHub and create a stable tag:

```bash
git tag -a v1 -m "Autograder v1"
git push origin v1
```

Use a moving tag (`v1`) for the term and bump it (`v1.1`, `v1.2`) for fixes.
Pin students at `v1` so they auto-pick up patch releases without breaking
mid-semester.

### 2. Add the caller workflow to the base course repo

Copy these files from `context/student_repo/SE423_repo/` into the *real* base
course repo:

```
.github/workflows/grade.yml
workspace/assignment.txt
autograder/.gitkeep
```

Then edit `.github/workflows/grade.yml`:

- `uses: <owner>/AutomaticGrader/.github/workflows/grade.reusable.yml@v1`
  — change `<owner>` to the GitHub account / org hosting AutomaticGrader.
- `grader-ref: v1` — the tag you just pushed.

That's the entire setup. On the next push, every student fork that has
synced from upstream will run the autograder.

### 3. Make the AutomaticGrader repo accessible to students

Two options:

- **Public repo** (simplest). The reusable workflow is fetched automatically
  by every student fork; no tokens needed.
- **Private repo**. You must also enable `Settings → Actions → General →
  "Accessible from repositories owned by …"` on AutomaticGrader, and (per
  GitHub) add the calling repos to the access list. A public mirror is
  usually less hassle for a course.

### 4. Permissions on student repos

The reusable workflow declares `permissions: contents: write` so the bot can
commit the rendered report back to the student branch. Make sure the **base
repo** template has `Settings → Actions → General → Workflow permissions →
Read and write permissions` enabled before students fork — forks inherit
that setting.

## What the pipeline does, on every push

1. Compute changed paths from `${{ github.event.before }}..${{ github.sha }}`.
2. Parse `workspace/assignment.txt`.
3. Build a matrix of assignments to grade:
    - touched-folder entries, **or**
    - everything if `assignment.txt` itself changed, **or**
    - everything on `workflow_dispatch` with `grade-all=true`, **or**
    - everything on the very first push to a branch (no `before` SHA).
4. Per matrix leg: configure cmake with `ASSIGNMENT=<id> STUDENT_SRC=<path>`,
   build the grader, run with `--report-json`, capture log + JSON.
5. Aggregate: render `autograder/index.md`, `autograder/<id>.md`,
   `autograder/<id>.json`, append a single line to `autograder/history/<id>.jsonl`.
6. Commit the changed files in `autograder/` back to the branch with
   `[skip ci]` so the bot push doesn't re-trigger CI.
7. Upload artifacts and exit non-zero if any check failed.

Selecting only the changed assignments keeps CI ~30–90 s. A full re-grade
of all assignments runs in roughly *N × 60 s* (matrix is parallel up to
GitHub's default of 256 jobs).

## Adding a new assignment to the grader

This is the same process as today (see top-level `CLAUDE.md` "Adding a new
assignment checker"). The CI integration adds nothing new — once
`src/checks/<id>.cpp` and the corresponding `ASSIGNMENT=<id>` selector
work locally, push and tag a new release. Students who add the assignment
to their `workspace/assignment.txt` will be graded on the next push.

Bumping the tag students pin to:

```bash
git tag -af v1 -m "Autograder v1.x with HW6 support"
git push origin v1 --force
```

Forced moving tags are conventional for major-version pins; if you prefer
immutability, tag `v1.1` and update `grader-ref: v1.1` in the base repo's
`grade.yml`.

## Manually re-grading

Course staff (or students) can force a full re-grade via the **Actions** tab:

1. Open the autograder workflow on a student repo.
2. Click **Run workflow** → set `grade-all` to `true` → run.

This is the recommended path after pushing a grader update — it guarantees
every student's `autograder/` folder is rebuilt against the new logic.

For a class-wide re-grade, a small script using `gh workflow run` against
each student fork is the easiest approach.

## Files you may need to edit

| Need to…                                             | Edit                                                                                |
|------------------------------------------------------|-------------------------------------------------------------------------------------|
| Add or rename an assignment ID                       | `CMakeLists.txt` (the `ASSIGNMENT` validator) and the manifest doc in the base repo |
| Change which folder the report lands in              | `report-dir:` input in `grade.yml` (default `autograder`)                           |
| Pin students to a specific grader version            | `grader-ref:` input in `grade.yml`                                                  |
| Change matrix selection logic                        | `tools/select_assignments.py` (run `--self-test` after editing)                     |
| Change how the report renders                        | `tools/render_report.py`                                                            |
| Add a new output surface (PR comment, badge, …)      | A new step in the `aggregate` job of `grade.reusable.yml`                           |
| Stop committing the report back (e.g. for a dry-run) | Pass `commit-results: false` in the caller workflow                                 |

## Local development of the workflow

Two practical loops:

- **Tools (Python).** `python3 tools/select_assignments.py --self-test`
  exercises the selector. The renderer can be exercised against a real
  grader run:

  ```bash
  ASSIGNMENT=HW1 STUDENT_SRC=context/code_solutions/myHW_Yingyord_First/HW1/HW1fy_main.c \
    cmake -S . -B cmake-build-debug && cmake --build cmake-build-debug
  ./cmake-build-debug/AutomaticGrader --report-json /tmp/HW1.json > /tmp/HW1.log 2>&1 || true

  mkdir -p /tmp/results /tmp/report
  cp /tmp/HW1.json /tmp/HW1.log /tmp/results/
  printf 'HW1 HW1mj\n' > /tmp/manifest.txt
  python3 tools/render_report.py \
    --results-dir /tmp/results --manifest /tmp/manifest.txt \
    --report-dir /tmp/report --commit deadbeef \
    --step-summary /tmp/summary.md
  ```

- **Workflow YAML.** Use [`act`](https://github.com/nektos/act) or push to a
  test fork of the base repo and watch the **Actions** tab. `act` doesn't
  run reusable workflows fully, so a throwaway repo is usually faster.

## Troubleshooting

| Symptom                                            | Most likely cause / fix                                                                                                                                                                             |
|----------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Workflow runs but matrix is empty                  | Manifest is empty or the push touched only files outside `workspace/`. Run via `workflow_dispatch` with `grade-all=true`.                                                                           |
| `No *_main.c found in workspace/<folder>`          | Student renamed the entry file or has multiple `*_main.c`. Add a 3rd column to `assignment.txt`: `HW3 HW3 HW3_main.c`.                                                                              |
| Bot push gets rejected with `403`                  | Workflow permissions are read-only. Enable read-write under base-repo *Settings → Actions*.                                                                                                         |
| Bot push triggers another workflow run             | The `[skip ci]` marker should prevent this. Verify the commit message wasn't sanitized; consider also adding `paths-ignore: ['autograder/**']` to the caller workflow's `push:` block.              |
| `dladdr` returns `check_<n>` instead of real names | The grader was linked without `-rdynamic`. CI uses `Release` mode where this is wired in CMake; if you swap build types, keep the `target_link_options(... -rdynamic)` block.                       |
| ccache key never hits                              | Cache key includes a hash over `workspace/<folder>/**.c`; one-line edits do invalidate it. Use the broader `restore-keys:` to fall back to the previous build.                                      |
| Selector picks up too many assignments             | A path under `workspace/<folder>/` was changed by a merge or rebase. Use `workflow_dispatch` to do a clean run; selector is intentionally permissive (better to over-grade than miss a regression). |

## Security notes

- Student code runs as a normal Linux process inside an ephemeral
  GitHub-hosted runner. There is no special isolation beyond the runner
  itself; do not enable self-hosted runners without sandboxing.
- The bot uses the built-in `GITHUB_TOKEN` — no secrets leave the runner.
- The reusable workflow checks out AutomaticGrader at the pinned `grader-ref`.
  If a student modifies their copy of `.github/workflows/grade.yml` to
  point at a different grader-ref, that's their right — but their
  `autograder/` results will be tagged with whatever ref they used. Spot-check
  via `git log autograder/` if grading authenticity matters.
