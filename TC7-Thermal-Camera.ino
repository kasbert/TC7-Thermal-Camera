
/*
 * Modified for TC7 transistor multi tester
 */

/***************************************************************************
  This is a library for the AMG88xx GridEYE 8x8 IR camera
  This sketch makes a 64 pixel thermal camera with the GridEYE sensor
  and a 128x128 tft screen https://www.adafruit.com/product/2088
  Designed specifically to work with the Adafruit AMG88 breakout
  ----> http://www.adafruit.com/products/3538
  These sensors use I2C to communicate. The device's I2C address is 0x69
  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!
  Written by Dean Miller for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

//#include <Wire.h>

// use 1.5.0
//#include <SoftwareWire.h>
#include "SoftwareWire/SoftwareWire.h"
#include "SoftwareWire/SoftwareWire.cpp"

// MODIFIED version, with software Wire
//#include <Adafruit_AMG88xx.h>
#include "Adafruit_AMG88xx/Adafruit_AMG88xx.h"
#include "Adafruit_AMG88xx/Adafruit_AMG88xx.cpp"

// Thermal camera sensor SDA , SCL
SoftwareWire myWire( PB1, PB0); 

#define TFT_MOSI       PIN_PB6  // Data out
#define TFT_SCLK       PIN_PB7  // Clock out
#define TFT_CS        -1
#define TFT_RST        PIN_PB4 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         PIN_PB5

//low range of the sensor (this will be blue on the screen)
#define MINTEMP 20

//high range of the sensor (this will be red on the screen)
#define MAXTEMP 40

//the colors we will be using
const uint16_t camColors[] = {ST77XX_BLACK, 0x400F, 0x400F, 0x4010, 0x3810, 0x3810, 0x3810, 0x3810, 0x3010, 0x3010,
                              0x3010, 0x2810, 0x2810, 0x2810, 0x2810, 0x2010, 0x2010, 0x2010, 0x1810, 0x1810,
                              0x1811, 0x1811, 0x1011, 0x1011, 0x1011, 0x0811, 0x0811, 0x0811, 0x0011, 0x0011,
                              0x0011, 0x0011, 0x0011, 0x0031, 0x0031, 0x0051, 0x0072, 0x0072, 0x0092, 0x00B2,
                              0x00B2, 0x00D2, 0x00F2, 0x00F2, 0x0112, 0x0132, 0x0152, 0x0152, 0x0172, 0x0192,
                              0x0192, 0x01B2, 0x01D2, 0x01F3, 0x01F3, 0x0213, 0x0233, 0x0253, 0x0253, 0x0273,
                              0x0293, 0x02B3, 0x02D3, 0x02D3, 0x02F3, 0x0313, 0x0333, 0x0333, 0x0353, 0x0373,
                              0x0394, 0x03B4, 0x03D4, 0x03D4, 0x03F4, 0x0414, 0x0434, 0x0454, 0x0474, 0x0474,
                              0x0494, 0x04B4, 0x04D4, 0x04F4, 0x0514, 0x0534, 0x0534, 0x0554, 0x0554, 0x0574,
                              0x0574, 0x0573, 0x0573, 0x0573, 0x0572, 0x0572, 0x0572, 0x0571, 0x0591, 0x0591,
                              0x0590, 0x0590, 0x058F, 0x058F, 0x058F, 0x058E, 0x05AE, 0x05AE, 0x05AD, 0x05AD,
                              0x05AD, 0x05AC, 0x05AC, 0x05AB, 0x05CB, 0x05CB, 0x05CA, 0x05CA, 0x05CA, 0x05C9,
                              0x05C9, 0x05C8, 0x05E8, 0x05E8, 0x05E7, 0x05E7, 0x05E6, 0x05E6, 0x05E6, 0x05E5,
                              0x05E5, 0x0604, 0x0604, 0x0604, 0x0603, 0x0603, 0x0602, 0x0602, 0x0601, 0x0621,
                              0x0621, 0x0620, 0x0620, 0x0620, 0x0620, 0x0E20, 0x0E20, 0x0E40, 0x1640, 0x1640,
                              0x1E40, 0x1E40, 0x2640, 0x2640, 0x2E40, 0x2E60, 0x3660, 0x3660, 0x3E60, 0x3E60,
                              0x3E60, 0x4660, 0x4660, 0x4E60, 0x4E80, 0x5680, 0x5680, 0x5E80, 0x5E80, 0x6680,
                              0x6680, 0x6E80, 0x6EA0, 0x76A0, 0x76A0, 0x7EA0, 0x7EA0, 0x86A0, 0x86A0, 0x8EA0,
                              0x8EC0, 0x96C0, 0x96C0, 0x9EC0, 0x9EC0, 0xA6C0, 0xAEC0, 0xAEC0, 0xB6E0, 0xB6E0,
                              0xBEE0, 0xBEE0, 0xC6E0, 0xC6E0, 0xCEE0, 0xCEE0, 0xD6E0, 0xD700, 0xDF00, 0xDEE0,
                              0xDEC0, 0xDEA0, 0xDE80, 0xDE80, 0xE660, 0xE640, 0xE620, 0xE600, 0xE5E0, 0xE5C0,
                              0xE5A0, 0xE580, 0xE560, 0xE540, 0xE520, 0xE500, 0xE4E0, 0xE4C0, 0xE4A0, 0xE480,
                              0xE460, 0xEC40, 0xEC20, 0xEC00, 0xEBE0, 0xEBC0, 0xEBA0, 0xEB80, 0xEB60, 0xEB40,
                              0xEB20, 0xEB00, 0xEAE0, 0xEAC0, 0xEAA0, 0xEA80, 0xEA60, 0xEA40, 0xF220, 0xF200,
                              0xF1E0, 0xF1C0, 0xF1A0, 0xF180, 0xF160, 0xF140, 0xF100, 0xF0E0, 0xF0C0, 0xF0A0,
                              0xF080, 0xF060, 0xF040, 0xF020, 0xF800, ST77XX_WHITE
                             };

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

Adafruit_AMG88xx amg;
unsigned long delayTime;
float pixels[AMG88xx_PIXEL_ARRAY_SIZE];
//uint16_t displayPixelWidth, displayPixelHeight;

uint8_t lastcolor[AMG88xx_PIXEL_ARRAY_SIZE];

void setup() {
  Serial.begin(9600);
  Serial.println(F("AMG88xx thermal camera!"));

  // For STC15L104W power controller
  // D port is broken in my TC7
  //pinMode(PD2, OUTPUT);
  //digitalWrite(PD2,1);

  //tft.setSPISpeed(24000000);

  tft.initR(INITR_BLACKTAB); // initialize a ST7735S chip
  tft.fillScreen(ST7735_BLACK);
  //tft.writeFillRectPreclipped(0, 0, 128, 160, ST7735_BLACK);
  //displayPixelWidth = tft.wddidth() / 8;
  //displayPixelHeight = tft.height() / 8;

  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextWrap(true);

  bool status;

  // default settings
  status = amg.begin();
  if (!status) {
    tft.print("Could not find a valid AMG88xx sensor, check wiring!");
    while (1);
  }

  delay(100); // let sensor boot up

  for (int i = 0; i < AMG88xx_PIXEL_ARRAY_SIZE; i++) {
    lastcolor[i] = -1;
  }
  for (int i = 0; i < 128; i++) {
    tft.drawFastVLine(i, 136, 24, camColors[i * 2]);
  }

  tft.setRotation(3);
  tft.setCursor(12, 4);
  tft.print(MINTEMP);
  tft.setCursor(12, 118);
  tft.print(MAXTEMP);
  tft.setRotation(1);
}

void loop() {
  //read all the pixels
  amg.readPixels(pixels);

  for (int i = 0; i < AMG88xx_PIXEL_ARRAY_SIZE; i++) {
    uint8_t colorIndex;
    if (pixels[i] < MINTEMP)
      colorIndex = 0;
    else if (pixels[i] > MAXTEMP)
      colorIndex = 255;
    else 
      colorIndex = 1 + (pixels[i] - MINTEMP) * 253 / (MAXTEMP - MINTEMP);
    //map(pixels[i], MINTEMP, MAXTEMP, 1, 254);
    //colorIndex = constrain(colorIndex, 0, 255);

    if (lastcolor[i] != colorIndex) {
      //draw the pixels!
      tft.writeFillRectPreclipped( 16 * (i & 7), 16 * (i >> 3),
                                   16, 16, camColors[colorIndex]);
      //    tft.fillRect( displayPixelWidth * (i & 7), displayPixelHeight * (i >> 3),
      //        displayPixelWidth, displayPixelHeight,camColors[colorIndex]);
      lastcolor[i] = colorIndex;
    }
  }

  delay(100);
}
