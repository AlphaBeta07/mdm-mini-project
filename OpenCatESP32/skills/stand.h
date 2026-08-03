/**
 * @file skills/stand.h
 * @brief Standing pose skill definition.
 *
 * All legs straight down, body raised to standing height.
 * This is the default "ready" posture.
 *
 * Servo mapping:
 *   CH0=FL Hip, CH1=FL Knee, CH2=FR Hip, CH3=FR Knee,
 *   CH4=RL Hip, CH5=RL Knee, CH6=RR Hip, CH7=RR Knee
 *
 * Angles are degrees (0–180), center at 90°.
 * Hip 90° = neutral, Knee 90° = straight leg.
 *
 * @note These are initial estimates — calibrate with your specific robot.
 * @author OpenCatESP32 Project
 */

#ifndef SKILL_STAND_H
#define SKILL_STAND_H

#include "../MotionEngine.h"

namespace Skills {

    static const Keyframe standKeyframes[] = {
        // Keyframe 0: Standing pose
        {
            .pose = { .angles = {
                90.0f,   // CH0: FL Hip   — neutral
                45.0f,   // CH1: FL Knee  — extended down
                90.0f,   // CH2: FR Hip   — neutral
                45.0f,   // CH3: FR Knee  — extended down
                90.0f,   // CH4: RL Hip   — neutral
                45.0f,   // CH5: RL Knee  — extended down
                90.0f,   // CH6: RR Hip   — neutral
                45.0f    // CH7: RR Knee  — extended down
            }},
            .durationMs    = 500,
            .interpolation = InterpolationType::EASE_IN_OUT
        }
    };

    static const Motion standMotion = {
        .name         = "stand",
        .keyframes    = standKeyframes,
        .numKeyframes = sizeof(standKeyframes) / sizeof(standKeyframes[0]),
        .loop         = false
    };

} // namespace Skills

#endif // SKILL_STAND_H
