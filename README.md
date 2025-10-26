# CPS511 Assignment 1 - Shooting a Duck ig

## Download

```
git clone --recurse-submodules https://github.com/muntalee/cps511-assignment/
```

## Customization

* By default, the project builds an executable named `game`.
* To change this:

  1. Update the variable `EXE = game` in the `Makefile`.
  2. Change `game` in the `project(game VERSION 1.0)` line in `CMakeLists.txt`.

## Features

* Large Duck
* Rendered Booth with Wave on top
* Duck moving alongside the Wave
* Flipping the target 90 degrees by pressing `F`
* Remove/Reveal base of booth by pressing `Space`
* Ground mesh rendered using VBOs and shaders
* Camera movement
  * Hold Left click for panning
  * Hold Right click for zooming in/out

## Not Included

* Any bonus marks
  * 4x4 model
  * Texture mapping


## Dependencies

* **CMake ≥ 3.16**
* **A C++17 (or newer) compiler**
  * GCC or Clang on Linux
  * Clang (via Xcode tools) on macOS
  * MSVC (Visual Studio) or MinGW on Windows
* **Ninja** (build tool)

## Install

* **Windows**

  * **Option 1: Scoop + MinGW (GCC toolchain) RECOMMENDED**

    * Install [Scoop](https://scoop.sh/) (a Windows package manager).
    * Then all other dependencies

      ```powershell
      scoop install mingw
      scoop install ninja cmake
      ```

  * **Option 2: Visual Studio (MSVC toolchain)**

    * Install [Visual Studio Community](https://visualstudio.microsoft.com/)
      (Choose **Desktop development with C++** workload.)
    * Install [Ninja](https://ninja-build.org/) or with Winget:

      ```powershell
      winget install Ninja-build.Ninja
      ```

* **macOS**

  ```bash
  brew install cmake ninja
  ```

  (OpenGL and Clang come with macOS.)

* **Linux**

  * **Ubuntu / Debian**

    ```bash
    sudo apt update
    sudo apt install build-essential cmake ninja-build libgl1-mesa-dev xorg-dev
    ```

  * **Fedora**

    ```bash
    sudo dnf install gcc-c++ cmake ninja-build mesa-libGL-devel libX11-devel libXrandr-devel libXcursor-devel libXi-devel
    ```

## Build & Run

The `Makefile` comes bundled with preset commands you may use to compile your project.

```bash
make build     # creates build
make compile   # compiles the build
make run       # build and run
make clean     # cleans up build files / executable
```