# SPH Fluid Simulation
A 2D fluid simulation built from scratch using C++ and SFML, based on the Smoothed-Particle Hydrodynamics (SPH) method.

<img width="512" height="288" alt="fluid_sim_v1_finished" src="https://github.com/user-attachments/assets/382d0800-48db-489b-9258-7d767ac66311" />

*Demo of the fluid dynamics on action.*

## How it works
The physics engine drives the fluid behavior using **SPH**, calculationg density, pressure and viscosity forces for each particle. For movement, it currently uses **Euler integration** to update particle velocities and positions frame-by-frame, with SFML handling the real-time graphical rendering.

## Project Structure
Currently, the simulation logic is contained within a single file to establish the base physics, but the repository is set up with CMake for modular expansion:

```text
.
├── CMakeLists.txt
├── include/          # Future work: Headers for modular physics and rendering classes
└── src/
    └── main.cpp      # Current core implementation (SPH logic + SFML game loop)
``` 

# Prerequisites
To build and run this project, you will need the following instaled on your system:
- C++ Compiler (`C++17 or higher recommended`)
- CMake (`>= 3.10`)
- SFML(`libsfml-dev`)

# Building and running
This project uses Cmake for compilation. To build and run:
```bash
 mkdir build
 cd build
 cmake ..
 make
 ./fluid_sim
```

# Future work
- [] **Code Refactoring**: Extract the particle system, grid/boundary logic, and physics calculations from `main.cpp` into dedicated headers inside the `include/` directory.
- [] **Optimization**: Implement a spatial hasing grid to optmize the neighbor-search algorithm (currently the biggest bottleneck in raw SPH).
- [] **Physics Upgrades**: Explore more stable integration methods (like Semi-Implicit Euler or Verlet) for better high velocity stability.
