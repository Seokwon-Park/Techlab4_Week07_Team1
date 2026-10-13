#pragma once

#include "LocalLightComponent.h"

class URectLightComponent :public ULocalLightComponent
{
	DECLARE_CLASS(URectLightComponent, ULocalLightComponent)
	REFLECT_START(URectLightComponent)
		PROPERTY(HalfWidth)
		PROPERTY(HalfHeight)
		PROPERTY(BarnDoorAngle)
		PROPERTY(BarnDoorLength)
	REFLECT_END()
public:
	URectLightComponent();
	virtual ~URectLightComponent() override;
	ELightType GetLightType() const { return ELightType::Rect; }
	void DrawDebug(FLineBatcher* LineBatcher) const;

	// Getter
	float GetWidth() const { return HalfWidth * 2.0f; }
	float GetHeight() const { return HalfHeight * 2.0f; }
	float GetBarnDoorAngle() const { return BarnDoorAngle; }
	float GetBarnDoorLength() const { return BarnDoorLength; }
	float SetBarnDoorAngle(float InAngle) { BarnDoorAngle = std::max(0.0f, InAngle); }
	float SetBarnDoorLength(float InLength) { BarnDoorLength = std::clamp(InLength, 0.0f, 90.0f); }

private:
	float HalfWidth = 6.0f;
	float HalfHeight = 6.0f;
	float BarnDoorAngle = 88.0f;	// 도 단위
	float BarnDoorLength = 4.0f;	

};