#pragma once

#define WS2801_STRIP false
#if WS2801_STRIP
#include <Adafruit_WS2801.h>
#else
#include <Adafruit_NeoPixel.h>
#endif

// Elsewhere we always refer to the pixel count as strip.numPixels().
const size_t NUM_PIXELS = 50;

const uint8_t dataPin  = 4;
const uint8_t clockPin = 5;
const uint8_t inputPin = 14;

#if WS2801_STRIP
Adafruit_WS2801 strip = Adafruit_WS2801(NUM_PIXELS, dataPin, clockPin);
#else
// Only need one pin, dataPin (4 on the corner) makes way more sense but I messed up Amanada's so here we are.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, clockPin, NEO_RGB + NEO_KHZ800);
#endif

const int8_t BRIGHTNESS_FACTORS[] = {
  // disable full brightness
  255 / 4,  // full brightness
  255 / 6,  // less bright <- (default when bandolier comes on)
  255 / 8,  // even less
  255 / 12, // even even less
  0,        // off
};
const size_t brightnessCount = sizeof(BRIGHTNESS_FACTORS) / sizeof(BRIGHTNESS_FACTORS[0]);
size_t brightnessIndex = 1; // Start a little less than full bright
// global used to more easily set brightness
byte brightnessLevel = BRIGHTNESS_FACTORS[brightnessIndex];

void changeBrightness() {
  // TODO Turn off the WiFi radio when brightnessFactor is -1.
  brightnessIndex = (brightnessIndex + 1) % brightnessCount;
  brightnessLevel = BRIGHTNESS_FACTORS[brightnessIndex];
  strip.setBrightness(brightnessLevel);
}

// Create a 24 bit color value from R,G,B
uint32_t Color(byte rgb[])
{
  // https://adafruit.github.io/Adafruit_NeoPixel/html/class_adafruit___neo_pixel.html
  // https://learn.adafruit.com/fancyled-library-for-circuitpython/led-colors
  return Adafruit_NeoPixel::Color(rgb[0], rgb[1], rgb[2]);
}

void setRGB(byte r, byte g, byte b, byte rgb[]) {
  rgb[0] = r;
  rgb[1] = g;
  rgb[2] = b;
}

byte weightedAvg(byte a, byte b, byte weight) {
  return byte((int(a) * (255 - weight) + int(b) * weight) / 255);
}

// return a color that is a blend of the two inputs, weight between 0 (color1) and 255 (color2)
void blend(byte color1[], byte color2[], byte weight, byte out[]) {
  for (int i = 0; i < 3; i++) {
    out[i] = weightedAvg(color1[i], color2[i], weight);
  }
}

void clearStrip() {
  for (int i = 0; i < strip.numPixels(); i++) {
    byte rgb[] = {0, 0, 0};
    strip.setPixelColor(i, Color(rgb));
  }
  strip.show();
}
