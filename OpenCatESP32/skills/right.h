/**
 * @file skills/right.h
 * @brief Turn right gait skill definition (keyframe-based fallback).
 *
 * Differential turn: left legs stride full, right legs stride short.
 *
 * @note These are initial estimates — calibrate with your specific robot.
 * @author OpenCatESP32 Project
 */

#ifndef SKILL_RIGHT_H
#define SKILL_RIGHT_H

#include "../MotionEngine.h"

namespace Skills {

    static const Keyframe rightKeyframes[] = {
        // Phase 1: Left side steps forward, right side minimal
        {
            .pose = { .angles = {
                110.0f, 30.0f,   // FL: big step forward, lifted
                92.0f,  45.0f,   // FR: minimal movement
                90.0f,  45.0f,   // RL: stance
                88.0f,  45.0f    // RR: minimal
            }},
            .durationMs = 200,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 2: Plant left side
        {
            .pose = { .angles = {
                100.0f, 45.0f,   // FL: planted
                88.0f,  45.0f,   // FR: slight push
                110.0f, 30.0f,   // RL: big step forward, lifted
                92.0f,  45.0f    // RR: slight push
            }},
            .durationMs = 200,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 3: Left rear plants, right side resets
        {
            .pose = { .angles = {
                95.0f,  45.0f,   // FL: pushing back
                92.0f,  30.0f,   // FR: small lift
                100.0f, 45.0f,   // RL: planted
                88.0f,  30.0f    // RR: small lift
            }},
            .durationMs = 200,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 4: Return to neutral
        {
            .pose = { .angles = {
                90.0f,  45.0f,   // FL: neutral
                90.0f,  45.0f,   // FR: neutral
                90.0f,  45.0f,   // RL: neutral
                90.0f,  45.0f    // RR: neutral
            }},
            .durationMs = 200,
            .interpolation = InterpolationType::COSINE
        }
    };

    static const Motion rightMotion = {
        .name         = "right",
        .keyframes    = rightKeyframes,
        .numKeyframes = sizeof(rightKeyframes) / sizeof(rightKeyframes[0]),
        .loop         = true
    };

} // namespace Skills

#endif // SKILL_RIGHT_H
