/**
 * @file Robot.cpp
 * @brief Implementation of the Robot state machine and coordinator.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#include "Robot.h"
#include "Utils.h"

// Static singleton pointer for the Bluetooth callback bridge
Robot* Robot::s_instance = nullptr;

// ─────────────────────────────────────────────────────────────────────────────
// Construction
// ─────────────────────────────────────────────────────────────────────────────

Robot::Robot()
    : m_state(RobotState::IDLE)
    , m_previousState(RobotState::IDLE)
    , m_lastActivityMs(0)
    , m_initialized(false)
{
    s_instance = this;
}

// ─────────────────────────────────────────────────────────────────────────────
// Initialization
// ─────────────────────────────────────────────────────────────────────────────

bool Robot::begin() {
    DEBUG_PRINTLN(F("\n╔══════════════════════════════════════╗"));
    DEBUG_PRINTLN(F("║        OpenCat ESP32 v1.0.0          ║"));
    DEBUG_PRINTLN(F("╚══════════════════════════════════════╝\n"));

    // 1. Initialize I2C bus
    Wire.begin(Config::I2C::SDA_PIN, Config::I2C::SCL_PIN);
    Wire.setClock(Config::I2C::CLOCK_HZ);
    DEBUG_PRINTLN(F("[Robot] I2C bus initialized."));

    // 2. Initialize servo driver (PCA9685)
    bool servoOk = m_servoDriver.begin();
    if (!servoOk) {
        DEBUG_PRINTLN(F("[Robot] CRITICAL: PCA9685 not detected!"));
        DEBUG_PRINTLN(F("        Check wiring: SDA=GPIO21, SCL=GPIO22"));
        // Continue anyway — allows BT testing without servos
    }

    // 3. Initialize calibration and load offsets
    m_calibration.begin(m_servoDriver);

    // 4. Initialize motion systems
    m_motionPlayer.begin(m_servoDriver);
    m_gaitEngine.begin(m_servoDriver);

    // 5. Register built-in skills
    m_skills.registerBuiltins();

    // 6. Initialize Bluetooth
    bool btOk = m_bluetooth.begin();
    m_bluetooth.setCommandCallback(&Robot::onBluetoothCommand);

    if (!btOk) {
        DEBUG_PRINTLN(F("[Robot] WARNING: Bluetooth failed to start."));
    }

    m_lastActivityMs = millis();
    m_initialized = true;

    DEBUG_PRINTLN(F("\n[Robot] Initialization complete."));
    DEBUG_PRINTF("[Robot] Servos: %s | Bluetooth: %s\n",
                 servoOk ? "OK" : "FAIL",
                 btOk ? "OK" : "FAIL");
    DEBUG_PRINTLN(F("[Robot] Send 'c' via Serial to enter calibration mode."));
    DEBUG_PRINTLN(F("[Robot] Waiting for Bluetooth commands...\n"));

    return servoOk && btOk;
}

// ─────────────────────────────────────────────────────────────────────────────
// Main Update Loop
// ─────────────────────────────────────────────────────────────────────────────

void Robot::update(uint32_t deltaMs) {
    if (!m_initialized) return;

    // Update the active motion subsystem
    if (isLocomotionState(m_state)) {
        m_gaitEngine.update(deltaMs);

        // If gait has stopped naturally (ramp-down complete), go to IDLE
        if (!m_gaitEngine.isActive() && m_state != RobotState::IDLE) {
            m_state = RobotState::IDLE;
            DEBUG_PRINTLN(F("[Robot] Gait completed → IDLE"));
        }
    } else {
        m_motionPlayer.update(deltaMs);
    }

    // Safety: servo timeout check
    checkServoTimeout(deltaMs);
}

void Robot::updateBluetooth() {
    if (!m_initialized) return;
    m_bluetooth.update();
}

// ─────────────────────────────────────────────────────────────────────────────
// State Machine
// ─────────────────────────────────────────────────────────────────────────────

void Robot::setState(RobotState newState) {
    if (newState == m_state) return;  // No change

    DEBUG_PRINTF("[Robot] State: %d → %d\n",
                 static_cast<int>(m_state),
                 static_cast<int>(newState));

    m_previousState = m_state;
    m_state = newState;
    m_lastActivityMs = millis();

    handleStateTransition(newState);
}

RobotState Robot::getState() const {
    return m_state;
}

void Robot::handleStateTransition(RobotState newState) {
    // Stop current motion systems
    m_gaitEngine.stopGait();
    m_motionPlayer.stop();

    // Enable servos if we're coming from IDLE
    if (m_previousState == RobotState::IDLE) {
        m_servoDriver.enableAll();
    }

    switch (newState) {
        case RobotState::IDLE:
            // Stop everything, hold current position
            break;

        case RobotState::STAND:
            startStaticPose("stand");
            break;

        case RobotState::SIT:
            startStaticPose("sit");
            break;

        case RobotState::LAY:
            startStaticPose("lay");
            break;

        case RobotState::SLEEP:
            startStaticPose("sleep");
            break;

        case RobotState::WALK_FORWARD:
            startLocomotion(GaitType::WALK);
            break;

        case RobotState::WALK_BACKWARD:
            startLocomotion(GaitType::BACK);
            break;

        case RobotState::TURN_LEFT:
            startLocomotion(GaitType::TURN_L);
            break;

        case RobotState::TURN_RIGHT:
            startLocomotion(GaitType::TURN_R);
            break;

        case RobotState::TROT:
            startLocomotion(GaitType::TROT);
            break;

        case RobotState::CRAWL:
            startLocomotion(GaitType::WALK);  // Crawl uses same gait, slower
            break;

        case RobotState::WAVE:
            startStaticPose("wave");
            break;

        case RobotState::SHAKE:
            startStaticPose("shake");
            break;

        case RobotState::STRETCH:
            startStaticPose("stretch");
            break;
    }
}

void Robot::startStaticPose(const char* skillName) {
    const Motion* skill = m_skills.getSkill(skillName);
    if (skill) {
        // If only 1 keyframe, use smooth transition to it
        if (skill->numKeyframes == 1) {
            m_motionPlayer.transitionTo(
                skill->keyframes[0].pose,
                Config::Motion::DEFAULT_TRANSITION_MS,
                InterpolationType::EASE_IN_OUT
            );
        } else {
            m_motionPlayer.playMotion(*skill);
        }
    } else {
        DEBUG_PRINTF("[Robot] Skill \"%s\" not found in registry.\n", skillName);
    }
}

void Robot::startLocomotion(GaitType gaitType) {
    m_gaitEngine.startGait(gaitType);
}

// ─────────────────────────────────────────────────────────────────────────────
// Safety
// ─────────────────────────────────────────────────────────────────────────────

void Robot::emergencyStop() {
    DEBUG_PRINTLN(F("[Robot] *** EMERGENCY STOP ***"));
    m_gaitEngine.stopGait();
    m_motionPlayer.stop();
    m_state = RobotState::IDLE;
    // Don't disable servos immediately — hold position to avoid dropping
    m_lastActivityMs = millis();
}

void Robot::checkServoTimeout(uint32_t deltaMs) {
    // If in IDLE and no activity for SERVO_TIMEOUT_MS, disable servos
    if (m_state == RobotState::IDLE) {
        if (Utils::hasElapsed(m_lastActivityMs, Config::Safety::SERVO_TIMEOUT_MS)) {
            m_servoDriver.disableAll();
            m_lastActivityMs = millis();  // Reset to avoid repeated disable messages
            DEBUG_PRINTLN(F("[Robot] Servo timeout — servos disabled."));
        }
    } else {
        m_lastActivityMs = millis();
    }
}

void Robot::enterCalibration() {
    m_gaitEngine.stopGait();
    m_motionPlayer.stop();
    m_state = RobotState::IDLE;
    m_calibration.enterCalibrationMode();
}

// ─────────────────────────────────────────────────────────────────────────────
// State Classification Helpers
// ─────────────────────────────────────────────────────────────────────────────

bool Robot::isLocomotionState(RobotState state) {
    switch (state) {
        case RobotState::WALK_FORWARD:
        case RobotState::WALK_BACKWARD:
        case RobotState::TURN_LEFT:
        case RobotState::TURN_RIGHT:
        case RobotState::TROT:
        case RobotState::CRAWL:
            return true;
        default:
            return false;
    }
}

bool Robot::isStaticPoseState(RobotState state) {
    switch (state) {
        case RobotState::STAND:
        case RobotState::SIT:
        case RobotState::LAY:
        case RobotState::SLEEP:
        case RobotState::WAVE:
        case RobotState::SHAKE:
        case RobotState::STRETCH:
            return true;
        default:
            return false;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Bluetooth Callback Bridge
// ─────────────────────────────────────────────────────────────────────────────

void Robot::onBluetoothCommand(RobotState newState) {
    if (s_instance) {
        s_instance->setState(newState);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Subsystem Accessors
// ─────────────────────────────────────────────────────────────────────────────

ServoDriver&         Robot::getServoDriver()  { return m_servoDriver; }
Calibration&         Robot::getCalibration()   { return m_calibration; }
MotionPlayer&        Robot::getMotionPlayer()  { return m_motionPlayer; }
GaitEngine&          Robot::getGaitEngine()    { return m_gaitEngine; }
BluetoothController& Robot::getBluetooth()     { return m_bluetooth; }
SkillRegistry&       Robot::getSkills()        { return m_skills; }
