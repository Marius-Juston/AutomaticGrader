# Manifest format

The manifest lives at `workspace/assignment.txt` inside the **student
repo** (not this repo). The grader's CI consumes it on every push.

## Grammar

```
manifest    := ( line | comment | blank )*
line        := id WS folder ( WS main )?
id          := "HW1" | "HW2" | ... | "HW6"
             | "Lab1" | ... | "Lab6"
             | "Lab7-1" | "Lab7-2"
folder      := <relative path inside workspace/>
main        := <filename of the *_main.c to compile>
comment     := "#" <rest of line>
blank       := <whitespace-only>
WS          := one or more whitespace
```

- Blank lines and `#` comments are ignored.
- Whitespace between tokens is any sequence of spaces or tabs.
- An ID that's not in the valid set causes `CMakeLists.txt` to fail
  at configure time.

## Examples

=== "Minimal"

    ```
    HW1 HW1
    Lab1 Lab1
    ```

=== "Multiple students"

    ```
    HW1 students/alice/hw1
    HW1 students/bob/hw1
    HW2 students/alice/hw2
    ```

    Both `HW1` rows run independently. The renderer produces an
    aggregate `HW1.md` showing per-folder status. See
    [Manifest & slots](../workflow/manifest-and-slots.md#duplicate-ids).

=== "Explicit main file"

    ```
    HW3 students/alice/hw3 HW3_main.c
    ```

    Use the third column when the folder contains more than one
    `*_main.c` and you need to disambiguate.

=== "Mixed"

    ```
    # Spring 2026 manifest

    HW1 students/alice/hw1
    HW2 students/alice/hw2        HW2_main.c
    Lab3 students/alice/lab3       # explicit comment
    ```

## What happens on parse failure

`tools/select_assignments.py` raises a `ValueError` with the offending
line quoted:

```
manifest line needs at least 'ID FOLDER': '   HW1\n'
```

The workflow exits with a non-zero code; the cause is shown in the
Actions log.

## Authoritative parser

The reference implementation is
[`tools/select_assignments.py::parse_manifest`](python-tools.md#select_assignments).
Read the module docstring for matrix selection rules, slot derivation,
and the `--self-test` entry point.
