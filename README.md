# Heater_control_system
Heater Control System using ESP32 reads temperature via DHT22 and controls a heater (LED) based on thresholds. It uses FreeRTOS tasks for state tracking, visual/auditory alerts, and serial logging. The system simulates heating logic with future IoT scalability.

# Heater Control System using ESP32 & FreeRTOS

## Project Overview
This project implements a basic Heater Control System using an ESP32 microcontroller, a DHT22 temperature sensor, LEDs, and a buzzer. The system turns the heater ON/OFF based on preset temperature thresholds and includes visual and auditory feedback for different heating states.

## System Features
- Temperature-based state machine with 5 states: IDLE, HEATING, STABILIZING, TARGET REACHED, OVERHEAT.
- Heater control simulated via LED.
- Buzzer frequency increases as temperature exceeds the Overheat threshold.
- Visual Indicator (LED) for Target Reached state.
- Serial logging of Heater State & Current Temperature.
- FreeRTOS-based multitasking for concurrent operations.
- (Optional) BLE Advertising of Heater State (for future roadmap).

## Hardware Setup
| Component         | ESP32 Pin |
|-------------------|----------:|
| DHT22 Data         | GPIO15    |
| Heater LED         | GPIO5     |
| Target LED         | GPIO4     |
| Buzzer (Passive)   | GPIO2     |
| Power Supply       | 3.3V      |
| Common Ground      | GND       |

## Simulation
- Simulated on **Wokwi ESP32 Simulator**.
- DHT22 sensor readings are manually adjusted using the simulation slider.
- Buzzer PWM tones can be observed only on physical hardware.

## How It Works
1. The system initializes and reads the temperature from DHT22.
2. Based on temperature, it transitions through HEATING, STABILIZING, TARGET REACHED, and OVERHEAT states.
3. Heater LED is turned ON when heating is required.
4. Buzzer frequency increases dynamically when OVERHEAT is detected.
5. Serial Monitor continuously logs the current state and temperature.

## Future Roadmap
- Integration of I2C Digital Sensors (SHT31) for enhanced accuracy.
- EEPROM-based Heating Profiles for customizable settings.
- BLE-based remote monitoring of Heater State.
- OLED Display for real-time temperature display.
- PID Control Loop for smooth temperature stabilization.

