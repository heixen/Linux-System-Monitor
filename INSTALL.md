# Installation

## Recommended: Nix Flakes

This project provides a `flake.nix` with all required dependencies.

You **do not need NixOS** to use it. Nix can be installed on other Linux distributions as well.

Install Nix:

https://nixos.org/download/

Then clone the project:

```bash
git clone https://github.com/heixen/Linux-System-Monitor
cd linux-resource-monitor
```

Enter the development environment:

```bash
nix develop
```

Build and run:

```bash
./run.sh
```

The Nix environment provides the required compiler, CMake, SDL2, OpenGL, X11 libraries, and development tools.

## Without Nix

If you don't want to use Nix, you can install the required dependencies manually using your distribution's package manager.

You will need:

- GCC
- CMake
- Ninja
- pkg-config
- SDL2
- OpenGL / Mesa
- X11 libraries

Then build and run:

```bash
./run.sh
```

Or manually:

```bash
mkdir -p build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/linux-resource-monitor
```
