// // //constant stream of step pulses
// #include <TMCStepper.h>

// #define STEP_PIN 14   // GPIO14
// #define DIR_PIN  12   // GPIO12
// #define EN_PIN   13   // GPIO13

// #define SERIAL_PORT Serial
// #define R_SENSE 0.11f   // Correct for Teyleten Robot TMC2209

// TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, 0b00);

// void setup() {
//   SERIAL_PORT.begin(115200);

//   pinMode(STEP_PIN, OUTPUT);
//   pinMode(DIR_PIN, OUTPUT);
//   pinMode(EN_PIN, OUTPUT);

//   digitalWrite(EN_PIN, LOW); // Enable driver

//   driver.begin();
//   driver.rms_current(1200);      // SET CURRENT HERE (1.2A RMS)
//   driver.microsteps(16);
//   driver.en_spreadCycle(false);  // StealthChop ON
// }

// void loop() {
//   digitalWrite(STEP_PIN, HIGH);
//   delayMicroseconds(800);
//   digitalWrite(STEP_PIN, LOW);
//   delayMicroseconds(800);
// }

// Code for HOLDING torque test
// #include <TMCStepper.h>

// #define STEP_PIN 14   // GPIO14
// #define DIR_PIN  12   // GPIO12
// #define EN_PIN   13   // GPIO13

// #define SERIAL_PORT Serial
// #define R_SENSE 0.11f

// TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, 0b00);

// void setup() {
//   SERIAL_PORT.begin(115200);

//   pinMode(STEP_PIN, OUTPUT);
//   pinMode(DIR_PIN, OUTPUT);
//   pinMode(EN_PIN, OUTPUT);

//   digitalWrite(DIR_PIN, LOW);
//   digitalWrite(EN_PIN, LOW); // enable 

//   driver.begin();
//   driver.pdn_disable(true);
//   driver.I_scale_analog(false);

//   driver.toff(4);                // 
//   driver.en_spreadCycle(true);   // max torque mode 
//   driver.microsteps(1);          // full step for max holding torque
//   driver.rms_current(1200);      // change this for your test points
// }

// void loop() {

// }
/*
  ESP8266 + TMC2209 (StepStick) — 24V, 2.0A RMS torque test
    STEP = GPIO14
    DIR  = GPIO12
    EN   = GPIO13 (active LOW on most boards)

  UART pins for PDN_UART (recommended and boot-safe):
    RX = GPIO4
    TX = GPIO5  (put 1k resistor in series from TX -> PDN_UART)

  Notes:
  - 24V is your motor supply (VM).
  - Current (2.0A RMS) .
  - SpreadCycle + full-step = max torque/holding.
*/

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TMCStepper.h>


#define STEP_PIN 14   // GPIO14
#define DIR_PIN  12   // GPIO12
#define EN_PIN   13   // GPIO13

// --- UART (PDN_UART) pins ---
#define UART_RX  4    // GPIO4  (D2)
#define UART_TX  5    // GPIO5  (D1)  -> add 1k series resistor to PDN_UART

// --- Driver config ---
#define R_SENSE 0.11f
#define DRIVER_ADDR 0b00

SoftwareSerial TMCSerial(UART_RX, UART_TX);  // RX, TX
TMC2209Stepper driver(&TMCSerial, R_SENSE, DRIVER_ADDR);

// --- Torque settings ---
const int RMS_mA = 2000;      // 2.0A RMS (aggressive; requires heatsink+fan)
const bool SPIN_MODE = true;  // true = rotate slowly, false = hold only
int stepDelayUs = 2000;       // increase (2000-4000) if it stalls under load

static inline void stepPulse() {
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(stepDelayUs);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(stepDelayUs);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);

  // Start disabled (safer during boot)
  digitalWrite(EN_PIN, HIGH);     // disable (active LOW)
  digitalWrite(DIR_PIN, LOW);
  delay(50);

  // Start UART to driver
  TMCSerial.begin(115200);

  // Init + configure driver
  driver.begin();
  driver.pdn_disable(true);       // PDN_UART pin used for UART
  driver.I_scale_analog(false);   // use UART current control

  driver.toff(4);                 // REQUIRED: enable power stage
  driver.blank_time(24);

  driver.en_spreadCycle(true);    // torque/accuracy mode
  driver.microsteps(1);           // full-step for max torque
  driver.rms_current(RMS_mA);     // ⭐ max torque current
  driver.pwm_autoscale(true);

  // Enable after configuration
  digitalWrite(EN_PIN, LOW);      // enable (active LOW)

  Serial.println("TMC2209 configured: 2.0A RMS, SpreadCycle, full-step");
}

void loop() {
  if (!SPIN_MODE) {
    // HOLD torque test
    delay(1000);
    return;
  }

  // MOVING torque test (slow rotation)
  stepPulse();
}

