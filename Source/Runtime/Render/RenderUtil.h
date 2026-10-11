#pragma once

#include "RenderEnums.h"

// "Lighting_model", "1"
using FShaderDefines = std::vector<std::pair<FString, FString>>;
struct FShaderByteCode
{
	ComPtr<ID3DBlob> Blob;

	const void* GetData() const { return Blob ? Blob->GetBufferPointer() : nullptr; }
	SIZE_T      GetSize() const { return Blob ? Blob->GetBufferSize() : 0; }
	bool        IsValid() const { return Blob != nullptr; }
};

class RenderUtil
{
public:
	static FShaderByteCode CompileShader(FString Path, const char* EntryPoint, EShaderType ShaderType, const FShaderDefines& Defines);
	static void SaveByteCode(FString TargetPath, const FShaderByteCode& ByteCode);
	static FShaderByteCode LoadByteCode(const FString& CsoPath);

	static FString GetCsoPath(const FString& ShaderPath, EShaderType ShaderType, const FString& Suffix);
	static bool IsCsoUpToDate(const FString& ShaderPath, const FString& CsoPath);
	static FShaderByteCode GetOrCompile(const FString& ShaderPath, const char* EntryPoint, EShaderType ShaderType,
										const FString& Suffix, const FShaderDefines& Defines, FString& OutCSOPath);
};