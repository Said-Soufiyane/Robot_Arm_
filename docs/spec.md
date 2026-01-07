# Robot Arm Spec (v0.2)

## Purpose
A table-mountable 6-axis desktop robot arm designed to be publishable, reproducible, and upgradeable.
Primary goal is a **real** 2.0 kg payload capability using **COTS metal planetary gearboxes** on the high-torque joints.

## Target Performance
- Axes: 6 (J1–J6)
- Payload: **2.0 kg at tool flange** (static target, arm extended)
- Reach: **TBD** (recommended: 330 mm; acceptable: 300–350 mm)
- Motor rail: **24 V** system recommended
- Repeatability target: **0.5–1.0 mm** (realistic for stepper + gearbox)
- Typical joint speed: 30–60°/s on large joints (not chasing insane speed)

## Drivetrain Strategy
- J2 (shoulder) + J3 (elbow): **COTS metal planetary gearboxes** (primary torque joints)
- J1 (base yaw): planetary or belt reduction (TBD)
- J4–J6 (wrist): smaller planetary gearboxes or belt reductions (TBD)

### Key Mechanical Rule (non-negotiable)
**Gearboxes should not be used as structural bearings.**
Each joint must have its own bearing stack so arm loads go through bearings, not the gearbox output shaft.

## Electronics / Control (current plan)
- Controller: **ESP32** for multi-axis control (ESP8266 used only for single-axis bench bring-up)
- Motor drivers: **current-limited stepper drivers only** for 12–24V operation (TMC2209/TB6600-class)
- Control interface: STEP/DIR per axis, shared EN optional
- Driver configuration: UART optional (used for current setting + tuning where supported)

### Power / Protection (required for 24V)
- VM decoupling: electrolytic capacitor at each driver across VM–GND (50V rated recommended)
- EN default-disable: pull resistor so drivers stay disabled during boot/upload
- Bring-up rule: **VM OFF during MCU upload/reset**, no hot-plug motor wiring

## Materials / Construction
- Printed parts: PC (primary), with heat-set inserts where needed
- Structural links: printed shells OR aluminum tube links (TBD)
- Pins/shafts: metal (e.g., hardened dowels / steel shafts)
- Bearings: real bearings for every joint, sized based on loads

## Safety / Bring-up
- Emergency stop required during testing
- Mechanical hard-stops where practical
- Cable management to avoid snags and pinch points

## Validation / Tests (minimum)
- Single-axis: coil pairing, current set verification, holding torque “breakaway” test, thermal check
- Multi-axis: simultaneous motion test, no missed steps at target speed/accel, repeatability check
- Mechanical: backlash measurement per joint, stiffness check under load

## Deliverables to Publish
- STEP assembly exports
- STLs for printed parts
- BOM (CSV)
- Assembly + calibration docs
- Firmware + wiring notes
- Media: build photos + short test videos