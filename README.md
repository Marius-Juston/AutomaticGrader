
# Requiresments

This cannot run in the MinGW (windows) compiler as  `__attribute__((weak))` does not exist otherwise.

Please run in Linux as the easiest option


```bash
HW=1 cmake ..&& make && ./AutomaticGrader
```