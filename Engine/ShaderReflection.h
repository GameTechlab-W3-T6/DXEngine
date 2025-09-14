#pragma once

#include <cassert>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>
#include <string>
#include <vector>
#include <wrl/client.h>

#include "DynamicBuffer.h"
#include "UEngineStatics.h"

enum class EShaderType
{
	VertexShader,
	//GeometryShader
	PixelShader
};


// TODO: Change names

/**
 * @class UShaderReflection
 * @brief A utility class for performing reflection on compiled HLSL shaders using the D3D11 API.
 * * This class parses a compiled shader blob to extract information about its constant buffers,
 * input signature (for vertex shaders), and other resources. It automatically creates
 * `UBufferElementLayout` objects that match the shader's constant buffer and vertex buffer
 * structures, enabling a type-safe way to manage shader data on the CPU side.
 */
class UShaderReflection
{
private:
	/**
	 * @struct FConstantBufferInfo
	 * @brief Stores essential information about a constant buffer from shader reflection.
	 */
	struct FConstantBufferInfo
	{
		UINT Size;
		UINT BindPoint;
	};

public:
	/** @brief Default destructor. */
	~UShaderReflection() = default;

	UShaderReflection(ID3D11Device* Device, ID3DBlob* ShaderBlob, EShaderType InShaderType)
		: ShaderType(InShaderType)
	{
		/** @note A `NOTE` from the original code asks if `ComPtr` is needed and mentions that `ID3D11ShaderReflection` may not require an explicit release. However, `ComPtr` is generally the safest way to manage COM object lifetimes. `D3DReflect` automatically returns an interface with a reference count of 1. */
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> ShaderReflection;
		HRESULT hr = D3DReflect(
			ShaderBlob->GetBufferPointer(),
			ShaderBlob->GetBufferSize(),
			IID_PPV_ARGS(ShaderReflection.ReleaseAndGetAddressOf())
		);
		// TODO: Add HRESULT check for D3DReflect.

		if (InShaderType == EShaderType::VertexShader)
		{
			ReflectVertexShader(Device, ShaderBlob, ShaderReflection.Get());
		}

		D3D11_SHADER_DESC ShaderDesc;
		ShaderReflection->GetDesc(&ShaderDesc);

		for (UINT i = 0; i < ShaderDesc.ConstantBuffers; ++i)
		{
			ID3D11ShaderReflectionConstantBuffer*
				ShaderReflectionConstantBuffer = ShaderReflection->GetConstantBufferByIndex(i);

			D3D11_SHADER_BUFFER_DESC ShaderBufferDesc;
			ShaderReflectionConstantBuffer->GetDesc(&ShaderBufferDesc);

			/** @brief D3D11 buffer description for a constant buffer. */
			D3D11_BUFFER_DESC ConstantBufferDesc = {};
			ConstantBufferDesc.ByteWidth = ShaderBufferDesc.Size;
			ConstantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			ConstantBufferDesc.CPUAccessFlags = 0;
			ConstantBufferDesc.MiscFlags = 0;
			ConstantBufferDesc.StructureByteStride = 0;

			/** Constant Buffer */
			Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;
			Device->CreateBuffer(&ConstantBufferDesc, nullptr, ConstantBuffer.ReleaseAndGetAddressOf());
			ConstantBufferMap.try_emplace(ShaderBufferDesc.Name, ConstantBuffer);

			/** Constant Buffer Info */
			D3D11_SHADER_INPUT_BIND_DESC ShaderInputBindDesc;
			ShaderReflection->GetResourceBindingDescByName(ShaderBufferDesc.Name, &ShaderInputBindDesc);

			FConstantBufferInfo ConstantBufferInfo = {};
			ConstantBufferInfo.BindPoint = ShaderInputBindDesc.BindPoint;
			ConstantBufferInfo.Size = ShaderBufferDesc.Size;
			ConstantBufferInfoMap.try_emplace(ShaderBufferDesc.Name, ConstantBufferInfo);

			/** Constant Dynamic Buffer */
			UBufferElementLayout Layout;
			ReflectConstantBuffer(ShaderReflectionConstantBuffer, &Layout);
			ConstantDynamicBufferMap.try_emplace(ShaderBufferDesc.Name, std::move(Layout));
		}
	}

	/** @brief Deleted copy and move constructors to prevent unwanted object copies. */
	UShaderReflection(const UShaderReflection&) = delete;
	UShaderReflection(UShaderReflection&&) = delete;

	/** @brief Deleted copy and move assignment operators. */
	UShaderReflection& operator=(const UShaderReflection&) = delete;
	UShaderReflection& operator=(UShaderReflection&&) = delete;

public:
	void Bind(ID3D11DeviceContext* DeviceContext, const FString& Name)
	{
		const auto& ConstantBuffer = ConstantBufferMap[Name];
		const auto& ConstantBufferInfo = ConstantBufferInfoMap[Name];
		const auto& ConstantDynamicBuffer = ConstantDynamicBufferMap[Name];

		/** Update Constant Buffer */
		DeviceContext->UpdateSubresource(ConstantBuffer.Get(), 0, nullptr, ConstantDynamicBuffer.GetData(), 0, 0);

		switch (ShaderType)
		{
		case EShaderType::VertexShader:
			DeviceContext->VSSetConstantBuffers(ConstantBufferInfo.BindPoint, 1, ConstantBuffer.GetAddressOf());
			break;
		case EShaderType::PixelShader:
			DeviceContext->PSSetConstantBuffers(ConstantBufferInfo.BindPoint, 1, ConstantBuffer.GetAddressOf());
			break;
		default:
			assert(false && "Unsupported shader type.");
			break;
		}
	}

	/**
	 * @brief Gets a cloned `UBufferElementLayout` for the vertex buffer.
	 * @return A new `UBufferElementLayout` object.
	 */
	UBufferElementLayout GetVertexBufferElementLayout()
	{
		assert(ShaderType == EShaderType::VertexShader && "GetVertexBufferElementLayout can be invoked by Vertex Shader.");

		return VertexBufferElementLayout.Clone();
	}

	ID3D11InputLayout* GetInputLayout()
	{
		assert(ShaderType == EShaderType::VertexShader && "GetInputLayout can be invoked by Vertex Shader.");

		return InputLayout.Get();
	}

	/**
	 * @brief Gets a specific dynamic buffer by name.
	 * @param Name The name of the constant buffer.
	 * @return A reference to the `UDynamicBuffer` associated with the name.
	 * @pre The buffer must exist in the map.
	 */
	UDynamicBuffer& GetConstantDynamicBuffer(const FString& Name)
	{
		return ConstantDynamicBufferMap.at(Name);
	}

private:
	/**
	 * @brief Reflects a single constant buffer's variables and builds a `UBufferElementLayout`.
	 * @param ShaderReflectionConstantBuffer The constant buffer reflection interface.
	 * @param OutLayout A pointer to the `UBufferElementLayout` to be populated.
	 * @note This method handles primitive types and calls a recursive helper for structs.
	 */
	static void ReflectConstantBuffer(ID3D11ShaderReflectionConstantBuffer* ShaderReflectionConstantBuffer, UBufferElementLayout* OutLayout)
	{
		D3D11_SHADER_BUFFER_DESC ShaderBufferDesc;
		ShaderReflectionConstantBuffer->GetDesc(&ShaderBufferDesc);

		for (UINT i = 0; i < ShaderBufferDesc.Variables; ++i)
		{
			ID3D11ShaderReflectionVariable* ShaderReflectionVariable = ShaderReflectionConstantBuffer->GetVariableByIndex(i);
			D3D11_SHADER_VARIABLE_DESC ShaderVariableDesc;
			ShaderReflectionVariable->GetDesc(&ShaderVariableDesc);

			ID3D11ShaderReflectionType* ShaderReflectionType = ShaderReflectionVariable->GetType();

			/** @note A `TODO` from the original code asks whether to use `StartOffset` or `Offset`. `StartOffset` from `D3D11_SHADER_VARIABLE_DESC` is more appropriate for reflecting variables in a constant buffer. `Offset` in `D3D11_SHADER_TYPE_DESC` is for members within a struct. */
			ReflectConstantBufferVariable(ShaderReflectionType, ShaderVariableDesc.Name, ShaderVariableDesc.StartOffset, OutLayout);
		}
	}

	/**
	 * @brief Recursive helper function for constant buffer variable reflection.
	 * @param ShaderReflectionType The type reflection interface for the variable.
	 * @param Name The name of the variable.
	 * @param StartOffset The offset of the variable from the beginning of its parent struct/buffer.
	 * @param OutLayout A pointer to the `UBufferElementLayout` to be populated.
	 */
	static void ReflectConstantBufferVariable(ID3D11ShaderReflectionType* ShaderReflectionType, const FString& Name, size_t StartOffset, UBufferElementLayout* OutLayout)
	{
		D3D11_SHADER_TYPE_DESC ShaderTypeDesc;
		ShaderReflectionType->GetDesc(&ShaderTypeDesc);

		/** @note Be careful to check negative padding size. */
		size_t CurrentStride = OutLayout->GetCurrentStride();
		assert(StartOffset >= CurrentStride && "StartOffset should not be smaller than current stride.");
		size_t PaddingSize = StartOffset - CurrentStride;
		OutLayout->AppendPadding(PaddingSize);

		if (ShaderTypeDesc.Class == D3D_SVC_SCALAR)
		{
			switch (ShaderTypeDesc.Type)
			{
			case D3D_SVT_BOOL:
				OutLayout->Append<HLSL::EType::Bool>(Name);
				break;
			case D3D_SVT_INT:
				OutLayout->Append<HLSL::EType::Int>(Name);
				break;
			case D3D_SVT_FLOAT:
				OutLayout->Append<HLSL::EType::Float>(Name);
				break;
			default:
				assert(false && "Unsupported Scalar type");
				break;
			}
		}
		else if (ShaderTypeDesc.Class == D3D_SVC_VECTOR)
		{
			assert(ShaderTypeDesc.Rows == 1 && "HLSL Vectors should have 1 row.");
			if (ShaderTypeDesc.Columns == 2)
			{
				OutLayout->Append<HLSL::EType::Float2>(Name);
			}
			else if (ShaderTypeDesc.Columns == 3)
			{
				OutLayout->Append<HLSL::EType::Float3>(Name);
			}
			else if (ShaderTypeDesc.Columns == 4)
			{
				OutLayout->Append<HLSL::EType::Float4>(Name);
			}
			else
			{
				assert(false && "Unsupported Vector size");
			}
		}
		else if (ShaderTypeDesc.Class == D3D_SVC_MATRIX_ROWS || ShaderTypeDesc.Class == D3D_SVC_MATRIX_COLUMNS)
		{
			assert(ShaderTypeDesc.Rows == 4 && ShaderTypeDesc.Columns == 4 && "Unsupported Matrix Size");
			OutLayout->Append<HLSL::EType::Matrix>(Name);
		}
		else if (ShaderTypeDesc.Class == D3D_SVC_STRUCT)
		{
			UBufferElementLayout Layout;
			for (UINT i = 0; i < ShaderTypeDesc.Members; ++i)
			{
				const char* MemberName = ShaderReflectionType->GetMemberTypeName(i);
				ID3D11ShaderReflectionType* MemberShaderReflectionType = ShaderReflectionType->GetMemberTypeByIndex(i);
				D3D11_SHADER_TYPE_DESC MemberShaderTypeDesc;
				MemberShaderReflectionType->GetDesc(&MemberShaderTypeDesc);
				// Recursive call for struct members
				ReflectConstantBufferVariable(MemberShaderReflectionType, MemberName, MemberShaderTypeDesc.Offset, &Layout);
			}
			OutLayout->AppendStruct(Name, std::move(Layout));
		}
		else
		{
			assert(false && "Unsupported Data Class");
		}
	}

	/**
	 * @brief Reflects the input layout of a vertex shader and creates the corresponding `UBufferElementLayout`.
	 * @param Device A pointer to the D3D11 device.
	 * @param ShaderBlob A pointer to the compiled vertex shader data.
	 * @param ShaderReflection The shader reflection interface.
	 * @note This method also prepares the `D3D11_INPUT_ELEMENT_DESC` array for later use in creating an input layout.
	 */
	void ReflectVertexShader(ID3D11Device* Device, ID3DBlob* ShaderBlob, ID3D11ShaderReflection* ShaderReflection)
	{
		assert(ShaderType == EShaderType::VertexShader && "ReflectVertexShader can be invoked only for Vertex Shader.");

		D3D11_SHADER_DESC ShaderDesc;
		ShaderReflection->GetDesc(&ShaderDesc);

		TArray<D3D11_INPUT_ELEMENT_DESC> InputElementDescs;

		for (UINT i = 0; i < ShaderDesc.InputParameters; ++i)
		{
			D3D11_SIGNATURE_PARAMETER_DESC SignatureParameterDesc;
			ShaderReflection->GetInputParameterDesc(i, &SignatureParameterDesc);

			D3D11_INPUT_ELEMENT_DESC InputElementDesc = {};
			InputElementDesc.SemanticName = SignatureParameterDesc.SemanticName;
			InputElementDesc.SemanticIndex = SignatureParameterDesc.SemanticIndex;
			InputElementDesc.InputSlot = 0;
			InputElementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			InputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			InputElementDesc.InstanceDataStepRate = 0;

			FString ParameterName = SignatureParameterDesc.SemanticName + std::to_string(SignatureParameterDesc.SemanticIndex);
			if (SignatureParameterDesc.Mask == 1) /** 0b0001 One component */
			{
				if (SignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
				{
					/** @note The original code has a `TODO` about supporting unsigned integer types directly. Currently, `DXGI_FORMAT_R32_UINT` is used, which is correct for unsigned integers. */
					InputElementDesc.Format = DXGI_FORMAT_R32_UINT;
					VertexBufferElementLayout.Append<HLSL::EType::Int>(ParameterName);
				}
				else if (SignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
				{
					InputElementDesc.Format = DXGI_FORMAT_R32_SINT;
					VertexBufferElementLayout.Append<HLSL::EType::Int>(ParameterName);
				}
				else if (SignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					InputElementDesc.Format = DXGI_FORMAT_R32_FLOAT;
					VertexBufferElementLayout.Append<HLSL::EType::Float>(ParameterName);
				}
				else
				{
					assert(false && "Unsupported Type.");
				}
			}
			else if (SignatureParameterDesc.Mask <= 3) /** 0b0011 Two component */
			{
				if (SignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					InputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
					VertexBufferElementLayout.Append<HLSL::EType::Float2>(ParameterName);
				}
				else
				{
					assert(false && "Unsupported Type.");
				}
			}
			else if (SignatureParameterDesc.Mask <= 7) /** 0b0111 Three component */
			{
				if (SignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					InputElementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					VertexBufferElementLayout.Append<HLSL::EType::Float3>(ParameterName);
				}
				else
				{
					assert(false && "Unsupported Type.");
				}
			}
			else if (SignatureParameterDesc.Mask <= 15) /** 0b1111 Four component */
			{
				if (SignatureParameterDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
				{
					InputElementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					VertexBufferElementLayout.Append<HLSL::EType::Float4>(ParameterName);
				}
				else
				{
					assert(false && "Unsupported Type.");
				}
			}
			else
			{
				assert(false && "Unsupported component size.");
			}

			InputElementDescs.push_back(InputElementDesc);
		}

		VertexBufferElementLayout.Finalize();

		Device->CreateInputLayout(
			InputElementDescs.data(),
			static_cast<UINT>(InputElementDescs.size()),
			ShaderBlob->GetBufferPointer(),
			ShaderBlob->GetBufferSize(),
			InputLayout.ReleaseAndGetAddressOf()
		);
	}

private:

	/** @brief Shader type */
	EShaderType ShaderType;

	/** @brief Stores the layout for vertex buffer data. */
	UBufferElementLayout VertexBufferElementLayout;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout;

	/** @brief Maps of constant buffer resources and their associated information. */
	TMap<FString, Microsoft::WRL::ComPtr<ID3D11Buffer>> ConstantBufferMap;
	TMap<FString, FConstantBufferInfo> ConstantBufferInfoMap;
	TMap<FString, UDynamicBuffer> ConstantDynamicBufferMap;
};
