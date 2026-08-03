/**
 * @file skills/left.h
 * @brief Turn left gait skill definition (keyframe-based fallback).
 *
 * Differential turn: right legs stride full, left legs stride short.
 *
 * @note These are initial estimates — calibrate with your specific robot.
 * @author OpenCatESP32 Project
 */

#ifndef SKILL_LEFT_H
#define SKILL_LEFT_H

#include "../MotionEngine.h"

namespace Skills {

    static const Keyframe leftKeyframes[] = {
        // Phase 1: Right side steps forward, left side minimal
        {
            .pose = { .angles = {
                88.0f,  45.0f,   // FL: minimal movement
                110.0f, 30.0f,   // FR: big step forward, lifted
                92.0f,  45.0f,   // RL: minimal
                90.0f,  45.0f    // RR: stance
            }},
            .durationMs = 200,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 2: Plant right side
        {
            .pose = { .angles = {
                92.0f,  45.0f,   // FL: slight push
                100.0f, 45.0f,   // FR: planted
                88.0f,  45.0f,   // RL: slight push
                110.0f, 30.0f    // RR: big step forward, lifted
            }},
            .durationMs = 200,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 3: Right rear plants, left side resets
        {
            .pose = { .angles = {
                92.0f,  30.0f,   // FL: small lift
                95.0f,  45.0f,   // FR: pushing back
                88.0f,  30.0f,   // RL: small lift
                100.0f, 45.0f    // RR: planted
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

    static const Motion leftMotion = {
        .name         = "left",
        .keyframes    = leftKeyframes,
        .numKeyframes = sizeof(leftKeyframes) / sizeof(leftKeyframes[0]),
        .loop         = true
    };

} // namespace Skills

#endif // SKILL_LEFT_H
