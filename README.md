# DirectXProject

DirectX renderer developed step by step as a single project. Each stage of the development adds new functionality on top of the previous one.

## Requirements

- Windows 10 or newer
- Visual Studio with the "Desktop development with C++" workload
- C++20 (MSVC)

## Build

1. Open the solution in Visual Studio.
2. Select the `x64` platform and the `Release` configuration.
3. Build with `Ctrl+Shift+B`.

The executable is placed in `x64/Release/`.

## Features

- GPU adapter enumeration through DXGI: vendor, model, device and subsystem IDs, revision, dedicated video memory, dedicated system memory, shared system memory, driver version, LUID, number of outputs

## Usage

The program takes no arguments.

```
DirectXProject.exe
```

## Screenshot

![GPU adapters output](docs/adapters.png)

## Project structure

- `DirectXProject.cpp` - entry point
- `Renderer.h` / `Renderer.cpp` - `Renderer` class, the core of the project

## Development history

Each stage is developed in its own branch based on the previous one.

| Branch | Content |
|---|---|
| `lecture-01` | DXGI adapter enumeration |

## Releases

Prebuilt binaries are available in the Releases section.
