/**
 * @file OpenCatESP32.ino
 * @brief Main entry point for the OpenCatESP32 quadruped robot firmware.
 *
 * This sketch initializes all subsystems and creates two FreeRTOS tasks:
 *   - Motion Task (Core 1, 50 Hz): Runs the robot state machine and motion
 *   - Bluetooth Task (Core 0, ~100 Hz): Polls for Bluetooth commands
 *
 * The Arduino loop() is left mostly idle, handling only serial input
 * for calibration and debug commands.
 *
 * Hardware:
 *   - ESP32 DevKit V1 (ESP-WROOM-32)
 *   - PCA9685 16-Channel Servo Driver (I2C: SDA=GPIO21, SCL=GPIO22)
 *   - 8× SG90/MG90S Servos on channels 0–7
 *   - External 5V servo power supply (common ground with ESP32)
 *
 * @author OpenCatESP32 Project
 * @version 1.0.0
 */

#include <Wire.h>
#include "Config.h"
#include "Constants.h"
#include "Robot.h"
#include "Utils.h"

// ─────────────────────────────────────────────────────────────────────────────
// Global Robot Instance
// ─────────────────────────────────────────────────────────────────────────────

/** @brief The single global robot instance. */
Robot robot;

// ─────────────────────────────────────────────────────────────────────────────
// FreeRTOS Task Handles
// ─────────────────────────────────────────────────────────────────────────────

TaskHandle_t motionTaskHandle    = nullptr;
TaskHandle_t bluetoothTaskHandle = nullptr;

// ─────────────────────────────────────────────────────────────────────────────
// FreeRTOS Tasks
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Motion update task — runs at 50 Hz on Core 1.
 *
 * This task handles all servo motion computation and output:
 *   - State machine updates
 *   - MotionPlayer keyframe playback
 *   - GaitEngine procedural gait generation
 *   - Servo safety checks
 *
 * @param parameter Unused.
 */
void motionTask(void* parameter) {
    const TickType_t interval = pdMS_TO_TICKS(1000 / Config::Tasks::MOTION_FREQ_HZ);
    TickType_t lastWakeTime = xTaskGetTickCount();
    uint32_t lastMs = millis();

    for (;;) {
        uint32_t nowMs = millis();
        uint32_t deltaMs = nowMs - lastMs;
        lastMs = nowMs;

        // Clamp delta to prevent huge jumps after delays
        if (deltaMs > 100) deltaMs = 100;

        robot.update(deltaMs);

        // Sleep until next tick (precise 50 Hz timing)
        vTaskDelayUntil(&lastWakeTime, interval);
    }
}

/**
 * @brief Bluetooth polling task — runs on Core 0.
 *
 * Polls for incoming Bluetooth characters and dispatches commands
 * to the Robot state machine.
 *
 * @param parameter Unused.
 */
void bluetoothTask(void* parameter) {
    const TickType_t interval = pdMS_TO_TICKS(Config::Bluetooth::POLL_INTERVAL_MS);

    for (;;) {
        robot.updateBluetooth();
        vTaskDelay(interval);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Arduino Setup
// ─────────────────────────────────────────────────────────────────────────────

void setup() {
    // Initialize serial for debug output
    Serial.begin(Config::Serial_::BAUD_RATE);
    delay(500);  // Allow serial monitor to connect

    Serial.println(F("\n"));
    Serial.println(F("  ____                    ____      _   "));
    Serial.println(F(" / __ \\                  / __ \\    | |  "));
    Serial.println(F("| |  | |_ __   ___ _ __ | |  |__ _| |_ "));
    Serial.println(F("| |  | | '_ \\ / _ \\ '_ \\| |  / _` | __|"));
    Serial.println(F("| |__| | |_) |  __/ | | | |__| (_| | |_ "));
    Serial.println(F(" \\____/| .__/ \\___|_| |_|\\____\\__,_|\\__|"));
    Serial.println(F("       | |       ESP32 Quadruped v1.0   "));
    Serial.println(F("       |_|                              "));
    Serial.println(F(""));

    // Initialize the robot (I2C, PCA9685, BT, calibration)
    bool ok = robot.begin();

    if (!ok) {
        Serial.println(F("[Setup] WARNING: Some subsystems failed to initialize."));
        Serial.println(F("        The robot will continue with available hardware."));
    }

    // Create Motion Task on Core 1
    xTaskCreatePinnedToCore(
        motionTask,
        "MotionTask",
        Config::Tasks::MOTION_STACK_SIZE,
        nullptr,
        Config::Tasks::MOTION_PRIORITY,
        &motionTaskHandle,
        Config::Tasks::MOTION_CORE
    );
    Serial.println(F("[Setup] Motion task started on Core 1 (50 Hz)."));

    // Create Bluetooth Task on Core 0
    xTaskCreatePinnedToCore(
        bluetoothTask,
        "BT_Task",
        Config::Tasks::BT_STACK_SIZE,
        nullptr,
        Config::Tasks::BT_PRIORITY,
        &bluetoothTaskHandle,
        Config::Tasks::BT_CORE
    );
    Serial.println(F("[Setup] Bluetooth task started on Core 0."));

    Serial.println(F("\n[Setup] Ready! Serial commands:"));
    Serial.println(F("  c  = Enter calibration mode"));
    Serial.println(F("  i  = Print system info"));
    Serial.println(F("  t  = Test servos (center all at 90°)"));
    Serial.println(F(""));
}

// ─────────────────────────────────────────────────────────────────────────────
// Arduino Loop — Serial command handler (low priority)
// ─────────────────────────────────────────────────────────────────────────────

void loop() {
    // The main loop handles only serial debug/calibration commands.
    // All real-time work happens in FreeRTOS tasks.

    if (Serial.available()) {
        char c = Serial.read();

        switch (c) {
            case 'c':
            case 'C':
                // Enter calibration mode (blocks until exit)
                robot.enterCalibration();
                break;

            case 'i':
            case 'I':
                // Print system info
                Serial.println(F("\n── System Info ─────────────────────"));
                Serial.printf("  State:       %d\n", static_cast<int>(robot.getState()));
                Serial.printf("  Servos:      %s\n",
                              robot.getServoDriver().isReady() ? "OK" : "OFFLINE");
                Serial.printf("  Bluetooth:   %s\n",
                              robot.getBluetooth().isConnected() ? "CONNECTED" : "waiting");
                Serial.printf("  BT Commands: %lu\n",
                              robot.getBluetooth().getCommandCount());
                Serial.printf("  Skills:      %d registered\n",
                              robot.getSkills().getCount());
                Serial.printf("  Free heap:   %d bytes\n", ESP.getFreeHeap());
                Serial.println(F("────────────────────────────────────\n"));
                break;

            case 't':
            case 'T':
                // Test: center all servos at 90°
                Serial.println(F("[Test] Centering all servos at 90°..."));
                robot.getServoDriver().enableAll();
                robot.getServoDriver().writeAllAngles(90.0f);
                Serial.println(F("[Test] Done. All servos should be at center."));
                break;

            case 'p':
            case 'P':
                // Print calibration offsets
                robot.getCalibration().printOffsets();
                break;

            case 'k':
            case 'K':
                // Print registered skills
                robot.getSkills().printSkills();
                break;

            case 'w':
            case 'W':
                // I2C bus scanner — find all connected devices
                {
                    Serial.println(F("\n── I2C Bus Scan ────────────────────"));
                    uint8_t found = 0;
                    for (uint8_t addr = 1; addr < 127; addr++) {
                        Wire.beginTransmission(addr);
                        uint8_t err = Wire.endTransmission();
                        if (err == 0) {
                            Serial.printf("  Device found at 0x%02X\n", addr);
                            found++;
                        }
                    }
                    if (found == 0) {
                        Serial.println(F("  NO devices found!"));
                        Serial.println(F("  Check: VCC, GND, SDA(21), SCL(22)"));
                    } else {
                        Serial.printf("  %d device(s) found.\n", found);
                    }
                    Serial.println(F("────────────────────────────────────\n"));
                }
                break;

            default:
                // Try Bluetooth command chars via Serial for testing
                {
                    RobotState testState;
                    // Uppercase the char for matching
                    char upper = toupper(c);
                    bool matched = false;

                    switch (upper) {
                        case BtCommand::STAND:        testState = RobotState::STAND;         matched = true; break;
                        case BtCommand::SIT:          testState = RobotState::SIT;           matched = true; break;
                        case BtCommand::LAY:          testState = RobotState::LAY;           matched = true; break;
                        case BtCommand::SLEEP:        testState = RobotState::SLEEP;         matched = true; break;
                        case BtCommand::WALK_FORWARD: testState = RobotState::WALK_FORWARD;  matched = true; break;
                        case BtCommand::WALK_BACK:    testState = RobotState::WALK_BACKWARD; matched = true; break;
                        case BtCommand::TURN_LEFT:    testState = RobotState::TURN_LEFT;     matched = true; break;
                        case BtCommand::TURN_RIGHT:   testState = RobotState::TURN_RIGHT;    matched = true; break;
                        case BtCommand::STOP:         testState = RobotState::IDLE;          matched = true; break;
                    }

                    if (matched) {
                        Serial.printf("[Serial] Command '%c' → State %d\n",
                                      upper, static_cast<int>(testState));
                        robot.setState(testState);
                    }
                }
                break;
        }
    }

    // Yield to FreeRTOS scheduler — this task is low priority
    vTaskDelay(pdMS_TO_TICKS(50));
}
