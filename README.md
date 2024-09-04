# 🎨 Parallel Screensaver with OpenMP

## Table of Contents
- [Overview](#overview)
- [Objectives and Competencies](#objectives-and-competencies)
- [Project Structure](#project-structure)
- [Libraries and Tools](#libraries-and-tools)
- [Installation](#installation)
- [Usage](#usage)
- [Performance Testing](#performance-testing)
- [Screensaver Preview](#screensaver-preview)

## Overview
This project is a parallelized screensaver that utilizes OpenMP to draw graphical elements on the screen. The primary focus is to achieve high performance by maintaining a smooth frame rate (60 FPS or more) while rendering `N` elements in parallel. The project demonstrates significant performance improvements over a sequential approach by leveraging shared memory parallelization.

## Objectives and Competencies
- **Implement and design** a program that parallelizes processes using shared memory with OpenMP.
- **Apply the PCAM method** (Partitioning, Communication, Agglomeration, and Mapping) and concepts of decomposition and programming patterns to modify a sequential program for parallel execution.
- **Iteratively improve** and modify the program to create more efficient versions.

## Project Structure
```sh
  BubbleScreensaver/
  ├── CMakeLists.txt       # CMake configuration file
  ├── main.cpp             # Sequential implementation
  ├── mainParallel.cpp     # Parallel implementation using OpenMP
  ├── image/               # Bubble images to render
  │   └── ...
  └── README.md            # This file
```

## Libraries and Tools
- **OpenGL**: For rendering graphics.
- **SDL2**: Provides cross-platform access to audio, keyboard, mouse, and graphics hardware.
- **SDL2_image**: A companion library to SDL2 for loading images.
- **GLM**: A header-only C++ mathematics library for graphics software.
- **OpenMP**: For parallel programming using shared memory.

## Installation
Follow these steps to get the project running on your local machine:

1. **Clone the repository**:
    ```sh
    git clone https://github.com/Andrea-gt/openmp-screensaver
    cd openmp-screensaver
    ```

2. **Install necessary libraries**:
    Ensure that your system has the required libraries installed:
    - CMake (version 3.5 or later)
    - A C++ compiler
    - OpenGL
    - SDL2 and SDL2_image

3. **Build the project**:
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```

4. **Run the executable**:
    Depending on whether you want to run the sequential or parallel version, execute:
    ```sh
    ./BubbleScreensaver <Number of Bubbles> <Target FPS>           # For the sequential version
    ./BubbleScreensaverParallel <Number of Bubbles> <Target FPS>   # For the parallel version
    ```

## Usage
After building the project, you can run either the sequential or parallel version of the screensaver.

- **Sequential Version**: Runs without any parallelization, useful for comparing performance.
- **Parallel Version**: Utilizes OpenMP to draw graphics in parallel, showcasing performance improvements.

Adjust the number of elements (`N`) to see how well the parallel version scales compared to the sequential version.

## Performance Testing
The performance of the program is measured by the execution time taken to generate `N` elements without dropping below the target FPS. Various values of `N` are tested to demonstrate the improvements achieved through parallelization.

You can perform these tests by modifying the source code or through runtime arguments. The results should highlight the differences between the sequential and parallel implementations, particularly in how they handle increasing workloads.

## Screensaver Preview
![screensaver_preview](https://github.com/Andrea-gt/openmp-screensaver/blob/main/screensaver.png?raw=true)
