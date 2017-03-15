
#include "MiniGrafx.h"

MiniGrafx::MiniGrafx(Adafruit_ILI9341 *driver) {
  this->driver = driver;
}

void MiniGrafx::setColor(uint16_t color) {
  this->color = color;
}

void MiniGrafx::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1) {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      driver->setPixel(y0, x0, color);
    } else {
      driver->setPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void MiniGrafx::drawCircle(int16_t x0, int16_t y0, uint16_t radius) {
    int16_t x = 0, y = radius;
  int16_t dp = 1 - radius;
  do {
    if (dp < 0)
      dp = dp + 2 * (++x) + 3;
    else
      dp = dp + 2 * (++x) - 2 * (--y) + 5;

    driver->setPixel(x0 + x, y0 + y, color);     //For the 8 octants
    driver->setPixel(x0 - x, y0 + y, color);
    driver->setPixel(x0 + x, y0 - y, color);
    driver->setPixel(x0 - x, y0 - y, color);
    driver->setPixel(x0 + y, y0 + x, color);
    driver->setPixel(x0 - y, y0 + x, color);
    driver->setPixel(x0 + y, y0 - x, color);
    driver->setPixel(x0 - y, y0 - x, color);

  } while (x < y);

  driver->setPixel(x0 + radius, y0, color);
  driver->setPixel(x0, y0 + radius, color);
  driver->setPixel(x0 - radius, y0, color);
  driver->setPixel(x0, y0 - radius, color);
}

void MiniGrafx::drawRect(int16_t x, int16_t y, int16_t width, int16_t height) {
  drawHorizontalLine(x, y, width);
  drawVerticalLine(x, y, height);
  drawVerticalLine(x + width - 1, y, height);
  drawHorizontalLine(x, y + height - 1, width);
}

void MiniGrafx::fillRect(int16_t xMove, int16_t yMove, int16_t width, int16_t height) {
  for (int16_t x = xMove; x < xMove + width; x++) {
    drawVerticalLine(x, yMove, height);
  }
}

void MiniGrafx::fillCircle(int16_t x0, int16_t y0, int16_t radius) {
  int16_t x = 0, y = radius;
  int16_t dp = 1 - radius;
  do {
    if (dp < 0)
      dp = dp + 2 * (++x) + 3;
    else
      dp = dp + 2 * (++x) - 2 * (--y) + 5;

    drawHorizontalLine(x0 - x, y0 - y, 2*x);
    drawHorizontalLine(x0 - x, y0 + y, 2*x);
    drawHorizontalLine(x0 - y, y0 - x, 2*y);
    drawHorizontalLine(x0 - y, y0 + x, 2*y);


  } while (x < y);
  drawHorizontalLine(x0 - radius, y0, 2 * radius);

}

void MiniGrafx::drawHorizontalLine(int16_t x, int16_t y, int16_t length) {
  for (int16_t i = 0; i < length; i++) {
    driver->setPixel(x + i, y, color);
  }
}

void MiniGrafx::drawVerticalLine(int16_t x, int16_t y, int16_t length) {
  for (int16_t i = 0; i < length; i++) {
    driver->setPixel(x, y + i, color);
  }

}

void MiniGrafx::drawString(int16_t xMove, int16_t yMove, String strUser) {
  uint16_t lineHeight = pgm_read_byte(fontData + HEIGHT_POS);

  // char* text must be freed!
  char* text = utf8ascii(strUser);

  uint16_t yOffset = 0;
  // If the string should be centered vertically too
  // we need to now how heigh the string is.
  if (textAlignment == TEXT_ALIGN_CENTER_BOTH) {
    uint16_t lb = 0;
    // Find number of linebreaks in text
    for (uint16_t i=0;text[i] != 0; i++) {
      lb += (text[i] == 10);
    }
    // Calculate center
    yOffset = (lb * lineHeight) / 2;
  }

  uint16_t line = 0;
  char* textPart = strtok(text,"\n");
  while (textPart != NULL) {
    uint16_t length = strlen(textPart);
    drawStringInternal(xMove, yMove - yOffset + (line++) * lineHeight, textPart, length, getStringWidth(textPart, length));
    textPart = strtok(NULL, "\n");
  }
  free(text);
}

void MiniGrafx::drawStringInternal(int16_t xMove, int16_t yMove, char* text, uint16_t textLength, uint16_t textWidth) {
  uint8_t textHeight       = pgm_read_byte(fontData + HEIGHT_POS);
  uint8_t firstChar        = pgm_read_byte(fontData + FIRST_CHAR_POS);
  uint16_t sizeOfJumpTable = pgm_read_byte(fontData + CHAR_NUM_POS)  * JUMPTABLE_BYTES;

  uint8_t cursorX         = 0;
  uint8_t cursorY         = 0;

  switch (textAlignment) {
    case TEXT_ALIGN_CENTER_BOTH:
      yMove -= textHeight >> 1;
    // Fallthrough
    case TEXT_ALIGN_CENTER:
      xMove -= textWidth >> 1; // divide by 2
      break;
    case TEXT_ALIGN_RIGHT:
      xMove -= textWidth;
      break;
  }

  // Don't draw anything if it is not on the screen.
  if (xMove + textWidth  < 0 || xMove > driver->getScreenWidth() ) {return;}
  if (yMove + textHeight < 0 || yMove > driver->getScreenHeight()) {return;}

  for (uint16_t j = 0; j < textLength; j++) {
    int16_t xPos = xMove + cursorX;
    int16_t yPos = yMove + cursorY;

    byte code = text[j];
    if (code >= firstChar) {
      byte charCode = code - firstChar;

      // 4 Bytes per char code
      byte msbJumpToChar    = pgm_read_byte( fontData + JUMPTABLE_START + charCode * JUMPTABLE_BYTES );                  // MSB  \ JumpAddress
      byte lsbJumpToChar    = pgm_read_byte( fontData + JUMPTABLE_START + charCode * JUMPTABLE_BYTES + JUMPTABLE_LSB);   // LSB /
      byte charByteSize     = pgm_read_byte( fontData + JUMPTABLE_START + charCode * JUMPTABLE_BYTES + JUMPTABLE_SIZE);  // Size
      byte currentCharWidth = pgm_read_byte( fontData + JUMPTABLE_START + charCode * JUMPTABLE_BYTES + JUMPTABLE_WIDTH); // Width

      // Test if the char is drawable
      if (!(msbJumpToChar == 255 && lsbJumpToChar == 255)) {
        // Get the position of the char data
        uint16_t charDataPosition = JUMPTABLE_START + sizeOfJumpTable + ((msbJumpToChar << 8) + lsbJumpToChar);
        drawInternal(xPos, yPos, currentCharWidth, textHeight, fontData, charDataPosition, charByteSize);
      }

      cursorX += currentCharWidth;
    }
  }
}

void MiniGrafx::setFont(const char *fontData) {
  this->fontData = fontData;
}


// Code form http://playground.arduino.cc/Main/Utf8ascii
uint8_t MiniGrafx::utf8ascii(byte ascii) {
  static uint8_t LASTCHAR;

  if ( ascii < 128 ) { // Standard ASCII-set 0..0x7F handling
    LASTCHAR = 0;
    return ascii;
  }

  uint8_t last = LASTCHAR;   // get last char
  LASTCHAR = ascii;

  switch (last) {    // conversion depnding on first UTF8-character
    case 0xC2: return  (ascii);  break;
    case 0xC3: return  (ascii | 0xC0);  break;
    case 0x82: if (ascii == 0xAC) return (0x80);    // special case Euro-symbol
  }

  return  0; // otherwise: return zero, if character has to be ignored
}

// You need to free the char!
char* MiniGrafx::utf8ascii(String str) {
  uint16_t k = 0;
  uint16_t length = str.length() + 1;

  // Copy the string into a char array
  char* s = (char*) malloc(length * sizeof(char));
  if(!s) {
    //DEBUG_OLEDDISPLAY("[OLEDDISPLAY][utf8ascii] Can't allocate another char array. Drop support for UTF-8.\n");
    return (char*) str.c_str();
  }
  str.toCharArray(s, length);

  length--;

  for (uint16_t i=0; i < length; i++) {
    char c = utf8ascii(s[i]);
    if (c!=0) {
      s[k++]=c;
    }
  }

  s[k]=0;

  // This will leak 's' be sure to free it in the calling function.
  return s;
}

void MiniGrafx::setTextAlignment(TEXT_ALIGNMENT textAlignment) {
  this->textAlignment = textAlignment;
}

uint16_t MiniGrafx::getStringWidth(const char* text, uint16_t length) {
  uint16_t firstChar        = pgm_read_byte(fontData + FIRST_CHAR_POS);

  uint16_t stringWidth = 0;
  uint16_t maxWidth = 0;

  while (length--) {
    stringWidth += pgm_read_byte(fontData + JUMPTABLE_START + (text[length] - firstChar) * JUMPTABLE_BYTES + JUMPTABLE_WIDTH);
    if (text[length] == 10) {
      maxWidth = max(maxWidth, stringWidth);
      stringWidth = 0;
    }
  }

  return max(maxWidth, stringWidth);
}

void inline MiniGrafx::drawInternal(int16_t xMove, int16_t yMove, int16_t width, int16_t height, const char *data, uint16_t offset, uint16_t bytesInData) {
  if (width < 0 || height < 0) return;
  if (yMove + height < 0 || yMove > driver->getScreenHeight())  return;
  if (xMove + width  < 0 || xMove > driver->getScreenWidth())   return;

  uint8_t  rasterHeight = 1 + ((height - 1) >> 3); // fast ceil(height / 8.0)
  int8_t   yOffset      = yMove & 7;

  bytesInData = bytesInData == 0 ? width * rasterHeight : bytesInData;

  int16_t initYMove   = yMove;
  int8_t  initYOffset = yOffset;

  uint8_t arrayHeight = (int) ceil(height / 8.0);
  for (uint16_t i = 0; i < bytesInData; i++) {
    byte currentByte = pgm_read_byte(data + offset + i);

    for (int b = 0; b < 8; b++) {
      if(bitRead(currentByte, b)) {
        uint16_t currentBit = i * 8 + b;
        uint16_t pixelX = (i / arrayHeight); 
        uint16_t pixelY = (i % arrayHeight) * 8;
        driver->setPixel(pixelX + xMove, pixelY + yMove + b, color);
      }
    }
    yield();

  }
}


