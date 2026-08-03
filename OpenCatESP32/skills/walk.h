/**
 * @file skills/walk.h
 * @brief Walking gait skill definition (keyframe-based fallback).
 *
 * This provides a keyframe-based walk animation. The primary walking
 * behavior uses the GaitEngine (procedural), but this keyframe version
 * serves as a fallback and reference.
 *
 * @note These are initial estimates — calibrate with your specific robot.
 * @author OpenCatESP32 Project
 */

#ifndef SKILL_WALK_H
#define SKILL_WALK_H

#include "../MotionEngine.h"

namespace Skills {

    static const Keyframe walkKeyframes[] = {
        // Phase 1: FL leg swing forward
        {
            .pose = { .angles = {
                110.0f, 30.0f,   // FL: hip forward, knee lifted
                90.0f,  45.0f,   // FR: stance
                85.0f,  45.0f,   // RL: pushing back slightly
                90.0f,  45.0f    // RR: stance
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 2: FL leg plants, body shifts
        {
            .pose = { .angles = {
                105.0f, 45.0f,   // FL: planted
                90.0f,  45.0f,   // FR: stance
                90.0f,  45.0f,   // RL: neutral
                85.0f,  45.0f    // RR: pushing back slightly
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 3: RR leg swing forward
        {
            .pose = { .angles = {
                95.0f,  45.0f,   // FL: stance, pushing back
                90.0f,  45.0f,   // FR: stance
                90.0f,  45.0f,   // RL: stance
                110.0f, 30.0f    // RR: hip forward, knee lifted
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 4: RR plants
        {
            .pose = { .angles = {
                90.0f,  45.0f,   // FL: stance
                85.0f,  45.0f,   // FR: pushing back slightly
                90.0f,  45.0f,   // RL: stance
                105.0f, 45.0f    // RR: planted
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 5: FR leg swing forward
        {
            .pose = { .angles = {
                90.0f,  45.0f,   // FL: stance
                110.0f, 30.0f,   // FR: hip forward, knee lifted
                85.0f,  45.0f,   // RL: pushing back slightly
                95.0f,  45.0f    // RR: stance, pushing back
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 6: FR plants
        {
            .pose = { .angles = {
                90.0f,  45.0f,   // FL: stance
                105.0f, 45.0f,   // FR: planted
                90.0f,  45.0f,   // RL: neutral
                90.0f,  45.0f    // RR: stance
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Phase 7: RL leg swing forward
        {
            .pose = { .angles = {
                90.0f,  45.0f,   // FL: stance
                95.0f,  45.0f,   // FR: pushing back
                110.0f, 30.0f,   // RL: hip forward, knee lifted
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
                105.0f, 45.0f,   // RL: planted
                90.0f,  45.0f    // RR: neutral
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        }
    };

    static const Motion walkMotion = {
        .name         = "walk",
        .keyframes    = walkKeyframes,
        .numKeyframes = sizeof(walkKeyframes) / sizeof(walkKeyframes[0]),
        .loop         = true
    };

} // namespace Skills

#endif // SKILL_WALK_H
