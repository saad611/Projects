#include <FastLED.h>

#define DATA_PIN           15           // Output pin for LEDs [5]
#define COLOR_ORDER       GRB         // Color order of LED string [GRB]
#define CHIPSET           WS2812B     // LED string type [WS2182B]
#define BRIGHTNESS        255         // Overall brightness [50]
#define LAST_VISIBLE_LED  220         // Last LED that's visible [102]
#define MAX_MILLIAMPS     5000        // Max current in mA to draw from supply [500]
#define SAMPLE_WINDOW     100         // How many ms to sample audio for [100]
#define PATTERN_TIME      10          // Seconds to show each pattern on autoChange [10]
#define kMatrixWidth      17          // Matrix width [15]
#define kMatrixHeight     17          // Matrix height [11]
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)                                       // Total number of Leds
#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)   // Largest dimension of matrix

CRGB leds[ NUM_LEDS ];


// Used to check RAM availability. Usage: Serial.println(freeRam());
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

// Include circle patterns
#include "Rainbow.h"

    uint16_t XY (uint16_t x, uint16_t y) {
    // any out of bounds address maps to the first hidden pixel
    if( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
    return (LAST_VISIBLE_LED + 1);
    }
    
    const uint16_t XYTable[] = {
    277, 269, 263, 259, 257, 255, 136, 119, 102, 85, 68, 253, 251, 247, 241, 233, 221,
    278, 270, 264, 260, 168, 153, 137, 120, 103, 86, 69, 53, 38, 248, 242, 234, 222,
    279, 271, 265, 183, 169, 154, 138, 121, 104, 87, 70, 54, 39, 25, 243, 235, 223,
    280, 272, 196, 184, 170, 155, 139, 122, 105, 88, 71, 55, 40, 26, 14, 236, 224,
    281, 207, 197, 185, 171, 156, 140, 123, 106, 89, 72, 56, 41, 27, 15, 5, 225,
    282, 208, 198, 186, 172, 157, 141, 124, 107, 90, 73, 57, 42, 28, 16, 6, 226,
    216, 209, 199, 187, 173, 158, 142, 125, 108, 91, 74, 58, 43, 29, 17, 7, 0,
    217, 210, 200, 188, 174, 159, 143, 126, 109, 92, 75, 59, 44, 30, 18, 8, 1,
    218, 211, 201, 189, 175, 160, 144, 127, 110, 93, 76, 60, 45, 31, 19, 9, 2,
    219, 212, 202, 190, 176, 161, 145, 128, 111, 94, 77, 61, 46, 32, 20, 10, 3,
    220, 213, 203, 191, 177, 162, 146, 129, 112, 95, 78, 62, 47, 33, 21, 11, 4,
    283, 214, 204, 192, 178, 163, 147, 130, 113, 96, 79, 63, 48, 34, 22, 12, 227,
    284, 215, 205, 193, 179, 164, 148, 131, 114, 97, 80, 64, 49, 35, 23, 13, 228,
    285, 273, 206, 194, 180, 165, 149, 132, 115, 98, 81, 65, 50, 36, 24, 237, 229,
    286, 274, 266, 195, 181, 166, 150, 133, 116, 99, 82, 66, 51, 37, 244, 238, 230,
    287, 275, 267, 261, 182, 167, 151, 134, 117, 100, 83, 67, 52, 249, 245, 239, 231,
    288, 276, 268, 262, 258, 256, 152, 135, 118, 101, 84, 254, 252, 250, 246, 240, 232
    };
    
    uint16_t i = (y * kMatrixWidth) + x;
    uint16_t j = XYTable[i];
    return j;
    }

void setup() {
  FastLED.addLeds < CHIPSET, DATA_PIN, COLOR_ORDER > (leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear(true);
  
  Serial.begin(57600);
  Serial.print(F("Starting pattern "));
//  Serial.println(buttonPushCounter);
}

void runRainbow(){
  bool isRunning = true;
  Rainbow rainbow = Rainbow();
  while(isRunning) isRunning = rainbow.runPattern();
}


// Run selected pattern
void loop() {

    runRainbow();
}
