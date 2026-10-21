# Button matrix to resistance converter

This project adapts matrix buttons to button resistance input. It scans the matrix and tells MCP4241 what resistance to set on its secondary output P1.
The output is connected to A0 just to use the soldered pin, not needed by the firmware.

Opel radio buttons are one example, which are 3x4 matrix with the following pinout:
- pins 3,4,5,6: rows
- pins 7,8,9: columns
- pin 1: +12V for illumination
- pin 2: resistor to ground for panel presence detection
- pin 10: ground

Arduino should be connected to entertainment system (Junsun) as follows:

| Junsun       | Arduino  | MCP4241              | Opel button panel |
|--------------|----------|----------------------|-------------------|
| Gnd          | Gnd      | pin 4 Vss, pin 5 P1B | pin 10            |
| +5V from USB | Raw      |                      |                   |
| KEY1         | A3       | pin 6 P1W            |                   |
| +ACC         |          |                      | pin 1             |
|              | Vcc      | pin 14 Vdd           |                   |
|              | GPIO 3   |                      | pin 3             |
|              | GPIO 4   |                      | pin 4             |
|              | GPIO 5   |                      | pin 5             |
|              | GPIO 6   |                      | pin 6             |
|              | GPIO 7   |                      | pin 7             |
|              | GPIO 8   |                      | pin 8             |
|              | GPIO 9   |                      | pin 9             |
|              | GPIO 10  | pin  1 CS\           |                   |
|              | GPIO     | pin  2 SCK           |                   |
|              | GPIO 12  | pin  3 SDI           |                   |
|              | GPIO     | pin 13 SDO           |                   |
|              | pullup R | pin 12 SHDN\         |                   |
|              | pullup R | pin 11 WP\           |                   |
