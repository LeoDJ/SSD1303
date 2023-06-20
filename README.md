# SSD1303
Arduino library for OLED displays with SSD1303 controller.

## Connection
Which pins to connect where (based on a raw SSD1303 connection)

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
