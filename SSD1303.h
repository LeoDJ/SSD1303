#include <Adafruit_GrayOLED.h>

#define BLACK 0
#define WHITE 1

#define SSD1303_I2C_ADDRESS 0x3C    // 011110+SA0+RW - 0x3C or 0x3D
#define SSD1303_I2C_RETRIES 5

#define SSD1303_SETLOWCOLUMN        0x00
#define SSD1303_SETHIGHCOLUMN       0x10
#define SSD1303_MEMORYMODE          0x20
#define SSD1303_SETCOLADDR          0x21
#define SSD1303_SETPAGEADDR         0x22
#define SSD1303_HORZSCROLL_STOP     0x2E
#define SSD1303_HORZSCROLL_START    0x2F
#define SSD1303_SETSTARTLINE        0x40

#define SSD1303_SETCONTRAST         0x81
#define SSD1303_SETBRIGHTNESS       0x82

#define SSD1303_SETLUT              0x91

#define SSD1303_SEGREMAP            0xA0
#define SSD1303_DISPLAYALLON_RESUME 0xA4
#define SSD1303_DISPLAYALLON        0xA5
#define SSD1303_NORMALDISPLAY       0xA6
#define SSD1303_INVERTDISPLAY       0xA7
#define SSD1303_SETMULTIPLEX        0xA8
#define SSD1303_DCDC                0xAD
#define SSD1303_DCDC_VAL            0x8A
#define SSD1303_DISPLAYOFF          0xAE
#define SSD1303_DISPLAYON           0xAF

#define SSD1303_SETPAGESTART        0xB0

#define SSD1303_COMSCANINC          0xC0    // COM scan normal
#define SSD1303_COMSCANDEC          0xC8    // COM scan remap
#define SSD1303_SETDISPLAYOFFSET    0xD3
#define SSD1303_SETDISPLAYCLOCKDIV  0xD5
#define SSD1303_SETAREACOLOR        0xD8
#define SSD1303_SETPRECHARGE        0xD9
#define SSD1303_SETCOMPINS          0xDA
#define SSD1303_SETVCOMLEVEL        0xDB
#define SSD1303_NOP                 0xE3

class SSD1303 : public Adafruit_GrayOLED {
  public:
    SSD1303(uint16_t w, uint16_t h, TwoWire *twi = &Wire, int8_t rst_pin = -1, uint32_t preclk = 400000, uint32_t postclk = 100000);
    SSD1303(uint16_t w, uint16_t h, int8_t mosi_pin, int8_t sclk_pin, int8_t dc_pin, int8_t rst_pin, int8_t cs_pin);
    SSD1303(uint16_t w, uint16_t h, SPIClass *spi, int8_t dc_pin, int8_t rst_pin, int8_t cs_pin, uint32_t bitrate = 8000000UL);

    bool begin(uint8_t i2caddr = SSD1303_I2C_ADDRESS, bool reset = true);
    void display(void);
    bool display(bool autoRetry);
    void sleep();
    void wake();

  private:
    uint8_t page_offset = 0;
    uint8_t column_offset = 0;

    void spiWriteFast(const uint8_t *buffer, size_t len);
    bool doDisplay();

    // Caution: needs to be under 32 byte long
    static constexpr inline uint8_t init_128x64[] = {
        // Init sequence for 128x32 OLED module
        SSD1303_DISPLAYOFF,         
        SSD1303_SETLOWCOLUMN |      0x0,                    // low col = 0
        SSD1303_SETHIGHCOLUMN |     0x0,                    // hi col = 0
        SSD1303_SETSTARTLINE |      0x0,
        SSD1303_HORZSCROLL_STOP,
        SSD1303_SETCONTRAST,        0x32,
        SSD1303_SETBRIGHTNESS,      0x80,
        SSD1303_SEGREMAP |          0x00,
        SSD1303_NORMALDISPLAY,
        SSD1303_SETMULTIPLEX,       0x3F,                   // 1/64
        SSD1303_DCDC,               SSD1303_DCDC_VAL | 1,   // external Vcc supply
        SSD1303_COMSCANDEC,
        SSD1303_SETDISPLAYOFFSET,   0x00,
        SSD1303_SETDISPLAYCLOCKDIV, 0xF0,
        SSD1303_SETAREACOLOR,       0x05,
        SSD1303_SETPRECHARGE,       0xF1,
        SSD1303_SETCOMPINS,         0x12,
        SSD1303_SETLUT,             0x3F, 0x3F, 0x3F, 0x3F
        };
};