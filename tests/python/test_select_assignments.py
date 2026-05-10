"""Run select_assignments.py's built-in self-tests under unittest so they
become part of CI coverage and surface in the same junit/output as the
render_report tests."""
from __future__ import annotations

import subprocess
import sys
import unittest
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "tools" / "select_assignments.py"


class TestSelectAssignmentsSelfTest(unittest.TestCase):
    def test_self_test_passes(self) -> None:
        result = subprocess.run(
            [sys.executable, str(SCRIPT), "--self-test"],
            capture_output=True, text=True, timeout=30,
        )
        self.assertEqual(
            result.returncode, 0,
            f"select_assignments self-tests failed:\n"
            f"stdout:\n{result.stdout}\nstderr:\n{result.stderr}",
        )
        self.assertIn("passed", result.stdout.lower())


if __name__ == "__main__":
    unittest.main()
