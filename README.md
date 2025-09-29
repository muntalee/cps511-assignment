# Simple C/C++ Project Setup

This is a minimal cross-platform C++ project that currently uses **GLFW** and **OpenGL**, built with **CMake**.

A `Makefile` is included to make building and running the project consistent across Linux, macOS, and Windows.

## Dependencies

* **CMake ≥ 3.16**
* **A C++17 (or newer) compiler**
  * GCC or Clang on Linux
  * Clang (via Xcode tools) on macOS
  * MSVC (Visual Studio) or MinGW on Windows
* **Ninja** (build tool)

## Download

```
git clone --recurse-submodules https://github.com/muntalee/sample-c-cpp-project
```

## Install

* **Ubuntu / Debian**

  ```bash
  sudo apt update
  sudo apt install build-essential cmake ninja-build libgl1-mesa-dev xorg-dev
  ```

* **Fedora**

  ```bash
  sudo dnf install gcc-c++ cmake ninja-build mesa-libGL-devel libX11-devel libXrandr-devel libXcursor-devel libXi-devel
  ```

* **macOS**

  ```bash
  brew install cmake ninja
  ```

  (OpenGL and Clang come with macOS.)

* **Windows**

  * **Option 1: Visual Studio (MSVC toolchain)**

    * Install [Visual Studio Community](https://visualstudio.microsoft.com/)
      (Choose **Desktop development with C++** workload.)
    * Install [Ninja](https://ninja-build.org/) or with Winget:

      ```powershell
      winget install Ninja-build.Ninja
      ```

  * **Option 2: Scoop + MinGW (GCC toolchain)**

    * Install [Scoop](https://scoop.sh/) (a Windows package manager).
    * Then all other dependencies

      ```powershell
      scoop install mingw
      scoop install ninja cmake
      ```

## Build & Run

The `Makefile` comes bundled with preset commands you may use to compile your project.

```bash
make build     # creates build
make compile   # compiles the build
make run       # build and run
make clean     # cleans up build files / executable
```

## Customization

* By default, the project builds an executable named `game`.
* To change this:

  1. Update the variable `EXE = game` in the `Makefile`.
  2. Change `game` in the `project(game VERSION 1.0)` line in `CMakeLists.txt`.

## Adding more libraries

### Option 1: Drop-in

* Place the library source under `extern/`
* In `CMakeLists.txt`:

  ```cmake
  add_subdirectory(extern/mylib)
  target_link_libraries(${OUT} PRIVATE mylib)
  target_include_directories(${OUT} PRIVATE extern/mylib/include)
  ```

### Option 2: Git Submodules

If the library is hosted on GitHub or elsewhere, you can add it as a `submodule`:

```bash
git submodule add https://github.com/some/library.git extern/library
git submodule update --init --recursive
```

Then link it in `CMakeLists.txt`:

```cmake
add_subdirectory(extern/library)
target_link_libraries(${OUT} PRIVATE library)
```

When cloning this repo later, don’t forget to pull submodules too:

```bash
git clone --recurse-submodules https://github.com/your/repo.git
```

Or if you forgot:

```bash
git submodule update --init --recursive
```
