# Python tools

Auto-generated reference for the Python tools under `tools/`. Module
docstrings are the authoritative spec for CLI behaviour and output
contracts; the prose pages link here for symbol-level detail.

## select_assignments

The matrix-selection driver. Reads `workspace/assignment.txt`, diffs
the push, and emits the GitHub Actions matrix.

::: select_assignments
    options:
      show_root_heading: false
      show_source: false
      members:
        - ManifestEntry
        - parse_manifest
        - assign_slots
        - select
        - git_diff
        - main

## render_report

The post-grade renderer. Aggregates per-slot results into per-folder
detail pages, ID-level aggregate pages, and a top-level index with
clickable folder + commit links.

::: render_report
    options:
      show_root_heading: false
      show_source: false
      members:
        - load_manifest
        - render_assignment_md
        - render_aggregate_md
        - render_index_md
        - render_step_summary
        - append_history
        - main

## patch_student_source

Build-time student-source rewriter. Injects the init-guard macro and
rewrites `while(1)` to the cooperative-driver loop. See
[Cooperative driver](../architecture/cooperative-driver.md) for the
contract.

::: patch_student_source
    options:
      show_root_heading: false
      show_source: false
      members:
        - patch
        - main

## Codegen tools

These three tools regenerate the auto-generated `src/checks/generated.cpp`,
`src/checks/compare_generated.cpp`, and the body of
`HardwareStateValidator::populate_all_zero()` from TI peripheral
register headers. Run them when a TI header changes or a new
peripheral is added.

### validation

::: validation
    options:
      show_root_heading: false
      show_source: false

### compare_validation

::: compare_validation
    options:
      show_root_heading: false
      show_source: false

### hash_map_setter

::: hash_map_setter
    options:
      show_root_heading: false
      show_source: false
