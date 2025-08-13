![FLYSMPS logo](https://github.com/aemeltsev/FLySMPS/blob/master/img/flogo.png)

## Introduction to FLYSMPS
FLYSMPS(Small-Signal Model and Parameter Simulator for Flyback Switched-Mode Power Supply) is a small specialized software utility designed for calculating electrical parameters, component values, and dynamic properties of asymmetric power converters, with a primary focus on flyback converters in its current version.

---

## Key Purpose
FLYSMPS simplifies the **design**, **analysis**, and **optimization** of flyback converters by providing:

- **Accurate calculations** of electrical parameters (e.g. input circuit values, power MOSFET properties, power transformer parameters).
- **Small-signal modeling** for both Continuous Conduction Mode (CCM) and Discontinuous Conduction Mode (DCM).
- **Graphical visualization** of key waveforms, control loops, and filter responses.

---

## Core Feature

### 1. Comprehensive Calculation Methods
- **Magnetic component parameters** (e.g., inductor/capacitor values, core losses).
- **Transformer design** (turns ratio, winding configuration).
- **Power stage characteristics** (efficiency, voltage/current stress).

### 2. Small-Signal Modeling
| Mode               | Features                                                                 |
|--------------------|--------------------------------------------------------------------------|
| **Discontinuous (DCM)** | Small-signal model, compensator tuning, stability analysis (available now). |
| **Continuous (CCM)**   | *(Planned for future updates)*                                           |

- **Control Loop Design**:
  - Compensator tuning (Type I/II/III).
  - Transient response analysis.
- **Graphical Analysis**:
  - **Bode plots** (gain/phase margins).
  - **Transfer functions** (open-loop/closed-loop).

### 3. Graphical User Interface (GUI) Visualization with QCustomPlot
- Built using the **Qt 5 framework**. 
- **Interactive plots** for:
  - **LC filter responses** (frequency and time domain).
  - **Control loop dynamics** (open-loop/closed-loop responses).


![FLYSMPS screenshot](https://github.com/aemeltsev/FLySMPS/blob/master/img/screen_main.png)

![FLYSMPS screenshot](https://github.com/aemeltsev/FLySMPS/blob/master/img/db_bcap.png)

![FLYSMPS screenshot](https://github.com/aemeltsev/FLySMPS/blob/master/img/first_trans.png)

![FLYSMPS screenshot](https://github.com/aemeltsev/FLySMPS/blob/master/img/filter.png)

---

## Roadmap & TODO
| Task                                      | Status          | Priority |
|-------------------------------------------|-----------------|----------|
| Debugging and bug fixing                  | ⏳ In Progress   | High     |
| Unit tests                                | ❌ Not Started   | High     |
| Add version info                          | ❌ Not Started   | Medium   |
| Documentation                             | ⏳ In Progress   | High     |
| Estimate RC-snubber for secondary side    | ⏳ In Progress   | High     |
| Add forward converter support             | ❌ Not Started   | Low      |
| Quasi-resonant topology support           | ❌ Not Started   | Low      |
| Two-switch forward/flyback estimation     | ❌ Not Started   | Medium   |
| Optimize transformer hot-side algorithm   | ⏳ Partial       | High     |
| Optimize power stage/feedback control     | ⏳ Partial       | High     |
| Review data transfer containers           | ❌ Not Started   | Medium   |

---

## Compatibility
| Platform       | Status          | Notes                          |
|----------------|-----------------|--------------------------------|
| **Linux (Ubuntu/Debian)** | ✅ Tested | Install `libqt5charts5-dev`   |
| **Windows 10/11** | ❓ Untested     | Requires Qt 5.15+              |

---

## Installation
TODO

---

## Usage at a glance
1. **Input Specifications**:

2. **Run Calculations**:

3. **Visualize Results**:

---

## License
**GNU GPLv3** – See [LICENSE](LICENSE) for details.
- **Free to use, modify, and distribute**.
- **Contributions welcome!** (Submit a pull request.)

---

## Notes
