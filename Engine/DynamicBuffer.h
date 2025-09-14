#pragma once

#pragma once

#include "TArray.h"
#include "UEngineStatics.h"
#include "Vector.h"
#include "Vector4.h"
#include "Matrix.h"

#include <memory>
#include <cassert>
#include <stdexcept> // For std::runtime_error

// TODO: Change names

/**
 * @namespace HLSL
 * @brief Contains definitions and utilities for HLSL (High-Level Shading Language) data types.
 */
namespace HLSL
{
    /**
     * @brief Defines the data types that can be represented in the buffer.
     * This enum maps to standard HLSL data types.
     */
    enum class EType
    {
        Bool,
        Int,
        Float,
        Float3,
        Float4,
        Matrix,
        Struct,
        /** @brief Special data type introduced to implement padding. */
        Padding
    };

    /** @brief Use a 32-bit integer for HLSL bools, as they are typically 4 bytes. */
    using bool32 = uint32;

    /**
     * @brief Maps HLSL::EType enum to a C++ type and its size.
     * This is a template metaprogramming utility to get compile-time information about HLSL types.
     * @tparam Type The HLSL::EType to get information for.
     */
    template<EType Type>
    struct TypeInfo;

    template<>
    struct TypeInfo<EType::Bool>
    {
        using InternalType = bool32;
        static constexpr size_t Size = sizeof(InternalType);
        static constexpr const char* Name = "Bool";
    };
    template<>
    struct TypeInfo<EType::Int>
    {
        using InternalType = int32_t;
        static constexpr size_t Size = sizeof(InternalType);
        static constexpr const char* Name = "Int";
    };
    /** * @brief A standard float is typically 32-bit.
     * @note A `TODO` comment from the original code asks if float is always 32-bit. This is a reasonable assumption in most graphics APIs and HLSL contexts.
     */
    template<>
    struct TypeInfo<EType::Float>
    {
        using InternalType = float;
        static constexpr size_t Size = sizeof(InternalType);
        static constexpr const char* Name = "Float";
    };
    template<>
    struct TypeInfo<EType::Float3>
    {
        using InternalType = FVector;
        static constexpr size_t Size = sizeof(InternalType);
        static constexpr const char* Name = "Float3";
    };
    template<>
    struct TypeInfo<EType::Float4>
    {
        using InternalType = FVector4;
        static constexpr size_t Size = sizeof(InternalType);
        static constexpr const char* Name = "Float4";
    };
    template<>
    struct TypeInfo<EType::Matrix>
    {
        using InternalType = FMatrix;
        static constexpr size_t Size = sizeof(InternalType);
        static constexpr const char* Name = "Matrix";
    };
    /** @brief Struct has no predefined `InternalType` or `Size` and is handled dynamically. */
    // template<> struct TypeInfo<EType::Struct> {}

} // namespace HLSL

/**
 * @class UBufferElementLayout
 * @brief Manages the memory layout of a buffer, including nested structs and padding.
 * * This class is responsible for defining the structure of data in a buffer,
 * calculating offsets, and ensuring correct alignment. It supports a
 * builder-like pattern for defining the layout before it is finalized.
 * * @note The class does not handle duplicated names in the same scope, which is
 * an area for potential improvement.
 * @warning Users should be careful when accessing fields by index, as the presence of
 * padding fields can make indices unpredictable. It is highly recommended to use
 * the field name as a key for access unless you have full knowledge of the padding
 * implementation.
 */
class UBufferElementLayout
{
public:
    /**
     * @struct FField
     * @brief Represents a single field within the buffer's layout.
     */
    struct FField
    {
        HLSL::EType Type;
        FString Name;
        size_t Stride;
        size_t Offset;
        TUniquePtr<UBufferElementLayout> Layout;
    };

public:
    ~UBufferElementLayout() = default;

    /**
     * @brief Default constructor.
     * Initializes a non-finalized layout with a stride of 0.
     */
    UBufferElementLayout() : Stride(0), bIsFinalized(false) {}

    /** @brief Allow move construction and assignment. */
    UBufferElementLayout(const UBufferElementLayout&) = delete;
    UBufferElementLayout(UBufferElementLayout&&) noexcept = default;

    UBufferElementLayout& operator=(const UBufferElementLayout&) = delete;
    UBufferElementLayout& operator=(UBufferElementLayout&&) noexcept = default;

    /**
     * @brief Subscript operator to access a nested struct by name.
     * @param Name The name of the struct field.
     * @return A reference to the nested `UBufferElementLayout`.
     * @pre The layout must not be finalized.
     * @pre The field must exist and be of type `HLSL::EType::Struct`.
     */
    UBufferElementLayout& operator[](const char* Name)
    {
        return (*this)[FString(Name)];
    }

    /**
     * @brief Subscript operator to access a nested struct by name.
     * @param Name The name of the struct field.
     * @return A reference to the nested `UBufferElementLayout`.
     * @pre The layout must not be finalized.
     * @pre The field must exist and be of type `HLSL::EType::Struct`.
     */
    UBufferElementLayout& operator[](const FString& Name)
    {
        assert(!bIsFinalized && "Finalized layout is not subscriptable.");
        assert(FieldIndexMap.count(Name) && "Struct should be appended before using.");

        auto& Field = Fields[FieldIndexMap[Name]];
        assert(Field.Type == HLSL::EType::Struct && "Only struct is subscriptable");
        return *(Field.Layout.get());
    }

    /**
     * @brief Appends a new field of a primitive HLSL type to the layout.
     * @tparam Type The `HLSL::EType` of the field. Must not be `Struct`.
     * @param InName The name of the field.
     * @pre The layout must not be finalized.
     */
    template<HLSL::EType Type>
    void Append(FString InName)
    {
        assert(!bIsFinalized && "Cannot append to finalized layout.");

        FField Field = {};
        Field.Type = Type;
        Field.Name = InName;
        if constexpr (Type == HLSL::EType::Struct)
        {
            Field.Stride = 0; /** Deferred */
            Field.Layout = MakeUnique<UBufferElementLayout>();
        }
        else
        {
            Field.Stride = HLSL::TypeInfo<Type>::Size;
            Field.Layout = nullptr;
        }
        Field.Offset = 0; /** Deferred */
        Fields.push_back(std::move(Field));
        FieldIndexMap[InName] = Fields.size() - 1;
    }

    /**
     * @brief Appends a struct field with a pre-defined layout.
     * @param InName The name of the struct.
     * @param Layout The `UBufferElementLayout` for the nested struct.
     * @pre The layout must not be finalized.
     */
    void AppendStruct(FString InName, UBufferElementLayout Layout)
    {
        assert(!bIsFinalized && "Cannot append to finalized layout.");

        FField Field = {};
        Field.Type = HLSL::EType::Struct;
        Field.Name = InName;
        Field.Stride = 0;
        Field.Layout = MakeUnique<UBufferElementLayout>(std::move(Layout));
        Field.Offset = 0;

        Fields.push_back(std::move(Field));
        FieldIndexMap[InName] = Fields.size() - 1;
    }

    /**
     * @brief Appends a padding field to the layout.
     * This is useful for explicit alignment or to match HLSL-specific
     * packing rules.
     * @param Bytes The number of bytes to pad.
     * @pre The layout must not be finalized.
     */
    void AppendPadding(size_t Bytes)
    {
        assert(!bIsFinalized && "Cannot append to finalized layout.");
        assert(Bytes >= 0 && "Padding size should be larger or equal to 0.");

        if (Bytes == 0)
        {
            return; /** Do nothing when padding size is 0. */
        }
        FField Field = {};
        Field.Type = HLSL::EType::Padding;
        Field.Name = "Padding";
        Field.Stride = Bytes;
        Field.Layout = nullptr;
        Field.Offset = 0; /** Deferred */
        Fields.push_back(std::move(Field));
        /** * @note No entry in FieldIndexMap to keep it inaccessible from the user.
         * A `TODO` comment from the original code suggests introducing a unique identifier for paddings, which could be useful for debugging but is not required for the current implementation.
         */
    }

    /**
     * @brief Creates a deep copy of the finalized layout.
     * @return A new `UBufferElementLayout` object that is a copy of the current one.
     * @pre The layout must be finalized before cloning.
     */
    UBufferElementLayout Clone() const
    {
        assert(bIsFinalized && "Cannot clone before being finalized.");
        UBufferElementLayout Layout;
        for (const auto& Field : Fields)
        {
            FField NewField = {};
            NewField.Type = Field.Type;
            NewField.Name = Field.Name;
            NewField.Stride = Field.Stride;
            NewField.Offset = Field.Offset;
            if (Field.Type == HLSL::EType::Struct && Field.Layout)
            {
                NewField.Layout = MakeUnique<UBufferElementLayout>(Field.Layout->Clone());
            }
            else
            {
                NewField.Layout = nullptr;
            }
            Layout.Fields.push_back(std::move(NewField));
            if (Field.Type != HLSL::EType::Padding)
            {
                Layout.FieldIndexMap[Field.Name] = Layout.Fields.size() - 1;
            }
        }
        Layout.bIsFinalized = true;
        return Layout;
    }

    /**
     * @brief Gets the total stride of the finalized layout.
     * @return The total size in bytes of the layout.
     * @pre The layout must be finalized.
     */
    size_t GetStride() const
    {
        assert(bIsFinalized && "Cannot get stride before being finalized.");
        return Stride;
    }

    /**
     * @brief Calculates the current stride based on the appended fields.
     * This is a utility function used during the finalization process to
     * calculate offsets.
     * @note It doesn't affect `bIsFinalized` and the state of fields.
     * @return The current stride based on the fields added so far.
     */
    size_t GetCurrentStride() const
    {
        size_t Stride = 0;
        for (const auto& Field : Fields)
        {
            if (Field.Type == HLSL::EType::Struct)
            {
                if (!Field.Layout)
                {
                    continue;
                }
                Stride += Field.Layout->GetCurrentStride();
            }
            else
            {
                Stride += Field.Stride;
            }
        }
        return Stride;
    }

    /**
     * @brief Gets a const reference to the array of fields.
     * @return A `const TArray<FField>&` containing all fields.
     * @pre The layout must be finalized.
     */
    const TArray<FField>& GetFields() const
    {
        assert(bIsFinalized && "Cannot get fields before being finalized.");
        return Fields;
    }

    /**
     * @brief Gets a const reference to a field by its index.
     * @param Index The index of the field in the `Fields` array.
     * @return A `const FField&`.
     * @pre The layout must be finalized.
     * @pre `Index` must be within bounds.
     */
    const FField& GetField(size_t Index) const
    {
        assert(bIsFinalized && "Cannot get field before being finalized.");
        /** @note A `TODO` comment from the original code suggests throwing an exception on OOB, which is a good practice for robust error handling. */
        return Fields[Index];
    }

    /**
     * @brief Gets a const reference to a field by its name.
     * @param Name The name of the field.
     * @return A `const FField&`.
     * @pre The layout must be finalized.
     * @pre The name must exist in the layout.
     */
    const FField& GetField(const FString& Name) const
    {
        assert(bIsFinalized && "Cannot get field before being finalized.");
        /** @note A `TODO` comment from the original code suggests throwing an exception if the key is not found, which is a good practice for robust error handling. */
        auto it = FieldIndexMap.find(Name);
        return GetField(it->second);
    }

    /**
     * @brief Prints the layout structure to a stream in a human-readable format.
     * @param Stream The output stream (e.g., `std::cout`).
     * @param IndentLevel The current indentation level for nested structs.
     * @pre The layout must be finalized.
     */
    void Print(std::ostream& Stream, int IndentLevel = 0) const
    {
        assert(bIsFinalized && "Cannot print layout before being finalized.");
        auto PrintIndent = [&](int Level) {
            for (int i = 0; i < Level; ++i)
            {
                Stream << "  ";
            }
            };
        for (const auto& Field : Fields)
        {
            PrintIndent(IndentLevel);
            Stream << Field.Name << " (Offset: " << Field.Offset << ", Size: " << Field.Stride << ", Type: ";
            switch (Field.Type)
            {
            case HLSL::EType::Bool:
                Stream << "Bool";
                break;
            case HLSL::EType::Int:
                Stream << "Int";
                break;
            case HLSL::EType::Float:
                Stream << "Float";
                break;
            case HLSL::EType::Float3:
                Stream << "Float3";
                break;
            case HLSL::EType::Float4:
                Stream << "Float4";
                break;
            case HLSL::EType::Matrix:
                Stream << "Matrix";
                break;
            case HLSL::EType::Struct:
                Stream << "Struct";
                break;
            case HLSL::EType::Padding:
                Stream << "Padding";
                break;
            }
            Stream << ")" << std::endl;
            if (Field.Type == HLSL::EType::Struct)
            {
                Field.Layout->Print(Stream, IndentLevel + 1);
            }
        }
    }

    /**
     * @brief Finalizes the layout, calculating all field offsets and the total stride.
     * @pre The layout must not be finalized already.
     */
    void Finalize()
    {
        assert(!bIsFinalized && "Cannot finalize a layout for multiple times");
        size_t Offset = 0;
        for (size_t i = 0; i < Fields.size(); ++i)
        {
            auto& Field = Fields[i];
            Field.Offset = Offset;
            if (Field.Type == HLSL::EType::Struct)
            {
                /** @note A `TODO` from the original code asks if it is safe to allow an empty struct. This is safe, but the stride would be 0. */
                if (!Field.Layout)
                {
                    continue;
                }
                Field.Layout->Finalize();
                Field.Stride = Field.Layout->GetStride();
            }
            Offset += Field.Stride;
        }
        Stride = Offset;
        bIsFinalized = true;
    }

private:
    TArray<FField> Fields;
    /** @brief Maps field names to their index in the `Fields` array for quick lookup. */
    TMap<FString, size_t> FieldIndexMap;
    /** @brief The total stride (size in bytes) of the finalized layout. */
    size_t Stride;
    /** @brief A flag to indicate whether the layout has been finalized. */
    bool bIsFinalized;
};

/**
 * @class UBufferElement
 * @brief A proxy class for accessing and modifying data in a buffer.
 * * This class provides a convenient, type-safe interface for reading from and
 * writing to a specific field within a `UDynamicBuffer`, abstracting away the
 * raw memory operations and offsets.
 */
class UBufferElement
{
public:
    using value_type = char;

    /** @brief Default destructor. */
    ~UBufferElement() = default;

    /**
     * @brief Constructs a root `UBufferElement` for a buffer.
     * @param InBufferData The raw buffer data pointer.
     * @param InLayout The layout of the buffer.
     * @pre `InBufferData` must not be a null pointer.
     */
    UBufferElement(value_type* InBufferData, const UBufferElementLayout* InLayout)
        : BufferData(InBufferData), Layout(InLayout), Field(nullptr)
    {
        assert(BufferData && "Buffer Data must not be a null pointer.");
    }

    /**
     * @brief Subscript operator to get a nested `UBufferElement`.
     * @param Name The name of the field to access.
     * @return A new `UBufferElement` proxy for the nested field.
     */
     /**
  * @brief Subscript operator to get a nested `UBufferElement`.
  * @param Name The name of the field to access.
  * @return A new `UBufferElement` proxy for the nested field.
  */
    UBufferElement operator[](const char* Name)
    {
        return (*this)[FString(Name)];
    }

    /**
     * @brief Subscript operator to get a nested `UBufferElement`.
     * @param Name The name of the field to access.
     * @return A new `UBufferElement` proxy for the nested field.
     */
    UBufferElement operator[](const FString& Name)
    {
        const auto& Field = Layout->GetField(Name);
        if (Field.Type == HLSL::EType::Struct)
        {
            return UBufferElement(BufferData + Field.Offset, Field.Layout.get(), &Field);
        }
        else
        {
            return UBufferElement(BufferData + Field.Offset, Layout, &Field);
        }
    }

    /**
     * @brief Assignment operator to write a value into the buffer.
     * @tparam TValue The type of the value to write.
     * @param Value The value to be copied into the buffer.
     * @return A reference to the current `UBufferElement` for chaining.
     * @pre The element must represent a non-struct field.
     * @pre The size of `TValue` must match the field's stride.
     * @pre For `bool`, the field must be of type `HLSL::EType::Bool`.
     */
    template<typename TValue>
    UBufferElement& operator=(const TValue& Value)
    {
        assert(Field && Field->Type != HLSL::EType::Struct && "Cannot assign a value to a struct.");
        // Special case for C++ bool type to match HLSL::bool32
        if constexpr (std::is_same_v<TValue, bool>)
        {
            assert(Field->Type == HLSL::EType::Bool && "Field is not a bool type.");
            assert(Field->Stride == sizeof(HLSL::bool32) && "Mismatched size for bool type.");
            HLSL::bool32 BoolValue = Value; // Converts true to 1 and false to 0
            std::memcpy(BufferData, &BoolValue, sizeof(HLSL::bool32));
        }
        else // For all other types
        {
            assert(Field->Stride == sizeof(TValue) && "Mismatched size between provided type and layout field.");
            /** @note A `TODO` comment from the original code suggests checking `is_trivially_copyable` before using `memcpy`. This is a good practice to prevent undefined behavior for non-POD types. */
            std::memcpy(BufferData, &Value, sizeof(TValue));
        }
        return *this;
    }

    /**
     * @brief Type-cast operator to read a value from the buffer.
     * @tparam Type The type to cast the buffer data to.
     * @return The value read from the buffer.
     * @pre The element must represent a non-struct field.
     * @pre The size of `Type` must match the field's stride.
     * @pre For `bool`, the field must be of type `HLSL::EType::Bool`.
     * @note Using this operator can lead to undefined behavior (UB) if the type does not match the data in the buffer.
     */
    template <typename Type>
    operator Type() const
    {
        assert(Field && Field->Type != HLSL::EType::Struct && "Cannot read a value from a struct.");
        /** @note A `TODO` comment from the original code suggests improving bool support and type stability, which is a good goal for future development. */
        if constexpr (std::is_same_v<Type, bool>)
        {
            assert(Field->Type == HLSL::EType::Bool && "Field is not a bool type.");
            assert(Field->Stride == sizeof(HLSL::bool32) && "MIsmtached size for bool type.");
            return *reinterpret_cast<const HLSL::bool32*>(BufferData) != 0;
        }
        else
        {
            assert(Field->Stride == sizeof(Type) && "MIsmtached size between requested type and layout field");
            return *reinterpret_cast<const Type*>(BufferData);
        }
        /** @note The original code notes that there is only one case of size mismatch (bool), but the generic assertions provide a good safety net. */
    }

private:
    /**
     * @brief Private constructor for nested `UBufferElement` instances.
     * @param InBufferData The raw buffer data pointer, offset to the current field.
     * @param InLayout The layout of the parent buffer.
     * @param InField The field corresponding to this element.
     */
    UBufferElement(value_type* InBufferData, const UBufferElementLayout* InLayout, const UBufferElementLayout::FField* InField)
        : BufferData(InBufferData), Layout(InLayout), Field(InField) {
    }

    value_type* BufferData;
    const UBufferElementLayout* Layout;
    const UBufferElementLayout::FField* Field;
};

/**
 * @class UDynamicBuffer
 * @brief A dynamic, type-safe buffer that uses a layout for structured data access.
 * * This class encapsulates a `TArray<char>` and provides a high-level interface
 * to access and modify data based on a `UBufferElementLayout`. It is designed
 * to manage a contiguous block of memory with a defined structure.
 */
class UDynamicBuffer
{
public:
    using value_type = char;

    /**
     * @brief Constructs a dynamic buffer with a single element of a given layout.
     * @param InLayout The layout to use for the buffer. The layout is finalized upon construction.
     */
    UDynamicBuffer() = default;

    /**
     * @brief Constructs a dynamic buffer with a single element of a given layout.
     * @param InLayout The layout to use for the buffer. The layout is finalized upon construction.
     */
    UDynamicBuffer(UBufferElementLayout InLayout)
        : Layout(std::move(InLayout))
    {
        Layout.Finalize();
        Buffer.resize(Layout.GetStride());
    }

    /**
     * @brief Constructs a dynamic buffer with a specified number of elements.
     * @param InLayout The layout for each element.
     * @param Count The number of elements in the buffer.
     */
    UDynamicBuffer(UBufferElementLayout InLayout, size_t Count)
        : Layout(std::move(InLayout))
    {
        Layout.Finalize();
        Buffer.resize(Layout.GetStride() * Count);
    }

    /**
     * @brief Subscript operator to get a `UBufferElement` for a specific element in the buffer array.
     * @param Index The index of the element to access.
     * @return A `UBufferElement` proxy for the element at the given index.
     * @pre `Index` must be less than the total count of elements.
     */
    UBufferElement operator[](size_t Index)
    {
        assert(Index < GetCount() && "Buffer access Out-of-Bounds.");
        return UBufferElement(const_cast<value_type*>(Buffer.data()) + Index * Layout.GetStride(), &Layout);
    }

    /**
     * @brief Gets the number of elements in the buffer.
     * @return The number of elements, calculated as `buffer_size / stride`.
     */
    size_t GetCount() const
    {
        if (Layout.GetStride() > 0)
        {
            return Buffer.size() / Layout.GetStride();
        }
        else
        {
            /** @note A `NOTE` comment from the original code warns about a potential divide-by-zero, which is handled here by returning 0. */
            return 0;
        }
    }

    void* GetData()
    {
        return reinterpret_cast<void*>(Buffer.data());
    }

    const void* GetData() const
    {
        return reinterpret_cast<const void*>(Buffer.data());
    }

#ifndef NDEBUG
    /**
     * @brief Gets a const reference to the buffer's layout.
     * This method is intended for debugging purposes only.
     * @return A `const UBufferElementLayout&`.
     */
    const UBufferElementLayout& GetLayout() const
    {
        return Layout;
    }
#endif

private:
    TArray<value_type> Buffer;
    UBufferElementLayout Layout;
};
