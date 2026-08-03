/**
 * @file Config.h
 * @brief Master configuration file for the OpenCatESP32 firmware.
 *
 * All hardware settings, pin assignments, servo parameters, timing constants,
 * and tuning values live here. Modify this file to adapt the firmware to
 * different mechanical designs or hardware configurations.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

/**
 * @brief Master debug toggle — used by preprocessor macros in Utils.h.
 *
 * Set to 1 to enable debug output, 0 to strip all debug prints from the binary.
 * This must be a #define (not constexpr) because it is used in #if directives.
 */
#define OPENCAT_DEBUG 1

// ═════════════════════════════════════════════════════════════════════════════
//  HARDWARE — I2C & PCA9685
// ═════════════════════════════════════════════════════════════════════════════

namespace Config {
namespace I2C {
    /** @brief I2C SDA pin (ESP32 default). */
    static constexpr uint8_t SDA_PIN = 21;

    /** @brief I2C SCL pin (ESP32 default). */
    static constexpr uint8_t SCL_PIN = 22;

    /** @brief I2C bus clock frequency in Hz. */
    static constexpr uint32_t CLOCK_HZ = 400000;
}

namespace PCA9685 {
    /** @brief I2C address of the PCA9685 servo driver. */
    static constexpr uint8_t ADDRESS = 0x40;

    /** @brief PWM frequency for servo control (Hz). Standard for SG90/MG90S. */
    static constexpr float PWM_FREQ_HZ = 50.0f;

    /** @brief PCA9685 PWM resolution (12-bit = 4096 ticks per cycle). */
    static constexpr uint16_t PWM_RESOLUTION = 4096;

    /** @brief Period of one PWM cycle in microseconds at 50 Hz. */
    static constexpr uint32_t PWM_PERIOD_US = 20000;
}

// ═════════════════════════════════════════════════════════════════════════════
//  SERVO — Pulse Width & Angle Limits
// ═════════════════════════════════════════════════════════════════════════════

namespace Servo {
    /**
     * @brief Minimum pulse width in microseconds (corresponds to 0°).
     *
     * Typical SG90/MG90S lower bound. Adjust if servos buzz at the low end.
     */
    static constexpr uint16_t PULSE_MIN_US = 600;

    /**
     * @brief Maximum pulse width in microseconds (corresponds to 180°).
     *
     * Typical SG90/MG90S upper bound. Adjust if servos buzz at the high end.
     */
    static constexpr uint16_t PULSE_MAX_US = 2400;

    /** @brief Minimum allowed servo angle in degrees. */
    static constexpr float ANGLE_MIN_DEG = 0.0f;

    /** @brief Maximum allowed servo angle in degrees. */
    static constexpr float ANGLE_MAX_DEG = 180.0f;

    /** @brief Default center angle for servo calibration (degrees). */
    static constexpr float CENTER_ANGLE_DEG = 90.0f;

    /**
     * @brief Per-servo safe operating limits (degrees).
     *
     * These prevent mechanical over-travel. Index matches ServoID (0–7).
     * Format: {min_angle, max_angle}
     *
     * Adjust these to match your robot's mechanical constraints.
     */
    static constexpr float SAFE_MIN[8] = {
        10.0f,  // CH0: Front Left Hip
        10.0f,  // CH1: Front Left Knee
        10.0f,  // CH2: Front Right Hip
        10.0f,  // CH3: Front Right Knee
        10.0f,  // CH4: Rear Left Hip
        10.0f,  // CH5: Rear Left Knee
        10.0f,  // CH6: Rear Right Hip
        10.0f   // CH7: Rear Right Knee
    };

    static constexpr float SAFE_MAX[8] = {
        170.0f, // CH0: Front Left Hip
        170.0f, // CH1: Front Left Knee
        170.0f, // CH2: Front Right Hip
        170.0f, // CH3: Front Right Knee
        170.0f, // CH4: Rear Left Hip
        170.0f, // CH5: Rear Left Knee
        170.0f, // CH6: Rear Right Hip
        170.0f  // CH7: Rear Right Knee
    };

    /**
     * @brief Default servo inversion flags.
     *
     * Set to true for servos that are mounted in the opposite direction.
     * Right-side servos are typically mirrored from left-side.
     * Index matches ServoID (0–7).
     */
    static constexpr bool INVERTED[8] = {
        false,  // CH0: Front Left Hip
        false,  // CH1: Front Left Knee
        true,   // CH2: Front Right Hip   — mirrored
        true,   // CH3: Front Right Knee  — mirrored
        false,  // CH4: Rear Left Hip
        false,  // CH5: Rear Left Knee
        true,   // CH6: Rear Right Hip    — mirrored
        true    // CH7: Rear Right Knee   — mirrored
    };
}

// ═════════════════════════════════════════════════════════════════════════════
//  LEG GEOMETRY — For Inverse Kinematics
// ═════════════════════════════════════════════════════════════════════════════

namespace Leg {
    /** @brief Upper leg segment length in mm (hip joint to knee joint). */
    static constexpr float UPPER_LENGTH_MM = 45.0f;

    /** @brief Lower leg segment length in mm (knee joint to foot). */
    static constexpr float LOWER_LENGTH_MM = 45.0f;

    /** @brief Hip offset from body center in X axis (mm). */
    static constexpr float HIP_OFFSET_X_MM = 50.0f;

    /** @brief Hip offset from body center in Y axis (mm). */
    static constexpr float HIP_OFFSET_Y_MM = 30.0f;
}

// ═════════════════════════════════════════════════════════════════════════════
//  GAIT — Walking Parameters
// ═════════════════════════════════════════════════════════════════════════════

namespace Gait {
    /** @brief Default stride length in degrees of hip swing. */
    static constexpr float STRIDE_DEG = 30.0f;

    /** @brief Default step height in degrees of knee lift. */
    static constexpr float STEP_HEIGHT_DEG = 25.0f;

    /** @brief Default walking cycle duration in milliseconds. */
    static constexpr uint32_t CYCLE_DURATION_MS = 1200;

    /** @brief Trot cycle duration (faster than walk). */
    static constexpr uint32_t TROT_CYCLE_MS = 600;

    /** @brief Turn stride reduction factor for inside legs (0.0–1.0). */
    static constexpr float TURN_INNER_FACTOR = 0.3f;

    /** @brief Body sway amplitude in degrees (lateral weight shift). */
    static constexpr float BODY_SWAY_DEG = 3.0f;

    /** @brief Gait ramp-up duration in ms (smooth start/stop). */
    static constexpr uint32_t RAMP_DURATION_MS = 400;

    /** @brief Default body standing height offset in degrees. */
    static constexpr float BODY_HEIGHT_DEG = 45.0f;
}

// ═════════════════════════════════════════════════════════════════════════════
//  BLUETOOTH
// ═════════════════════════════════════════════════════════════════════════════

namespace Bluetooth {
    /** @brief Bluetooth Classic device name (visible during pairing). */
    static constexpr const char* DEVICE_NAME = "OpenCat ESP32";

    /** @brief Bluetooth task polling interval in milliseconds. */
    static constexpr uint32_t POLL_INTERVAL_MS = 10;
}

// ═════════════════════════════════════════════════════════════════════════════
//  MOTION ENGINE
// ═════════════════════════════════════════════════════════════════════════════

namespace Motion {
    /** @brief Default transition duration for pose changes (ms). */
    static constexpr uint32_t DEFAULT_TRANSITION_MS = 500;

    /** @brief Maximum number of keyframes in a single motion. */
    static constexpr uint8_t MAX_KEYFRAMES = 32;

    /** @brief Maximum motion queue depth. */
    static constexpr uint8_t MAX_QUEUE_DEPTH = 8;

    /** @brief Default interpolation type for transitions. */
    static constexpr uint8_t DEFAULT_INTERPOLATION = 5; // EASE_IN_OUT
}

// ═════════════════════════════════════════════════════════════════════════════
//  SAFETY
// ═════════════════════════════════════════════════════════════════════════════

namespace Safety {
    /** @brief Servo inactivity timeout in milliseconds. Servos disable after this. */
    static constexpr uint32_t SERVO_TIMEOUT_MS = 30000;

    /** @brief Maximum servo speed in degrees per second (rate limiter). */
    static constexpr float MAX_SPEED_DEG_PER_SEC = 300.0f;

    /** @brief Enable brownout detection response. */
    static constexpr bool BROWNOUT_PROTECTION = true;

    /** @brief Enable watchdog timer. */
    static constexpr bool WATCHDOG_ENABLED = true;

    /** @brief Watchdog timeout in seconds. */
    static constexpr uint8_t WATCHDOG_TIMEOUT_SEC = 5;
}

// ═════════════════════════════════════════════════════════════════════════════
//  FREERTOS TASKS
// ═════════════════════════════════════════════════════════════════════════════

namespace Tasks {
    /** @brief Motion update task frequency in Hz. */
    static constexpr uint32_t MOTION_FREQ_HZ = 50;

    /** @brief Motion task stack size in bytes. */
    static constexpr uint32_t MOTION_STACK_SIZE = 4096;

    /** @brief Motion task priority (higher = more important). */
    static constexpr uint8_t MOTION_PRIORITY = 2;

    /** @brief Motion task pinned to Core 1. */
    static constexpr uint8_t MOTION_CORE = 1;

    /** @brief Bluetooth task stack size in bytes. */
    static constexpr uint32_t BT_STACK_SIZE = 4096;

    /** @brief Bluetooth task priority. */
    static constexpr uint8_t BT_PRIORITY = 1;

    /** @brief Bluetooth task pinned to Core 0. */
    static constexpr uint8_t BT_CORE = 0;
}

// ═════════════════════════════════════════════════════════════════════════════
//  CALIBRATION
// ═════════════════════════════════════════════════════════════════════════════

namespace Calibration {
    /** @brief NVS namespace for storing calibration data. */
    static constexpr const char* NVS_NAMESPACE = "ocal";

    /** @brief NVS key prefix for servo offsets (appended with channel number). */
    static constexpr const char* OFFSET_KEY_PREFIX = "off";

    /** @brief Default calibration offset in degrees. */
    static constexpr float DEFAULT_OFFSET = 0.0f;

    /** @brief Maximum allowed calibration offset magnitude (degrees). */
    static constexpr float MAX_OFFSET = 30.0f;

    /** @brief Calibration adjustment step size in degrees. */
    static constexpr float ADJUST_STEP = 1.0f;
}

// ═════════════════════════════════════════════════════════════════════════════
//  SERIAL DEBUGGING
// ═════════════════════════════════════════════════════════════════════════════

namespace Serial_ {
    /** @brief Serial baud rate for debug output. */
    static constexpr uint32_t BAUD_RATE = 115200;

    /** @brief Enable verbose debug output. */
    static constexpr bool DEBUG_ENABLED = true;
}

} // namespace Config

#endif // CONFIG_H
