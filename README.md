# DXEngine

A lightweight 3D game engine built with DirectX 11 and C++, featuring an integrated editor with interactive gizmos for object manipulation.

## Table of Contents

1. [Project Overview](#project-overview)
2. [Coding Conventions](#coding-conventions)
3. [Building and Usage](#building-and-usage)
4. [Project Structure](#project-structure)

## Project Overview

### Architecture

The engine follows a modular subsystem architecture:

- **Core Systems**: Object system, reflection, serialization
- **Rendering**: DirectX 11 renderer with shader management
- **Scene Management**: Component-based scene graph
- **Input/UI**: Input handling and ImGui integration
- **Editor Tools**: Interactive gizmos for object transformation

For detailed architecture documentation, see [architecture.md](architecture.md).

### Key Components

#### Core Classes

- `UObject` - Base class for all engine objects
- `UEngineSubsystem` - Base for engine subsystems
- `USceneComponent` - Base for scene objects with transforms
- `UPrimitiveComponent` - Renderable components with meshes

#### Subsystems

- `URenderer` - DirectX 11 rendering management
- `USceneManager` - Scene loading/saving
- `UInputManager` - Input handling with callbacks
- `UMeshManager` - Mesh resource management
- `UGizmoManager` - Editor gizmo system

## Coding Conventions

This section outlines the coding conventions and best practices for the DXEngine project. These conventions ensure code consistency, maintainability, and readability across the entire codebase.

### Basic Principles

1. **Primary Convention**: Follow Unreal Engine conventions for type definitions and general structure
2. **Secondary Convention**: Follow Google C++ Style Guide for all other aspects
   - Reference: https://google.github.io/styleguide/cppguide.html
3. **Error Handling**: Use assertions and optional types only; no std::exception
4. **Maintainability**: Prioritize long-term code maintainability over short-term convenience

### Type System

#### Unreal Engine Type Redefinitions

Use typedef to create aliases for standard types following Unreal Engine conventions:

```cpp
// Basic integer types
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

// Floating point types
typedef float float32;
typedef double float64;

// Character types
typedef char ANSICHAR;
typedef wchar_t WIDECHAR;
typedef char16_t UTF16CHAR;
typedef char32_t UTF32CHAR;

// Optional type
template<typename T>
using TOptional = std::optional<T>;

// Smart pointer types
template<typename T>
using TUniquePtr = std::unique_ptr<T>;

template<typename T>
using TSharedPtr = std::shared_ptr<T>;

template<typename T>
using TWeakPtr = std::weak_ptr<T>;
```

#### Usage Examples

```cpp
class FExampleClass
{
public:
    void ProcessData(int32 InputValue, float32 Scale);
    TOptional<FString> GetOptionalName() const;

private:
    int32 Counter;
    float64 Precision;
    TSharedPtr<FDataProcessor> DataProcessor;
};
```

### Naming Conventions

#### PascalCase for All Identifiers

All classes, member variables, and parameters use PascalCase:

```cpp
class FGameEngine
{
public:
    void InitializeRenderer(int32 ScreenWidth, int32 ScreenHeight);
    void SetPlayerPosition(const FVector3& NewPosition);

private:
    int32 CurrentFrameRate;
    FString GameTitle;
    TUniquePtr<FRenderer> GraphicsRenderer;
};
```

#### Boolean Variables

Boolean and flag variables must start with `bIs`:

```cpp
class FPlayerController
{
private:
    bool bIsMoving;
    bool bIsJumping;
    bool bIsInvulnerable;
    bool bIsGamePaused;

public:
    bool GetIsMoving() const { return bIsMoving; }
    void SetIsMoving(bool bInIsMoving) { bIsMoving = bInIsMoving; }
};
```

#### Parameter Naming

When parameter names conflict with member variables, use prefixes instead of `this`:

```cpp
class FInventorySystem
{
private:
    int32 ItemCount;
    FString InventoryName;

public:
    // ✅ Good: Use In/Out prefixes
    void SetItemCount(int32 InItemCount) { ItemCount = InItemCount; }
    void GetInventoryInfo(FString& OutName, int32& OutCount) const;

    // ❌ Bad: Don't use 'this'
    // void SetItemCount(int32 ItemCount) { this->ItemCount = ItemCount; }
};
```

### Class Structure

#### Access Specifier Order

Always follow this order: public, protected, private

```cpp
class FExampleGameObject
{
public:
    // Constructors and destructor
    FExampleGameObject();
    explicit FExampleGameObject(const FString& InName);
    virtual ~FExampleGameObject() = default;

    // Public methods
    void Initialize();
    void Update(float32 DeltaTime);
    void Render();

    // Getters and setters
    const FString& GetName() const { return ObjectName; }
    void SetName(const FString& InName) { ObjectName = InName; }

    int32 GetHealth() const { return Health; }
    void SetHealth(int32 InHealth) { Health = InHealth; }

protected:
    // Protected methods for inheritance
    virtual void OnInitialize() {}
    virtual void OnDestroy() {}

    // Protected members
    FString ObjectName;

private:
    // Private implementation details
    void UpdateInternal(float32 DeltaTime);
    void CleanupResources();

    // Private members
    int32 Health;
    float32 LastUpdateTime;
    TUniquePtr<FRenderComponent> RenderComponent;
    bool bIsInitialized;
};
```

#### Mandatory Getters and Setters

Even simple public members should use getters and setters for long-term maintainability:

```cpp
class FGameSettings
{
public:
    // Simple getters and setters
    int32 GetScreenWidth() const { return ScreenWidth; }
    void SetScreenWidth(int32 InWidth) { ScreenWidth = InWidth; }

    int32 GetScreenHeight() const { return ScreenHeight; }
    void SetScreenHeight(int32 InHeight) { ScreenHeight = InHeight; }

    bool GetIsFullscreen() const { return bIsFullscreen; }
    void SetIsFullscreen(bool bInIsFullscreen) { bIsFullscreen = bInIsFullscreen; }

    const FString& GetWindowTitle() const { return WindowTitle; }
    void SetWindowTitle(const FString& InTitle) { WindowTitle = InTitle; }

private:
    // Even simple values use getters/setters
    int32 ScreenWidth;
    int32 ScreenHeight;
    bool bIsFullscreen;
    FString WindowTitle;
};
```

### Error Handling

#### Assert and TOptional Only

Use assertions for debug-time checks and TOptional for runtime error handling:

```cpp
class FResourceManager
{
public:
    /// Load a texture resource
    TOptional<FTexture> LoadTexture(const FString& FilePath)
    {
        // Assert for debug-time validation
        assert(!FilePath.empty() && "FilePath cannot be empty");

        if (!FileExists(FilePath))
        {
            // Return empty optional for runtime errors
            return {};
        }

        FTexture LoadedTexture;
        if (LoadTextureFromFile(FilePath, LoadedTexture))
        {
            return LoadedTexture;
        }

        return {}; // Failed to load
    }

    /// Get cached texture
    TOptional<FTexture> GetCachedTexture(const FString& TextureName)
    {
        auto Iterator = TextureCache.find(TextureName);
        if (Iterator != TextureCache.end())
        {
            return Iterator->second;
        }
        return {}; // Not found
    }

private:
    std::unordered_map<FString, FTexture> TextureCache;

    bool FileExists(const FString& FilePath) const;
    bool LoadTextureFromFile(const FString& FilePath, FTexture& OutTexture) const;
};

// Usage example
void UseResourceManager()
{
    FResourceManager ResourceManager;

    auto OptionalTexture = ResourceManager.LoadTexture("assets/player.png");
    if (OptionalTexture.has_value())
    {
        FTexture PlayerTexture = OptionalTexture.value();
        // Use the texture
    }
    else
    {
        // Handle the error case
        LogError("Failed to load player texture");
    }
}
```

### Memory Management

#### Smart Pointer Lifecycle Rules

1. **TUniquePtr**: Default choice for object lifetime management
2. **TSharedPtr**: For objects that can be created/destroyed during runtime and need shared ownership
3. **TWeakPtr**: For non-owning references to SharedPtr objects

```cpp
class FGameWorld
{
private:
    // Unique ownership - objects exist for the lifetime of GameWorld
    TUniquePtr<FRenderer> Renderer;
    TUniquePtr<FPhysicsWorld> PhysicsWorld;

    // Shared ownership - players can be added/removed dynamically
    std::vector<TSharedPtr<FPlayer>> Players;

    // Game systems that need to reference players but don't own them
    TUniquePtr<FPlayerManager> PlayerManager;

public:
    void AddPlayer(const FString& PlayerName)
    {
        auto NewPlayer = std::make_shared<FPlayer>(PlayerName);
        Players.push_back(NewPlayer);

        // PlayerManager holds weak references
        PlayerManager->RegisterPlayer(NewPlayer);
    }

    void RemovePlayer(const FString& PlayerName)
    {
        Players.erase(
            std::remove_if(Players.begin(), Players.end(),
                [&PlayerName](const TSharedPtr<FPlayer>& Player) {
                    return Player->GetName() == PlayerName;
                }),
            Players.end());
    }
};

class FPlayerManager
{
private:
    std::vector<TWeakPtr<FPlayer>> PlayerReferences;

public:
    void RegisterPlayer(TSharedPtr<FPlayer> Player)
    {
        PlayerReferences.push_back(Player);
    }

    void UpdateAllPlayers(float32 DeltaTime)
    {
        for (auto& WeakPlayer : PlayerReferences)
        {
            if (auto Player = WeakPlayer.lock()) // Check if still valid
            {
                Player->Update(DeltaTime);
            }
        }

        // Clean up expired weak pointers
        PlayerReferences.erase(
            std::remove_if(PlayerReferences.begin(), PlayerReferences.end(),
                [](const TWeakPtr<FPlayer>& WeakPtr) {
                    return WeakPtr.expired();
                }),
            PlayerReferences.end());
    }
};
```

### Singleton Pattern

Use static local variables within methods (Meyer's Singleton):

```cpp
class FEngineCore
{
public:
    // ✅ Good: Meyer's Singleton
    static FEngineCore& GetInstance()
    {
        static FEngineCore Instance;
        return Instance;
    }

    void Initialize();
    void Shutdown();

    // Delete copy constructor and assignment operator
    FEngineCore(const FEngineCore&) = delete;
    FEngineCore& operator=(const FEngineCore&) = delete;

private:
    FEngineCore() = default;
    ~FEngineCore() = default;

    bool bIsInitialized = false;
};

// Usage
void GameMain()
{
    FEngineCore& Engine = FEngineCore::GetInstance();
    Engine.Initialize();

    // Game loop...

    Engine.Shutdown();
}
```

```cpp
// ❌ Bad: Don't use pointer-based singletons
class FBadSingleton
{
public:
    static FBadSingleton* GetInstance()
    {
        static FBadSingleton* Instance = nullptr;
        if (!Instance)
        {
            Instance = new FBadSingleton();
        }
        return Instance;
    }
};
```

### Method Naming Consistency

#### Standardized Verbs

Use consistent verbs across the codebase:

| **Use** | **Instead of** | **Example** |
|---------|----------------|-------------|
| `Create` | `Make`, `Build`, `Construct` | `CreatePlayer()`, `CreateTexture()` |
| `Get` | `Retrieve`, `Fetch`, `Obtain` | `GetPlayerCount()`, `GetTexture()` |
| `Set` | `Assign`, `Update` | `SetPlayerName()`, `SetPosition()` |
| `Initialize` | `Init`, `Setup` | `InitializeRenderer()` |
| `Destroy` | `Delete`, `Remove`, `Kill` | `DestroyActor()` |

#### Exceptions for Common Patterns

Some widely-used patterns are acceptable:

```cpp
class FResourceFactory
{
public:
    // ✅ Acceptable: MakeShared is a well-known pattern
    TSharedPtr<FTexture> MakeSharedTexture(const FString& FilePath);

    // ✅ Preferred: But Create is still preferred for consistency
    TSharedPtr<FTexture> CreateSharedTexture(const FString& FilePath);

    // ✅ Standard usage
    TUniquePtr<FMesh> CreateMesh(const FVertexData& Vertices);
    FTexture GetDefaultTexture() const;
    void SetDefaultTexture(const FTexture& InTexture);
};
```

### Documentation

#### Doxygen Comments

Use Doxygen-style comments for public APIs, but avoid over-documentation:

```cpp
class FPlayerController
{
public:
    /**
     * Initialize the player controller with input settings
     * @param InputSettings Configuration for input handling
     * @param bEnableMouseLook Whether to enable mouse look functionality
     */
    void Initialize(const FInputSettings& InputSettings, bool bEnableMouseLook = true);

    /**
     * Process player input for the current frame
     * @param DeltaTime Time elapsed since last frame in seconds
     */
    void ProcessInput(float32 DeltaTime);

    // Simple getters/setters don't need documentation
    // if the purpose is obvious from the name
    FVector3 GetPlayerPosition() const { return PlayerPosition; }
    void SetPlayerPosition(const FVector3& InPosition) { PlayerPosition = InPosition; }

    /**
     * Calculate damage after applying armor and resistances
     * @param BaseDamage Raw damage value before modifications
     * @param DamageType Type of damage (Physical, Magical, etc.)
     * @return Final damage value after all modifications
     * @note This method applies complex calculations including armor reduction,
     *       elemental resistances, and temporary buffs
     */
    float32 CalculateModifiedDamage(float32 BaseDamage, EDamageType DamageType) const;

private:
    FVector3 PlayerPosition;
    float32 MovementSpeed;

    // Complex internal methods may need brief comments
    void ApplyMovementInput(const FVector2& InputVector, float32 DeltaTime);
};
```

#### When to Document

**✅ Document these:**
- Public API methods with complex behavior
- Methods with non-obvious parameters or return values
- Complex algorithms or business logic
- Unusual or non-standard implementations

**❌ Don't document these:**
- Simple getters and setters
- Methods where the name clearly describes the functionality
- Private implementation details that are self-explanatory

### Deprecated Code

#### Using [[deprecated]] Attribute

When refactoring code that's still in use elsewhere:

```cpp
class FLegacyRenderer
{
public:
    /**
     * @deprecated Use CreateModernTexture() instead. Will be removed in v2.0
     */
    [[deprecated("Use CreateModernTexture() instead")]]
    FTexture CreateTexture(const FString& FilePath);

    /**
     * Modern replacement for CreateTexture()
     * Supports HDR and compressed formats
     */
    FTexture CreateModernTexture(const FTextureCreateInfo& CreateInfo);

    /**
     * @deprecated Legacy method. Use SetRenderSettings() instead.
     */
    [[deprecated("Use SetRenderSettings() instead")]]
    void SetLegacyRenderMode(int32 Mode) { /* Legacy implementation */ }

    void SetRenderSettings(const FRenderSettings& Settings);

private:
    // Implementation details...
};

// Usage - compiler will warn about deprecated methods
void ExampleUsage()
{
    FLegacyRenderer Renderer;

    // ⚠️ Compiler warning: deprecated
    auto Texture = Renderer.CreateTexture("test.png");

    // ✅ Preferred modern approach
    FTextureCreateInfo CreateInfo;
    CreateInfo.FilePath = "test.png";
    CreateInfo.Format = ETextureFormat::RGBA8;
    auto ModernTexture = Renderer.CreateModernTexture(CreateInfo);
}
```

### Complete Example

Here's a complete example demonstrating all conventions:

```cpp
/**
 * Game actor that can be placed in the world
 * Handles rendering, physics, and gameplay logic
 */
class FActor
{
public:
    // Constructor and destructor
    FActor();
    explicit FActor(const FString& InName);
    virtual ~FActor() = default;

    /**
     * Initialize the actor in the world
     * @param InWorld The world this actor belongs to
     * @param InTransform Initial transform (position, rotation, scale)
     */
    virtual void Initialize(TWeakPtr<FWorld> InWorld, const FTransform& InTransform);

    /**
     * Update actor logic each frame
     * @param DeltaTime Time elapsed since last frame
     */
    virtual void Update(float32 DeltaTime);

    // Getters and setters
    const FString& GetName() const { return ActorName; }
    void SetName(const FString& InName) { ActorName = InName; }

    bool GetIsVisible() const { return bIsVisible; }
    void SetIsVisible(bool bInIsVisible) { bIsVisible = bInIsVisible; }

    const FTransform& GetTransform() const { return Transform; }
    void SetTransform(const FTransform& InTransform) { Transform = InTransform; }

    /**
     * @deprecated Use GetTransform().GetPosition() instead
     */
    [[deprecated("Use GetTransform().GetPosition() instead")]]
    FVector3 GetPosition() const { return Transform.GetPosition(); }

protected:
    // Protected virtual methods for derived classes
    virtual void OnInitialize() {}
    virtual void OnUpdate(float32 DeltaTime) {}
    virtual void OnDestroy() {}

    // Protected members
    FString ActorName;
    TWeakPtr<FWorld> OwningWorld;

private:
    // Private implementation
    void UpdateComponents(float32 DeltaTime);
    TOptional<FRenderData> CreateRenderData() const;

    // Private members
    FTransform Transform;
    bool bIsVisible;
    bool bIsInitialized;
    TUniquePtr<FRenderComponent> RenderComponent;
    TSharedPtr<FPhysicsComponent> PhysicsComponent;
};
```

## Building and Usage

### Prerequisites

- Windows 10/11
- Visual Studio 2019 or later
- DirectX 11 SDK (included with Windows SDK)

### Building

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/DXEngine.git
   cd DXEngine
   ```

2. Open `Engine.sln` in Visual Studio

3. Build the solution (Ctrl+Shift+B)

4. Run the project (F5)

### Usage

#### Basic Scene Setup

```cpp
// Create an editor application
EditorApplication app;

// Initialize with window settings
app.Initialize(hInstance, L"DXEngine Editor", 1280, 720);

// Run the main loop
app.Run(true); // Enable shader reflection
```

#### Creating Objects

```cpp
// Create primitive components
auto cube = new UCubeComp(FVector(0, 0, 0));
auto sphere = new USphereComp(FVector(2, 0, 0));

// Add to scene
scene->AddObject(cube);
scene->AddObject(sphere);
```

#### Gizmo Manipulation

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

### Dependencies

- **DirectX 11** - Graphics API
- **Dear ImGui** - Immediate mode GUI
- **nlohmann/json** - JSON parsing and serialization
