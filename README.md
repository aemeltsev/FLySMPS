![FLYSMPS logo](https://github.com/aemeltsev/FLySMPS/blob/master/img/flogo.png)

## Introduction to FLYSMPS
FLYSMPS(Small-Signal Model and Parameter Simulator for Flyback Switched-Mode Power Supply) is a small specialized software utility designed for calculating electrical parameters, component values, and dynamic properties of asymmetric power converters, with a primary focus on flyback converters in its current version.


## Key Purpose
FLYSMPS simplifies the **design**, **analysis**, and **optimization** of flyback converters by providing:

- **Accurate calculations** of electrical parameters (e.g. input circuit values, power MOSFET properties, power transformer parameters).
- **Small-signal modeling** for both Discontinuous Conduction Mode (DCM) and maybe for Continuous Conduction Mode (CCM) in future
- **Graphical visualization** of key waveforms, control loops, and filter responses.


## Core Feature

### 1. Complex calculation methods
- **Parameters of magnetic components** (for example, values of inductance of the primary side of the power choke using various methods, losses in the core depending on the presence of a gap).
- **Transformer design** (inductance of the windings of the primary and secondary sides, transformation ratio, optimal calculation of the cross-section of the windings).
- **Characteristics of the power stage** (efficiency, permissible load of the power switch by voltage/current).

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

![FLYSMPS screenshot](https://github.com/aemeltsev/FLySMPS/blob/master/img/transf_select.png)


## Roadmap & TODO
| Task                                      | Status          | Priority |
|-------------------------------------------|-----------------|----------|
| Add a database to store parameters        | ⏳ In Progress   | High     |
| Infrastructure for working with the database | ⏳ In Progress   | High     |
| Modal win. for writing/selecting core and geometry | ⏳ In Progress   | High     |
| MVC controller for working with the modal window | ⏳ In Progress   | High     |
| RC-snubber for secondary side             | ❌ Not Started   | High     |
| Tests                                     | ❌ Not Started   | High     |
| Logging                                   | ⏳ In Progress   | High     |
| Stabilization of functionality and preparation for the alpha version | ❌ Not Started   | High     |
| Documentation                             | ❌ Not Started   | Medium   |
| Optimize transformer hot-side algorithm   | ❌ Not Started   | Medium   |
| Optimization of feedback circuit calculations | ❌ Not Started   | Medium   |
| Review data transfer containers           | ❌ Not Started   | Medium   |
| Add forward converter support             | ❌ Not Started   | Low      |
| Two-switch forward/flyback estimation     | ❌ Not Started   | Low      |
| Quasi-resonant topology support           | ❌ Not Started   | Low      |


## Compatibility
| Platform       | Status          | Notes                          |
|----------------|-----------------|--------------------------------|
| **Linux (Ubuntu/Debian)** | ✅ Tested | Requires Qt 5.12+ |
| **Windows 10/11** | ❓ Not tested | Requires Qt 5.12+ |


## How to build
TODO


## Usage at a glance
1. **Input Specifications**:

2. **Run Calculations**:

3. **Visualize Results**:


## License
**GNU GPLv3** – See [LICENSE](LICENSE) for details.
- **Free to use, modify, and distribute**.


## Notes
