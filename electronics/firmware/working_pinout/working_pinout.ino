/* FULL WORKING PINOUT — ESP32 (38-pin)
Driver plan: TB6600 = J2/J3 (NEMA23). TMC2209 = J1/J4/J5/J6 (NEMA17).
UART is ONLY for J1 (TMC #1).

========================
SHARED ENABLE (TMC only)
========================
TMC_EN_BUS -> GPIO21
- Tie ALL TMC2209 EN pins (J1-1 on each TMC) together to GPIO21
- In code: EN LOW = enabled, EN HIGH = disabled (active LOW)
- TB6600 ENA is separate (don’t tie it into this bus)

========================
TB6600 #1  (J2 / NEMA23)
========================
STEP/PUL- -> GPIO25
DIR/DIR-  -> GPIO26
PUL+ & DIR+ -> ESP32 5V (USB 5V)  [common-anode]
(ENA optional: ENA+ -> 5V, ENA- -> GND)

========================
TB6600 #2  (J3 / NEMA23)
========================
STEP/PUL- -> GPIO27
DIR/DIR-  -> GPIO14
PUL+ & DIR+ -> ESP32 5V (USB 5V)  [common-anode]
(ENA optional: ENA+ -> 5V, ENA- -> GND)

========================
TMC2209 #1 (J1 / NEMA17)  [UART-configured]
========================
STEP -> GPIO23
DIR  -> GPIO22
EN   -> GPIO21  (shared EN bus)

UART (PDN_UART pin on the driver, J1-4 OR J1-5):
  RX -> GPIO4
  TX -> GPIO5   (1k series resistor on TX recommended)

========================
TMC2209 #2 (J4 / NEMA17)
========================
STEP -> GPIO19
DIR  -> GPIO18
EN   -> GPIO21  (shared EN bus)
PDN_UART not connected (optional later)

========================
TMC2209 #3 (J5 / NEMA17)
========================
STEP -> GPIO17
DIR  -> GPIO16
EN   -> GPIO21  (shared EN bus)
PDN_UART not connected (optional later)

========================
TMC2209 #4 (J6 / NEMA17)
========================
STEP -> GPIO33
DIR  -> GPIO32
EN   -> GPIO21  (shared EN bus)
PDN_UART not connected (optional later)

========================
SERVO (gripper)
========================
Signal -> GPIO13
Power  -> external +5V brick (NOT ESP32 5V)
GND    -> external GND tied to ESP32 GND

========================
LIMIT SWITCHES (one per axis)
Wire each switch between GPIO and GND, use INPUT_PULLUP
(Pressed = LOW)
========================
J1 limit -> GPIO34
J2 limit -> GPIO35
J3 limit -> GPIO36
J4 limit -> GPIO39
J5 limit -> GPIO12
J6 limit -> GPIO15*/

#include <Arduino.h>

// No-UART STEP/DIR test for TMC2209
// STEP -> GPIO23, DIR -> GPIO22

const int STEP_PIN = 23;
const int DIR_PIN  = 22;

void setup() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
  delay(500);
}

void stepOnce() {
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(STEP_PIN, LOW);
}

void runSteps(uint32_t steps, uint16_t stepDelayMs) {
  for (uint32_t i = 0; i < steps; i++) {
    stepOnce();
    delay(stepDelayMs); // super slow on purpose
  }
}

void loop() {
  // Forward crawl
  digitalWrite(DIR_PIN, LOW);
  runSteps(400, 10);   // 100 steps/sec (very slow)
  delay(500);

  // Reverse crawl
  digitalWrite(DIR_PIN, HIGH);
  runSteps(400, 10);
  delay(1200);
}