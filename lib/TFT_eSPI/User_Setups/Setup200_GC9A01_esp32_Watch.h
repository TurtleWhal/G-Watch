// See SetupX_Template.h for all options available
#define USER_SETUP_ID 200

#define GC9A01_DRIVER

// For ESP32 Dev board (only tested with GC9A01 display)
// The hardware SPI can be mapped to any pins

#define TFT_MOSI 13 // In some display driver board, it might be written as "SDA" and so on.
#define TFT_DC   19  // Data Command control pin
#define TFT_SCLK 14
#define TFT_CS   15 // Chip select control pin
#define TFT_RST  27  // Reset pin (could connect to Arduino RESET pin)
#define TFT_BL   21  // LED back-light

#define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)

//#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
//#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
//#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
//#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
//#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
//#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT


//#define SPI_FREQUENCY  80000000
#define SPI_FREQUENCY  80000000

// Optional reduced SPI frequency for reading TFT
#define SPI_READ_FREQUENCY  5000000

//#define RP2040_PIO_SPI // Leave commented out to use standard RP2040 SPI port interface
#define TFT_SPI_PORT 0 // Set to 0 if SPI0 pins are used, or 1 if spi1 pins used

//Fake touch pin
//#define TOUCH_CS      // Chip select pin (T_CS) of touch screen