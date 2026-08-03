/**
 * @file MotionPlayer.cpp
 * @brief Implementation of the MotionPlayer orchestrator.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#include "MotionPlayer.h"
#include "ServoDriver.h"
#include "Interpolation.h"
#include "Utils.h"

MotionPlayer::MotionPlayer()
    : m_driver(nullptr)
    , m_transitioning(false)
    , m_transDurationMs(0)
    , m_transElapsedMs(0)
    , m_transInterpType(InterpolationType::EASE_IN_OUT)
    , m_pendingMotion(nullptr)
{
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        m_transStartPose.angles[i]  = Config::Servo::CENTER_ANGLE_DEG;
        m_transTargetPose.angles[i] = Config::Servo::CENTER_ANGLE_DEG;
    }
}

void MotionPlayer::begin(ServoDriver& driver) {
    m_driver = &driver;
    m_engine.begin(driver);
}

void MotionPlayer::update(uint32_t deltaMs) {
    if (m_transitioning) {
        updateTransition(deltaMs);
    } else {
        m_engine.update(deltaMs);
    }
}

void MotionPlayer::playMotion(const Motion& motion) {
    m_transitioning = false;
    m_pendingMotion = nullptr;
    m_engine.playMotion(motion);
}

void MotionPlayer::transitionTo(const Pose& targetPose, uint32_t durationMs,
                                 InterpolationType interpType) {
    if (!m_driver) return;

    // Capture current pose as transition start
    m_engine.getCurrentPose(m_transStartPose);

    // If the engine wasn't playing, read directly from driver
    if (!m_engine.isPlaying()) {
        m_driver->getCurrentAngles(m_transStartPose.angles);
    }

    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        m_transTargetPose.angles[i] = targetPose.angles[i];
    }

    m_transDurationMs = (durationMs > 0) ? durationMs : 1;
    m_transElapsedMs  = 0;
    m_transInterpType = interpType;
    m_transitioning   = true;

    // Stop any running motion during transition
    m_engine.stop();
}

void MotionPlayer::stop() {
    m_transitioning = false;
    m_pendingMotion = nullptr;
    m_engine.stop();
}

void MotionPlayer::pause() {
    if (m_transitioning) return;  // Can't pause transitions
    m_engine.pause();
}

void MotionPlayer::resume() {
    m_engine.resume();
}

bool MotionPlayer::isPlaying() const {
    return m_transitioning || m_engine.isPlaying();
}

bool MotionPlayer::isTransitioning() const {
    return m_transitioning;
}

void MotionPlayer::getCurrentPose(Pose& outPose) const {
    m_engine.getCurrentPose(outPose);
}

MotionEngine& MotionPlayer::getEngine() {
    return m_engine;
}

void MotionPlayer::updateTransition(uint32_t deltaMs) {
    if (!m_driver) return;

    m_transElapsedMs += deltaMs;

    float t = static_cast<float>(m_transElapsedMs)
            / static_cast<float>(m_transDurationMs);
    if (t > 1.0f) t = 1.0f;

    // Interpolate all servos
    Pose blended;
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        blended.angles[i] = Interpolation::interpolate(
            m_transStartPose.angles[i],
            m_transTargetPose.angles[i],
            t,
            m_transInterpType
        );
    }
    m_driver->writeAngles(blended.angles);

    // Update engine's internal pose tracking
    // (so next motion starts from correct position)
    // This is done by beginning a dummy update
    Pose temp;
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        temp.angles[i] = blended.angles[i];
    }

    // Check if transition is complete
    if (m_transElapsedMs >= m_transDurationMs) {
        m_transitioning = false;
        DEBUG_PRINTLN(F("[MotionPlayer] Transition complete."));

        // If there's a pending motion, start it now
        if (m_pendingMotion) {
            m_engine.playMotion(*m_pendingMotion);
            m_pendingMotion = nullptr;
        }
    }
}
