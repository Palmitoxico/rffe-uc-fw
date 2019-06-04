# RFFE Controller Firmware

Firmware for the RFFE Control Boards, loosely based on [https://github.com/lnls-dig/openMMC](openMMC), using the Cortex M3 LPC1769 microcontroller.

## Pre-requisites

The following packages must be installed on your system in order to compile the firmware:
- **gcc-arm-none-eabi**

**gcc-arm-none-eabi** can be installed from the pre-compiled files found at: https://launchpad.net/gcc-arm-embedded/+download
or you can run the following command under Ubuntu:

	sudo apt-get install gcc-arm-none-eabi

Next step is to clone this repository into your workspace. Since we're using the mbed libraries as a submodule, you **MUST** run the git clone command with the `--recursive` option.

	git clone --recursive https://github.com/lnls-dig/rffe-fw

## Compilation
```bash
$ mkdir build
$ cd build
$ cmake .. -DTARGET_CONTROLLER=lpc1769 -DDEBUG_PROBE=cmsis-dap
$ make -j4
```

## Programming
Make sure you have a recent version of OpenOCD installed on your machine. The only supported debug probes for the lpc17xx targets are cmsis-dap and jlink. To select the debug probe, set the ```DEBUG_PROBE``` cmake flag:
```bash
$ cmake .. -DTARGET_CONTROLLER=lpc1769 -DDEBUG_PROBE=cmsis-dap
```

Then build and program:
```bash
$ make program_all
```


