/**
 * @file Bluetooth.h
 * @brief Bluetooth Classic communication controller.
 *
 * Wraps ESP32's BluetoothSerial for single-character command reception.
 * Parses commands and notifies the Robot state machine via callback.
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <Arduino.h>
#include "BluetoothSerial.h"
#include "Constants.h"
#include "Config.h"

/**
 * @brief Callback function type for received commands.
 *
 * Called when a valid Bluetooth command is parsed.
 * The callback receives the new RobotState corresponding to the command.
 */
typedef void (*CommandCallback)(RobotState newState);

/**
 * @class BluetoothController
 * @brief Manages Bluetooth Classic communication and command parsing.
 *
 * Usage:
 * @code
 *   BluetoothController bt;
 *   bt.begin();
 *   bt.setCommandCallback(myHandler);
 *   // In loop or FreeRTOS task:
 *   bt.update();
 * @endcode
 */
class BluetoothController {
public:
    BluetoothController();

    /**
     * @brief Initialize Bluetooth with the configured device name.
     * @return true if Bluetooth started successfully.
     */
    bool begin();

    /**
     * @brief Poll for incoming data and parse commands.
     *
     * Non-blocking. Should be called frequently (e.g., every 10ms).
     * When a valid single-character command is received, the registered
     * callback is invoked with the corresponding RobotState.
     */
    void update();

    /**
     * @brief Register a callback for received commands.
     * @param callback Function to call when a command is parsed.
     */
    void setCommandCallback(CommandCallback callback);

    /**
     * @brief Send a text message via Bluetooth.
     * @param message The string to send.
     */
    void send(const char* message);

    /**
     * @brief Check if a Bluetooth client is connected.
     * @return true if connected.
     */
    bool isConnected();

    /**
     * @brief Get the number of commands received since boot.
     */
    uint32_t getCommandCount() const;

private:
    BluetoothSerial m_serial;        ///< ESP32 Bluetooth Serial instance
    CommandCallback m_callback;      ///< Command handler callback
    bool            m_initialized;   ///< Whether begin() was called
    uint32_t        m_commandCount;  ///< Total commands received

    /**
     * @brief Parse a single character into a RobotState.
     * @param c The received character.
     * @param outState Output: the corresponding RobotState.
     * @return true if the character was a valid command.
     */
    bool parseCommand(char c, RobotState& outState) const;
};

#endif // BLUETOOTH_H
