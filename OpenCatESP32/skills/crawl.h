/**
 * @file skills/crawl.h
 * @brief Slow crawl gait skill definition.
 *
 * One leg moves at a time for maximum stability.
 * Same sequence as walk but with longer timing for caution.
 *
 * @note These are initial estimates — calibrate with your specific robot.
 * @author OpenCatESP32 Project
 */

#ifndef SKILL_CRAWL_H
#define SKILL_CRAWL_H

#include "../MotionEngine.h"

namespace Skills {

    static const Keyframe crawlKeyframes[] = {
        // Phase 1: FL lifts and moves forward slowly
        {
            .pose = { .angles = {
                105.0f, 30.0f,   // FL: forward, lifted
                90.0f,  45.0f,   // FR: stable
                87.0f,  45.0f,   // RL: slight push
                90.0f,  45.0f    // RR: stable
            }},
            .durationMs = 250,
            .interpolation = InterpolationType::EASE_IN_OUT
        },
        // Phase 2: FL plants
        {
            .pose = { .angles = {
                100.0f, 45.0f,   // FL: planted
                90.0f,  45.0f,   // FR: stable
                90.0f,  45.0f,   // RL: neutral
                87.0f,  45.0f    // RR: slight push
            }},
            .durationMs = 200,
            .interpolation = InterpolationType::EASE_IN_OUT
        },
        // Phase 3: RR lifts and moves forward
        {
            .pose = { .angles = {
                97.0f,  45.0f,   // FL: stance
                90.0f,  45.0f,   // FR: stable
                90.0f,  45.0f,   // RL: stable
                105.0f, 30.0f    // RR: forward, lifted
            }},
            .durationMs = 250,
            .interpolation = InterpolationType::EASE_IN_OUT
        },
        // Phase 4: RR plants
        {
            .pose = { .angles = {
                93.0f,  45.0f,   // FL: pushing back
                87.0f,  45.0f,   // FR: slight push
                90.0f,  45.0f,   // RL: stable
                100.0f, 45.0f    // RR: planted
            }},
            .durationMs = 200,
            .interpolation = InterpolationType::EASE_IN_OUT
        },
        // Phase 5: FR lifts and moves forward
        {
            .pose = { .angles = {
                90.0f,  45.0f,   // FL: stable
                105.0f, 30.0f,   // FR: forward, lifted
                87.0f,  45.0f,   // RL: slight push
                97.0f,  45.0f    // RR: stance
            }},
            .durationMs = 250,
            .interpolation = InterpolationType::EASE_IN_OUT
        },
        // Phase 6: FR plants
        {
            .pose = { .angles = {
                90.0f,  45.0f,   // FL: stable
                100.0f, 45.0f,   // FR: planted
                90.0f,  45.0f,   // RL: neutral
                93.0f,  45.0f    // RR: pushing back
            }},
            .durationMs = 200,
            .interpolation = InterpolationType::EASE_IN_OUT
        },
        // Phase 7: RL lifts and moves forward
        {
            .pose = { .angles = {
                90.0f,  45.0f,   // FL: stable
                97.0f,  45.0f,   // FR: stance
                105.0f, 30.0f,   // RL: forward, lifted
                90.0f,  45.0f    // RR: stable
            }},
            .durationMs = 250,
            .interpolation = InterpolationType::EASE_IN_OUT
        },
        // Phase 8: RL plants, return to neutral
        {
            .pose = { .angles = {
                90.0f,  45.0f,   // FL: neutral
                93.0f,  45.0f,   // FR: pushing back
                100.0f, 45.0f,   // RL: planted
                90.0f,  45.0f    // RR: neutral
            }},
            .durationMs = 200,
            .interpolation = InterpolationType::EASE_IN_OUT
        }
    };

    static const Motion crawlMotion = {
        .name         = "crawl",
        .keyframes    = crawlKeyframes,
        .numKeyframes = sizeof(crawlKeyframes) / sizeof(crawlKeyframes[0]),
        .loop         = true
    };

} // namespace Skills

#endif // SKILL_CRAWL_H
