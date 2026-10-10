#pragma once

#include "LocalLightComponent.h"

class UPointLightComponent : public ULocalLightComponent
{
	DECLARE_CLASS(UPointLightComponent, ULocalLightComponent)
	REFLECT_START(UPointLightComponent)
		PROPERTY(LightFalloffExponent)
	REFLECT_END()
public:
	UPointLightComponent();
	virtual ~UPointLightComponent() override;

	ELightType GetLightType() const { return ELightType::Point; }
	
	// spherical wireframe
	void DrawDebug(FLineBatcher* LineBatcher) const override;

	void FillLightData(FLightRenderData& Out) const override;

	float GetLightFalloffExponent() const { return LightFalloffExponent; }
	void SetLightFalloffExponent(float InLightFalloffExponent) { LightFalloffExponent = InLightFalloffExponent; }

protected:
private:
	float LightFalloffExponent;
};