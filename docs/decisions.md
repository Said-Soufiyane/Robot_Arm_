# Decisions (ADRs)
- ADR: Use current-limited stepper drivers for 12–24V operation (TMC2209/TB6600-class).  
  Reason: DC H-bridges (TB6612/L293/DRV8871) don’t current-limit steppers → high failure risk at 24V.
- ADR: VM OFF during MCU upload/reset; driver disabled by default.  
  Implementation: EN pull-up (10k to 3.3V), plus VM decoupling cap at driver.
- ADR: Single-axis bench bring-up on spare ESP8266; multi-axis control will be ESP32.  
  Reason: ESP8266 GPIO/timing limits for 6 drivers + servo + fan + sensors.
- ADR: Torque-focused settings for testing: SpreadCycle + full-step when validating max torque; microstepping reserved for smooth motion later.
- ADR: Add VM decoupling per driver (electrolytic near MOT VCC/GND).  
  Reason: reduces spikes/dips and improves stability at 24V.