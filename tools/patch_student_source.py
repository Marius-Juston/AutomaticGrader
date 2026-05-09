#!/usr/bin/env python3
"""Patch a student firmware source file for the cooperative main-loop driver.

Two transformations are applied:

1. Inject `GRADER_MAIN_INIT_GUARD` immediately after the opening `{` of
   `main()`. The macro (defined in include/ti_stubs.h under AUTO_GRADER)
   uses a static flag plus a goto to skip the init code on every re-entry
   into temp_main() after the first one. This is what makes
   `grader::step_main_loop()` cheap (no re-init, no re-running of any
   busy-waits the student's setup helpers may contain).

2. Replace `while(1)` with `_grader_loop_start: ; GRADER_MAIN_LOOP`. The
   label is the goto target from (1); GRADER_MAIN_LOOP expands to a
   for-loop bounded by `grader_main_loop_iterations()`.

Usage: patch_student_source.py <input.c> <output.c>
"""
import re
import sys


def patch(src: str) -> str:
    # (1) inject the init guard after the main() opening brace.
    # Match: `(int|void) main ( ... ) { ` (any whitespace including newlines)
    src = re.sub(
        r'(\b(?:void|int)\s+main\s*\([^)]*\)\s*)\{',
        r'\1{ GRADER_MAIN_INIT_GUARD;',
        src,
        count=1,
    )
    # (2) Replace `while(1)` with the loop label + GRADER_MAIN_LOOP macro.
    src = re.sub(
        r'while\s*\(\s*1\s*\)',
        '_grader_loop_start: ; GRADER_MAIN_LOOP',
        src,
    )
    return src


def main() -> int:
    if len(sys.argv) != 3:
        sys.stderr.write(f"usage: {sys.argv[0]} <input.c> <output.c>\n")
        return 2
    # Student source files occasionally contain non-UTF-8 bytes (smart quotes
    # pasted from a PDF, latin-1 dashes). Read/write as bytes via latin-1 so
    # every byte round-trips losslessly.
    with open(sys.argv[1], 'r', encoding='latin-1') as f:
        src = f.read()
    out = patch(src)
    with open(sys.argv[2], 'w', encoding='latin-1') as f:
        f.write(out)
    return 0


if __name__ == '__main__':
    sys.exit(main())
