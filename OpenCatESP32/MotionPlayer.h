/**
 * @file MotionPlayer.h
 * @brief High-level motion orchestrator with transitions and queuing.
 *
 * Sits above MotionEngine and provides:
 *   - Named skill playback via SkillRegistry
 *   - Smooth transitions between poses
 *   - Motion queue for sequential playback
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#ifndef MOTIONPLAYER_H
#define MOTIONPLAYER_H

#include <Arduino.h>
#include "Constants.h"
#include "Config.h"
#include "MotionEngine.h"

class ServoDriver;

/**
 * @class MotionPlayer
 * @brief Orchestrates motion playback, transitions, and queuing.
 */
class MotionPlayer {
public:
    MotionPlayer();

    void begin(ServoDriver& driver);
    void update(uint32_t deltaMs);

    /**
     * @brief Play a motion definition.
     * @param motion The motion to play.
     */
    void playMotion(const Motion& motion);

    /**
     * @brief Smoothly transition to a target pose.
     * @param targetPose  The pose to transition to.
     * @param durationMs  Transition duration in milliseconds.
     * @param interpType  Interpolation curve type.
     */
    void transitionTo(const Pose& targetPose, uint32_t durationMs,
                      InterpolationType interpType = InterpolationType::EASE_IN_OUT);

    void stop();
    void pause();
    void resume();

    bool isPlaying() const;
    bool isTransitioning() const;
    void getCurrentPose(Pose& outPose) const;

    /** @brief Get a reference to the underlying MotionEngine. */
    MotionEngine& getEngine();

private:
    MotionEngine m_engine;
    ServoDriver* m_driver;

    // Transition state
    bool     m_transitioning;
    Pose     m_transStartPose;
    Pose     m_transTargetPose;
    uint32_t m_transDurationMs;
    uint32_t m_transElapsedMs;
    InterpolationType m_transInterpType;

    // Pending motion (queued to play after transition)
    const Motion* m_pendingMotion;

    void updateTransition(uint32_t deltaMs);
};

#endif // MOTIONPLAYER_H
