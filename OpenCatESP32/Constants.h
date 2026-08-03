/**
 * @file Constants.h
 * @brief Central definitions for enums, named constants, and type aliases
 *        used throughout the OpenCatESP32 firmware.
 *
 * This file eliminates magic numbers and provides a single source of truth
 * for all symbolic constants in the project.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

// ─────────────────────────────────────────────────────────────────────────────
// Servo & Leg Topology
// ─────────────────────────────────────────────────────────────────────────────

/** @brief Total number of servos on the robot. */
static constexpr uint8_t NUM_SERVOS = 8;

/** @brief Total number of legs on the robot. */
static constexpr uint8_t NUM_LEGS = 4;

/** @brief Number of joints (servos) per leg. */
static constexpr uint8_t JOINTS_PER_LEG = 2;

// ─────────────────────────────────────────────────────────────────────────────
// Enumerations
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Identifies each servo by its PCA9685 channel and anatomical role.
 *
 * The mapping is fixed and must match the physical wiring:
 *   CH0 = Front Left Hip,   CH1 = Front Left Knee,
 *   CH2 = Front Right Hip,  CH3 = Front Right Knee,
 *   CH4 = Rear Left Hip,    CH5 = Rear Left Knee,
 *   CH6 = Rear Right Hip,   CH7 = Rear Right Knee.
 */
enum class ServoID : uint8_t {
    FRONT_LEFT_HIP   = 0,
    FRONT_LEFT_KNEE  = 1,
    FRONT_RIGHT_HIP  = 2,
    FRONT_RIGHT_KNEE = 3,
    REAR_LEFT_HIP    = 4,
    REAR_LEFT_KNEE   = 5,
    REAR_RIGHT_HIP   = 6,
    REAR_RIGHT_KNEE  = 7
};

/**
 * @brief Identifies each leg of the quadruped.
 */
enum class LegID : uint8_t {
    FRONT_LEFT  = 0,
    FRONT_RIGHT = 1,
    REAR_LEFT   = 2,
    REAR_RIGHT  = 3
};

/**
 * @brief Joint type within a leg.
 */
enum class JointType : uint8_t {
    HIP  = 0,
    KNEE = 1
};

/**
 * @brief Robot state machine states.
 *
 * These states determine which motion subsystem is active.
 * Static poses use keyframe playback; locomotion uses the gait engine.
 */
enum class RobotState : uint8_t {
    IDLE          = 0,   ///< No active motion, servos hold last position
    STAND         = 1,   ///< Standing pose (static)
    SIT           = 2,   ///< Sitting pose (static)
    LAY           = 3,   ///< Laying down pose (static)
    SLEEP         = 4,   ///< Sleep pose — compact, low power (static)
    WALK_FORWARD  = 5,   ///< Forward walking gait (continuous)
    WALK_BACKWARD = 6,   ///< Backward walking gait (continuous)
    TURN_LEFT     = 7,   ///< Turning left gait (continuous)
    TURN_RIGHT    = 8,   ///< Turning right gait (continuous)
    TROT          = 9,   ///< Trotting gait — diagonal pairs (continuous)
    CRAWL         = 10,  ///< Slow crawl gait — one leg at a time (continuous)
    WAVE          = 11,  ///< Wave gesture (single playback)
    SHAKE         = 12,  ///< Shake gesture (single playback)
    STRETCH       = 13   ///< Stretch gesture (single playback)
};

/**
 * @brief Interpolation curve types for smooth motion.
 */
enum class InterpolationType : uint8_t {
    LINEAR      = 0,  ///< Constant speed
    COSINE      = 1,  ///< Smooth S-curve (cosine-based)
    CUBIC       = 2,  ///< Cubic ease (Hermite)
    EASE_IN     = 3,  ///< Slow start, fast end (quadratic)
    EASE_OUT    = 4,  ///< Fast start, slow end (quadratic)
    EASE_IN_OUT = 5   ///< Slow start and end, fast middle (cubic)
};

/**
 * @brief Motion playback mode.
 */
enum class MotionPlayMode : uint8_t {
    ONCE    = 0,  ///< Play the motion once and stop
    LOOP    = 1,  ///< Loop the motion indefinitely
    QUEUE   = 2   ///< Play once, then advance to next in queue
};

/**
 * @brief Gait pattern type for the walking engine.
 */
enum class GaitType : uint8_t {
    WALK    = 0,  ///< Standard crawl — one leg at a time
    TROT    = 1,  ///< Diagonal pair gait
    TURN_L  = 2,  ///< Turn left — differential stride
    TURN_R  = 3,  ///< Turn right — differential stride
    BACK    = 4   ///< Walk backward
};

// ─────────────────────────────────────────────────────────────────────────────
// Bluetooth Command Characters
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Single-character Bluetooth commands matching the Android application.
 *
 * These must NOT be changed — they are dictated by the existing Android app.
 */
namespace BtCommand {
    static constexpr char STAND        = 'A';
    static constexpr char SIT          = 'M';
    static constexpr char LAY          = 'D';
    static constexpr char SLEEP        = 'C';
    static constexpr char WALK_FORWARD = 'F';
    static constexpr char WALK_BACK    = 'B';
    static constexpr char TURN_LEFT    = 'L';
    static constexpr char TURN_RIGHT   = 'R';
    static constexpr char STOP         = 'S';
}

// ─────────────────────────────────────────────────────────────────────────────
// Utility: Servo ↔ Leg mapping helpers
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Get the ServoID for a given leg and joint.
 * @param leg   The leg identifier.
 * @param joint The joint type (HIP or KNEE).
 * @return The corresponding ServoID.
 */
inline ServoID servoForJoint(LegID leg, JointType joint) {
    return static_cast<ServoID>(
        static_cast<uint8_t>(leg) * JOINTS_PER_LEG +
        static_cast<uint8_t>(joint)
    );
}

/**
 * @brief Get the LegID that a servo belongs to.
 * @param servo The servo identifier.
 * @return The leg that owns this servo.
 */
inline LegID legForServo(ServoID servo) {
    return static_cast<LegID>(static_cast<uint8_t>(servo) / JOINTS_PER_LEG);
}

/**
 * @brief Get the JointType of a servo (HIP or KNEE).
 * @param servo The servo identifier.
 * @return The joint type.
 */
inline JointType jointTypeForServo(ServoID servo) {
    return static_cast<JointType>(static_cast<uint8_t>(servo) % JOINTS_PER_LEG);
}

#endif // CONSTANTS_H
