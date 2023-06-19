#include "SSD1303.h"

// CONSTRUCTORS, DESTRUCTOR ------------------------------------------------

/*!
    @brief  Constructor for I2C-interfaced SSD1303 displays.
    @param  w
            Display width in pixels
    @param  h
            Display height in pixels
    @param  twi
            Pointer to an existing TwoWire instance (e.g. &Wire, the
            microcontroller's primary I2C bus).
    @param  rst_pin
            Reset pin (using Arduino pin numbering), or -1 if not used
            (some displays might be wired to share the microcontroller's
            reset pin).
    @param  clkDuring
            Speed (in Hz) for Wire transmissions in SSD1303 library calls.
            Defaults to 400000 (400 KHz), a known 'safe' value for most
            microcontrollers, and meets the SSD1303 datasheet spec.
            Some systems can operate I2C faster (800 KHz for ESP32, 1 MHz
            for many other 32-bit MCUs), and some (perhaps not all)
            SSD1303's can work with this -- so it's optionally be specified
            here and is not a default behavior. (Ignored if using pre-1.5.7
            Arduino software, which operates I2C at a fixed 100 KHz.)
    @param  clkAfter
            Speed (in Hz) for Wire transmissions following SSD1303 library
            calls. Defaults to 100000 (100 KHz), the default Arduino Wire
            speed. This is done rather than leaving it at the 'during' speed
            because other devices on the I2C bus might not be compatible
            with the faster rate. (Ignored if using pre-1.5.7 Arduino
            software, which operates I2C at a fixed 100 KHz.)
    @note   Call the object's begin() function before use -- buffer
            allocation is performed there!
*/
SSD1303::SSD1303(uint16_t w, uint16_t h, TwoWire *twi, int8_t rst_pin, uint32_t clkDuring, uint32_t clkAfter)
    : Adafruit_GrayOLED(1, w, h, twi, rst_pin, clkDuring, clkAfter) {}

/*!
    @brief  Constructor for SPI SSD1303 displays, using software (bitbang)
            SPI.
    @param  w
            Display width in pixels
    @param  h
            Display height in pixels
    @param  mosi_pin
            MOSI (master out, slave in) pin (using Arduino pin numbering).
            This transfers serial data from microcontroller to display.
    @param  sclk_pin
            SCLK (serial clock) pin (using Arduino pin numbering).
            This clocks each bit from MOSI.
    @param  dc_pin
            Data/command pin (using Arduino pin numbering), selects whether
            display is receiving commands (low) or data (high).
    @param  rst_pin
            Reset pin (using Arduino pin numbering), or -1 if not used
            (some displays might be wired to share the microcontroller's
            reset pin).
    @param  cs_pin
            Chip-select pin (using Arduino pin numbering) for sharing the
            bus with other devices. Active low.
    @note   Call the object's begin() function before use -- buffer
            allocation is performed there!
*/
SSD1303::SSD1303(uint16_t w, uint16_t h, int8_t mosi_pin, int8_t sclk_pin, int8_t dc_pin, int8_t rst_pin, int8_t cs_pin)
    : Adafruit_GrayOLED(1, w, h, mosi_pin, sclk_pin, dc_pin, rst_pin, cs_pin) {}

/*!
    @brief  Constructor for SPI SSD1303 displays, using native hardware SPI.
    @param  w
            Display width in pixels
    @param  h
            Display height in pixels
    @param  spi
            Pointer to an existing SPIClass instance (e.g. &SPI, the
            microcontroller's primary SPI bus).
    @param  dc_pin
            Data/command pin (using Arduino pin numbering), selects whether
            display is receiving commands (low) or data (high).
    @param  rst_pin
            Reset pin (using Arduino pin numbering), or -1 if not used
            (some displays might be wired to share the microcontroller's
            reset pin).
    @param  cs_pin
            Chip-select pin (using Arduino pin numbering) for sharing the
            bus with other devices. Active low.
    @param  bitrate
            SPI clock rate for transfers to this display. Default if
            unspecified is 8000000UL (8 MHz).
    @note   Call the object's begin() function before use -- buffer
            allocation is performed there!
*/
SSD1303::SSD1303(uint16_t w, uint16_t h, SPIClass *spi, int8_t dc_pin, int8_t rst_pin, int8_t cs_pin, uint32_t bitrate)
    : Adafruit_GrayOLED(1, w, h, spi, dc_pin, rst_pin, cs_pin, bitrate) {}

// ALLOCATE & INIT DISPLAY -------------------------------------------------

/*!
    @brief  Allocate RAM for image buffer, initialize peripherals and pins.
    @param  addr
            I2C address of corresponding SSD1303 display.
            SPI displays (hardware or software) do not use addresses, but
            this argument is still required (pass 0 or any value really,
            it will simply be ignored). Default if unspecified is 0.
    @param  reset
            If true, and if the reset pin passed to the constructor is
            valid, a hard reset will be performed before initializing the
            display. If using multiple SSD1303 displays on the same bus, and
            if they all share the same reset pin, you should only pass true
            on the first display being initialized, false on all others,
            else the already-initialized displays would be reset. Default if
            unspecified is true.
    @return true on successful allocation/init, false otherwise.
            Well-behaved code should check the return value before
            proceeding.
    @note   MUST call this function before any drawing or updates!
*/
bool SSD1303::begin(uint8_t addr, bool reset) {
    if (!Adafruit_GrayOLED::_init(addr, reset)) {
        return false;
    }

    if (!oled_commandList(init_128x64, sizeof(init_128x64))) {
        return false;
    }

    delay(10); 
    column_offset = 2; // currently hardcoded, TODO: make adjustable
    page_offset = 0;
    oled_command(SSD1303_DISPLAYON);    // // t_AF is 100ms, display takes that long to actually turn on
    // setContrast(0x2F);
    return true;
}

// Faster SPI write function (about 6x faster than spi_dev->write)
void SSD1303::spiWriteFast(const uint8_t *buffer, size_t len) {
    uint8_t scratchpad[len];
    memcpy(scratchpad, buffer, len);
    spi_dev->beginTransactionWithAssertingCS();
    spi_dev->transfer(scratchpad, len);
    spi_dev->endTransactionWithDeassertingCS();
}

/*!
    @brief  Do the actual writing of the internal frame buffer to display RAM
*/
void SSD1303::display() {
    yield();

    uint8_t *ptr = buffer;
    uint8_t dc_byte = 0x40;

    uint8_t bytes_per_page = WIDTH;
    uint8_t maxbuff = 128;

    uint8_t first_page = window_y1 / 8;
    uint8_t last_page = (window_y2 + 7) / 8;
    uint8_t page_start = min((int16_t)bytes_per_page, window_x1);
    uint8_t page_end = max((int16_t)0, window_x2);

    if (i2c_dev) { // I2C
        // Set high speed clk
        i2c_dev->setSpeed(i2c_preclk);
        maxbuff = i2c_dev->maxBufferSize() - 1;
    }

    for (uint8_t p = first_page; p < last_page; p++) {
        uint8_t bytes_remaining = bytes_per_page;
        ptr = buffer + (uint16_t)p * (uint16_t)bytes_per_page;
        // fast forward to dirty rectangle beginning
        ptr += page_start;
        bytes_remaining -= page_start;
        // cut off end of dirty rectangle
        bytes_remaining -= (WIDTH - 1) - page_end;

        uint8_t cmd[] = {
            uint8_t(SSD1303_SETPAGESTART + p + page_offset), 
            uint8_t(0x10 + ((page_start + column_offset) >> 4)),
            uint8_t((page_start + column_offset) & 0xF)};
        oled_commandList(cmd, sizeof(cmd));

        while (bytes_remaining) {
            uint8_t to_write = min(bytes_remaining, maxbuff);
            if (i2c_dev) {
                i2c_dev->write(ptr, to_write, true, &dc_byte, 1);
            } else {
                digitalWrite(dcPin, HIGH);
                spiWriteFast(ptr, to_write);
            }
            ptr += to_write;
            bytes_remaining -= to_write;
            yield();
        }
    }

    if (i2c_dev) { // I2C
        // Set low speed clk
        i2c_dev->setSpeed(i2c_postclk);
    }
    else {
        // Datasheet specifies in §7.7: "During data writing an additional NOP command should be inserted before the CS# is pulled HIGH"
        oled_command(SSD1303_NOP);
    }

    // reset dirty window
    window_x1 = 1024;
    window_y1 = 1024;
    window_x2 = -1;
    window_y2 = -1;
}

void SSD1303::sleep() { oled_command(SSD1303_DISPLAYOFF); }

void SSD1303::wake() { oled_command(SSD1303_DISPLAYON); }