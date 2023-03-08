// display settings for piPod

#define USER_SETUP_ID 702

// Define to disable all #warnings in library (can be put in User_Setup_Select.h)
#define DISABLE_ALL_LIBRARY_WARNINGS

// ##################################################################################
//
// Section 1. Call up the right driver file and any options for it
//
// ##################################################################################


// Tell the library to use 8 bit parallel mode (otherwise SPI is assumed)
#define TFT_PARALLEL_8_BIT

// There are two drivers available for the pod display, choose the one.
#define ILI9341_DRIVER       // Generic driver for common displays
//#define ST7789_DRIVER      // Full configuration option, define additional parameters below for this display
//#define ST7789_2_DRIVER    // Minimal configuration option, define additional parameters below for this display

// If using ST7789, uncomment those lines
// #define TFT_WIDTH  240 // ST7789 240 x 240 and 240 x 320
// #define TFT_HEIGHT 320 // ST7789 240 x 320

// If colours are inverted (white shows as black) then uncomment one of the next
// 2 lines try both options, one of the options should correct the inversion.

// #define TFT_INVERSION_ON
// #define TFT_INVERSION_OFF


// ##################################################################################
//
// Section 2. Define the pins that are used to interface with the display here
//
// ##################################################################################

// default backlight control
// it's controlled by the main program, so PWM is available

#define TFT_BL     24            // LED back-light control pin
#define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)

//#define TFT_CS   -1  // Do not define, chip select control pin permanently connected to 0V

// These pins can be moved and are controlled directly by the library software
#define TFT_DC      1    // Data Command control pin
#define TFT_RST     0    // Reset pin

#define TFT_RD   3  // Do not define, read pin permanently connected to 3V3

// Note: All the following pins are PIO hardware configured and driven

  #define TFT_WR    2

  // PIO requires these to be sequentially increasing (and I believe it only needs the first one.)
  #define TFT_D0    4
  #define TFT_D1    5
  #define TFT_D2    6
  #define TFT_D3    7
  #define TFT_D4    8
  #define TFT_D5    9
  #define TFT_D6   10
  #define TFT_D7   11


// ##################################################################################
//
// Section 3. Define the fonts that are to be used here
//
// ##################################################################################

// Comment out the #defines below with // to stop that font being loaded
// If all fonts are loaded the extra FLASH space required is about 17Kbytes.
// To save FLASH space only enable the fonts you need!

 #define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
 #define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
 #define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
 #define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
 #define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
 #define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
 #define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// Comment out the #define below to stop the SPIFFS filing system and smooth font code being loaded
// this will save ~20kbytes of FLASH
#define SMOOTH_FONT
