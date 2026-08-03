/**
 * @file Calibration.cpp
 * @brief Implementation of the Calibration class.
 *
 * Manages persistent servo offsets using ESP32's NVS (Preferences library).
 * Provides an interactive serial calibration menu for live tuning.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#include "Calibration.h"
#include "ServoDriver.h"
#include "Utils.h"

// ─────────────────────────────────────────────────────────────────────────────
// Static servo name table for display
// ─────────────────────────────────────────────────────────────────────────────

static const char* const SERVO_NAMES[NUM_SERVOS] = {
    "FL Hip ",   // CH0
    "FL Knee",   // CH1
    "FR Hip ",   // CH2
    "FR Knee",   // CH3
    "RL Hip ",   // CH4
    "RL Knee",   // CH5
    "RR Hip ",   // CH6
    "RR Knee"    // CH7
};

// ─────────────────────────────────────────────────────────────────────────────
// Construction
// ─────────────────────────────────────────────────────────────────────────────

Calibration::Calibration()
    : m_driver(nullptr)
    , m_selectedChannel(0)
    , m_calibrating(false)
    , m_initialized(false)
{
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        m_offsets[i] = Config::Calibration::DEFAULT_OFFSET;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Initialization
// ─────────────────────────────────────────────────────────────────────────────

bool Calibration::begin(ServoDriver& driver) {
    m_driver = &driver;

    bool ok = m_prefs.begin(Config::Calibration::NVS_NAMESPACE, false);
    if (!ok) {
        DEBUG_PRINTLN(F("[Calibration] Failed to open NVS namespace."));
        return false;
    }

    m_initialized = true;
    loadOffsets();

    DEBUG_PRINTLN(F("[Calibration] Initialized. Offsets loaded from NVS."));
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// Offset Management
// ─────────────────────────────────────────────────────────────────────────────

void Calibration::saveOffsets() {
    if (!m_initialized) return;

    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        char key[8];
        buildKey(i, key);
        m_prefs.putFloat(key, m_offsets[i]);
    }

    DEBUG_PRINTLN(F("[Calibration] Offsets saved to NVS."));
}

void Calibration::loadOffsets() {
    if (!m_initialized) return;

    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        char key[8];
        buildKey(i, key);
        m_offsets[i] = m_prefs.getFloat(key, Config::Calibration::DEFAULT_OFFSET);
    }

    applyAllOffsets();
    DEBUG_PRINTLN(F("[Calibration] Offsets loaded from NVS."));
}

void Calibration::resetOffsets() {
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        m_offsets[i] = 0.0f;
    }

    applyAllOffsets();
    saveOffsets();

    DEBUG_PRINTLN(F("[Calibration] All offsets reset to zero."));
}

void Calibration::adjustOffset(ServoID id, float delta) {
    uint8_t ch = static_cast<uint8_t>(id);
    if (ch >= NUM_SERVOS) return;

    m_offsets[ch] = Utils::clampf(
        m_offsets[ch] + delta,
        -Config::Calibration::MAX_OFFSET,
         Config::Calibration::MAX_OFFSET
    );

    applyOffset(id);

    // If calibrating, move servo to show the change
    if (m_calibrating && m_driver) {
        centerServo(id);
    }
}

void Calibration::setOffset(ServoID id, float offset) {
    uint8_t ch = static_cast<uint8_t>(id);
    if (ch >= NUM_SERVOS) return;

    m_offsets[ch] = Utils::clampf(
        offset,
        -Config::Calibration::MAX_OFFSET,
         Config::Calibration::MAX_OFFSET
    );

    applyOffset(id);
}

float Calibration::getOffset(ServoID id) const {
    uint8_t ch = static_cast<uint8_t>(id);
    if (ch >= NUM_SERVOS) return 0.0f;
    return m_offsets[ch];
}

// ─────────────────────────────────────────────────────────────────────────────
// Calibration Mode
// ─────────────────────────────────────────────────────────────────────────────

void Calibration::enterCalibrationMode() {
    if (!m_driver) {
        DEBUG_PRINTLN(F("[Calibration] ERROR: No ServoDriver attached."));
        return;
    }

    m_calibrating = true;
    m_selectedChannel = 0;

    Serial.println(F("\n╔══════════════════════════════════════╗"));
    Serial.println(F("║     SERVO CALIBRATION MODE           ║"));
    Serial.println(F("╚══════════════════════════════════════╝"));

    // Center all servos for reference
    centerAllServos();
    printCalibrationMenu();
    printOffsets();

    // Block until user exits
    while (m_calibrating) {
        if (Serial.available()) {
            char c = Serial.read();
            processSerialChar(c);
        }
        delay(10);  // Acceptable here since calibration is interactive
    }

    Serial.println(F("\n[Calibration] Exited calibration mode."));
}

bool Calibration::processSerialChar(char c) {
    if (!m_driver) return false;

    // Channel selection: '0' through '7'
    if (c >= '0' && c <= '7') {
        m_selectedChannel = c - '0';
        Serial.printf(">> Selected CH%d: %s\n",
                       m_selectedChannel, servoName(m_selectedChannel));
        return true;
    }

    switch (c) {
        case '+':
        case '=':
            // Increase offset
            adjustOffset(static_cast<ServoID>(m_selectedChannel),
                         Config::Calibration::ADJUST_STEP);
            Serial.printf("   CH%d (%s) offset: %+.1f°\n",
                          m_selectedChannel,
                          servoName(m_selectedChannel),
                          m_offsets[m_selectedChannel]);
            return true;

        case '-':
        case '_':
            // Decrease offset
            adjustOffset(static_cast<ServoID>(m_selectedChannel),
                         -Config::Calibration::ADJUST_STEP);
            Serial.printf("   CH%d (%s) offset: %+.1f°\n",
                          m_selectedChannel,
                          servoName(m_selectedChannel),
                          m_offsets[m_selectedChannel]);
            return true;

        case 's':
        case 'S':
            saveOffsets();
            Serial.println(F(">> Offsets SAVED to NVS."));
            return true;

        case 'r':
            resetOffsets();
            centerAllServos();
            Serial.println(F(">> All offsets RESET to zero."));
            return true;

        case 'p':
        case 'P':
            printOffsets();
            return true;

        case 'x':
        case 'X':
            m_calibrating = false;
            return true;

        case 'h':
        case 'H':
        case '?':
            printCalibrationMenu();
            return true;

        default:
            return false;
    }
}

bool Calibration::isCalibrating() const {
    return m_calibrating;
}

void Calibration::printOffsets() const {
    Serial.println(F("\n┌──────────────────────────────────────┐"));
    Serial.println(F("│         CURRENT OFFSETS              │"));
    Serial.println(F("├──────┬──────────┬───────────────────┤"));
    Serial.println(F("│  CH  │  Offset  │  Servo            │"));
    Serial.println(F("├──────┼──────────┼───────────────────┤"));

    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        char marker = (m_calibrating && i == m_selectedChannel) ? '>' : ' ';
        Serial.printf("│ %c %d  │  %+5.1f°  │  %s           │\n",
                       marker, i, m_offsets[i], servoName(i));
    }

    Serial.println(F("└──────┴──────────┴───────────────────┘"));
}

// ─────────────────────────────────────────────────────────────────────────────
// Private Helpers
// ─────────────────────────────────────────────────────────────────────────────

void Calibration::buildKey(uint8_t channel, char* buffer) const {
    // Generate key like "off0", "off1", ..., "off7"
    snprintf(buffer, 8, "%s%d", Config::Calibration::OFFSET_KEY_PREFIX, channel);
}

void Calibration::applyOffset(ServoID id) {
    if (!m_driver) return;
    uint8_t ch = static_cast<uint8_t>(id);
    m_driver->setOffset(id, m_offsets[ch]);
}

void Calibration::applyAllOffsets() {
    if (!m_driver) return;
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        m_driver->setOffset(static_cast<ServoID>(i), m_offsets[i]);
    }
}

void Calibration::printCalibrationMenu() const {
    Serial.println(F("\n── Calibration Commands ──────────────"));
    Serial.println(F("  0-7  : Select servo channel"));
    Serial.println(F("  +/-  : Adjust offset ±1°"));
    Serial.println(F("  s    : Save offsets to NVS"));
    Serial.println(F("  r    : Reset all offsets to 0"));
    Serial.println(F("  p    : Print current offsets"));
    Serial.println(F("  h    : Show this menu"));
    Serial.println(F("  x    : Exit calibration mode"));
    Serial.println(F("──────────────────────────────────────\n"));
}

void Calibration::centerServo(ServoID id) {
    if (!m_driver) return;
    m_driver->writeAngle(id, Config::Servo::CENTER_ANGLE_DEG);
}

void Calibration::centerAllServos() {
    if (!m_driver) return;
    m_driver->enableAll();
    for (uint8_t i = 0; i < NUM_SERVOS; ++i) {
        m_driver->writeAngle(static_cast<ServoID>(i),
                             Config::Servo::CENTER_ANGLE_DEG);
    }
    Serial.println(F("[Calibration] All servos centered at 90°."));
}

const char* Calibration::servoName(uint8_t channel) {
    if (channel >= NUM_SERVOS) return "Unknown";
    return SERVO_NAMES[channel];
}
