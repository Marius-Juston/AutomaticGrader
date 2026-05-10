# AutomaticGrader

Automated checker for SE 423 (UIUC) homework and lab firmware. Compiles
student code written for the TI C2000 (TMS320F28379D LaunchPad) against
Linux stubs, drives the patched `main()` cooperatively from a single
thread, and grades the submission by inspecting simulated peripheral
register state plus captured `serial_printf` output.

Designed to slot into a course CI/CD pipeline as a reusable GitHub
Actions workflow so submissions are graded automatically before they
reach Gradescope.

---

## Where to start

<div class="grid cards" markdown>

-   :material-rocket-launch:{ .lg .middle } **Run it locally**

    ---

    Install dependencies, build for one assignment, run the grader on a
    reference fixture.

    [:octicons-arrow-right-24: Getting started](getting-started.md)

-   :material-school:{ .lg .middle } **Add a new checker**

    ---

    Step-by-step from a fresh assignment spec to a passing `hw{N}.cpp`
    with a green CI run.

    [:octicons-arrow-right-24: Contributor tutorial](contributing/add-a-checker.md)

-   :material-layers:{ .lg .middle } **Understand the architecture**

    ---

    Three-layer model, cooperative main-loop driver, capture pipeline,
    and the four-phase check pattern.

    [:octicons-arrow-right-24: Architecture overview](architecture/overview.md)

-   :material-cog:{ .lg .middle } **Wire it into a course repo**

    ---

    Reusable workflow setup, manifest format, duplicate-id submissions,
    and per-folder report traceability.

    [:octicons-arrow-right-24: CI/CD workflow](workflow/ci-cd.md)

</div>

## What the grader does, in one diagram

```mermaid
flowchart LR
    Spec[HW{N}.tex<br/>Lab{N}.txt]
    Student[Student firmware<br/>workspace/&lt;folder&gt;]
    Patcher[patch_student_source.py]
    Driver[Cooperative<br/>main-loop driver]
    Checker[hw{N}.cpp /<br/>lab{N}.cpp]
    Stubs[TI peripheral<br/>stubs + capture]
    Report[Markdown +<br/>JSON report]

    Spec -.authoritative.-> Checker
    Student --> Patcher --> Driver
    Driver <--> Stubs
    Checker <--> Stubs
    Checker --> Report

    classDef src fill:#dbeafe,stroke:#2563eb
    classDef harness fill:#fef3c7,stroke:#d97706
    classDef out fill:#dcfce7,stroke:#16a34a
    class Spec,Student src
    class Patcher,Driver,Stubs,Checker harness
    class Report out
```

## Authority hierarchy

1. **Spec is absolute.** Every checker assertion must trace to a quoted
   sentence in `context/homeworks/HW{N}.tex` or `context/lab/Lab{N}.txt`.
2. **Where the spec is ambiguous**, tag the assertion `[ambiguous]` in
   the assignment roadmap and pick the most common interpretation.
3. **Reference solutions are non-authoritative.** They inform stub
   design and serve as one positive-test fixture. Where they deviate
   from the spec, the spec wins and the reference is allowed to fail.

## Repository status

The full per-assignment implementation matrix lives in the
[CLAUDE.md](https://github.com/Marius-Juston/AutomaticGrader/blob/master/CLAUDE.md)
file and the roadmap files under
[`progress/`](https://github.com/Marius-Juston/AutomaticGrader/tree/master/progress).
Skim those when planning a new checker — they encode the validation
matrix (mutation testing, snapshot hygiene, regression) you'll be held
to.
