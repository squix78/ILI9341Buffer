
#include <Adafruit_GFX.h>    // Core graphics library
#include "ILI9341Buffer.h" // Hardware-specific library
#include <SPI.h>

#define TFT_DC D2
#define TFT_CS D1
#define TFT_LED D8

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

struct Point {
  uint16_t x,y;
  int8_t xd, yd;
  int8_t xv, yv;
  uint8_t color;
};

Point p[200];
int numPoints = 200;

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
    p[i].color = random(15);
  }

  delay(1000);
}
uint8_t col = 0;

uint16_t counter = 0;
long startMillis = millis();
uint16_t interval = 100;
void loop() {
  tft.fillBuffer(15);
  for (uint16_t i = 0; i < 10; i++) {
    for (uint16_t j = 0; j < 10; j++) {
      for (int k = 0; k < numPoints; k++) {
        tft.setBufferPixel(p[k].x + i, p[k].y + j, p[k].color);
      }
    }
  }
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
