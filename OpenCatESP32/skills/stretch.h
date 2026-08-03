/**
 * @file skills/stretch.h
 * @brief Stretch gesture skill definition.
 *
 * Cat-like stretch: front legs extend forward, rear stays up,
 * then returns to standing.
 *
 * @note These are initial estimates — calibrate with your specific robot.
 * @author OpenCatESP32 Project
 */

#ifndef SKILL_STRETCH_H
#define SKILL_STRETCH_H

#include "../MotionEngine.h"

namespace Skills {

    static const Keyframe stretchKeyframes[] = {
        // Keyframe 1: Begin stretch — front goes down
        {
            .pose = { .angles = {
                110.0f, 90.0f,   // FL: hip forward, knee extending
                110.0f, 90.0f,   // FR: hip forward, knee extending
                90.0f,  40.0f,   // RL: stay up
                90.0f,  40.0f    // RR: stay up
            }},
            .durationMs = 500,
            .interpolation = InterpolationType::EASE_IN
        },
        // Keyframe 2: Full stretch — front flat, rear high
        {
            .pose = { .angles = {
                120.0f, 130.0f,  // FL: fully extended forward, low
                120.0f, 130.0f,  // FR: fully extended forward, low
                85.0f,  35.0f,   // RL: high stance
                85.0f,  35.0f    // RR: high stance
            }},
            .durationMs = 600,
            .interpolation = InterpolationType::EASE_OUT
        },
        // Keyframe 3: Hold stretch
        {
            .pose = { .angles = {
                120.0f, 130.0f,  // FL: hold
                120.0f, 130.0f,  // FR: hold
                85.0f,  35.0f,   // RL: hold
                85.0f,  35.0f    // RR: hold
            }},
            .durationMs = 400,
            .interpolation = InterpolationType::LINEAR
        },
        // Keyframe 4: Return to standing
        {
            .pose = { .angles = {
                90.0f,  45.0f,   // FL: neutral
                90.0f,  45.0f,   // FR: neutral
                90.0f,  45.0f,   // RL: neutral
                90.0f,  45.0f    // RR: neutral
            }},
            .durationMs = 500,
            .interpolation = InterpolationType::EASE_IN_OUT
        }
    };

    static const Motion stretchMotion = {
        .name         = "stretch",
        .keyframes    = stretchKeyframes,
        .numKeyframes = sizeof(stretchKeyframes) / sizeof(stretchKeyframes[0]),
        .loop         = false
    };

} // namespace Skills

#endif // SKILL_STRETCH_H
