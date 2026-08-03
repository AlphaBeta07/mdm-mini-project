/**
 * @file skills/shake.h
 * @brief Shake (handshake) gesture skill definition.
 *
 * Extends front-right leg forward for a handshake, then shakes
 * it up and down.
 *
 * @note These are initial estimates — calibrate with your specific robot.
 * @author OpenCatESP32 Project
 */

#ifndef SKILL_SHAKE_H
#define SKILL_SHAKE_H

#include "../MotionEngine.h"

namespace Skills {

    static const Keyframe shakeKeyframes[] = {
        // Keyframe 1: Lean left, extend FR forward
        {
            .pose = { .angles = {
                85.0f,  40.0f,   // FL: lean for balance
                110.0f, 30.0f,   // FR: extend forward, lifted
                85.0f,  40.0f,   // RL: lean for balance
                90.0f,  45.0f    // RR: stable
            }},
            .durationMs = 400,
            .interpolation = InterpolationType::EASE_IN_OUT
        },
        // Keyframe 2: Shake down
        {
            .pose = { .angles = {
                85.0f,  40.0f,   // FL: balanced
                110.0f, 50.0f,   // FR: paw down
                85.0f,  40.0f,   // RL: balanced
                90.0f,  45.0f    // RR: stable
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Keyframe 3: Shake up
        {
            .pose = { .angles = {
                85.0f,  40.0f,   // FL: balanced
                110.0f, 25.0f,   // FR: paw up
                85.0f,  40.0f,   // RL: balanced
                90.0f,  45.0f    // RR: stable
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Keyframe 4: Shake down again
        {
            .pose = { .angles = {
                85.0f,  40.0f,   // FL: balanced
                110.0f, 50.0f,   // FR: paw down
                85.0f,  40.0f,   // RL: balanced
                90.0f,  45.0f    // RR: stable
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Keyframe 5: Shake up again
        {
            .pose = { .angles = {
                85.0f,  40.0f,   // FL: balanced
                110.0f, 25.0f,   // FR: paw up
                85.0f,  40.0f,   // RL: balanced
                90.0f,  45.0f    // RR: stable
            }},
            .durationMs = 150,
            .interpolation = InterpolationType::COSINE
        },
        // Keyframe 6: Return to standing
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

    static const Motion shakeMotion = {
        .name         = "shake",
        .keyframes    = shakeKeyframes,
        .numKeyframes = sizeof(shakeKeyframes) / sizeof(shakeKeyframes[0]),
        .loop         = false
    };

} // namespace Skills

#endif // SKILL_SHAKE_H
