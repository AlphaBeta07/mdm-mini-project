/**
 * @file ServoDriver.h
 * @brief Hardware abstraction layer for servo control via PCA9685.
 *
 * Wraps the Adafruit PWM Servo Driver library and provides:
 *   - Per-servo angle writing with microsecond conversion
 *   - Safety clamping to configurable limits
 *   - Calibration offset support
 *   - Servo direction inversion for mirrored legs
 *   - Individual servo enable/disable (cut PWM to save power)
 *   - Speed limiting for servo protection
 *
 * This class is the ONLY module that communicates with the PCA9685 hardware.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#ifndef SERVODRIVER_H
#define SERVODRIVER_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "Constants.h"
#include "Config.h"

/**
 * @brief Per-servo runtime state.
 *
 * Tracks the current position, calibration offset, direction inversion,
 * and safety limits for each individual servo.
 */
struct ServoState {
    float currentAngle;   ///< Current angle in degrees (after offset/inversion)
    float targetAngle;    ///< Target angle in degrees (before offset/inversion)
    float offset;         ///< Calibration offset in degrees (added to target)
    float minAngle;       ///< Minimum safe angle in degrees
    float maxAngle;       ///< Maximum safe angle in degrees
    bool  inverted;       ///< If true, angle = 180 - angle before writing
    bool  enabled;        ///< If false, PWM output is cut for this channel
};

/**
 * @class ServoDriver
 * @brief Controls up to 16 servos via a PCA9685 PWM driver over I2C.
 *
 * Usage:
 * @code
 *   ServoDriver driver;
 *   driver.begin();
 *   driver.writeAngle(ServoID::FRONT_LEFT_HIP, 90.0f);
 * @endcode
 */
class ServoDriver {
public:
    /**
     * @brief Construct a ServoDriver with default PCA9685 address.
     */
    ServoDriver();

    /**
     * @brief Initialize the PCA9685 and configure all servo channels.
     *
     * Must be called after Wire.begin(). Sets PWM frequency and
     * initializes all servo states from Config.h defaults.
     *
     * @return true if PCA9685 was detected on the I2C bus.
     */
    bool begin();

    // ── Angle Control ───────────────────────────────────────────────────────

    /**
     * @brief Write a target angle to a servo.
     *
     * Applies calibration offset, inversion, and safety clamping before
     * converting to microseconds and writing to the PCA9685.
     *
     * @param id    The servo to move.
     * @param angle Target angle in degrees [0, 180].
     */
    void writeAngle(ServoID id, float angle);

    /**
     * @brief Write a target angle to a servo by channel number.
     * @param channel PCA9685 channel (0–15).
     * @param angle   Target angle in degrees.
     */
    void writeAngle(uint8_t channel, float angle);

    /**
     * @brief Write a raw microsecond pulse width to a servo.
     *
     * Bypasses angle conversion but still respects enable/disable state.
     *
     * @param id           The servo to control.
     * @param microseconds Pulse width in microseconds.
     */
    void writeMicroseconds(ServoID id, uint16_t microseconds);

    /**
     * @brief Write the same angle to all servos simultaneously.
     * @param angle Target angle in degrees.
     */
    void writeAllAngles(float angle);

    /**
     * @brief Write an array of 8 angles to all servos.
     *
     * Array index matches ServoID (0–7).
     *
     * @param angles Array of 8 target angles in degrees.
     */
    void writeAngles(const float angles[NUM_SERVOS]);

    // ── Servo State ─────────────────────────────────────────────────────────

    /**
     * @brief Get the current angle of a servo (after offset/inversion).
     * @param id The servo to query.
     * @return Current angle in degrees.
     */
    float getCurrentAngle(ServoID id) const;

    /**
     * @brief Get all current servo angles.
     * @param outAngles Output array of 8 angles.
     */
    void getCurrentAngles(float outAngles[NUM_SERVOS]) const;

    /**
     * @brief Get the target angle of a servo (before offset/inversion).
     * @param id The servo to query.
     * @return Target angle in degrees.
     */
    float getTargetAngle(ServoID id) const;

    // ── Enable / Disable ────────────────────────────────────────────────────

    /**
     * @brief Enable a servo (allow PWM output).
     * @param id The servo to enable.
     */
    void enableServo(ServoID id);

    /**
     * @brief Disable a servo (cut PWM output to save power/reduce buzz).
     * @param id The servo to disable.
     */
    void disableServo(ServoID id);

    /**
     * @brief Enable all servos.
     */
    void enableAll();

    /**
     * @brief Disable all servos.
     */
    void disableAll();

    /**
     * @brief Check if a servo is enabled.
     * @param id The servo to query.
     * @return true if the servo is enabled.
     */
    bool isEnabled(ServoID id) const;

    // ── Calibration ─────────────────────────────────────────────────────────

    /**
     * @brief Set the calibration offset for a servo.
     * @param id     The servo to calibrate.
     * @param offset Offset in degrees (clamped to ±MAX_OFFSET).
     */
    void setOffset(ServoID id, float offset);

    /**
     * @brief Get the current calibration offset for a servo.
     * @param id The servo to query.
     * @return Offset in degrees.
     */
    float getOffset(ServoID id) const;

    /**
     * @brief Set the inversion flag for a servo.
     * @param id       The servo to configure.
     * @param inverted true to invert the servo direction.
     */
    void setInverted(ServoID id, bool inverted);

    /**
     * @brief Check if a servo is inverted.
     * @param id The servo to query.
     * @return true if inverted.
     */
    bool isInverted(ServoID id) const;

    // ── Safety ──────────────────────────────────────────────────────────────

    /**
     * @brief Set safe angle limits for a servo.
     * @param id       The servo to configure.
     * @param minAngle Minimum allowed angle in degrees.
     * @param maxAngle Maximum allowed angle in degrees.
     */
    void setLimits(ServoID id, float minAngle, float maxAngle);

    /**
     * @brief Check if the PCA9685 was successfully initialized.
     * @return true if the driver is ready.
     */
    bool isReady() const;

private:
    Adafruit_PWMServoDriver m_pwm;         ///< PCA9685 driver instance
    ServoState m_servos[NUM_SERVOS];        ///< Per-servo state array
    bool m_ready;                           ///< PCA9685 initialization status

    /**
     * @brief Apply offset and inversion to a raw target angle.
     *
     * The processing pipeline is:
     *   1. Add calibration offset
     *   2. Clamp to safe limits
     *   3. Apply inversion (180 - angle) if inverted
     *   4. Final clamp to hardware limits
     *
     * @param id    The servo being processed.
     * @param angle Raw target angle in degrees.
     * @return Processed angle ready for microsecond conversion.
     */
    float processAngle(ServoID id, float angle) const;

    /**
     * @brief Write a processed angle to the PCA9685 hardware.
     * @param channel PCA9685 channel number.
     * @param angle   Processed angle in degrees.
     */
    void writeHardware(uint8_t channel, float angle);
};

#endif // SERVODRIVER_H
