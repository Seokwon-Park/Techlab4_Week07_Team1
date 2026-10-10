#pragma once

#pragma once

#include "LocalLightComponent.h"

class URectLightComponent :public ULocalLightComponent
{
	DECLARE_CLASS(URectLightComponent, ULocalLightComponent)
	REFLECT_START(URectLightComponent)
		PROPERTY(SourceWidth)
		PROPERTY(SourceHeight)
	REFLECT_END()
public:
	URectLightComponent();
	virtual ~URectLightComponent() override;

	ELightType GetLightType() const { return ELightType::Rect; }
	void FillLightData(FLightRenderData& Out) const override;

	float GetSourceWidth() const { return SourceWidth; }
	float GetSourceHeight() const { return SourceHeight; }
	void SetSourceWidth(float InSourceWidth) { SourceWidth = InSourceWidth; }
	void SetSourceHeight(float InSourceHeight) { SourceHeight = InSourceHeight; }

private:
	// 발광면 가로 크기
	float SourceWidth;
	// 발광면 세로 크기
	float SourceHeight;
};