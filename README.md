# Robot Arm (ESP32) — 6-Axis Desktop Arm (In Progress)
![Robot Arm cover](media/photos/cover.jpeg)

A table-mountable 6-axis desktop robot arm project focused on being **publishable, reproducible, and upgradeable**.  
Target is a **real 2.0 kg payload** using **COTS metal planetary gearboxes** on the high-torque joints, with careful bearing design so gearboxes are not used as structural supports.

---

## Status
- Parts: most items arrived; **waiting on gearboxes + NEMA23-related hardware**
- Current work: stepper driver bring-up and bench testing (TMC2209 + NEMA17)
- Controller: single-axis tests done on ESP8266; multi-axis control will be **ESP32**

See: `LOG.md`

---

## Goals (v0.2)
- **Axes:** 6 (J1–J6)
- **Payload:** 2.0 kg at tool flange (static target, arm extended)
- **Reach:** TBD (goal ~330 mm; acceptable 300–350 mm)
- **Motor rail:** 24V recommended
- **Repeatability:** 0.5–1.0 mm (realistic for stepper + gearbox)
- **Joint speed:** ~30–60°/s on large joints

---

## Architecture Overview

### Drivetrain plan
- J2 (shoulder) + J3 (elbow): COTS metal planetary gearboxes (primary torque joints)
- J1 (base): planetary or belt reduction (TBD)
- J4–J6 (wrist): smaller planetary gearboxes or belt reductions (TBD)

**Mechanical rule (non-negotiable):** Gearboxes are not structural bearings.  
Each joint uses a bearing stack so loads go through bearings, not gearbox shafts.

### Electronics plan
- Controller: **ESP32** (multi-axis)
- Drivers: current-limited stepper drivers (TMC2209/TB6600-class)
- Control: STEP/DIR per axis (shared EN optional)
- Optional: UART for TMC2209 current/tuning

### Power plan
- 24V motor rail for steppers
- Buck converters for 12V (PC fan) and 5V (servo)
- Single ground reference (common ground bus)

---

## Repository Layout
- `LOG.md` — engineering log (daily progress, issues, next steps)
- `specs/` — project specs (payload/reach/power targets, constraints)
- `docs/` — assembly notes, wiring notes, test plans
- `firmware/` — controller firmware (ESP32 focus)
- `cad/` — CAD exports / notes (STEPs, screenshots, etc.)
- `media/`
  - `photos/` — build photos
  - `videos/` — short test clips
  - `renders/` — CAD renders
- `tools/` — bring-up sketches and helper scripts/templates

---

## Bring-Up Notes (read before powering anything)
**Hard rules that came from bench testing:**
1. Use **current-limited stepper drivers** for 12–24V operation (no DC H-bridges for steppers at 24V).
2. Add **VM decoupling** at each driver (electrolytic across VM–GND near the driver; 50V rated recommended).
3. Keep drivers **disabled by default** during boot/upload (EN pull resistor).
4. **VM OFF during MCU upload/reset.**
5. Never hot-plug motor leads.

---

## Media
Test photos and clips live in `media/` and are referenced from `LOG.md`.

---

## Roadmap (high level)
- Finish single-axis validation: current set, thermal behavior, holding torque breakaway test
- Move to ESP32 and validate 2–3 axes together
- Assemble one full joint stack (gearbox + bearings + structure) and measure backlash/stiffness
- Scale to 6 axes, add homing, then repeatability testing
- Publish assembly + wiring + firmware documentation

---

## License
TBD
