# DXEngine Architecture

This document provides a comprehensive overview of the DXEngine architecture, designed to help developers understand the engine's structure and contribute effectively.

## Table of Contents

- [Overview](#overview)
- [Core Architecture](#core-architecture)
- [Subsystem Architecture](#subsystem-architecture)
- [Object System](#object-system)
- [Component System](#component-system)
- [Rendering Pipeline](#rendering-pipeline)
- [Scene Management](#scene-management)
- [Input System](#input-system)
- [Editor System](#editor-system)
- [Memory Management](#memory-management)
- [Build System](#build-system)

## Overview

DXEngine is a component-based 3D game engine built with DirectX 11 and C++. The engine follows modern game engine design patterns with a clear separation of concerns through a subsystem architecture.

### Key Design Principles

- **Modularity**: Each system is self-contained and loosely coupled
- **Performance**: DirectX 11 hardware acceleration with efficient resource management
- **Extensibility**: Component-based architecture allows easy feature additions
- **Developer Experience**: Integrated editor with real-time manipulation tools

## Core Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                        │
│  ┌─────────────────────┐  ┌─────────────────────────────────┐ │
│  │   UApplication      │  │     EditorApplication           │ │
│  │   (Base Runtime)    │  │     (Editor Extensions)         │ │
│  └─────────────────────┘  └─────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
                                  │
┌─────────────────────────────────────────────────────────────┐
│                   Subsystem Layer                           │
│ ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌─────────────────┐ │
│ │URenderer │ │USceneMgr │ │UInputMgr │ │ UGizmoManager   │ │
│ └──────────┘ └──────────┘ └──────────┘ └─────────────────┘ │
│ ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌─────────────────┐ │
│ │UMeshMgr  │ │UTimeMgr  │ │   UGUI   │ │URaycastManager  │ │
│ └──────────┘ └──────────┘ └──────────┘ └─────────────────┘ │
└─────────────────────────────────────────────────────────────┘
                                  │
┌─────────────────────────────────────────────────────────────┐
│                    Object Layer                             │
│ ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌─────────────────┐ │
│ │ UObject  │ │ UScene   │ │ UCamera  │ │   UMesh         │ │
│ └──────────┘ └──────────┘ └──────────┘ └─────────────────┘ │
│ ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌─────────────────┐ │
│ │USceneComp│ │UPrimitive│ │UGizmoComp│ │   UClass        │ │
│ └──────────┘ └──────────┘ └──────────┘ └─────────────────┘ │
└─────────────────────────────────────────────────────────────┘
                                  │
┌─────────────────────────────────────────────────────────────┐
│                   Foundation Layer                          │
│ ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌─────────────────┐ │
│ │ FVector  │ │ FMatrix  │ │FQuaternion│ │   TArray<T>     │ │
│ └──────────┘ └──────────┘ └──────────┘ └─────────────────┘ │
│ ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌─────────────────┐ │
│ │  TMap<T> │ │FDynBitset│ │   JSON   │ │ ISerializable   │ │
│ └──────────┘ └──────────┘ └──────────┘ └─────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

## Subsystem Architecture

All major engine systems inherit from `UEngineSubsystem`, providing a consistent lifecycle and initialization pattern.

### Subsystem Lifecycle

```cpp
class UEngineSubsystem : public UObject {
public:
    virtual bool Initialize() { return true; }
    virtual void Shutdown() {}
};
```

### Core Subsystems

#### URenderer
- **Purpose**: DirectX 11 rendering pipeline management
- **Responsibilities**:
  - Device and context management
  - Shader compilation and binding
  - Resource creation (buffers, textures)
  - Draw call execution
  - Viewport and render target management

#### USceneManager
- **Purpose**: Scene lifecycle and serialization
- **Responsibilities**:
  - Scene loading/saving from JSON
  - Current scene management
  - Scene transition handling

#### UInputManager
- **Purpose**: Input event processing and distribution
- **Responsibilities**:
  - Keyboard/mouse state tracking
  - Input event callbacks
  - Mouse look functionality
  - Per-frame input state updates

#### UMeshManager
- **Purpose**: Mesh resource management
- **Responsibilities**:
  - Mesh creation from vertex data
  - Mesh caching and retrieval
  - GPU buffer management

#### UTimeManager
- **Purpose**: Frame timing and delta time calculation
- **Responsibilities**:
  - High-precision timing using QueryPerformanceCounter
  - Frame rate limiting
  - Delta time calculation

#### UGizmoManager
- **Purpose**: Editor gizmo system
- **Responsibilities**:
  - Gizmo component management
  - Mouse ray intersection testing
  - Transform manipulation logic
  - World/local space switching

## Object System

The engine uses a custom object system based on `UObject` with reflection capabilities.

### UObject Features

```cpp
class UObject : public ISerializable {
private:
    static TArray<UObject*> GUObjectArray;  // Global object tracking
    uint32 UUID;                            // Unique identifier
    uint32 InternalIndex;                   // Array index for fast lookup

public:
    // Type checking and casting
    template<typename T> bool IsA() const;
    template<typename T> T* Cast();

    // Serialization
    virtual json::JSON Serialize() const override;
    virtual bool Deserialize(const json::JSON& data) override;
};
```

### Reflection System

The reflection system is built around `UClass`:

```cpp
class UClass : public UObject {
private:
    static TArray<TUniquePtr<UClass>> classList;
    TFunction<UObject*()> createFunction;
    FDynamicBitset typeBitset;  // For fast type checking

public:
    static UClass* RegisterToFactory(const FString& typeName,
                                   const TFunction<UObject*()>& createFunction,
                                   const FString& superClassTypeName);

    bool IsChildOrSelfOf(UClass* baseClass) const;
    UObject* CreateDefaultObject() const;
};
```

### Class Registration Macros

```cpp
#define DECLARE_UCLASS(ClassName, SuperClass) \
    static UClass* StaticClass(); \
    virtual UClass* GetClass() const override;

#define IMPLEMENT_UCLASS(ClassName, SuperClass) \
    UClass* ClassName::StaticClass() { \
        static UClass* cls = UClass::RegisterToFactory(#ClassName, \
            []() -> UObject* { return new ClassName(); }, #SuperClass); \
        return cls; \
    }
```

## Component System

The engine uses a hierarchical component system:

```
UObject
├── USceneComponent           // Base for objects with transforms
│   ├── UPrimitiveComponent   // Renderable components
│   │   ├── UCubeComp
│   │   ├── USphereComp
│   │   └── UPlaneComp
│   └── UGizmoComponent      // Editor gizmos
│       ├── UGizmoArrowComp
│       ├── UGizmoGridComp
│       ├── UGizmoRotationHandleComp
│       └── UGizmoScaleHandleComp
└── UScene                   // Scene container
```

### Component Transform System

```cpp
class USceneComponent : public UObject {
public:
    FVector RelativeLocation;
    FVector RelativeScale3D;
    FQuaternion RelativeQuaternion;

    virtual FMatrix GetWorldTransform();

    // Transform manipulation
    void SetPosition(const FVector& pos);
    void SetRotation(const FVector& rot);
    void SetScale(const FVector& scl);
};
```

## Rendering Pipeline

The rendering pipeline is built on DirectX 11 with a forward rendering approach.

### Render Loop

```cpp
void URenderer::Render() {
    // 1. Clear render targets
    Clear(0.0f, 0.0f, 0.0f, 1.0f);

    // 2. Prepare shaders and states
    PrepareShader(bShaderReflectionEnabled);

    // 3. Set view-projection matrices
    SetViewProj(viewMatrix, projectionMatrix);

    // 4. Render all primitive components
    for (auto& primitive : primitives) {
        // Update model matrix and constants
        SetModel(primitive->GetWorldTransform(),
                 primitive->GetColor(),
                 primitive->IsSelected());

        // Draw the mesh
        DrawMesh(primitive->GetMesh());
    }

    // 5. Present frame
    SwapBuffer();
}
```

### Shader System

```cpp
struct CBTransform {
    float MVP[16];      // Model-View-Projection matrix
    float MeshColor[4]; // Object color
    float IsSelected;   // Selection state
    float padding[3];   // GPU alignment
};
```

### Vertex Layout

```cpp
struct FVertexPosColor {
    FVector Position;
    FVector4 Color;

    static D3D11_INPUT_ELEMENT_DESC GetLayout();
};
```

## Scene Management

### Scene Structure

```cpp
class UScene : public UObject {
private:
    TArray<USceneComponent*> objects;  // All scene objects
    UCamera* camera;                   // Scene camera
    URenderer* renderer;               // Rendering context

public:
    void AddObject(USceneComponent* obj);
    void Update(float deltaTime);
    void Render(bool bShaderReflectionEnabled);

    // Serialization
    json::JSON Serialize() const override;
    bool Deserialize(const json::JSON& data) override;
};
```

### Scene Serialization

Scenes are saved in JSON format:

```json
{
    "version": 1,
    "objects": [
        {
            "type": "UCubeComp",
            "position": [0, 0, 0],
            "rotation": [0, 0, 0],
            "scale": [1, 1, 1],
            "color": [1, 1, 1, 1]
        }
    ],
    "camera": {
        "position": [0, 0, -5],
        "target": [0, 0, 0]
    }
}
```

## Input System

### Input Processing

```cpp
class UInputManager : public UEngineSubsystem {
private:
    bool keyStates[256];
    bool prevKeyStates[256];
    bool mouseButtons[3];
    int32 mouseX, mouseY;
    int32 mouseDeltaX, mouseDeltaY;

public:
    // State queries
    bool IsKeyDown(int32 keyCode) const;
    bool IsKeyPressed(int32 keyCode) const;
    bool IsMouseButtonDown(int32 button) const;

    // Callback system
    using KeyCallback = TFunction<void(int32)>;
    using MouseCallback = TFunction<void(int32, int32, int32)>;

    void RegisterKeyCallback(const FString& id, KeyCallback callback);
    void RegisterMouseCallback(const FString& id, MouseCallback callback);
};
```

### Mouse Look System

The engine supports mouse look for camera control:

```cpp
void UInputManager::BeginMouseLook() {
    mouseLook = true;
    // Center cursor and capture mouse
}

void UInputManager::ConsumeMouseDelta(float& dx, float& dy) {
    dx = accumDX;
    dy = accumDY;
    accumDX = accumDY = 0.0f;  // Reset for next frame
}
```

## Editor System

### Gizmo Architecture

```cpp
class UGizmoManager : public UEngineSubsystem {
private:
    ETranslationType translationType;  // Location/Rotation/Scale
    bool isWorldSpace;                 // World/Local space
    bool isDragging;                   // Drag state
    EAxis selectedAxis;                // Current manipulation axis

    TArray<UGizmoComponent*> locationGizmos;
    TArray<UGizmoComponent*> rotationGizmos;
    TArray<UGizmoComponent*> scaleGizmos;

public:
    void SetTarget(UPrimitiveComponent* target);
    void BeginDrag(const FRay& mouseRay, EAxis axis, FVector impactPoint);
    void UpdateDrag(const FRay& mouseRay);
    void EndDrag();
};
```

### Gizmo Types

- **Translation Gizmo**: Arrow components for X/Y/Z movement
- **Rotation Gizmo**: Ring handles for axis rotation
- **Scale Gizmo**: Box handles for uniform/non-uniform scaling

### Ray Casting

```cpp
struct FRay {
    FVector Origin;
    FVector Direction;
};

class URaycastManager : public UEngineSubsystem {
public:
    FRay ScreenPointToRay(int32 screenX, int32 screenY,
                          const FMatrix& view, const FMatrix& proj);

    bool RayIntersectSphere(const FRay& ray, const FVector& center,
                           float radius, float& distance);
};
```

## Memory Management

### Object Tracking

All `UObject` instances are tracked in a global array:

```cpp
class UObject {
private:
    static TArray<UObject*> GUObjectArray;
    static TArray<uint32> FreeIndices;  // Recycled indices
    static uint32 NextFreshIndex;

    static void AddTrackedObject(UObject* obj);
    static void RemoveTrackedObject(UObject* obj);
};
```

### Memory Tracking

Custom new/delete operators track allocations:

```cpp
void* UObject::operator new(size_t size) {
    UEngineStatics::AddAllocation(size);
    return ::operator new(size);
}

void UObject::operator delete(void* ptr, size_t size) {
    UEngineStatics::RemoveAllocation(size);
    ::operator delete(ptr);
}
```

## Build System

### Project Structure

```
Engine.sln                 # Visual Studio solution
├── Engine/                # Main engine project
│   ├── Engine.vcxproj     # Project file
│   ├── *.h, *.cpp        # Source files
│   ├── ImGui/            # ImGui library
│   └── Shaders/          # HLSL shader files
└── Dependencies/          # External libraries
```

### Build Configuration

- **Debug**: Full debugging information, no optimizations
- **Release**: Optimizations enabled, minimal debug info
- **Target Platform**: x64 Windows

### Dependencies

- **Windows SDK**: DirectX 11, Windows APIs
- **Dear ImGui**: Immediate mode GUI (included)
- **nlohmann/json**: JSON library (header-only, included)

## Best Practices

### Adding New Components

1. Inherit from appropriate base class (`USceneComponent` or `UPrimitiveComponent`)
2. Add `DECLARE_UCLASS` and `IMPLEMENT_UCLASS` macros
3. Override virtual methods as needed (`Update`, `Draw`, etc.)
4. Implement serialization if the component needs persistence

### Adding New Subsystems

1. Inherit from `UEngineSubsystem`
2. Override `Initialize()` and `Shutdown()`
3. Add to application initialization sequence
4. Register with dependency injection if needed

### Performance Considerations

- Use object pooling for frequently created/destroyed objects
- Batch similar draw calls together
- Minimize state changes in render loop
- Use efficient data structures (TArray, TMap)
- Profile regularly with Visual Studio diagnostics

This architecture provides a solid foundation for 3D game development while remaining extensible and maintainable.