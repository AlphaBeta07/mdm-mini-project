/**
 * @file MotionEngine.h
 * @brief Keyframe-based motion playback engine.
 *
 * Stub for Batch 1 compilation. Full implementation in Batch 2.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#ifndef MOTIONENGINE_H
#define MOTIONENGINE_H

#include <Arduino.h>
#include "Constants.h"
#include "Config.h"

class ServoDriver;

/**
 * @brief A single servo pose — angles for all 8 servos.
 */
struct Pose {
    float angles[NUM_SERVOS];
};

/**
 * @brief A single keyframe in a motion sequence.
 */
struct Keyframe {
    Pose              pose;           ///< Target servo angles
    uint32_t          durationMs;     ///< Time to reach this keyframe from previous
    InterpolationType interpolation;  ///< Curve type for transition
};

/**
 * @brief A complete motion definition (sequence of keyframes).
 */
struct Motion {
    const char*    name;                             ///< Human-readable motion name
    const Keyframe* keyframes;                       ///< Array of keyframes
    uint8_t        numKeyframes;                     ///< Number of keyframes
    bool           loop;                             ///< Whether to loop
};

/**
 * @class MotionEngine
 * @brief Plays keyframe-based motions with interpolated transitions.
 */
class MotionEngine {
public:
    MotionEngine();

    void begin(ServoDriver& driver);
    void playMotion(const Motion& motion);
    void stop();
    void pause();
    void resume();
    void update(uint32_t deltaMs);

    bool isPlaying() const;
    bool isPaused() const;
    void getCurrentPose(Pose& outPose) const;

private:
    ServoDriver* m_driver;
    const Motion* m_activeMotion;
    Pose m_currentPose;
    Pose m_startPose;
    uint8_t m_currentKeyframe;
    uint32_t m_elapsedMs;
    bool m_playing;
    bool m_paused;

    void advanceKeyframe();
    void applyPose(const Pose& pose);
};

#endif // MOTIONENGINE_H
