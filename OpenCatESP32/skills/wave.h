/**
 * @file skills/wave.h
 * @brief Wave gesture skill definition.
 *
 * Lifts the front-right leg and waves it side to side.
 * A friendly greeting gesture.
 *
 * @note These are initial estimates — calibrate with your specific robot.
 * @author OpenCatESP32 Project
 */

#ifndef SKILL_WAVE_H
#define SKILL_WAVE_H

#include "../MotionEngine.h"

namespace Skills {

    static const Keyframe waveKeyframes[] = {
        // Keyframe 1: Shift weight to left side, lift FR leg
        {
            .pose = { .angles = {
                85.0f,  40.0f,   // FL: lean left for balance
                90.0f,  20.0f,   // FR: lift leg high
                85.0f,  40.0f,   // RL: lean left for balance
                90.0f,  45.0f    // RR: stable
            }},
            .durationMs = 400,
            .interpolation = InterpolationType::EASE_IN_OUT
        },
        // Keyframe 2: Wave right
        {
            .pose = { .angles = {
                85.0f,  40.0f,   // FL: balanced
                110.0f, 20.0f,   // FR: wave right
                85.0f,  40.0f,   // RL: balanced
                90.0f,  45.0f    // RR: stable
            }},
            .durationMs = 300,
            .interpolation = InterpolationType::COSINE
        },
        // Keyframe 3: Wave left
        {
            .pose = { .angles = {
                85.0f,  40.0f,   // FL: balanced
                70.0f,  20.0f,   // FR: wave left
                85.0f,  40.0f,   // RL: balanced
                90.0f,  45.0f    // RR: stable
            }},
            .durationMs = 300,
            .interpolation = InterpolationType::COSINE
        },
        // Keyframe 4: Return to standing
        {
            .pose = { .angles = {
                90.0f,  45.0f,   // FL: neutral
                90.0f,  45.0f,   // FR: neutral
                90.0f,  45.0f,   // RL: neutral
                90.0f,  45.0f    // RR: neutral
            }},
            .durationMs = 400,
            .interpolation = InterpolationType::EASE_OUT
        }
    };

    static const Motion waveMotion = {
        .name         = "wave",
        .keyframes    = waveKeyframes,
        .numKeyframes = sizeof(waveKeyframes) / sizeof(waveKeyframes[0]),
        .loop         = false
    };

} // namespace Skills

#endif // SKILL_WAVE_H
