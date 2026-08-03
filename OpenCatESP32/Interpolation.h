/**
 * @file Interpolation.h
 * @brief Interpolation engine for smooth servo motion.
 *
 * Provides multiple curve types for professional-quality motion blending.
 * Full implementation in Batch 2.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <Arduino.h>
#include "Constants.h"

/**
 * @class Interpolation
 * @brief Static utility class providing various interpolation curves.
 *
 * All methods take a normalized parameter t ∈ [0.0, 1.0] and return
 * a factor that can be used to blend between two values.
 */
class Interpolation {
public:
    /** @brief Linear interpolation (constant speed). */
    static float linear(float t);

    /** @brief Cosine interpolation (smooth S-curve). */
    static float cosine(float t);

    /** @brief Cubic Hermite interpolation. */
    static float cubic(float t);

    /** @brief Quadratic ease-in (slow start). */
    static float easeIn(float t);

    /** @brief Quadratic ease-out (slow end). */
    static float easeOut(float t);

    /** @brief Cubic ease-in-out (slow start and end). */
    static float easeInOut(float t);

    /**
     * @brief Interpolate between two values using a specified curve.
     * @param from Start value.
     * @param to   End value.
     * @param t    Normalized parameter [0.0, 1.0].
     * @param type The interpolation curve to use.
     * @return Interpolated value.
     */
    static float interpolate(float from, float to, float t,
                              InterpolationType type);
};

#endif // INTERPOLATION_H
