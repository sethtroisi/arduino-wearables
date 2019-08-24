#pragma once

#include <list>
#include <math.h>
#include "common.h"
#include "mesh.h"

typedef struct {
  uint32_t timeMs;
  size_t ledCount;
  size_t ledIndex;
  size_t meshCount;
  size_t meshIndex;
  int8_t dimnessFactor; // Not used?
} AnimationState;

unsigned long lastAnimationTimeMs = 0;
size_t frameDelayMs = 16;
std::list<uint32_t> nodes;

size_t computeRank() {
  size_t rank = 0;
  uint32_t ownNodeId = mesh.getNodeId();
  for (auto it = nodes.begin(); it != nodes.end(); ++it) {
    if (*it < ownNodeId) {
      ++rank;
    }
  }
  return rank;
}

// Return a brightness value 0-1 based on the time.
// Pulse is a sine wave of period 1s, magnitude 10% to 90%.
// TODO consider correcting brightness to percieved brightness
double brightnessPulse(uint32_t timeMs) {
  double sinFactor = sin(3.14159 * timeMs / 1000);
  return (0.9 * sinFactor + 1.) / 2.;
}

void animate(void (*fn)(AnimationState*, byte[3])) {
  unsigned long now = millis();
  // Render frames no more often than every 16ms, which gives a smooth 60fps.
  if (now - lastAnimationTimeMs < frameDelayMs) return;
  lastAnimationTimeMs = now;

  size_t otherNodesCount = meshGetNodes(nodes);

  AnimationState state = {
    // We use the built-in millis() function to throttle animation frames on this device,
    // but we use the synchronized mesh time for time-dependent animations, so all devices
    // connected to the mesh network can agree on the current time, regardless of when
    // they were first turned on.
    .timeMs = meshTimeMs(),
    .ledCount = strip.numPixels(),
    .ledIndex = 0,
    .meshCount = otherNodesCount + 1,
    .meshIndex = computeRank()
  };

  // Steady glow if we are the only node
  // Needs to stack with brightnessLevel in common.h
  byte brightness = brightnessLevel;
  if (otherNodesCount > 0) {
    // Otherwise pulse the color.
    brightness = brightness * brightnessPulse(state.timeMs);
  }
  strip.setBrightness(brightness);

  byte rgb[] = { 0, 0, 0 };
  while (state.ledIndex < state.ledCount) {
    fn(&state, rgb);
    strip.setPixelColor(state.ledIndex, Color(rgb));
    state.ledIndex += 1;
  }

  strip.show();
}
