/**
 * @file Calibration.h
 * @brief Persistent servo calibration system using ESP32 NVS (Preferences).
 *
 * Provides:
 *   - Save/load/reset of per-servo angle offsets to non-volatile storage
 *   - Interactive serial-based calibration menu
 *   - Live adjustment with immediate servo feedback
 *
 * Calibration offsets are applied by the ServoDriver during angle writes.
 * This module only manages the offset values and their persistence.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Arduino.h>
#include <Preferences.h>
#include "Constants.h"
#include "Config.h"

// Forward declaration to avoid circular includes
class ServoDriver;

/**
 * @class Calibration
 * @brief Manages persistent servo calibration offsets via ESP32 NVS.
 *
 * Workflow:
 *   1. Call begin() after ServoDriver is initialized
 *   2. Offsets are loaded automatically from NVS
 *   3. Use the serial calibration menu to adjust offsets
 *   4. Save offsets to persist across power cycles
 *
 * Usage:
 * @code
 *   ServoDriver driver;
 *   Calibration cal;
 *   driver.begin();
 *   cal.begin(driver);
 *   // Offsets are now loaded and applied to driver
 * @endcode
 */
class Calibration {
public:
    /**
     * @brief Construct the Calibration manager.
     */
    Calibration();

    /**
     * @brief Initialize calibration system and load saved offsets.
     *
     * Opens the NVS namespace, reads saved offsets, and applies them
     * to the provided ServoDriver.
     *
     * @param driver Reference to the active ServoDriver.
     * @return true if NVS was opened successfully.
     */
    bool begin(ServoDriver& driver);

    // ── Offset Management ───────────────────────────────────────────────────

    /**
     * @brief Save all current offsets to NVS.
     *
     * Writes each servo's offset value to non-volatile storage
     * so it persists across power cycles.
     */
    void saveOffsets();

    /**
     * @brief Load all offsets from NVS and apply to ServoDriver.
     *
     * Called automatically by begin(), but can be called manually
     * to reload from storage.
     */
    void loadOffsets();

    /**
     * @brief Reset all offsets to zero and save.
     *
     * Clears calibration data from NVS and sets all offsets to 0.0.
     */
    void resetOffsets();

    /**
     * @brief Adjust the offset for a single servo by a delta.
     *
     * The offset is clamped to ±MAX_OFFSET and immediately applied
     * to the ServoDriver.
     *
     * @param id    The servo to adjust.
     * @param delta Degrees to add to the current offset (can be negative).
     */
    void adjustOffset(ServoID id, float delta);

    /**
     * @brief Set the offset for a single servo to an absolute value.
     * @param id     The servo to set.
     * @param offset New offset in degrees.
     */
    void setOffset(ServoID id, float offset);

    /**
     * @brief Get the current offset for a servo.
     * @param id The servo to query.
     * @return Offset in degrees.
     */
    float getOffset(ServoID id) const;

    // ── Calibration Mode ────────────────────────────────────────────────────

    /**
     * @brief Enter interactive calibration mode.
     *
     * Centers all servos at 90° + their current offset, then presents
     * a serial menu for live adjustment. This method blocks until the
     * user exits calibration mode.
     *
     * Serial commands in calibration mode:
     *   0–7   : Select servo channel
     *   + / = : Increase offset by ADJUST_STEP
     *   - / _ : Decrease offset by ADJUST_STEP
     *   s     : Save all offsets
     *   r     : Reset all offsets to zero
     *   p     : Print current offsets
     *   x     : Exit calibration mode
     */
    void enterCalibrationMode();

    /**
     * @brief Process a single character from serial for calibration.
     *
     * Non-blocking alternative to enterCalibrationMode(). Returns true
     * if the character was consumed (i.e., it was a calibration command).
     *
     * @param c The character to process.
     * @return true if the character was a calibration command.
     */
    bool processSerialChar(char c);

    /**
     * @brief Check if calibration mode is currently active.
     * @return true if in calibration mode.
     */
    bool isCalibrating() const;

    /**
     * @brief Print all current offsets to Serial.
     */
    void printOffsets() const;

private:
    Preferences  m_prefs;                   ///< NVS Preferences instance
    ServoDriver* m_driver;                  ///< Pointer to the active ServoDriver
    float        m_offsets[NUM_SERVOS];     ///< Current offset values (degrees)
    uint8_t      m_selectedChannel;         ///< Currently selected channel in cal mode
    bool         m_calibrating;             ///< Whether calibration mode is active
    bool         m_initialized;             ///< Whether begin() was called

    /**
     * @brief Build the NVS key string for a servo channel.
     * @param channel Servo channel number.
     * @param buffer  Output buffer (must be at least 8 chars).
     */
    void buildKey(uint8_t channel, char* buffer) const;

    /**
     * @brief Apply the current offset for a servo to the ServoDriver.
     * @param id The servo to update.
     */
    void applyOffset(ServoID id);

    /**
     * @brief Apply all offsets to the ServoDriver.
     */
    void applyAllOffsets();

    /**
     * @brief Print the calibration menu to Serial.
     */
    void printCalibrationMenu() const;

    /**
     * @brief Center a servo at 90° for calibration.
     * @param id The servo to center.
     */
    void centerServo(ServoID id);

    /**
     * @brief Center all servos at 90° for calibration.
     */
    void centerAllServos();

    /** @brief Servo name lookup for display. */
    static const char* servoName(uint8_t channel);
};

#endif // CALIBRATION_H
