# Neutron Cascade

Neutron Cascade is a C++20 real-time simulation and visualizer of an induced nuclear fission chain reaction:

$$\text{n} + \text{U-235} \longrightarrow [\text{U-236}^*] \longrightarrow \text{Ba-141} + \text{Kr-92} + 3\text{n} + Q\ (200\text{ MeV})$$

This project focuses on:
- Induced nuclear fission physics model
- Cartesian crystal lattice layout for $\text{U-235}$ nuclei
- Staged visual effect pipeline (absorption, excited compound nucleus, fragment separation)
- Deterministic C++20 simulation core with seedable RNG (`std::mt19937_64`)
- Modular architecture with clear separation between simulation, rendering, effects, and input
- Automated test suite, benchmark tool, and CSV log analysis workflow

---

## Demo
![Simulation GIF](docs/demo.gif)

---

## Core Features
- **Nuclear Fission Model**: Simulates neutron absorption into $^{235}\text{U}$, producing an excited compound nucleus $^{236}\text{U}^*$, splitting into fission fragments with $3$ secondary neutrons and $Q = 200\text{ MeV}$ energy release per fission.
- **Cartesian Crystal Lattice**: Configurable $\text{U-235}$ nuclei grid (`u235_radius`, `u235_gap`) with non-overlapping spacing and a seed nucleus at the center for reliable initial interaction.
- **Staged Visual Effects**: Two-phase rendering animation for fission events:
  - **Phase 1**: Neutron absorption & excited compound nucleus glow.
  - **Phase 2**: Separation into fission fragments ($\text{Ba-141}$ & $\text{Kr-92}$) with isotropic emission of secondary neutrons.
- **Deterministic Simulation Core**: Double-buffer particle update pipeline (`current` $\rightarrow$ `next` $\rightarrow$ `swap`) driven by `std::mt19937_64`.
- **Modular Design**: Clean decoupling between physics simulation, particle rendering, input handling, and logging.
- **Logging & Analytics**: Timestamped CSV logging for post-run analysis with Python plotting scripts (`matplotlib`).
- **Safety Guards**: Population cap (`max_population`) to maintain memory and frame stability during exponential chain reactions.

---

## Architecture
### Separation of Responsibilities
- `Simulation` (`include/simulation.hpp`, `src/simulation.cpp`):
  - Particle movement & boundary checking
  - Cartesian lattice grid initialization & collision/fission decision logic
  - Population, peak population, and total energy ($Q$) statistics tracking
- `FissionEffect` (`include/fission_effect.hpp`, `src/fission_effect.cpp`):
  - Life cycle and two-stage animation state for fission visual effects
- `Renderer` (`include/renderer.hpp`, `src/renderer.cpp`):
  - Raylib rendering for arena bounds, equation banner, $\text{U-235}$ lattice sites, fission effects, neutrons, and HUD
- `Input` (`include/input.hpp`, `src/input.cpp`):
  - Keyboard shortcut handling and speed controls
- `SimulationLogger` (`include/simulation_logger.hpp`, `src/simulation_logger.cpp`):
  - Timestamped CSV output in `logs/`
- `Main` (`src/main.cpp`):
  - Orchestrates simulation ticks, rendering pipeline, and logging

---

## Build Requirements

- CMake 3.20+
- C++20 compiler (GCC 10+, Clang 12+, MSVC 2019+)
- Ninja
- Python 3 with `matplotlib` (optional, for plotting script)

raylib is fetched automatically by CMake (`FetchContent`).

---

## Build and Run

### Linux
```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/neutron_visualizer
```

### Windows (PowerShell)
```bash
cmake -S . -B build -G Ninja
cmake --build build
.\build\neutron_visualizer.exe
```

## Controls
- `Space`: Pause / Resume simulation
- `R`: Reset simulation and re-initialize $\text{U-235}$ lattice
- `Up` / `Down`: Increase / Decrease simulation speed multiplier
- `D`: Toggle neutron direction vectors
- `G`: Toggle generation debug labels
- `H`: Toggle help overlay panel

## Tests
Run the automated test suite:
```bash
ctest --test-dir build --output-on-failure
```
Current unit tests:
- `reproducibility`: Verifies bit-identical simulation runs given the same RNG seed.
- `population_cap`: Verifies safety bounds on maximum particle count.
- `boundary_escape`: Verifies particle removal upon leaving domain boundaries.
- `reset_behavior`: Verifies complete state restoration upon reset.

## Benchmark
Build and run the performance benchmark:
```bash
cmake --build build --target benchmark_simulation
./build/benchmark_simulation
```

## Plot CSV Logs
Install plotting dependencies:
```bash
python3 -m pip install --user matplotlib
```

Plot the latest CSV log:
```bash
python3 scripts/plot_population.py
```

Plot a specific CSV file:
```bash
python3 scripts/plot_population.py --file logs/<file>.csv
```
The generated chart is saved to `docs/population_plot.png`.

## License
This project is licensed under the MIT License.
See [LICENSE](LICENSE) for details.