"""Unit tests for tools/render_report.py.

Covers the behaviour added for duplicate-id manifests:

  - per-slot detail files (md + json) with embedded commit + folder
  - aggregate <id>.md / <id>.json when an ID has multiple manifest rows
  - per-slot history/<slot>.jsonl with commit + folder on every line
  - partial-regrade aggregate preserves prior runs' commits
  - --repo-url turns folder names + commit SHAs into hyperlinks pointing
    at the *exact* commit each row was graded against

Run via:  python -m unittest discover -s tests/python
"""
from __future__ import annotations

import json
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(REPO_ROOT / "tools"))

import render_report  # noqa: E402


def _write(path: Path, payload) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    if isinstance(payload, (dict, list)):
        path.write_text(json.dumps(payload))
    else:
        path.write_text(payload)


class RenderReportTestBase(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = Path(tempfile.mkdtemp(prefix="rr-test-"))
        self.results = self.tmp / "results"
        self.report = self.tmp / "report"
        self.results.mkdir()
        self.report.mkdir()
        self.manifest = self.tmp / "manifest.txt"

    def tearDown(self) -> None:
        shutil.rmtree(self.tmp, ignore_errors=True)

    def render(self, *, commit: str, repo_url: str = "",
               workspace: str = "workspace") -> int:
        argv = [
            "--results-dir", str(self.results),
            "--manifest", str(self.manifest),
            "--report-dir", str(self.report),
            "--workspace", workspace,
            "--commit", commit,
        ]
        if repo_url:
            argv.extend(["--repo-url", repo_url])
        return render_report.main(argv)


class TestDuplicateIdAggregation(RenderReportTestBase):
    def test_two_rows_same_id_produce_aggregate_and_per_slot(self) -> None:
        self.manifest.write_text(
            "HW1 HW1mj\nHW1 HW1_attempt2\nHW2 HW2mj\n"
        )
        _write(self.results / "HW1__HW1mj.json", {
            "assignment": "HW1", "passed": 5, "failed": 1,
            "checks": [{"name": "c", "status": "fail", "messages": ["[error] x"]}],
        })
        _write(self.results / "HW1__HW1mj.meta.json",
               {"id": "HW1", "folder": "HW1mj", "slot": "HW1__HW1mj"})
        _write(self.results / "HW1__HW1_attempt2.json", {
            "assignment": "HW1", "passed": 6, "failed": 0, "checks": [],
        })
        _write(self.results / "HW1__HW1_attempt2.meta.json",
               {"id": "HW1", "folder": "HW1_attempt2",
                "slot": "HW1__HW1_attempt2"})
        _write(self.results / "HW2.json",
               {"assignment": "HW2", "passed": 4, "failed": 0, "checks": []})
        _write(self.results / "HW2.meta.json",
               {"id": "HW2", "folder": "HW2mj", "slot": "HW2"})

        rc = self.render(commit="cafef00d12345678")
        # Any failure should bubble up so CI reports it.
        self.assertEqual(rc, 1, "non-zero rc expected because HW1__HW1mj failed")

        # Per-slot detail files for every row.
        for name in ("HW1__HW1mj", "HW1__HW1_attempt2", "HW2"):
            self.assertTrue((self.report / f"{name}.md").exists(), name)
            self.assertTrue((self.report / f"{name}.json").exists(), name)

        # Aggregate page exists for the duplicate id, NOT for single-row HW2.
        self.assertTrue((self.report / "HW1.md").exists())
        self.assertTrue((self.report / "HW1.json").exists())
        # HW2 has a single row so HW2.md is mirrored from HW2 (same slot==id)
        self.assertTrue((self.report / "HW2.md").exists())

        agg = json.loads((self.report / "HW1.json").read_text())
        self.assertEqual(agg["passed"], 11)
        self.assertEqual(agg["failed"], 1)
        self.assertEqual([r["folder"] for r in agg["runs"]],
                         ["HW1mj", "HW1_attempt2"])
        for r in agg["runs"]:
            self.assertEqual(r["commit"], "cafef00d12345678",
                             "every run carries the run's commit SHA")

        # History JSONL gets one line per slot with commit + folder embedded.
        h = self.report / "history" / "HW1__HW1mj.jsonl"
        self.assertTrue(h.exists())
        entry = json.loads(h.read_text().strip().splitlines()[-1])
        self.assertEqual(entry["commit"], "cafef00d12345678")
        self.assertEqual(entry["folder"], "HW1mj")
        self.assertEqual(entry["slot"], "HW1__HW1mj")
        self.assertEqual(entry["status"], "fail")

    def test_partial_regrade_preserves_prior_run_commit(self) -> None:
        """A re-grade of one folder must NOT clobber the other folder's
        recorded commit — that's the whole point of per-folder traceability."""
        self.manifest.write_text("HW1 HW1mj\nHW1 HW1_attempt2\n")
        # First push: both rows graded at commit A.
        _write(self.results / "HW1__HW1mj.json",
               {"assignment": "HW1", "passed": 6, "failed": 0, "checks": []})
        _write(self.results / "HW1__HW1mj.meta.json",
               {"id": "HW1", "folder": "HW1mj", "slot": "HW1__HW1mj"})
        _write(self.results / "HW1__HW1_attempt2.json",
               {"assignment": "HW1", "passed": 6, "failed": 0, "checks": []})
        _write(self.results / "HW1__HW1_attempt2.meta.json",
               {"id": "HW1", "folder": "HW1_attempt2",
                "slot": "HW1__HW1_attempt2"})
        self.render(commit="a" * 40)

        # Second push: only HW1mj re-grades at commit B.
        results2 = self.tmp / "results2"
        results2.mkdir()
        _write(results2 / "HW1__HW1mj.json",
               {"assignment": "HW1", "passed": 6, "failed": 0, "checks": []})
        _write(results2 / "HW1__HW1mj.meta.json",
               {"id": "HW1", "folder": "HW1mj", "slot": "HW1__HW1mj"})

        render_report.main([
            "--results-dir", str(results2),
            "--manifest", str(self.manifest),
            "--report-dir", str(self.report),
            "--workspace", "workspace",
            "--commit", "b" * 40,
        ])

        agg = json.loads((self.report / "HW1.json").read_text())
        commits_by_folder = {r["folder"]: r["commit"] for r in agg["runs"]}
        self.assertEqual(commits_by_folder["HW1mj"], "b" * 40,
                         "freshly graded folder records the new commit")
        self.assertEqual(commits_by_folder["HW1_attempt2"], "a" * 40,
                         "untouched folder keeps the prior commit SHA")

        # History for the unchanged slot should NOT have grown.
        h_unchanged = self.report / "history" / "HW1__HW1_attempt2.jsonl"
        self.assertEqual(len(h_unchanged.read_text().splitlines()), 1)
        h_regraded = self.report / "history" / "HW1__HW1mj.jsonl"
        self.assertEqual(len(h_regraded.read_text().splitlines()), 2)


class TestHyperlinkRendering(RenderReportTestBase):
    REPO = "https://github.com/owner/repo"

    def setUp(self) -> None:
        super().setUp()
        self.manifest.write_text("HW1 HW1mj\nHW1 HW1_alt\n")
        _write(self.results / "HW1__HW1mj.json",
               {"assignment": "HW1", "passed": 6, "failed": 0, "checks": []})
        _write(self.results / "HW1__HW1mj.meta.json",
               {"id": "HW1", "folder": "HW1mj", "slot": "HW1__HW1mj"})
        _write(self.results / "HW1__HW1_alt.json",
               {"assignment": "HW1", "passed": 6, "failed": 0, "checks": []})
        _write(self.results / "HW1__HW1_alt.meta.json",
               {"id": "HW1", "folder": "HW1_alt", "slot": "HW1__HW1_alt"})

    def test_folder_and_commit_links_use_run_commit(self) -> None:
        self.render(commit="deadbeef" + "0" * 32, repo_url=self.REPO)
        index = (self.report / "index.md").read_text()
        self.assertIn(
            f"[`HW1mj`]({self.REPO}/tree/{'deadbeef' + '0' * 32}/workspace/HW1mj)",
            index,
        )
        self.assertIn(
            f"[`deadbeef`]({self.REPO}/commit/{'deadbeef' + '0' * 32})",
            index,
        )

        agg = (self.report / "HW1.md").read_text()
        # Both folders + both commits are clickable.
        self.assertIn(f"({self.REPO}/tree/", agg)
        self.assertIn(f"({self.REPO}/commit/", agg)

        detail = (self.report / "HW1__HW1mj.md").read_text()
        self.assertIn(
            f"[`HW1mj`]({self.REPO}/tree/{'deadbeef' + '0' * 32}/workspace/HW1mj)",
            detail,
        )

    def test_partial_regrade_uses_per_folder_commit_in_links(self) -> None:
        # First run at commit A.
        self.render(commit="a" * 40, repo_url=self.REPO)
        # Regrade only HW1mj at commit B.
        results2 = self.tmp / "results2"
        results2.mkdir()
        _write(results2 / "HW1__HW1mj.json",
               {"assignment": "HW1", "passed": 6, "failed": 0, "checks": []})
        _write(results2 / "HW1__HW1mj.meta.json",
               {"id": "HW1", "folder": "HW1mj", "slot": "HW1__HW1mj"})
        render_report.main([
            "--results-dir", str(results2),
            "--manifest", str(self.manifest),
            "--report-dir", str(self.report),
            "--workspace", "workspace",
            "--commit", "b" * 40,
            "--repo-url", self.REPO,
        ])

        agg = (self.report / "HW1.md").read_text()
        self.assertIn(f"{self.REPO}/tree/{'b' * 40}/workspace/HW1mj", agg)
        self.assertIn(f"{self.REPO}/tree/{'a' * 40}/workspace/HW1_alt", agg)

        index = (self.report / "index.md").read_text()
        # Both folder links present, each pointing at its own commit.
        self.assertIn(f"{self.REPO}/tree/{'b' * 40}/workspace/HW1mj", index)
        self.assertIn(f"{self.REPO}/tree/{'a' * 40}/workspace/HW1_alt", index)

    def test_no_repo_url_falls_back_to_plain_text(self) -> None:
        self.render(commit="c" * 40, repo_url="")
        index = (self.report / "index.md").read_text()
        # No markdown link syntax for folders / commits.
        self.assertNotIn("](http", index)
        self.assertIn("`HW1mj`", index)
        self.assertIn("`cccccccc`", index)


class TestMetaSidecarFallback(RenderReportTestBase):
    def test_slot_inferred_from_filename_without_sidecar(self) -> None:
        """If the workflow forgets the sidecar, the renderer must still
        attribute a slot like 'HW1__HW1mj' to id=HW1, folder from manifest."""
        self.manifest.write_text("HW1 HW1mj\nHW1 HW1_alt\n")
        _write(self.results / "HW1__HW1mj.json",
               {"assignment": "HW1", "passed": 3, "failed": 0, "checks": []})
        # No sidecar.
        rc = self.render(commit="d" * 40)
        self.assertEqual(rc, 0)
        data = json.loads((self.report / "HW1__HW1mj.json").read_text())
        self.assertEqual(data["assignment"], "HW1")
        # Folder must resolve to a known manifest folder, not '—'.
        self.assertIn(data["folder"], ("HW1mj", "HW1_alt"))


class TestUrlHelpers(unittest.TestCase):
    def test_commit_url_handles_empty_inputs(self) -> None:
        self.assertIsNone(render_report._commit_url("", "abc"))
        self.assertIsNone(render_report._commit_url("https://x", ""))
        self.assertIsNone(render_report._commit_url("https://x", "—"))
        self.assertEqual(
            render_report._commit_url("https://x/", "abc"),
            "https://x/commit/abc",
        )

    def test_folder_url_strips_workspace_slashes(self) -> None:
        self.assertEqual(
            render_report._folder_url("https://x", "deadbeef", "workspace/", "HW1"),
            "https://x/tree/deadbeef/workspace/HW1",
        )
        self.assertEqual(
            render_report._folder_url("https://x", "deadbeef", "", "HW1"),
            "https://x/tree/deadbeef/HW1",
        )
        self.assertIsNone(
            render_report._folder_url("https://x", "deadbeef", "ws", "—")
        )


if __name__ == "__main__":
    unittest.main()
