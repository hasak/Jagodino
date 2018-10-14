# Jagodino

An Arduino-based automatic irrigation system for monitoring soil moisture and water level in a barrel, controlling valves and pumps, and providing feedback on an LCD. Designed to optimize watering for plants while tracking water and energy usage.

## **Features**

* Automatic watering based on soil moisture thresholds and water availability.
* Manual watering and stop via buttons.
* Ultrasonic sensor for water level measurement in the barrel.
* Visual feedback on PCD8544 LCD (Nokia 5110-style) with custom display of numbers and water level.
* LED indicators for:

  * Watering in progress
  * Low water
  * Sensor error
* Persistent logging in EEPROM of:

  * Total water used (liters)
  * Energy consumed (Wh)
  * Total watering time (minutes)
* Safety features:

  * Maximum watering duration
  * Minimum soil moisture check
  * Sensor failure detection

## **Hardware Requirements**

| Component                   | Pin Connection                              |
| --------------------------- | ------------------------------------------- |
| Arduino UNO / compatible    | —                                           |
| Relay (Valve)               | D8                                          |
| Relay (Pump)                | D9                                          |
| Ultrasonic sensor (HC-SR04) | Trig: D10, Echo: D11                        |
| Soil moisture sensors x4    | A0–A3                                       |
| Water level LED             | D0                                          |
| Low water LED               | D1                                          |
| Error LED                   | D2                                          |
| Manual water button         | D12 (pull-up)                               |
| Stop button                 | D13 (pull-up)                               |
| PCD8544 LCD                 | SCLK: D7, DIN: D6, D/C: D5, CS: D4, RST: D3 |

## **Software Setup**

1. Install the following Arduino libraries:

   * `Adafruit_GFX`
   * `Adafruit_PCD8544`
2. Upload the `Jagodino.ino` sketch to the Arduino.
3. Make sure EEPROM memory addresses are not conflicting with other uses:

   * `0–9` → liters
   * `10–19` → energy
   * `20–29` → runtime
4. Adjust soil moisture thresholds (`td`, `punido`, `minni`) to match your plant requirements.
5. Calibrate the ultrasonic sensor for barrel height (`precnikBureta`, `visinaBureta`, `offset`).

## **Operation**

* **Automatic Watering:** Triggers when average soil moisture is below a set threshold and sufficient water is available.
* **Manual Watering:** Press the "water" button to start watering regardless of soil moisture.
* **Stop Watering:** Press the "stop" button to halt watering.
* **Display:**

  * Water level in liters and %
  * Current watering status
  * Soil moisture for each sensor
  * Timer for ongoing watering

## **EEPROM Logging**

* Logs every `freqq` minutes (default: 15 min)
* Tracks:

  * Liters used
  * Energy consumption
  * Total watering time
* Allows cumulative tracking over days or months.

## **Notes**

* **Custom LCD rendering:** Functions `pr()` and `p()` for numeric and percentage display.
* **Real-time sensor monitoring:** Soil moisture and water level updates every loop.
* **Safety logic:** Prevents overwatering and detects sensor errors.
* **Modular design:** Functions for display, sensor reading, and control logic.
