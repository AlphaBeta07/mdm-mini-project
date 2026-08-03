/**
 * @file Bluetooth.cpp
 * @brief Implementation of the BluetoothController.
 *
 * Handles Bluetooth Classic initialization, non-blocking data polling,
 * single-character command parsing, and callback dispatch.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#include "Bluetooth.h"
#include "Utils.h"

BluetoothController::BluetoothController()
    : m_callback(nullptr)
    , m_initialized(false)
    , m_commandCount(0)
{
}

bool BluetoothController::begin() {
    bool ok = m_serial.begin(Config::Bluetooth::DEVICE_NAME);
    if (ok) {
        m_initialized = true;
        DEBUG_PRINTF("[Bluetooth] Started as \"%s\"\n",
                     Config::Bluetooth::DEVICE_NAME);
    } else {
        DEBUG_PRINTLN(F("[Bluetooth] ERROR: Failed to start Bluetooth."));
    }
    return ok;
}

void BluetoothController::update() {
    if (!m_initialized) return;

    while (m_serial.available()) {
        char c = static_cast<char>(m_serial.read());

        RobotState newState;
        if (parseCommand(c, newState)) {
            m_commandCount++;

            DEBUG_PRINTF("[Bluetooth] Command '%c' -> State %d (total: %lu)\n",
                         c, static_cast<int>(newState), m_commandCount);

            // Acknowledge the command
            m_serial.printf("OK:%c\n", c);

            // Notify the robot
            if (m_callback) {
                m_callback(newState);
            }
        }
    }
}

void BluetoothController::setCommandCallback(CommandCallback callback) {
    m_callback = callback;
}

void BluetoothController::send(const char* message) {
    if (m_initialized) {
        m_serial.println(message);
    }
}

bool BluetoothController::isConnected() {
    if (!m_initialized) return false;
    return m_serial.connected();
}

uint32_t BluetoothController::getCommandCount() const {
    return m_commandCount;
}

bool BluetoothController::parseCommand(char c, RobotState& outState) const {
    switch (c) {
        case BtCommand::STAND:
            outState = RobotState::STAND;
            return true;

        case BtCommand::SIT:
            outState = RobotState::SIT;
            return true;

        case BtCommand::LAY:
            outState = RobotState::LAY;
            return true;

        case BtCommand::SLEEP:
            outState = RobotState::SLEEP;
            return true;

        case BtCommand::WALK_FORWARD:
            outState = RobotState::WALK_FORWARD;
            return true;

        case BtCommand::WALK_BACK:
            outState = RobotState::WALK_BACKWARD;
            return true;

        case BtCommand::TURN_LEFT:
            outState = RobotState::TURN_LEFT;
            return true;

        case BtCommand::TURN_RIGHT:
            outState = RobotState::TURN_RIGHT;
            return true;

        case BtCommand::STOP:
            outState = RobotState::IDLE;
            return true;

        default:
            // Ignore unknown characters (newlines, spaces, etc.)
            return false;
    }
}
