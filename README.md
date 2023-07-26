# SSD1303
Arduino library for OLED displays with SSD1303 controller.

## Possible Problems and Solutions
- In I²C mode, the display sometimes glitches or the MCU freezes completely:
  - Check your I²C pull-ups, you might need stronger ones (recommended 2.2k @ 3.3V / 4.7k @ 5V) or your cabling is too long or noisy
    - To narrow the issue down: Check the return value of `display(true)`. If it's false sometimes, it means the MCU encountered x consecutive I²C errors.
  - Try lowering the I²C speed to 400 or 100kHz
  - If nothing is helping, you might have to switch to SPI mode

## Connection
Which pins to connect where (based on a raw SSD1303 connection).  
Also check out [my breakout board](https://github.com/LeoDJ/SSD1303_Breakout).

### SPI
| Display Pin | Connect to                    |
| ----------- | ----------------------------- |
| VCC         | 12V                           |
| VCOMH       | 10 uF capacitor against GND   |
| IREF        | 910 kOhm resistor against GND |
| D/C         | any GPIO                      |
| RES         | any GPIO                      |
| CS          | SPI_SS / any GPIO             |
| D0          | SPI_SCK                       |
| D1          | SPI_MOSI                      |
| BS2         | GND                           |
| BS1         | GND                           |
| VDD         | 3.3V                          |
| GND         | GND                           |

### I²C
| Display Pin | Connect to                              |
| ----------- | --------------------------------------- |
| VCC         | 12V                                     |
| VCOMH       | 10 uF capacitor against GND             |
| IREF        | 910 kOhm resistor against GND           |
| **D/C**     | **GND: addr = 0x3C / VDD: addr = 0x3D** |
| RES         | any GPIO                                |
| **CS**      | **N/C**                                 |
| **D0**      | **SCL**                                 |
| **D1**      | **SDA**                                 |
| **D2**      | **connect to D1**                       |
| BS2         | GND                                     |
| **BS1**     | **VDD**                                 |
| VDD         | 3.3V                                    |
| GND         | GND                                     |

(differences to SPI in bold)

## Recognition
- This library is based on the Adafruit GFX library
- Most of the basic code was adapted to the SSD1303 from the Adafruit_SSD1305 library
