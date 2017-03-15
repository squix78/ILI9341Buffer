#include "ILI9341_SPI.h"
#include "MiniGrafxFonts.h"


#ifndef _MINI_GRAFXH_
#define _MINI_GRAFXH_

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

// Header Values
#define JUMPTABLE_BYTES 4

#define JUMPTABLE_LSB   1
#define JUMPTABLE_SIZE  2
#define JUMPTABLE_WIDTH 3
#define JUMPTABLE_START 4

#define WIDTH_POS 0
#define HEIGHT_POS 1
#define FIRST_CHAR_POS 2
#define CHAR_NUM_POS 3

enum TEXT_ALIGNMENT {
  TEXT_ALIGN_LEFT = 0,
  TEXT_ALIGN_RIGHT = 1,
  TEXT_ALIGN_CENTER = 2,
  TEXT_ALIGN_CENTER_BOTH = 3
};

class MiniGrafx {

 public:
  MiniGrafx(Adafruit_ILI9341 *driver);
  void drawPixel(int16_t x, int16_t y);
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
  void setColor(uint16_t color);
  void drawCircle(int16_t x0, int16_t y0, uint16_t radius);
  void drawRect(int16_t x, int16_t y, int16_t width, int16_t height);
  void fillRect(int16_t xMove, int16_t yMove, int16_t width, int16_t height);
  void fillCircle(int16_t x0, int16_t y0, int16_t radius);
  void drawHorizontalLine(int16_t x, int16_t y, int16_t length);
  void drawVerticalLine(int16_t x, int16_t y, int16_t length);
  void drawString(int16_t xMove, int16_t yMove, String strUser);
  void drawStringInternal(int16_t xMove, int16_t yMove, char* text, uint16_t textLength, uint16_t textWidth);
  uint16_t getStringWidth(const char* text, uint16_t length);
  void setFont(const char *fontData);
  void setTextAlignment(TEXT_ALIGNMENT textAlignment);
  void inline drawInternal(int16_t xMove, int16_t yMove, int16_t width, int16_t height, const char *data, uint16_t offset, uint16_t bytesInData);


  static char* utf8ascii(String s);
  static byte utf8ascii(byte ascii);

 private: 
  Adafruit_ILI9341 *driver;
  uint16_t color;
  const char *fontData = ArialMT_Plain_16;
  TEXT_ALIGNMENT textAlignment;

};

#endif
