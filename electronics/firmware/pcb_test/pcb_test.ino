#include <Arduino.h>
#include <ESP32Servo.h>

// ===== SERVO =====
static const int SERVO_PIN = 21;
Servo gripper;

// ===== 4x TMC2209 (NO UART) =====
struct Motor {
  const char* name;
  int stepPin;
  int dirPin;
  uint16_t stepLowUs;   // lower = faster
};

// Different speeds per motor (tweak if you want)
Motor motors[] = {
  {"TMC1_J1", 23, 22, 3000}, // slow
  {"TMC2_J4", 19, 18, 2000},
  {"TMC3_J5",  5, 17, 1200},
  {"TMC4_J6", 16,  4,  700}, // faster
};

static const int MOTOR_COUNT = sizeof(motors) / sizeof(motors[0]);

static const uint16_t STEP_HIGH_US = 8;
static const uint32_t STEPS_PER_DIR = 1200;
static const uint16_t BETWEEN_MS = 400;

static inline void stepPulse(int stepPin, uint16_t stepLowUs) {
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(STEP_HIGH_US);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(stepLowUs);
}

void wiggleServo() {
  // gentle wiggle so you know it's alive
  gripper.write(70); delay(200);
  gripper.write(110); delay(200);
  gripper.write(90); delay(150);
}

void spinMotor(const Motor &m) {
  Serial.print("Spinning ");
  Serial.print(m.name);
  Serial.print(" stepLowUs=");
  Serial.println(m.stepLowUs);

  wiggleServo();

  // Forward
  digitalWrite(m.dirPin, LOW);
  for (uint32_t i = 0; i < STEPS_PER_DIR; i++) {
    stepPulse(m.stepPin, m.stepLowUs);
  }
  delay(BETWEEN_MS);

  wiggleServo();

  // Reverse
  digitalWrite(m.dirPin, HIGH);
  for (uint32_t i = 0; i < STEPS_PER_DIR; i++) {
    stepPulse(m.stepPin, m.stepLowUs);
  }
  delay(BETWEEN_MS);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  // Motor pins
  for (int i = 0; i < MOTOR_COUNT; i++) {
    pinMode(motors[i].stepPin, OUTPUT);
    pinMode(motors[i].dirPin, OUTPUT);
    digitalWrite(motors[i].stepPin, LOW);
    digitalWrite(motors[i].dirPin, LOW);
  }

  // Servo setup
  gripper.setPeriodHertz(50);
  gripper.attach(SERVO_PIN, 500, 2500);
  gripper.write(90);
  delay(300);

  Serial.println("TMC bring-up: 4 motors different speeds + servo wiggle (GPIO21).");
}

void loop() {
  for (int i = 0; i < MOTOR_COUNT; i++) {
    spinMotor(motors[i]);
    delay(600);
  }
  Serial.println("Cycle done.");
  delay(1500);
}