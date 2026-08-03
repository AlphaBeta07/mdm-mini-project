/**
 * @file Robot.h
 * @brief Central robot state machine and coordinator.
 *
 * The Robot class owns all subsystems and manages the state machine
 * that determines which motion system is active (MotionPlayer for
 * static poses, GaitEngine for locomotion).
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#ifndef ROBOT_H
#define ROBOT_H

#include <Arduino.h>
#include "Constants.h"
#include "Config.h"
#include "ServoDriver.h"
#include "Calibration.h"
#include "MotionPlayer.h"
#include "Gaits.h"
#include "Bluetooth.h"
#include "Skills.h"

/**
 * @class Robot
 * @brief The central coordinator — owns all subsystems and runs the state machine.
 *
 * Architecture:
 *   - Static poses (stand/sit/lay/sleep/wave/shake/stretch):
 *     Handled by MotionPlayer (keyframe playback with transitions)
 *   - Locomotion (walk/trot/turn):
 *     Handled by GaitEngine (procedural phase-based gait)
 *   - Bluetooth commands change the state, never move servos directly
 *
 * Usage:
 * @code
 *   Robot robot;
 *   robot.begin();
 *   robot.setState(RobotState::STAND);
 *   // In loop:
 *   robot.update(deltaMs);
 * @endcode
 */
class Robot {
public:
    Robot();

    /**
     * @brief Initialize all subsystems.
     *
     * Initializes I2C, PCA9685, Bluetooth, calibration, and motion systems.
     * Should be called in setup().
     *
     * @return true if all critical systems initialized successfully.
     */
    bool begin();

    /**
     * @brief Main update loop — call every tick from the motion FreeRTOS task.
     *
     * Delegates to the active motion subsystem (MotionPlayer or GaitEngine).
     *
     * @param deltaMs Time since last update in milliseconds.
     */
    void update(uint32_t deltaMs);

    /**
     * @brief Bluetooth update — call from the Bluetooth FreeRTOS task.
     *
     * Polls Bluetooth for incoming commands.
     */
    void updateBluetooth();

    /**
     * @brief Set the robot to a new state.
     *
     * Handles transitions: stops the current motion system, starts the
     * appropriate one for the new state, with smooth blending.
     *
     * @param newState The target state.
     */
    void setState(RobotState newState);

    /**
     * @brief Get the current robot state.
     */
    RobotState getState() const;

    /**
     * @brief Emergency stop — immediately halt all motion and disable servos.
     */
    void emergencyStop();

    /**
     * @brief Enter servo calibration mode.
     *
     * Blocks until calibration is complete (interactive serial menu).
     */
    void enterCalibration();

    // ── Subsystem Access ────────────────────────────────────────────────────

    ServoDriver&         getServoDriver();
    Calibration&         getCalibration();
    MotionPlayer&        getMotionPlayer();
    GaitEngine&          getGaitEngine();
    BluetoothController& getBluetooth();
    SkillRegistry&       getSkills();

private:
    // Owned subsystems
    ServoDriver         m_servoDriver;
    Calibration         m_calibration;
    MotionPlayer        m_motionPlayer;
    GaitEngine          m_gaitEngine;
    BluetoothController m_bluetooth;
    SkillRegistry       m_skills;

    // State
    RobotState m_state;
    RobotState m_previousState;
    uint32_t   m_lastActivityMs;   ///< For servo timeout safety
    bool       m_initialized;

    /**
     * @brief Handle state transition logic.
     *
     * Stops the current motion, starts the appropriate system for
     * the new state, applies transitions.
     *
     * @param newState The state to transition to.
     */
    void handleStateTransition(RobotState newState);

    /**
     * @brief Start a static pose skill (stand/sit/lay/sleep/wave/shake/stretch).
     * @param skillName Name of the skill in the registry.
     */
    void startStaticPose(const char* skillName);

    /**
     * @brief Start a locomotion gait (walk/trot/turn).
     * @param gaitType The gait type to start.
     */
    void startLocomotion(GaitType gaitType);

    /**
     * @brief Check if a state uses the gait engine (locomotion).
     */
    static bool isLocomotionState(RobotState state);

    /**
     * @brief Check if a state uses a static pose (keyframe).
     */
    static bool isStaticPoseState(RobotState state);

    /**
     * @brief Static callback for Bluetooth commands.
     *
     * Wraps the member setState() for use as a function pointer.
     */
    static void onBluetoothCommand(RobotState newState);

    /** @brief Pointer to the singleton instance for the static callback. */
    static Robot* s_instance;

    /**
     * @brief Check and handle servo timeout safety.
     * @param deltaMs Time since last update.
     */
    void checkServoTimeout(uint32_t deltaMs);
};

#endif // ROBOT_H
