#include <Arduino.h>
#include <SSD1303.h>

// uncomment the variant you want to use
#define USE_HW_SPI
// #define USE_SW_SPI
// #define USE_I2C


// Used for I2C and SPI
#define OLED_RESET 21

// Used for software and hardware SPI
#define OLED_CS PIN_SPI_SS
#define OLED_DC 20

// Used for software SPI
#define OLED_CLK 13
#define OLED_MOSI 11

#if defined(USE_HW_SPI)
SSD1303 disp(128, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS);
#elif defined(USE_SW_SPI)
SSD1303 disp(128, 64, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
#elif defined(USE_I2C)
SSD1303 disp(128, 64, &Wire, OLED_RESET, 1e6);
#endif

void testDrawChar(void) {
    disp.clearDisplay();
    disp.setTextSize(1);
    disp.setTextWrap(false);
    disp.setTextColor(WHITE);
    disp.setCursor(0, 0);

    for (uint8_t i = 0; i < 168; i++) {
        if (i == '\n')
            continue;
        disp.write(i);
        if ((i > 0) && (i % 21 == 0))
            disp.println();
    }
    disp.display();
}

void testDrawCheckerboard(bool inverted) {
    disp.clearDisplay();
    for (int y = 0; y < disp.height(); y++) {
        for (int x = 0; x < disp.width(); x++) {
            disp.drawPixel(x, y, (x % 2) ^ (y % 2) ^ (inverted));
        }
    }
}

void waitForSerial(uint32_t timeout) {
    uint32_t start = millis();
    while (!Serial && millis() - start < timeout); // wait for Serial to open
    delay(100);
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    waitForSerial(5000);    // wait for Serial (for boards with native USB serial, e.g. Leonardo / Pi Pico)
    Serial.println("\nSSD1303 OLED Test");
    
    Serial.print("OLED init... ");
    bool ret = disp.begin();   
    Serial.println(ret ? "success" : "failed");

    // set brightness and clear display
    disp.setContrast(128);
    disp.clearDisplay();
    disp.display();

    // show welcome text
    disp.setTextSize(1);
    disp.setTextColor(WHITE);
    disp.setCursor(0, 0);
    disp.print("SSD1303 OLED Test\n\nHello World!");
    disp.display();
    delay(2000);

    // show character test pattern
    testDrawChar();
    delay(2000);
}

void loop() {
    // switch between checkerboard patterns
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));   // toggle LED to indicate display update
    testDrawCheckerboard(false);
    disp.display();
    delay(1000);
    
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    testDrawCheckerboard(true);
    disp.display();
    delay(1000);
}

