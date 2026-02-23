
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