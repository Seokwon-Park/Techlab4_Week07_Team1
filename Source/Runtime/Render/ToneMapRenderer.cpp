#include "EnginePCH.h"
#include "ToneMapRenderer.h"
#include "RenderCommand.h"
#include "RenderResourceManager.h"
#include "Texture2D.h"

bool FToneMapRenderer::Init()
{
	ToneMapShader = FRenderResourceManager::GetShaderProgram("Resources/Shader/ToneMapping.hlsl");
	if (!ToneMapShader)
	{
		HTR_LOG(Error, "[ToneMapping] shader not found");
		return false;
	}

	PipelineState.Shader = ToneMapShader;
	PipelineState.Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	// 결과로 덮어쓰는 후처리라 블렌딩,깊이 테스트가 필요 없음
	PipelineState.BlendState = EBlendState::Opaque;
	PipelineState.DepthStencilState = EDepthStencilState::Disabled;
	return true;
}

void FToneMapRenderer::OnRender(FTexture2D* InputTexture)
{
	// 셰이더가 실제로 샘플링하는 텍스처 크기로 한 픽셀의 UV 크기를 구한다
	const uint32 Width = InputTexture->GetWidth();
	const uint32 Height = InputTexture->GetHeight();
	if (Width == 0 || Height == 0)
		return;

	RenderCommand::BindPipelineState(PipelineState);
	RenderCommand::BindShaderResource(0, InputTexture, EShaderBindFlagBits::Pixel);
	RenderCommand::BindSamplerState(0, ESamplerState::LinearClamp, EShaderBindFlagBits::Pixel);

	RenderCommand::Draw(3);
}
