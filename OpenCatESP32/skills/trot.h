/**
 * @file skills/trot.h
 * @brief Trotting gait skill definition (keyframe-based fallback).
 *
 * Diagonal pair gait: FL+RR move together, then FR+RL.
 * Faster than walk, requires more balance.
 *
 * @note These are initial estimates — calibrate with your specific robot.
 * @author OpenCatESP32 Project
 */

#ifndef SKILL_TROT_H
#define SKILL_TROT_H

#include "../MotionEngine.h"

namespace Skills {

    static const Keyframe trotKeyframes[] = {
        // Phase 1: FL + RR swing forward
        {
            .pose = { .angles = {
                110.0f, 25.0f,   // FL: hip forward, knee high
                85.0f,  50.0f,   // FR: stance, pushing back
                85.0f,  50.0f,   // RL: stance, pushing back
                110.0f, 25.0f    // RR: hip forward, knee high
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 2: FL + RR plant
        {
            .pose = { .angles = {
                100.0f, 45.0f,   // FL: planted
                90.0f,  45.0f,   // FR: neutral
                90.0f,  45.0f,   // RL: neutral
                100.0f, 45.0f    // RR: planted
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 3: FR + RL swing forward
        {
            .pose = { .angles = {
                85.0f,  50.0f,   // FL: stance, pushing back
                110.0f, 25.0f,   // FR: hip forward, knee high
                110.0f, 25.0f,   // RL: hip forward, knee high
                85.0f,  50.0f    // RR: stance, pushing back
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 4: FR + RL plant
        {
            .pose = { .angles = {
                90.0f,  45.0f,   // FL: neutral
                100.0f, 45.0f,   // FR: planted
                100.0f, 45.0f,   // RL: planted
                90.0f,  45.0f    // RR: neutral
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        }
    };

    static const Motion trotMotion = {
        .name         = "trot",
        .keyframes    = trotKeyframes,
        .numKeyframes = sizeof(trotKeyframes) / sizeof(trotKeyframes[0]),
        .loop         = true
    };

} // namespace Skills

#endif // SKILL_TROT_H
