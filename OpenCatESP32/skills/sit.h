/**
 * @file skills/sit.h
 * @brief Sitting pose skill definition.
 *
 * Front legs stay straight, rear knees fold to bring haunches down.
 *
 * @note These are initial estimates — calibrate with your specific robot.
 * @author OpenCatESP32 Project
 */

#ifndef SKILL_SIT_H
#define SKILL_SIT_H

#include "../MotionEngine.h"

namespace Skills {

    static const Keyframe sitKeyframes[] = {
        // Keyframe 0: Intermediate — slight crouch
        {
            .pose = { .angles = {
                90.0f,   // CH0: FL Hip   — neutral
                50.0f,   // CH1: FL Knee  — slightly bent
                90.0f,   // CH2: FR Hip   — neutral
                50.0f,   // CH3: FR Knee  — slightly bent
                90.0f,   // CH4: RL Hip   — neutral
                70.0f,   // CH5: RL Knee  — starting to fold
                90.0f,   // CH6: RR Hip   — neutral
                70.0f    // CH7: RR Knee  — starting to fold
            }},
            .durationMs    = 300,
            .interpolation = InterpolationType::EASE_IN
        },
        // Keyframe 1: Full sit
        {
            .pose = { .angles = {
                90.0f,   // CH0: FL Hip   — neutral
                45.0f,   // CH1: FL Knee  — straight
                90.0f,   // CH2: FR Hip   — neutral
                45.0f,   // CH3: FR Knee  — straight
                80.0f,   // CH4: RL Hip   — tilted back slightly
                120.0f,  // CH5: RL Knee  — fully folded
                80.0f,   // CH6: RR Hip   — tilted back slightly
                120.0f   // CH7: RR Knee  — fully folded
            }},
            .durationMs    = 400,
            .interpolation = InterpolationType::EASE_OUT
        }
    };

    static const Motion sitMotion = {
        .name         = "sit",
        .keyframes    = sitKeyframes,
        .numKeyframes = sizeof(sitKeyframes) / sizeof(sitKeyframes[0]),
        .loop         = false
    };

} // namespace Skills

#endif // SKILL_SIT_H
