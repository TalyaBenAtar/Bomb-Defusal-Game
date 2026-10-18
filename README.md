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


---

## 🎥 Demo Video

Watch the Bomb Defusal Game in action on the Digilent PIC32 development board!

[▶️ Watch the Full Demo](https://drive.google.com/file/d/1XgkU0teixp-o_1GSaAr-xydii4x0bIVV/view?usp=sharing)

---


## 📸 Project Gallery

A look at the Bomb Defusal Game running on the Digilent PIC32 development board, featuring interactive challenges, physical controls, and real-time feedback.

<table>
  <tr>
    <td align="center" width="33%">
      <img src="https://github.com/user-attachments/assets/1c81c24f-0dc6-4fc9-b8fd-90ae15b3d49b" width="230"/>
      <br/>
      <b>Wire Cutting Challenge</b>
    </td>
    <td align="center" width="33%">
      <img src="https://github.com/user-attachments/assets/ec234051-c1fa-4325-af13-1406a16d946e" width="230"/>
      <br/>
      <b>Target Challenge</b>
    </td>
    <td align="center" width="33%">
      <img src="https://github.com/user-attachments/assets/61330231-8380-4106-8a02-510ea647b276" width="230"/>
      <br/>
      <b>Letter Challenge</b>
    </td>
  </tr>
</table>



