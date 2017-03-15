
#include <Adafruit_GFX.h>    // Core graphics library
#include "ILI9341_SPI.h" // Hardware-specific library
#include "MiniGrafx.h"
#include "WeatherStationFonts.h"

#include <SPI.h>

#define TFT_DC D2
#define TFT_CS D1
#define TFT_LED D8

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
MiniGrafx gfx = MiniGrafx(&tft);
char iconMap[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O' }; 

struct Point {
  uint16_t x,y;
  int8_t xd, yd;
  int8_t xv, yv;
  uint8_t color;
};

uint8_t col = 0;

uint16_t counter = 0;
long startMillis = millis();
uint16_t interval = 100;

int numPoints = 30;
Point p[600];


void updatePoint(Point *p) {
  p->x = p->x + p->xd;
  p->y = p->y + p->yd;

  if (p->x < p->xv) {
    p->xd = p->xv;
  }
  if (p->x > 240 - 10 - p->xv) {
    p->xd = -p->xv;
  }
  if (p->y < p->yv) {
    p->yd = p->yv;
  }
  if (p->y > 320 - 10 - p->yv) {
    p->yd = -p->yv; 
  }
  
}

void setup() {
  Serial.begin(115200);
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);

  tft.begin();
  tft.fillScreen(ILI9341_BLACK);
  tft.fillScreen(ILI9341_RED);
  Serial.println(ESP.getFreeHeap());
  for (int i = 0; i < numPoints; i++) {
    p[i].x = random(230); 
    p[i].y = random(310);
    p[i].xv = random(6) + 1;
    p[i].yv = random(6) + 1;
    p[i].xd = p[i].xv;
    p[i].yd = p[i].yv;
    if (random(2) > 0) {
      p[i].xd = p[i].xv * -1;
    }
    if (random(2) > 0) {
      p[i].yd = p[i].yv * -1;
    }
    p[i].color = random(13) + 1;
  }

  startMillis = millis();
}

void loop() {
  tft.fillBuffer(0);
  gfx.setFont(Meteocons_Plain_42);
  for (int k = 0; k < numPoints - 2; k++) {
    //tft.setBufferPixel(p[k].x + i, p[k].y + j, p[k].color);
    //gfx.drawLine(p[k].x + i, p[k].y + j, p[k + 1].x + i, p[k + 1].y + j, p[k].color);
    gfx.setColor(p[k].color);
    //gfx.fillCircle(p[k].x, p[k].y, 4);
    gfx.drawString(p[k].x, p[k].y, String(iconMap[p[k].color]));
  }
  gfx.setColor(15);
  gfx.setFont(ArialMT_Plain_24);
  gfx.drawString(p[numPoints - 1].x, p[numPoints - 1].y, "ESP8266");
  //gfx.drawString(50, 50, "H A L L O");

  for (int i = 0; i < numPoints; i++) {
    updatePoint(&p[i]);
  }

  tft.writeBuffer();
  counter++;
  if (counter % interval == 0) {
    long millisSinceUpdate = millis() - startMillis;
    Serial.println(String(interval * 1000.0 / (millisSinceUpdate)) + "fps. Heap: " + String(ESP.getFreeHeap()));
    startMillis = millis();
  }
}
