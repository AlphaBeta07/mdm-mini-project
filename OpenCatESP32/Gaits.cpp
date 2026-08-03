/**
 * @file Gaits.cpp
 * @brief Stub implementation for Batch 1 compilation.
 *
 * Full procedural gait generation in Batch 3.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#include "Gaits.h"
#include "ServoDriver.h"
#include "Interpolation.h"
#include "Utils.h"
#include <math.h>

GaitEngine::GaitEngine()
    : m_driver(nullptr)
    , m_gaitType(GaitType::WALK)
    , m_active(false)
    , m_phase(0.0f)
    , m_amplitude(0.0f)
    , m_targetAmplitude(0.0f)
    , m_cycleDurationMs(Config::Gait::CYCLE_DURATION_MS)
{
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        m_currentPose.angles[i] = Config::Servo::CENTER_ANGLE_DEG;
    }
}

void GaitEngine::begin(ServoDriver& driver) {
    m_driver = &driver;
}

void GaitEngine::startGait(GaitType type) {
    m_gaitType        = type;
    m_active          = true;
    m_phase           = 0.0f;
    m_amplitude       = 0.0f;
    m_targetAmplitude = 1.0f;

    // Set cycle duration based on gait type
    if (type == GaitType::TROT) {
        m_cycleDurationMs = Config::Gait::TROT_CYCLE_MS;
    } else {
        m_cycleDurationMs = Config::Gait::CYCLE_DURATION_MS;
    }

    DEBUG_PRINTF("[GaitEngine] Started gait type %d\n", static_cast<int>(type));
}

void GaitEngine::stopGait() {
    m_targetAmplitude = 0.0f;
    // Will ramp down and set m_active = false when amplitude reaches 0
    DEBUG_PRINTLN(F("[GaitEngine] Stopping gait (ramping down)."));
}

void GaitEngine::update(uint32_t deltaMs) {
    if (!m_active || !m_driver) return;

    // Ramp amplitude toward target
    float rampRate = static_cast<float>(deltaMs) /
                     static_cast<float>(Config::Gait::RAMP_DURATION_MS);
    if (m_amplitude < m_targetAmplitude) {
        m_amplitude += rampRate;
        if (m_amplitude > m_targetAmplitude) m_amplitude = m_targetAmplitude;
    } else if (m_amplitude > m_targetAmplitude) {
        m_amplitude -= rampRate;
        if (m_amplitude < 0.0f) {
            m_amplitude = 0.0f;
            m_active = false;
            DEBUG_PRINTLN(F("[GaitEngine] Gait stopped."));
            return;
        }
    }

    // Advance master phase
    float phaseDelta = static_cast<float>(deltaMs) /
                       static_cast<float>(m_cycleDurationMs);
    m_phase += phaseDelta;
    if (m_phase >= 1.0f) m_phase -= 1.0f;

    // Compute angles for each leg
    for (uint8_t leg = 0; leg < NUM_LEGS; ++leg) {
        float legPhase = m_phase + getLegPhaseOffset(leg);
        if (legPhase >= 1.0f) legPhase -= 1.0f;

        float hipAngle, kneeAngle;
        computeLegAngles(leg, legPhase, hipAngle, kneeAngle);

        uint8_t hipCh  = leg * JOINTS_PER_LEG;
        uint8_t kneeCh = leg * JOINTS_PER_LEG + 1;

        m_currentPose.angles[hipCh]  = hipAngle;
        m_currentPose.angles[kneeCh] = kneeAngle;
    }

    m_driver->writeAngles(m_currentPose.angles);
}

bool GaitEngine::isActive() const {
    return m_active;
}

GaitType GaitEngine::getCurrentGait() const {
    return m_gaitType;
}

void GaitEngine::getCurrentPose(Pose& outPose) const {
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        outPose.angles[i] = m_currentPose.angles[i];
    }
}

void GaitEngine::computeLegAngles(uint8_t legIndex, float legPhase,
                                   float& hipAngle, float& kneeAngle) const {
    float strideFactor = getLegStrideFactor(legIndex);
    float stride       = Config::Gait::STRIDE_DEG * m_amplitude * strideFactor;
    float stepHeight   = Config::Gait::STEP_HEIGHT_DEG * m_amplitude;
    float center       = Config::Servo::CENTER_ANGLE_DEG;

    // Determine if we're in swing phase (0.0–0.25) or stance phase (0.25–1.0)
    // Swing is faster than stance to maintain stability
    float swingRatio = 0.25f;  // 25% of cycle is swing, 75% is stance

    if (legPhase < swingRatio) {
        // === SWING PHASE (leg in air, moving forward) ===
        float swingT = legPhase / swingRatio;

        // Hip: sweep from back to front
        hipAngle = center - stride * 0.5f +
                   stride * Interpolation::cosine(swingT);

        // Knee: lift with a parabolic arc
        float lift = 4.0f * swingT * (1.0f - swingT);  // Parabola peak at t=0.5
        kneeAngle = center + Config::Gait::BODY_HEIGHT_DEG - stepHeight * lift;

    } else {
        // === STANCE PHASE (leg on ground, pushing backward) ===
        float stanceT = (legPhase - swingRatio) / (1.0f - swingRatio);

        // Hip: sweep from front to back (opposite of swing)
        hipAngle = center + stride * 0.5f -
                   stride * stanceT;

        // Knee: keep leg straight on ground
        kneeAngle = center + Config::Gait::BODY_HEIGHT_DEG;
    }

    // Apply body sway (lateral weight shift during stance)
    // Sway is a sine wave based on master phase
    float sway = Config::Gait::BODY_SWAY_DEG * m_amplitude *
                 sinf(m_phase * 2.0f * PI);

    // Left legs sway one way, right legs the other
    if (legIndex == 0 || legIndex == 2) {
        // Left legs (FL, RL)
        hipAngle += sway;
    } else {
        // Right legs (FR, RR)
        hipAngle -= sway;
    }
}

float GaitEngine::getLegPhaseOffset(uint8_t legIndex) const {
    // Phase offsets determine which legs move together.
    // Index: 0=FL, 1=FR, 2=RL, 3=RR
    switch (m_gaitType) {
        case GaitType::WALK:
        case GaitType::BACK:
            // Crawl gait: legs move one at a time
            // Order: FL → RR → FR → RL
            {
                static const float walkOffsets[4] = {0.0f, 0.5f, 0.75f, 0.25f};
                return walkOffsets[legIndex];
            }

        case GaitType::TROT:
            // Trot: diagonal pairs move together
            // FL+RR together, FR+RL together
            {
                static const float trotOffsets[4] = {0.0f, 0.5f, 0.5f, 0.0f};
                return trotOffsets[legIndex];
            }

        case GaitType::TURN_L:
        case GaitType::TURN_R:
            // Same as walk pattern for turning
            {
                static const float turnOffsets[4] = {0.0f, 0.5f, 0.75f, 0.25f};
                return turnOffsets[legIndex];
            }

        default:
            return 0.0f;
    }
}

float GaitEngine::getLegStrideFactor(uint8_t legIndex) const {
    // For turning, inner legs have reduced stride
    switch (m_gaitType) {
        case GaitType::TURN_L:
            // Left legs have reduced stride
            if (legIndex == 0 || legIndex == 2) {
                return Config::Gait::TURN_INNER_FACTOR;
            }
            return 1.0f;

        case GaitType::TURN_R:
            // Right legs have reduced stride
            if (legIndex == 1 || legIndex == 3) {
                return Config::Gait::TURN_INNER_FACTOR;
            }
            return 1.0f;

        case GaitType::BACK:
            // Backward: negate stride direction
            return -1.0f;

        default:
            return 1.0f;
    }
}
