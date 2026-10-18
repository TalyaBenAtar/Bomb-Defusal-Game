# 💣 Bomb Defusal Game

An interactive, timed, multi-stage embedded game programmed in **C** for a **Microchip PIC32MX370F512L** microcontroller using **MPLAB X IDE** and the **XC32 compiler**.

Players interact with physical switches, buttons, keypad inputs, an analog control, and onboard displays to complete a sequence of challenges before the countdown expires.

## Gameplay

The game runs through four challenges:

1. **Wire-code challenge** — match the displayed code using physical switches. Incorrect attempts are limited; the stage repeats until completed.
2. **Letter challenge** — press the requested letters on the keypad, completing four correct selections.
3. **Target challenge** — navigate a cursor using buttons and confirm its position at a target.
4. **Analog matching challenge** — adjust an analog input until its measured ADC value matches the displayed target.

The program also uses visual feedback, including an LCD, RGB LEDs, and seven-segment display support. Hardware timer interrupts are used for time-related game behavior.

> **Implementation note:** The source's final path calls `bombExplodes(0)` after the fourth challenge. The end-of-game presentation should be checked against the demonstration video before describing a distinct victory screen.

## Technical highlights

- Register-level PIC32 peripheral configuration and GPIO interaction
- Timer 2 interrupt service routine
- Analog-to-digital conversion (ADC) for analog input challenges
- Input handling for switches, buttons, and keypad
- LCD feedback, RGB LEDs, and seven-segment display support
- Game logic, randomized challenge targets, and input validation

## Development environment

| Component | Details |
| --- | --- |
| MCU | PIC32MX370F512L (from MPLAB project configuration) |
| Language | C (embedded) |
| IDE | MPLAB X IDE |
| Compiler | Microchip XC32 (project configuration: 4.45) |
| Project format | MPLAB X `.X` project |

## Repository structure

```text
Bomb-Defusal-Game/
├── DEFUSE_BOMB_FINAL.X/
│   ├── BOMB_DEFUSING.c   # Main gameplay and peripheral control
│   ├── config.h          # Configuration and supporting definitions
│   ├── ssd.c             # Seven-segment display implementation
│   ├── ssd.h             # Seven-segment display declarations
│   ├── Makefile
│   └── nbproject/        # Shareable MPLAB X project settings
├── .gitignore
└── README.md
```

## Opening the project

1. Install **MPLAB X IDE** and the compatible **XC32** compiler / PIC32 device pack.
2. Choose **File → Open Project** and select `DEFUSE_BOMB_FINAL.X`.
3. Connect a compatible PIC32 development board and configure the programmer/debugger as appropriate.
4. Review the project's pin mapping and hardware assumptions before building or flashing.

**Note:** This repository preserves the original source and shared MPLAB X project configuration. It has not been rebuilt or tested on physical hardware as part of preparing this repository. The exact development-board model and peripheral wiring should be confirmed before reproducing the setup.

## Demo

A gameplay demonstration video can be linked here once uploaded.

## Project status

Archived student embedded-systems project. Source code is preserved as originally provided, without refactoring.
