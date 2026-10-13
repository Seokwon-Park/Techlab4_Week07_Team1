#pragma once
#include "Render/PipelineState.h"

class FTexture2D;

class FToneMapRenderer
{
public:
	FToneMapRenderer() = default;
	~FToneMapRenderer() = default;

	bool Init();
	void OnRender(FTexture2D* InputTexture);

	FToneMapRenderer(const FToneMapRenderer&) = delete;
	FToneMapRenderer& operator=(const FToneMapRenderer&) = delete;


private:
	FShaderProgram* ToneMapShader = nullptr;
	FPipelineState PipelineState;
};
