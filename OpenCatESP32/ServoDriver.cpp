/**
 * @file ServoDriver.cpp
 * @brief Implementation of the ServoDriver class.
 *
 * Handles all PCA9685 communication, angle processing (offset, inversion,
 * clamping), and per-servo state management.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#include "ServoDriver.h"
#include "Utils.h"

// ─────────────────────────────────────────────────────────────────────────────
// Construction
// ─────────────────────────────────────────────────────────────────────────────

ServoDriver::ServoDriver()
    : m_pwm(Config::PCA9685::ADDRESS)
    , m_ready(false)
{
    // Initialize all servo states to safe defaults
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        m_servos[i].currentAngle = Config::Servo::CENTER_ANGLE_DEG;
        m_servos[i].targetAngle  = Config::Servo::CENTER_ANGLE_DEG;
        m_servos[i].offset       = Config::Calibration::DEFAULT_OFFSET;
        m_servos[i].minAngle     = Config::Servo::SAFE_MIN[i];
        m_servos[i].maxAngle     = Config::Servo::SAFE_MAX[i];
        m_servos[i].inverted     = Config::Servo::INVERTED[i];
        m_servos[i].enabled      = true;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Initialization
// ─────────────────────────────────────────────────────────────────────────────

bool ServoDriver::begin() {
    m_pwm.begin();
    m_pwm.setPWMFreq(Config::PCA9685::PWM_FREQ_HZ);

    // Brief delay for oscillator to stabilize
    delay(10);

    // Verify PCA9685 is responding by reading a register
    // The Adafruit library doesn't provide a direct "isConnected" method,
    // so we verify by attempting I2C communication.
    Wire.beginTransmission(Config::PCA9685::ADDRESS);
    uint8_t error = Wire.endTransmission();

    if (error == 0) {
        m_ready = true;
        DEBUG_PRINTLN(F("[ServoDriver] PCA9685 initialized successfully."));
    } else {
        m_ready = false;
        DEBUG_PRINTF("[ServoDriver] PCA9685 NOT found at 0x%02X (error %d)\n",
                     Config::PCA9685::ADDRESS, error);
    }

    return m_ready;
}

// ─────────────────────────────────────────────────────────────────────────────
// Angle Control
// ─────────────────────────────────────────────────────────────────────────────

void ServoDriver::writeAngle(ServoID id, float angle) {
    uint8_t ch = static_cast<uint8_t>(id);
    if (ch >= NUM_SERVOS) return;

    m_servos[ch].targetAngle = angle;
    float processed = processAngle(id, angle);
    m_servos[ch].currentAngle = processed;

    if (m_servos[ch].enabled && m_ready) {
        writeHardware(ch, processed);
    }
}

void ServoDriver::writeAngle(uint8_t channel, float angle) {
    if (channel >= NUM_SERVOS) return;
    writeAngle(static_cast<ServoID>(channel), angle);
}

void ServoDriver::writeMicroseconds(ServoID id, uint16_t microseconds) {
    uint8_t ch = static_cast<uint8_t>(id);
    if (ch >= NUM_SERVOS) return;
    if (!m_servos[ch].enabled || !m_ready) return;

    uint16_t ticks = Utils::microsecondsToTicks(microseconds);
    m_pwm.setPWM(ch, 0, ticks);

    // Update tracked angle for consistency
    m_servos[ch].currentAngle = Utils::microsecondsToDegrees(microseconds);
}

void ServoDriver::writeAllAngles(float angle) {
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        writeAngle(static_cast<ServoID>(i), angle);
    }
}

void ServoDriver::writeAngles(const float angles[NUM_SERVOS]) {
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        writeAngle(static_cast<ServoID>(i), angles[i]);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Servo State Queries
// ─────────────────────────────────────────────────────────────────────────────

float ServoDriver::getCurrentAngle(ServoID id) const {
    uint8_t ch = static_cast<uint8_t>(id);
    if (ch >= NUM_SERVOS) return Config::Servo::CENTER_ANGLE_DEG;
    return m_servos[ch].currentAngle;
}

void ServoDriver::getCurrentAngles(float outAngles[NUM_SERVOS]) const {
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        outAngles[i] = m_servos[i].currentAngle;
    }
}

float ServoDriver::getTargetAngle(ServoID id) const {
    uint8_t ch = static_cast<uint8_t>(id);
    if (ch >= NUM_SERVOS) return Config::Servo::CENTER_ANGLE_DEG;
    return m_servos[ch].targetAngle;
}

// ─────────────────────────────────────────────────────────────────────────────
// Enable / Disable
// ─────────────────────────────────────────────────────────────────────────────

void ServoDriver::enableServo(ServoID id) {
    uint8_t ch = static_cast<uint8_t>(id);
    if (ch >= NUM_SERVOS) return;

    m_servos[ch].enabled = true;
    DEBUG_PRINTF("[ServoDriver] Servo CH%d enabled.\n", ch);

    // Immediately write the current target to restore position
    if (m_ready) {
        writeHardware(ch, m_servos[ch].currentAngle);
    }
}

void ServoDriver::disableServo(ServoID id) {
    uint8_t ch = static_cast<uint8_t>(id);
    if (ch >= NUM_SERVOS) return;

    m_servos[ch].enabled = false;

    // Cut PWM signal — servo goes limp
    if (m_ready) {
        m_pwm.setPWM(ch, 0, 0);
    }

    DEBUG_PRINTF("[ServoDriver] Servo CH%d disabled.\n", ch);
}

void ServoDriver::enableAll() {
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        enableServo(static_cast<ServoID>(i));
    }
}

void ServoDriver::disableAll() {
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        disableServo(static_cast<ServoID>(i));
    }
}

bool ServoDriver::isEnabled(ServoID id) const {
    uint8_t ch = static_cast<uint8_t>(id);
    if (ch >= NUM_SERVOS) return false;
    return m_servos[ch].enabled;
}

// ─────────────────────────────────────────────────────────────────────────────
// Calibration
// ─────────────────────────────────────────────────────────────────────────────

void ServoDriver::setOffset(ServoID id, float offset) {
    uint8_t ch = static_cast<uint8_t>(id);
    if (ch >= NUM_SERVOS) return;

    // Clamp offset to safe range
    m_servos[ch].offset = Utils::clampf(offset,
                                        -Config::Calibration::MAX_OFFSET,
                                         Config::Calibration::MAX_OFFSET);

    DEBUG_PRINTF("[ServoDriver] CH%d offset set to %.1f°\n",
                 ch, m_servos[ch].offset);
}

float ServoDriver::getOffset(ServoID id) const {
    uint8_t ch = static_cast<uint8_t>(id);
    if (ch >= NUM_SERVOS) return 0.0f;
    return m_servos[ch].offset;
}

void ServoDriver::setInverted(ServoID id, bool inverted) {
    uint8_t ch = static_cast<uint8_t>(id);
    if (ch >= NUM_SERVOS) return;
    m_servos[ch].inverted = inverted;
}

bool ServoDriver::isInverted(ServoID id) const {
    uint8_t ch = static_cast<uint8_t>(id);
    if (ch >= NUM_SERVOS) return false;
    return m_servos[ch].inverted;
}

// ─────────────────────────────────────────────────────────────────────────────
// Safety Limits
// ─────────────────────────────────────────────────────────────────────────────

void ServoDriver::setLimits(ServoID id, float minAngle, float maxAngle) {
    uint8_t ch = static_cast<uint8_t>(id);
    if (ch >= NUM_SERVOS) return;

    m_servos[ch].minAngle = Utils::clampf(minAngle,
                                          Config::Servo::ANGLE_MIN_DEG,
                                          Config::Servo::ANGLE_MAX_DEG);
    m_servos[ch].maxAngle = Utils::clampf(maxAngle,
                                          Config::Servo::ANGLE_MIN_DEG,
                                          Config::Servo::ANGLE_MAX_DEG);

    // Ensure min <= max
    if (m_servos[ch].minAngle > m_servos[ch].maxAngle) {
        float temp = m_servos[ch].minAngle;
        m_servos[ch].minAngle = m_servos[ch].maxAngle;
        m_servos[ch].maxAngle = temp;
    }
}

bool ServoDriver::isReady() const {
    return m_ready;
}

// ─────────────────────────────────────────────────────────────────────────────
// Private: Angle Processing Pipeline
// ─────────────────────────────────────────────────────────────────────────────

float ServoDriver::processAngle(ServoID id, float angle) const {
    uint8_t ch = static_cast<uint8_t>(id);

    // Step 1: Add calibration offset
    float processed = angle + m_servos[ch].offset;

    // Step 2: Clamp to per-servo safe limits
    processed = Utils::clampf(processed,
                              m_servos[ch].minAngle,
                              m_servos[ch].maxAngle);

    // Step 3: Apply inversion for mirrored mounting
    if (m_servos[ch].inverted) {
        processed = Config::Servo::ANGLE_MAX_DEG - processed;
    }

    // Step 4: Final clamp to absolute hardware limits
    processed = Utils::clampf(processed,
                              Config::Servo::ANGLE_MIN_DEG,
                              Config::Servo::ANGLE_MAX_DEG);

    return processed;
}

void ServoDriver::writeHardware(uint8_t channel, float angle) {
    uint16_t us    = Utils::degreesToMicroseconds(angle);
    uint16_t ticks = Utils::microsecondsToTicks(us);
    m_pwm.setPWM(channel, 0, ticks);
}
