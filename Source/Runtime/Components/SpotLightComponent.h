#pragma once

#include "PointLightComponent.h"

class FLineBatcher;

class USpotLightComponent : public UPointLightComponent
{
	DECLARE_CLASS(USpotLightComponent, UPointLightComponent)

	REFLECT_START(ClassName)
		PROPERTY(InnerConeAngle)
		PROPERTY(OuterConeAngle)
		PROPERTY(Falloff)
	REFLECT_END()

public:
	USpotLightComponent() = default;
	virtual ~USpotLightComponent() override = default;

	void DrawDebug(FLineBatcher* LineBatcher) const override;
	ELightType GetLightType() const { return ELightType::Spot; }
	void FillLightData(FLightRenderData& Out) const override;

	float GetInnerConeAngle() const { return InnerConeAngle; }
	void SetInnerConeAngle(float InAngle) { InnerConeAngle = InAngle; }

	float GetOuterConeAngle() const { return OuterConeAngle; }
	void SetOuterConeAngle(float InAngle) { OuterConeAngle = InAngle; }

	float GetFalloff() const { return Falloff; }
	void SetFallOff(float InFalloff) { Falloff = InFalloff; }

private:
	// 원뿔 중심축 기준 반각(도). 
	float InnerConeAngle = 15.0f;
	float OuterConeAngle = 30.0f;

	// Inner에서 Outer로 가며 밝기가 떨어지는 정도.
	float Falloff = 1.0f;
};
