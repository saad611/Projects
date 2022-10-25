// Basic rainbow effect.

#include "Arduino.h"

class Rainbow {
  public:
    Rainbow(){};
    bool runPattern();
  private:
    void drawOneFrame(byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8);
};

bool Rainbow::runPattern() {
//  if(checkButton()) return false;
  uint32_t ms = millis();
  int32_t yHueDelta32 = ((int32_t) cos16(ms * (27 / 1)) * (350 / kMatrixWidth));
  int32_t xHueDelta32 = ((int32_t) cos16(ms * (39 / 1)) * (310 / kMatrixHeight));
  drawOneFrame(ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
  FastLED.show();
  return true;
}

void Rainbow::drawOneFrame(byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8) {
  byte lineStartHue = startHue8;
  for (byte y = 0; y < kMatrixHeight; y++) {
    lineStartHue += yHueDelta8;
    byte pixelHue = lineStartHue;
    for (byte x = 0; x < kMatrixWidth; x++) {
      pixelHue += xHueDelta8;
      leds[XY(13, 13)] = CRGB(255,0,0);
      leds[XY(12, 12)] = CRGB(255,0,0);
      leds[XY(11, 11)] = CRGB(255,0,0);
      leds[XY(10, 10)] = CRGB(255,0,0);
      leds[XY(9, 9)] = CRGB(255,0,0);
      leds[XY(8, 8)] = CRGB(255,0,0);
      leds[XY(8, 8)] = CRGB(255,0,0);
      leds[XY(7, 7)] = CRGB(255,0,0);
      leds[XY(6, 6)] = CRGB(255,0,0);
      leds[XY(5, 5)] = CRGB(255,0,0);
      leds[XY(4, 4)] = CRGB(255,0,0);
      leds[XY(3, 3)] = CRGB(255,0,0);

      leds[XY(12, 13)] = CRGB(255,0,0);
      leds[XY(11, 12)] = CRGB(255,0,0);
      leds[XY(10, 11)] = CRGB(255,0,0);
      leds[XY(9, 10)] = CRGB(255,0,0);
      leds[XY(8, 9)] = CRGB(255,0,0);
      leds[XY(7, 8)] = CRGB(255,0,0);
      leds[XY(6, 7)] = CRGB(255,0,0);
      leds[XY(5, 6)] = CRGB(255,0,0);
      leds[XY(4, 5)] = CRGB(255,0,0);
      leds[XY(3, 4)] = CRGB(255,0,0);
      leds[XY(2, 3)] = CRGB(255,0,0);
      leds[XY(1, 2)] = CRGB(255,0,0);
      
      leds[XY(13, 12)] = CRGB(255,0,0);
      leds[XY(12, 11)] = CRGB(255,0,0);
      leds[XY(11, 10)] = CRGB(255,0,0);
      leds[XY(10, 9)] = CRGB(255,0,0);
      leds[XY(9, 8)] = CRGB(255,0,0);
      leds[XY(8, 7)] = CRGB(255,0,0);
      leds[XY(7, 6)] = CRGB(255,0,0);
      leds[XY(6, 5)] = CRGB(255,0,0);
      leds[XY(5, 4)] = CRGB(255,0,0);
      leds[XY(4, 3)] = CRGB(255,0,0);
      leds[XY(3, 2)] = CRGB(255,0,0);
      leds[XY(2, 1)] = CRGB(255,0,0);

      leds[XY(12, 11)] = CRGB(255,0,0);
      leds[XY(11, 10)] = CRGB(255,0,0);
      leds[XY(10, 9)] = CRGB(255,0,0);
      leds[XY(9, 8)] = CRGB(255,0,0);
      leds[XY(8, 7)] = CRGB(255,0,0);
      leds[XY(7, 6)] = CRGB(255,0,0);
      leds[XY(6, 5)] = CRGB(255,0,0);
      leds[XY(5, 4)] = CRGB(255,0,0);
      leds[XY(4, 3)] = CRGB(255,0,0);
      leds[XY(3, 2)] = CRGB(255,0,0);
      leds[XY(2, 1)] = CRGB(255,0,0);
      leds[XY(1, 0)] = CRGB(255,0,0);

    }
  }
}
