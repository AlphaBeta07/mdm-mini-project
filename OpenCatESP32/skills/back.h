/**
 * @file skills/back.h
 * @brief Backward walking gait skill definition (keyframe-based fallback).
 *
 * Reverse of the forward walk — legs sweep backward during swing.
 *
 * @note These are initial estimates — calibrate with your specific robot.
 * @author OpenCatESP32 Project
 */

#ifndef SKILL_BACK_H
#define SKILL_BACK_H

#include "../MotionEngine.h"

namespace Skills {

    static const Keyframe backKeyframes[] = {
        // Phase 1: FL leg swing backward
        {
            .pose = { .angles = {
                70.0f,  30.0f,   // FL: hip backward, knee lifted
                90.0f,  45.0f,   // FR: stance
                95.0f,  45.0f,   // RL: pushing forward
                90.0f,  45.0f    // RR: stance
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 2: FL plants
        {
            .pose = { .angles = {
                75.0f,  45.0f,   // FL: planted back
                90.0f,  45.0f,   // FR: stance
                90.0f,  45.0f,   // RL: neutral
                95.0f,  45.0f    // RR: pushing forward
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 3: RR leg swing backward
        {
            .pose = { .angles = {
                85.0f,  45.0f,   // FL: stance
                90.0f,  45.0f,   // FR: stance
                90.0f,  45.0f,   // RL: stance
                70.0f,  30.0f    // RR: hip backward, knee lifted
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 4: RR plants
        {
            .pose = { .angles = {
                90.0f,  45.0f,   // FL: neutral
                95.0f,  45.0f,   // FR: pushing forward
                90.0f,  45.0f,   // RL: stance
                75.0f,  45.0f    // RR: planted back
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 5: FR leg swing backward
        {
            .pose = { .angles = {
                90.0f,  45.0f,   // FL: stance
                70.0f,  30.0f,   // FR: hip backward, knee lifted
                95.0f,  45.0f,   // RL: pushing forward
                85.0f,  45.0f    // RR: stance
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 6: FR plants
        {
            .pose = { .angles = {
                90.0f,  45.0f,   // FL: stance
                75.0f,  45.0f,   // FR: planted back
                90.0f,  45.0f,   // RL: neutral
                90.0f,  45.0f    // RR: stance
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 7: RL leg swing backward
        {
            .pose = { .angles = {
                90.0f,  45.0f,   // FL: stance
                85.0f,  45.0f,   // FR: stance
                70.0f,  30.0f,   // RL: hip backward, knee lifted
                90.0f,  45.0f    // RR: stance
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 8: RL plants, return to start
        {
            .pose = { .angles = {
                90.0f,  45.0f,   // FL: neutral
                90.0f,  45.0f,   // FR: neutral
                75.0f,  45.0f,   // RL: planted back
                90.0f,  45.0f    // RR: neutral
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        }
    };

    static const Motion backMotion = {
        .name         = "back",
        .keyframes    = backKeyframes,
        .numKeyframes = sizeof(backKeyframes) / sizeof(backKeyframes[0]),
        .loop         = true
    };

} // namespace Skills

#endif // SKILL_BACK_H
