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

All classes, member variables, and parameters use PascalCase with appropriate prefixes:

```cpp
// Regular classes use F prefix
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

// Classes inheriting from UObject use U prefix
class UPlayerController : public UObject
{
public:
    void Initialize();
    void ProcessInput(float32 DeltaTime);

private:
    bool bIsInputEnabled;
    FVector3 PlayerPosition;
};

// Engine subsystems use U prefix
class URenderer : public UEngineSubsystem
{
public:
    void InitializeDirectX();
    void RenderFrame();

private:
    TUniquePtr<FDirectXContext> DirectXContext;
    bool bIsInitialized;
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

### Class Naming Prefix Rules

#### Understanding Prefix Usage

The choice of class prefix depends on the class's role and inheritance:

```cpp
// Use U prefix for classes that inherit from UObject or UObject-derived classes
class UObject { };                              // Base engine object class
class UEngineSubsystem : public UObject { };    // Engine subsystem base
class URenderer : public UEngineSubsystem { };  // ✅ U prefix - inherits from UObject
class UInputManager : public UObject { };       // ✅ U prefix - inherits from UObject
class USceneComponent : public UObject { };     // ✅ U prefix - inherits from UObject
class UActorComponent : public UObject { };     // ✅ U prefix - inherits from UObject

// Use F prefix for regular classes (data structures, utilities, non-UObject classes)
class FVector3 { };                     // ✅ F prefix - math data structure
class FTransform { };                   // ✅ F prefix - transformation data
class FString { };                      // ✅ F prefix - string utility class
class FGameSettings { };               // ✅ F prefix - configuration data
class FResourceManager { };            // ✅ F prefix - utility class
class FRenderData { };                 // ✅ F prefix - data structure

// Special cases and examples
class FMyActor { };                     // ✅ F prefix - regular class, doesn't inherit from UObject
class UMyActorComponent : public UActorComponent { }; // ✅ U prefix - inherits from UObject via UActorComponent

// ❌ Wrong prefix usage
class FPlayerController : public UObject { };    // ❌ Should be UPlayerController
class UMathUtils { };                            // ❌ Should be FMathUtils (utility class)
```

#### Inheritance-Based Naming Decision Tree

```cpp
// Decision process for class naming:
// 1. Does the class inherit from UObject (directly or indirectly)?
//    → YES: Use U prefix (UMyClass)
//    → NO: Continue to step 2
//
// 2. Is it a data structure, utility class, or math class?
//    → YES: Use F prefix (FMyClass)
//
// 3. Is it an enum?
//    → YES: Use E prefix (EMyEnum)
//
// 4. Is it a template class?
//    → YES: Use T prefix (TMyTemplate)

// Examples of correct usage:
class UObject { };                                    // Engine base class
class UMyCustomObject : public UObject { };          // ✅ Inherits from UObject → U prefix
class UMyCustomComponent : public USceneComponent { }; // ✅ USceneComponent inherits from UObject → U prefix

class FMyDataStructure { };                          // ✅ Regular data class → F prefix
class FMyUtility { };                                // ✅ Utility class → F prefix

enum class EMyEnum { Value1, Value2 };               // ✅ Enum → E prefix

template<typename T>
class TMyTemplate { };                               // ✅ Template → T prefix
```

### Static Members and Functions

#### Static Member Placement

Static members should be placed in specific locations within class declarations:

```cpp
class FGameManager
{
public:
    // Static factory methods first
    static FGameManager& GetInstance();
    static TSharedPtr<FGameManager> CreateShared();

    // Regular constructors and destructor
    FGameManager();
    virtual ~FGameManager() = default;

    // Regular public methods
    void Initialize();
    void Update(float32 DeltaTime);

    // Static utility methods after regular methods
    static bool IsValidGameState(EGameState State);
    static FString GetGameStateString(EGameState State);

protected:
    // Static protected members
    static constexpr int32 MaxPlayers = 64;
    static constexpr float32 DefaultTickRate = 60.0f;

private:
    // Static private members
    static FGameManager* Instance;
    static bool bIsInitialized;

    // Regular private members
    EGameState CurrentState;
    float32 GameTime;
    TArray<TSharedPtr<FPlayer>> Players;
};
```

#### Static Function Definitions

Static functions should be defined in source files, not headers (except for `static inline`):

```cpp
// GameManager.h
class FGameManager
{
public:
    static bool IsValidGameState(EGameState State);
    static FString GetGameStateString(EGameState State);

    // ✅ Good: Simple static inline functions are acceptable in headers
    static inline bool IsGameRunning() { return bIsGameRunning; }
    static inline int32 GetMaxPlayers() { return MaxPlayers; }

private:
    static bool bIsGameRunning;
    static constexpr int32 MaxPlayers = 64;
};

// GameManager.cpp
// ✅ Good: Complex static functions in source files
bool FGameManager::IsValidGameState(EGameState State)
{
    switch (State)
    {
        case EGameState::Menu:
        case EGameState::Playing:
        case EGameState::Paused:
        case EGameState::GameOver:
            return true;
        default:
            return false;
    }
}

FString FGameManager::GetGameStateString(EGameState State)
{
    // Complex implementation here...
    switch (State)
    {
        case EGameState::Menu: return TEXT("Menu");
        case EGameState::Playing: return TEXT("Playing");
        case EGameState::Paused: return TEXT("Paused");
        case EGameState::GameOver: return TEXT("Game Over");
        default: return TEXT("Unknown");
    }
}

// Static member definitions
bool FGameManager::bIsGameRunning = false;
```

### ODR (One Definition Rule) Guidelines

#### Understanding ODR Violations

The One Definition Rule states that each entity should have exactly one definition across the entire program:

```cpp
// ❌ Bad: ODR violation - function defined in header without inline
// Utils.h
FString FormatPlayerName(const FString& Name)  // ODR violation if included in multiple translation units
{
    return FString::Printf(TEXT("Player: %s"), *Name);
}

// ✅ Good: Use static inline for header-only functions
// Utils.h
static inline FString FormatPlayerName(const FString& Name)
{
    return FString::Printf(TEXT("Player: %s"), *Name);
}

// ✅ Good: Or use regular inline
// Utils.h
inline FString FormatPlayerName(const FString& Name)
{
    return FString::Printf(TEXT("Player: %s"), *Name);
}

// ✅ Good: Declare in header, define in source
// Utils.h
FString FormatPlayerName(const FString& Name);

// Utils.cpp
FString FormatPlayerName(const FString& Name)
{
    return FString::Printf(TEXT("Player: %s"), *Name);
}
```

#### Static Inline Usage Guidelines

Use `static inline` for simple utility functions that should be header-only:

```cpp
// MathUtils.h
class FMathUtils
{
public:
    // ✅ Good: Simple mathematical operations
    static inline float32 DegreesToRadians(float32 Degrees)
    {
        return Degrees * (PI / 180.0f);
    }

    static inline float32 RadiansToDegrees(float32 Radians)
    {
        return Radians * (180.0f / PI);
    }

    static inline bool IsNearlyEqual(float32 A, float32 B, float32 Tolerance = KINDA_SMALL_NUMBER)
    {
        return FMath::Abs(A - B) <= Tolerance;
    }

    // ❌ Bad: Complex functions should not be static inline in headers
    // static inline FMatrix CalculateViewProjectionMatrix(...) { /* complex code */ }

    // ✅ Good: Declare complex functions, define in source
    static FMatrix CalculateViewProjectionMatrix(const FVector& Position, const FRotator& Rotation, float32 FOV);

private:
    static constexpr float32 PI = 3.14159265359f;
};
```

#### Template Static Members

Template static members require special handling to avoid ODR violations:

```cpp
// ResourceManager.h
template<typename T>
class TResourceManager
{
public:
    static TResourceManager<T>& GetInstance()
    {
        static TResourceManager<T> Instance;  // ✅ Good: Each template instantiation gets its own static
        return Instance;
    }

    static void RegisterResource(const FString& Name, TSharedPtr<T> Resource)
    {
        GetInstance().Resources[Name] = Resource;
    }

private:
    TMap<FString, TSharedPtr<T>> Resources;

    // ✅ Good: Template static members are implicitly inline
    static inline int32 LoadedResourceCount = 0;
};

// Usage creates separate instances for each type
auto& TextureManager = TResourceManager<FTexture>::GetInstance();
auto& MeshManager = TResourceManager<FMesh>::GetInstance();
```

#### Global Constants and ODR

Handle global constants properly to avoid ODR violations:

```cpp
// Constants.h

// ❌ Bad: Non-const global variables cause ODR violations
// extern int32 GMaxTextureSize;  // Declared in header, defined in source

// ✅ Good: Const/constexpr globals are safe
constexpr int32 MAX_TEXTURE_SIZE = 4096;
constexpr float32 DEFAULT_FOV = 90.0f;

// ✅ Good: Static const class members
class FEngineConstants
{
public:
    static constexpr int32 MaxTextureSize = 4096;
    static constexpr float32 DefaultFOV = 90.0f;
    static constexpr TCHAR DefaultWindowTitle[] = TEXT("DXEngine");

    // ✅ Good: Static inline for complex constants
    static inline const FVector DefaultPlayerSize = FVector(1.0f, 1.0f, 2.0f);
    static inline const TArray<FString> SupportedFormats = {TEXT("png"), TEXT("jpg"), TEXT("tga")};
};

// ✅ Good: Namespace constants
namespace EngineConstants
{
    constexpr int32 MaxTextureSize = 4096;
    constexpr float32 DefaultFOV = 90.0f;

    // For complex types, use static inline
    static inline const FVector DefaultPlayerSize = FVector(1.0f, 1.0f, 2.0f);
}
```

#### ODR-Safe Utility Classes

Create utility classes that are safe to include in multiple translation units:

```cpp
// StringUtils.h
class FStringUtils
{
public:
    // ✅ Good: Simple static inline utilities
    static inline bool IsEmpty(const FString& String)
    {
        return String.IsEmpty();
    }

    static inline FString ToLower(const FString& String)
    {
        return String.ToLower();
    }

    static inline FString ToUpper(const FString& String)
    {
        return String.ToUpper();
    }

    // ✅ Good: Complex operations declared only
    static FString FormatFileSize(int64 Bytes);
    static TArray<FString> SplitString(const FString& String, const FString& Delimiter);
    static bool IsValidEmailAddress(const FString& Email);

private:
    // ✅ Good: Private helper functions can be static inline
    static inline bool IsAlphaNumeric(TCHAR Character)
    {
        return FChar::IsAlnum(Character);
    }
};

// StringUtils.cpp - Complex implementations
FString FStringUtils::FormatFileSize(int64 Bytes)
{
    const TCHAR* Units[] = {TEXT("B"), TEXT("KB"), TEXT("MB"), TEXT("GB"), TEXT("TB")};
    int32 UnitIndex = 0;
    float64 Size = static_cast<float64>(Bytes);

    while (Size >= 1024.0 && UnitIndex < 4)
    {
        Size /= 1024.0;
        UnitIndex++;
    }

    return FString::Printf(TEXT("%.2f %s"), Size, Units[UnitIndex]);
}
```

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

### Complete Examples

Here are complete examples demonstrating all conventions:

#### Example 1: UObject-derived Class (U prefix)

```cpp
/**
 * Player controller that handles input and player logic
 * Inherits from UObject for engine integration
 */
class UPlayerController : public UObject
{
public:
    // Constructor and destructor
    UPlayerController();
    explicit UPlayerController(const FString& InPlayerName);
    virtual ~UPlayerController() = default;

    /**
     * Initialize the player controller
     * @param InInputManager Reference to the input management system
     * @param InDefaultSettings Default input configuration
     */
    virtual void Initialize(UInputManager* InInputManager, const FInputSettings& InDefaultSettings);

    /**
     * Process player input each frame
     * @param DeltaTime Time elapsed since last frame
     */
    virtual void ProcessInput(float32 DeltaTime);

    // Getters and setters
    const FString& GetPlayerName() const { return PlayerName; }
    void SetPlayerName(const FString& InName) { PlayerName = InName; }

    bool GetIsInputEnabled() const { return bIsInputEnabled; }
    void SetIsInputEnabled(bool bInIsEnabled) { bIsInputEnabled = bInIsEnabled; }

    const FVector3& GetPlayerPosition() const { return PlayerPosition; }
    void SetPlayerPosition(const FVector3& InPosition) { PlayerPosition = InPosition; }

protected:
    // Protected virtual methods for derived classes
    virtual void OnInputReceived(const FInputData& InputData) {}
    virtual void OnPlayerMoved(const FVector3& NewPosition) {}

    // Protected members
    FString PlayerName;
    TWeakPtr<UInputManager> InputManager;

private:
    // Private implementation
    void ProcessMovementInput(const FVector2& InputVector, float32 DeltaTime);
    void UpdatePlayerState();

    // Private members
    FVector3 PlayerPosition;
    FInputSettings InputSettings;
    bool bIsInputEnabled;
    bool bIsInitialized;
    float32 MovementSpeed;
};
```

#### Example 2: Regular Class (F prefix)

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

#### Class Prefix Summary

```cpp
// UObject-derived classes (Engine objects, Components, Subsystems)
class UObject { };                    // Base engine object
class UEngineSubsystem : public UObject { };  // Engine subsystem base
class URenderer : public UEngineSubsystem { };     // Rendering subsystem
class UInputManager : public UEngineSubsystem { }; // Input management
class USceneComponent : public UObject { };        // Scene component base
class UPrimitiveComponent : public USceneComponent { }; // Renderable component

// Regular classes (Data structures, Utilities, Math, etc.)
class FVector3 { };                   // Math vector
class FTransform { };                 // Transformation data
class FString { };                    // String class
class FGameSettings { };              // Configuration data
class FResourceManager { };          // Resource management utility

// Enums
enum class EGameState { Menu, Playing, Paused };
enum class ERenderMode { Forward, Deferred };

// Templates
template<typename T> class TArray { };
template<typename T> class TSharedPtr { };
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

## Linter and Code Quality Setup

### Prerequisites

To enforce the coding conventions automatically, install the following tools:

1. **Visual Studio 2019/2022** with C++ development tools
2. **Clang-Format** (included with Visual Studio or install separately)
3. **Clang-Tidy** (optional, for advanced static analysis)

### Configuration Files

The project includes several configuration files to enforce coding standards:

- **`.clang-format`** - Code formatting rules (Unreal Engine + Google C++ style)
- **`.clang-tidy`** - Static analysis and linting rules
- **`.editorconfig`** - Editor-agnostic formatting settings
- **`VisualStudio.props`** - Visual Studio project property sheet
- **`DXEngine.ruleset`** - Visual Studio Code Analysis rules

### Setup Instructions

#### 1. Visual Studio Setup

1. **Import Property Sheet** (recommended):
   ```
   Right-click your project → Properties → VC++ Directories
   → Property Sheets → Add Existing Property Sheet
   → Select "VisualStudio.props"
   ```

2. **Enable Clang-Format**:
   ```
   Tools → Options → Text Editor → C/C++ → Code Style → Formatting
   → Enable "Format document on save"
   → Set "Clang-format" as formatter
   ```

3. **Enable Code Analysis**:
   ```
   Project Properties → Code Analysis → General
   → Enable "Enable Code Analysis on Build"
   → Rule Set: Select "DXEngine.ruleset"
   ```

#### 2. Manual Clang-Format Usage

Format a single file:
```bash
clang-format -i Source/MyFile.cpp
```

Format all project files:
```bash
# PowerShell
Get-ChildItem -Path "." -Recurse -Include "*.cpp","*.h","*.hpp" | ForEach-Object { clang-format -i $_.FullName }

# Command Prompt
for /r . %i in (*.cpp *.h *.hpp) do clang-format -i "%i"
```

#### 3. Git Pre-commit Hook (Optional)

Create `.git/hooks/pre-commit` to automatically format code before commits:

```bash
#!/bin/sh
# Format all staged C++ files
for file in $(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(cpp|hpp|h|cc|cxx)$'); do
    if [ -f "$file" ]; then
        clang-format -i "$file"
        git add "$file"
    fi
done
```

Make it executable:
```bash
chmod +x .git/hooks/pre-commit
```

#### 4. VS Code Setup (Alternative)

If using VS Code, install these extensions:
- **C/C++** (Microsoft)
- **Clang-Format** (xaver)
- **EditorConfig** (EditorConfig)

Add to `.vscode/settings.json`:
```json
{
    "C_Cpp.clang_format_style": "file",
    "C_Cpp.clang_format_fallbackStyle": "Google",
    "editor.formatOnSave": true,
    "editor.defaultFormatter": "ms-vscode.cpptools",
    "files.associations": {
        "*.h": "cpp",
        "*.hpp": "cpp",
        "*.cpp": "cpp",
        "*.inl": "cpp"
    }
}
```

### Enforced Conventions

The linter configuration enforces these key conventions:

#### Formatting Rules
- **Indentation**: 4 spaces (no tabs)
- **Line Length**: 120 characters maximum
- **Braces**: Allman style (opening brace on new line)
- **Pointer/Reference**: Left-aligned (`int* ptr`, `int& ref`)

#### Naming Rules
- **Regular Classes**: PascalCase with `F` prefix (`FGameEngine`, `FVector3`)
- **UObject-derived Classes**: PascalCase with `U` prefix (`UPlayerController`, `URenderer`)
- **Structs**: PascalCase with `F` prefix (`FVector3`, `FTransform`)
- **Enums**: PascalCase with `E` prefix (`EGameState`, `ERenderMode`)
- **Templates**: PascalCase with `T` prefix (`TArray`, `TSharedPtr`)
- **Functions/Methods**: PascalCase (`InitializeRenderer`, `ProcessInput`)
- **Variables**: PascalCase (`PlayerCount`, `ScreenWidth`)
- **Booleans**: `bIs` prefix (`bIsInitialized`, `bIsVisible`)
- **Parameters**: `In`/`Out` prefixes when needed (`InPlayerName`, `OutResult`)

#### Code Quality Rules
- **Modern C++**: Enforces C++17 standards
- **Performance**: Detects unnecessary copies, inefficient patterns
- **Safety**: Prevents common bugs and undefined behavior
- **Maintainability**: Limits function complexity, encourages best practices

### Custom Naming Convention Checker

Since clang-tidy cannot automatically detect UObject inheritance for prefix enforcement, we provide a custom Python script:

#### Using ClassNamingChecker.py

```bash
# Check current directory
python ClassNamingChecker.py

# Check specific directory
python ClassNamingChecker.py ./Engine

# Example output:
# ❌ Found 2 naming convention violations in 1 files:
#
# 📁 Engine/Core/PlayerController.h
#    Line   15: Class 'FPlayerController' should start with 'U' (found 'F'). UObject-derived classes should use U prefix
#               Code: class FPlayerController : public UObject
```

The script automatically:
- **Detects inheritance chains** to determine if classes derive from UObject
- **Validates class prefixes** (U for UObject-derived, F for regular classes)
- **Checks enum prefixes** (E prefix required)
- **Validates template prefixes** (T prefix required)
- **Provides line numbers** and code context for violations

### Continuous Integration

For automated enforcement in CI/CD:

```yaml
# GitHub Actions example
- name: Check Code Formatting
  run: |
    find . -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | xargs clang-format --dry-run --Werror

- name: Check Naming Conventions
  run: |
    python ClassNamingChecker.py ./Engine

- name: Run Static Analysis
  run: |
    clang-tidy --config-file=.clang-tidy Source/**/*.cpp -- -std=c++17
```

### Troubleshooting

#### Common Issues

1. **Clang-Format not found**:
   - Install LLVM tools or ensure Visual Studio C++ tools are installed
   - Add clang-format to system PATH

2. **Format conflicts with existing code**:
   - Run format on entire codebase once: `clang-format -i **/*.{cpp,h,hpp}`
   - Commit the formatting changes separately

3. **EditorConfig not working**:
   - Ensure your editor supports EditorConfig
   - Check that `.editorconfig` is in the root directory

4. **Visual Studio property sheet not applying**:
   - Verify the `.props` file is imported in the correct configuration
   - Check project file (`.vcxproj`) contains the import statement

### Dependencies

- **DirectX 11** - Graphics API
- **Dear ImGui** - Immediate mode GUI
- **nlohmann/json** - JSON parsing and serialization
