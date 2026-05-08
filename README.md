
# Automatic Grader for SE 423 UIUC's Homework and Lab code

The following repo is to automatically check and grade all of the Homework and Lab code for the TI  using the C2000 16-bit system compiler guide for the TMS320F28379D processor.

This is meant to be part of a CI/CD pipeline to automatically grade, give feedback about where mistakes are such that it will check the assignment before the submission to Gradescope or such.

This repo should be part of a standard larger project repo, where we make use of the CI/CD from this project to grade the student's work that will be part of the `workspace` folder. To mark which folder represents what, the `workspace` folder should have user made file as

```
<assigmnent ID> <relative path>
<assigmnent ID> <relative path>
<assigmnent ID> <relative path>
...
```

This repo would then run the necessary tests and provide output feedback in the GitHub action and in a outputted Markdown file showing the failed tests, suceeded tests and such.

This repo needs to be as seemless as possible.

The `context` folder contains a couple pieces of information. 
- `context/code_solutions` is the answer keys that the tests suite that we make should be based off. 
- `context/homeworks` is the actual homework assigment LaTeX instruction file that the students use
- `context/Lab` is the actual homework assigment text instruction file that the students use


# Requiresments

This cannot run in the MinGW (windows) compiler as  `__attribute__((weak))` does not exist otherwise.

Please run in Linux as the easiest option


```bash
sudo apt install libspdlog-dev
HW=1 cmake ..&& make && ./AutomaticGrader
```


From
```regexp
GPIO_SetupPinMux\((\d+),(.+)\);\n+\s+GPIO_SetupPinOptions\(\d+,(.+)\);
```
To
```regexp
expectedGpioSetup[$1] = {$2, $3};
```

