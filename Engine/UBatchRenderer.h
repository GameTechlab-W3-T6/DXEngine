#pragma once

#include "URenderer.h"

class UPrimitiveComponent;
class UTextholderComp;
class UGizmoComponent;

class UBatchRenderer : public URenderer
{
	DECLARE_UCLASS(UBatchRenderer, URenderer)
public:
	// type aliases
	using LayerID	= UPrimitiveComponent::LayerID;
	using MeshID	= UMesh::MeshID;
	using ShaderID	= UShader::ShaderID;

	virtual ~UBatchRenderer() = default;

	UBatchRenderer() = default;

	UBatchRenderer(const UBatchRenderer&) = delete;
	UBatchRenderer(UBatchRenderer&&) = delete;

	UBatchRenderer& operator=(const UBatchRenderer&) = delete;
	UBatchRenderer& operator=(UBatchRenderer&&) = delete;

	virtual void DrawPrimitiveComponent(UPrimitiveComponent* PrimitiveComponent) override;
	[[deprecated("Use DrawPrimitiveComponent to draw Gizmo.")]]
	virtual void DrawGizmoComponent(UGizmoComponent* GizmoComponent, bool drawOnTop) override;
    virtual void DrawTextholderComponent(UTextholderComp* Component) override;

	/** @note: You should call Draw() before moving onto other rendering step(e.g., GUI drawing).*/
	virtual void Draw() override;

private:
	// ===============================================
	/** @note: Caution! Do Not Read Below. */

	/** @todo: Use smaller type for Bits? */
	/** @note: If InBits is smaller than sizeof(T), then data might be truncated. */
	template<typename T, uint32 InBits>
	struct KeyField
	{
		using Type = T;
		static constexpr uint32 Bits = InBits;
	};

	/**
	 * @brief Manages packing and parsing of a key from multiple fields.
	 * @tparam TKey The underlying integer type for the key (e.g., uint64_t, __uint128_t).
	 * @tparam Fields The list of KeyField types, defining the key's structure.
	 */
	template<typename TKey, typename... Fields>
	class KeyManager
	{
	private:
		static constexpr uint32 TotalBits = (0 + ... + Fields::Bits);

		template<typename TargetField, typename... Fields>
		struct ShiftCalculator;

		template<typename TargetField, typename CurrentField, typename... RestFields>
		struct ShiftCalculator<TargetField, CurrentField, RestFields...>
		{
			static constexpr uint32 Get()
			{
				if constexpr (std::is_same_v<TargetField, CurrentField>)
				{
					return 0;
				}
				else
				{
					return CurrentField::Bits + ShiftCalculator<TargetField, RestFields...>::Get();
				}
			}
		};

		template<typename TargetField>
		struct ShiftCalculator<TargetField>
		{
			[[nodiscard]] static constexpr uint32 Get()
			{
				static_assert(false, "The requested field was not found in the list.");
				return 0;
			}
		};

	public:
		static_assert(TotalBits <= (sizeof(TKey) * 8), "Total bits for Key exceeds 64 bits.");

		[[nodiscard]] static TKey CreateKey(typename Fields::Type... Values)
		{
			TKey Key = 0;
			uint32 Shift = 0;
			([&] {
				Key |= (static_cast<TKey>(Values) << Shift);
				Shift += Fields::Bits;
				}(), ...);

			return Key;
		}

		template<typename Field>
		static typename Field::Type Get(TKey Key)
		{
			constexpr uint32 Shift = ShiftCalculator<Field, Fields...>::Get();
			constexpr TKey Mask = (static_cast<TKey>(1) << Field::Bits) - 1;
			return static_cast<typename Field::Type>((Key >> Shift) & Mask);
		}
	};

	/** @note: Do not use 'using' keyword. Compiler cannot distinguish template specialization with same paramters. */
	struct LayerField			: KeyField<LayerID, 8> {};
	struct VertexShaderField	: KeyField<ShaderID, 8> {};
	struct PixelShaderField		: KeyField<ShaderID, 8> {};
	//struct TextureField		: KeyField<TextureID, 10> {};
	struct MeshField			: KeyField<MeshID, 10> {};

	/** @brief: Rendering information from UPrimitiveComponent is packed into this type. */
	using RenderKeyType = uint64;

	/** @brief: The priority of information is handled at here. The lower one has higher priority. */
	using RenderKeyManager = KeyManager<
		RenderKeyType,		// #5.
		MeshField,			// #4.
		// TextureField,
		PixelShaderField,	// #3.
		VertexShaderField,	// #2.
		LayerField			// #1. 
	>;

	TArray<std::pair<RenderKeyType, UPrimitiveComponent*>> PrimitiveComponentArray;
    TArray<UTextholderComp*> TextholderComponentArray;
};
