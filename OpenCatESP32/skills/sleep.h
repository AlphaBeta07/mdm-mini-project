/**
 * @file skills/sleep.h
 * @brief Sleep pose skill definition.
 *
 * Similar to lay but with legs tucked tighter. Lowest power pose.
 *
 * @note These are initial estimates — calibrate with your specific robot.
 * @author OpenCatESP32 Project
 */

#ifndef SKILL_SLEEP_H
#define SKILL_SLEEP_H

#include "../MotionEngine.h"

namespace Skills {

    static const Keyframe sleepKeyframes[] = {
        // Keyframe 0: Transition to low position
        {
            .pose = { .angles = {
                80.0f,   // CH0: FL Hip   — tucked inward
                110.0f,  // CH1: FL Knee  — folding
                100.0f,  // CH2: FR Hip   — tucked inward
                110.0f,  // CH3: FR Knee  — folding
                100.0f,  // CH4: RL Hip   — tucked inward
                110.0f,  // CH5: RL Knee  — folding
                80.0f,   // CH6: RR Hip   — tucked inward
                110.0f   // CH7: RR Knee  — folding
            }},
            .durationMs    = 400,
            .interpolation = InterpolationType::EASE_IN
        },
        // Keyframe 1: Full sleep — compact position
        {
            .pose = { .angles = {
                70.0f,   // CH0: FL Hip   — tucked tight
                140.0f,  // CH1: FL Knee  — fully tucked
                110.0f,  // CH2: FR Hip   — tucked tight
                140.0f,  // CH3: FR Knee  — fully tucked
                110.0f,  // CH4: RL Hip   — tucked tight
                140.0f,  // CH5: RL Knee  — fully tucked
                70.0f,   // CH6: RR Hip   — tucked tight
                140.0f   // CH7: RR Knee  — fully tucked
            }},
            .durationMs    = 600,
            .interpolation = InterpolationType::EASE_OUT
        }
    };

    static const Motion sleepMotion = {
        .name         = "sleep",
        .keyframes    = sleepKeyframes,
        .numKeyframes = sizeof(sleepKeyframes) / sizeof(sleepKeyframes[0]),
        .loop         = false
    };

} // namespace Skills

#endif // SKILL_SLEEP_H
