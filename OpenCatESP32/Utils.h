/**
 * @file Utils.h
 * @brief Utility functions and macros for the OpenCatESP32 firmware.
 *
 * Provides float-safe math helpers, unit conversions, timing utilities,
 * and debug print macros used across all modules.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include "Config.h"

// ─────────────────────────────────────────────────────────────────────────────
// Debug Macros
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Debug print macro — compiles to nothing when OPENCAT_DEBUG is 0.
 */
#if OPENCAT_DEBUG
    #define DEBUG_PRINT(...)   Serial.print(__VA_ARGS__)
    #define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
    #define DEBUG_PRINTF(...)  Serial.printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...)
    #define DEBUG_PRINTLN(...)
    #define DEBUG_PRINTF(...)
#endif

// ─────────────────────────────────────────────────────────────────────────────
// Utility Namespace
// ─────────────────────────────────────────────────────────────────────────────

namespace Utils {

    // ── Float Math ──────────────────────────────────────────────────────────

    /**
     * @brief Clamp a float value between min and max bounds.
     * @param value The value to clamp.
     * @param minVal Lower bound.
     * @param maxVal Upper bound.
     * @return The clamped value.
     */
    float clampf(float value, float minVal, float maxVal);

    /**
     * @brief Map a float value from one range to another.
     * @param value  Input value.
     * @param inMin  Input range minimum.
     * @param inMax  Input range maximum.
     * @param outMin Output range minimum.
     * @param outMax Output range maximum.
     * @return The mapped value.
     */
    float mapf(float value, float inMin, float inMax, float outMin, float outMax);

    /**
     * @brief Linear interpolation between two values.
     * @param a Start value.
     * @param b End value.
     * @param t Normalized parameter [0.0, 1.0].
     * @return Interpolated value.
     */
    float lerpf(float a, float b, float t);

    // ── Unit Conversions ────────────────────────────────────────────────────

    /**
     * @brief Convert servo angle (degrees) to pulse width (microseconds).
     *
     * Uses the configured PULSE_MIN_US and PULSE_MAX_US from Config.h.
     *
     * @param degrees Servo angle in degrees [0, 180].
     * @return Pulse width in microseconds.
     */
    uint16_t degreesToMicroseconds(float degrees);

    /**
     * @brief Convert pulse width (microseconds) to servo angle (degrees).
     * @param microseconds Pulse width in microseconds.
     * @return Angle in degrees.
     */
    float microsecondsToDegrees(uint16_t microseconds);

    /**
     * @brief Convert microseconds to PCA9685 tick count.
     *
     * Uses 12-bit resolution (0–4095) at the configured PWM frequency.
     *
     * @param microseconds Pulse width in microseconds.
     * @return PCA9685 tick value (0–4095).
     */
    uint16_t microsecondsToTicks(uint16_t microseconds);

    /**
     * @brief Convert PCA9685 ticks to microseconds.
     * @param ticks PCA9685 tick count.
     * @return Pulse width in microseconds.
     */
    uint16_t ticksToMicroseconds(uint16_t ticks);

    /**
     * @brief Convert degrees to radians.
     * @param degrees Angle in degrees.
     * @return Angle in radians.
     */
    float degreesToRadians(float degrees);

    /**
     * @brief Convert radians to degrees.
     * @param radians Angle in radians.
     * @return Angle in degrees.
     */
    float radiansToDegrees(float radians);

    // ── Timing ──────────────────────────────────────────────────────────────

    /**
     * @brief Check if a time interval has elapsed since a given start time.
     *
     * Handles millis() overflow correctly.
     *
     * @param startMs  Start timestamp from millis().
     * @param intervalMs Duration to check against.
     * @return true if the interval has elapsed.
     */
    bool hasElapsed(uint32_t startMs, uint32_t intervalMs);

    /**
     * @brief Get elapsed time since a start timestamp.
     *
     * Handles millis() overflow correctly.
     *
     * @param startMs Start timestamp from millis().
     * @return Elapsed time in milliseconds.
     */
    uint32_t elapsedSince(uint32_t startMs);

} // namespace Utils

#endif // UTILS_H
