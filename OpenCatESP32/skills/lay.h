/**
 * @file skills/lay.h
 * @brief Laying down pose skill definition.
 *
 * All legs fold under the body, bringing it flat to the ground.
 *
 * @note These are initial estimates — calibrate with your specific robot.
 * @author OpenCatESP32 Project
 */

#ifndef SKILL_LAY_H
#define SKILL_LAY_H

#include "../MotionEngine.h"

namespace Skills {

    static const Keyframe layKeyframes[] = {
        // Keyframe 0: Crouch
        {
            .pose = { .angles = {
                90.0f,   // CH0: FL Hip
                80.0f,   // CH1: FL Knee  — starting to fold
                90.0f,   // CH2: FR Hip
                80.0f,   // CH3: FR Knee
                90.0f,   // CH4: RL Hip
                80.0f,   // CH5: RL Knee
                90.0f,   // CH6: RR Hip
                80.0f    // CH7: RR Knee
            }},
            .durationMs    = 300,
            .interpolation = InterpolationType::EASE_IN
        },
        // Keyframe 1: Fully laid down
        {
            .pose = { .angles = {
                90.0f,   // CH0: FL Hip
                130.0f,  // CH1: FL Knee  — fully folded
                90.0f,   // CH2: FR Hip
                130.0f,  // CH3: FR Knee  — fully folded
                90.0f,   // CH4: RL Hip
                130.0f,  // CH5: RL Knee  — fully folded
                90.0f,   // CH6: RR Hip
                130.0f   // CH7: RR Knee  — fully folded
            }},
            .durationMs    = 500,
            .interpolation = InterpolationType::EASE_OUT
        }
    };

    static const Motion layMotion = {
        .name         = "lay",
        .keyframes    = layKeyframes,
        .numKeyframes = sizeof(layKeyframes) / sizeof(layKeyframes[0]),
        .loop         = false
    };

} // namespace Skills

#endif // SKILL_LAY_H
