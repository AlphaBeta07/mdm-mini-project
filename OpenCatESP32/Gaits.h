/**
 * @file Gaits.h
 * @brief Procedural gait generation engine for continuous locomotion.
 *
 * Stub for Batch 1 compilation. Full implementation in Batch 3.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#ifndef GAITS_H
#define GAITS_H

#include <Arduino.h>
#include "Constants.h"
#include "Config.h"
#include "MotionEngine.h"

class ServoDriver;

/**
 * @class GaitEngine
 * @brief Phase-based gait generator for continuous walking, trotting, and turning.
 *
 * Unlike the MotionEngine (keyframe playback), the GaitEngine computes
 * servo angles procedurally each tick based on configurable gait parameters.
 */
class GaitEngine {
public:
    GaitEngine();

    void begin(ServoDriver& driver);

    /**
     * @brief Start a gait pattern.
     * @param type The gait pattern to run.
     */
    void startGait(GaitType type);

    /**
     * @brief Stop the current gait with smooth ramp-down.
     */
    void stopGait();

    /**
     * @brief Update gait computation and write servo angles.
     * @param deltaMs Time since last update in milliseconds.
     */
    void update(uint32_t deltaMs);

    /** @brief Check if a gait is currently active. */
    bool isActive() const;

    /** @brief Get the current gait type. */
    GaitType getCurrentGait() const;

    /**
     * @brief Get the current computed pose from the gait engine.
     * @param outPose Output pose.
     */
    void getCurrentPose(Pose& outPose) const;

private:
    ServoDriver* m_driver;
    GaitType     m_gaitType;
    bool         m_active;
    float        m_phase;            ///< Master phase [0.0, 1.0)
    float        m_amplitude;        ///< Current stride amplitude (for ramp)
    float        m_targetAmplitude;  ///< Target stride amplitude
    uint32_t     m_cycleDurationMs;  ///< Duration of one full gait cycle
    Pose         m_currentPose;      ///< Last computed pose

    /**
     * @brief Compute leg angles for a specific phase.
     * @param legIndex   Leg index (0–3).
     * @param legPhase   Phase for this leg [0.0, 1.0).
     * @param hipAngle   Output hip angle.
     * @param kneeAngle  Output knee angle.
     */
    void computeLegAngles(uint8_t legIndex, float legPhase,
                          float& hipAngle, float& kneeAngle) const;

    /** @brief Get the phase offset for a leg given the current gait type. */
    float getLegPhaseOffset(uint8_t legIndex) const;

    /** @brief Get the stride factor for a leg (for differential turning). */
    float getLegStrideFactor(uint8_t legIndex) const;
};

#endif // GAITS_H
