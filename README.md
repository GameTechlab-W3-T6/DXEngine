# DXEngine

A lightweight 3D game engine built with DirectX 11 and C++, featuring an integrated editor with interactive gizmos for object manipulation.

## Architecture

The engine follows a modular subsystem architecture:

- **Core Systems**: Object system, reflection, serialization
- **Rendering**: DirectX 11 renderer with shader management
- **Scene Management**: Component-based scene graph
- **Input/UI**: Input handling and ImGui integration
- **Editor Tools**: Interactive gizmos for object transformation

For detailed architecture documentation, see [architecture.md](architecture.md).

## Prerequisites

- Windows 10/11
- Visual Studio 2019 or later
- DirectX 11 SDK (included with Windows SDK)

## Building

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/DXEngine.git
   cd DXEngine
   ```

2. Open `Engine.sln` in Visual Studio

3. Build the solution (Ctrl+Shift+B)

4. Run the project (F5)

## Usage

### Basic Scene Setup

```cpp
// Create an editor application
EditorApplication app;

// Initialize with window settings
app.Initialize(hInstance, L"DXEngine Editor", 1280, 720);

// Run the main loop
app.Run(true); // Enable shader reflection
```

### Creating Objects

```cpp
// Create primitive components
auto cube = new UCubeComp(FVector(0, 0, 0));
auto sphere = new USphereComp(FVector(2, 0, 0));

// Add to scene
scene->AddObject(cube);
scene->AddObject(sphere);
```

### Gizmo Manipulation

- **Left Mouse**: Select objects
- **Right Mouse + Drag**: Camera look around
- **W/A/S/D**: Camera movement
- **Q/E**: Move up/down
- **T**: Toggle between Location/Rotation/Scale gizmos
- **Spacebar**: Toggle world/local space

## Project Structure

```
DXEngine/
├── Engine/                 # Core engine source
│   ├── Core/              # Object system, reflection
│   ├── Rendering/         # DirectX 11 renderer
│   ├── Scene/             # Scene management
│   ├── Input/             # Input handling
│   ├── Math/              # 3D math library
│   ├── Editor/            # Editor functionality
│   └── ImGui/             # UI library
├── Shaders/               # HLSL shader files
└── Data/                  # Assets and scenes
```

## Key Components

### Core Classes

- `UObject` - Base class for all engine objects
- `UEngineSubsystem` - Base for engine subsystems
- `USceneComponent` - Base for scene objects with transforms
- `UPrimitiveComponent` - Renderable components with meshes

### Subsystems

- `URenderer` - DirectX 11 rendering management
- `USceneManager` - Scene loading/saving
- `UInputManager` - Input handling with callbacks
- `UMeshManager` - Mesh resource management
- `UGizmoManager` - Editor gizmo system

## Dependencies

- **DirectX 11** - Graphics API
- **Dear ImGui** - Immediate mode GUI
- **nlohmann/json** - JSON parsing and serialization