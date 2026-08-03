/**
 * @file Utils.cpp
 * @brief Implementation of utility functions for the OpenCatESP32 firmware.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#include "Utils.h"
#include <math.h>

namespace Utils {

// ─────────────────────────────────────────────────────────────────────────────
// Float Math
// ─────────────────────────────────────────────────────────────────────────────

float clampf(float value, float minVal, float maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

float mapf(float value, float inMin, float inMax, float outMin, float outMax) {
    if (fabsf(inMax - inMin) < 1e-6f) {
        return outMin;  // Avoid division by zero
    }
    return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

float lerpf(float a, float b, float t) {
    return a + (b - a) * clampf(t, 0.0f, 1.0f);
}

// ─────────────────────────────────────────────────────────────────────────────
// Unit Conversions
// ─────────────────────────────────────────────────────────────────────────────

uint16_t degreesToMicroseconds(float degrees) {
    float clamped = clampf(degrees,
                           Config::Servo::ANGLE_MIN_DEG,
                           Config::Servo::ANGLE_MAX_DEG);
    float us = mapf(clamped,
                    Config::Servo::ANGLE_MIN_DEG,
                    Config::Servo::ANGLE_MAX_DEG,
                    static_cast<float>(Config::Servo::PULSE_MIN_US),
                    static_cast<float>(Config::Servo::PULSE_MAX_US));
    return static_cast<uint16_t>(us + 0.5f);  // Round to nearest
}

float microsecondsToDegrees(uint16_t microseconds) {
    return mapf(static_cast<float>(microseconds),
                static_cast<float>(Config::Servo::PULSE_MIN_US),
                static_cast<float>(Config::Servo::PULSE_MAX_US),
                Config::Servo::ANGLE_MIN_DEG,
                Config::Servo::ANGLE_MAX_DEG);
}

uint16_t microsecondsToTicks(uint16_t microseconds) {
    // tick = microseconds * 4096 / 20000
    uint32_t ticks = (static_cast<uint32_t>(microseconds)
                      * Config::PCA9685::PWM_RESOLUTION)
                     / Config::PCA9685::PWM_PERIOD_US;
    if (ticks > Config::PCA9685::PWM_RESOLUTION - 1) {
        ticks = Config::PCA9685::PWM_RESOLUTION - 1;
    }
    return static_cast<uint16_t>(ticks);
}

uint16_t ticksToMicroseconds(uint16_t ticks) {
    uint32_t us = (static_cast<uint32_t>(ticks)
                   * Config::PCA9685::PWM_PERIOD_US)
                  / Config::PCA9685::PWM_RESOLUTION;
    return static_cast<uint16_t>(us);
}

float degreesToRadians(float degrees) {
    return degrees * (PI / 180.0f);
}

float radiansToDegrees(float radians) {
    return radians * (180.0f / PI);
}

// ─────────────────────────────────────────────────────────────────────────────
// Timing
// ─────────────────────────────────────────────────────────────────────────────

bool hasElapsed(uint32_t startMs, uint32_t intervalMs) {
    return (millis() - startMs) >= intervalMs;
}

uint32_t elapsedSince(uint32_t startMs) {
    return millis() - startMs;
}

} // namespace Utils
