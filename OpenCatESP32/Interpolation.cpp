/**
 * @file Interpolation.cpp
 * @brief Implementation of interpolation curves.
 *
 * Full implementation provided — these are pure math functions
 * with no external dependencies beyond Arduino.h and math.h.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#include "Interpolation.h"
#include "Utils.h"
#include <math.h>

float Interpolation::linear(float t) {
    return Utils::clampf(t, 0.0f, 1.0f);
}

float Interpolation::cosine(float t) {
    t = Utils::clampf(t, 0.0f, 1.0f);
    return (1.0f - cosf(t * PI)) * 0.5f;
}

float Interpolation::cubic(float t) {
    t = Utils::clampf(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

float Interpolation::easeIn(float t) {
    t = Utils::clampf(t, 0.0f, 1.0f);
    return t * t;
}

float Interpolation::easeOut(float t) {
    t = Utils::clampf(t, 0.0f, 1.0f);
    return t * (2.0f - t);
}

float Interpolation::easeInOut(float t) {
    t = Utils::clampf(t, 0.0f, 1.0f);
    if (t < 0.5f) {
        return 4.0f * t * t * t;
    } else {
        float f = (2.0f * t - 2.0f);
        return 0.5f * f * f * f + 1.0f;
    }
}

float Interpolation::interpolate(float from, float to, float t,
                                  InterpolationType type) {
    float factor;
    switch (type) {
        case InterpolationType::LINEAR:      factor = linear(t);     break;
        case InterpolationType::COSINE:      factor = cosine(t);     break;
        case InterpolationType::CUBIC:       factor = cubic(t);      break;
        case InterpolationType::EASE_IN:     factor = easeIn(t);     break;
        case InterpolationType::EASE_OUT:    factor = easeOut(t);    break;
        case InterpolationType::EASE_IN_OUT: factor = easeInOut(t);  break;
        default:                             factor = linear(t);     break;
    }
    return from + (to - from) * factor;
}
