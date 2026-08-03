/**
 * @file MotionEngine.cpp
 * @brief Stub implementation for Batch 1 compilation.
 *
 * Full keyframe playback implementation in Batch 2.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#include "MotionEngine.h"
#include "ServoDriver.h"
#include "Interpolation.h"
#include "Utils.h"

MotionEngine::MotionEngine()
    : m_driver(nullptr)
    , m_activeMotion(nullptr)
    , m_currentKeyframe(0)
    , m_elapsedMs(0)
    , m_playing(false)
    , m_paused(false)
{
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        m_currentPose.angles[i] = Config::Servo::CENTER_ANGLE_DEG;
        m_startPose.angles[i]   = Config::Servo::CENTER_ANGLE_DEG;
    }
}

void MotionEngine::begin(ServoDriver& driver) {
    m_driver = &driver;
    // Sync current pose from driver
    driver.getCurrentAngles(m_currentPose.angles);
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        m_startPose.angles[i] = m_currentPose.angles[i];
    }
}

void MotionEngine::playMotion(const Motion& motion) {
    if (!m_driver || motion.numKeyframes == 0) return;

    m_activeMotion   = &motion;
    m_currentKeyframe = 0;
    m_elapsedMs      = 0;
    m_playing        = true;
    m_paused         = false;

    // Capture current position as the blend start
    m_driver->getCurrentAngles(m_startPose.angles);

    DEBUG_PRINTF("[MotionEngine] Playing: %s (%d keyframes, %s)\n",
                 motion.name, motion.numKeyframes,
                 motion.loop ? "loop" : "once");
}

void MotionEngine::stop() {
    m_playing      = false;
    m_paused       = false;
    m_activeMotion = nullptr;
}

void MotionEngine::pause() {
    if (m_playing) m_paused = true;
}

void MotionEngine::resume() {
    if (m_playing) m_paused = false;
}

void MotionEngine::update(uint32_t deltaMs) {
    if (!m_playing || m_paused || !m_activeMotion || !m_driver) return;

    m_elapsedMs += deltaMs;

    const Keyframe& target = m_activeMotion->keyframes[m_currentKeyframe];
    uint32_t duration = target.durationMs;
    if (duration == 0) duration = 1;  // Prevent division by zero

    float t = static_cast<float>(m_elapsedMs) / static_cast<float>(duration);
    if (t > 1.0f) t = 1.0f;

    // Interpolate between start pose and target keyframe
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        m_currentPose.angles[i] = Interpolation::interpolate(
            m_startPose.angles[i],
            target.pose.angles[i],
            t,
            target.interpolation
        );
    }

    applyPose(m_currentPose);

    // Check if keyframe is complete
    if (m_elapsedMs >= duration) {
        advanceKeyframe();
    }
}

bool MotionEngine::isPlaying() const {
    return m_playing;
}

bool MotionEngine::isPaused() const {
    return m_paused;
}

void MotionEngine::getCurrentPose(Pose& outPose) const {
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        outPose.angles[i] = m_currentPose.angles[i];
    }
}

void MotionEngine::advanceKeyframe() {
    if (!m_activeMotion) return;

    // Save the current keyframe's target as the new start
    const Keyframe& completed = m_activeMotion->keyframes[m_currentKeyframe];
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        m_startPose.angles[i] = completed.pose.angles[i];
    }

    m_currentKeyframe++;
    m_elapsedMs = 0;

    // Check if we've completed all keyframes
    if (m_currentKeyframe >= m_activeMotion->numKeyframes) {
        if (m_activeMotion->loop) {
            m_currentKeyframe = 0;
            DEBUG_PRINTF("[MotionEngine] Looping: %s\n", m_activeMotion->name);
        } else {
            m_playing = false;
            DEBUG_PRINTF("[MotionEngine] Completed: %s\n", m_activeMotion->name);
        }
    }
}

void MotionEngine::applyPose(const Pose& pose) {
    if (!m_driver) return;
    m_driver->writeAngles(pose.angles);
}
