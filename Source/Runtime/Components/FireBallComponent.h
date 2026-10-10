#pragma once
#include "PointLightComponent.h"

class UFireBallComponent : public UPointLightComponent
{
	DECLARE_CLASS(UFireBallComponent, UPointLightComponent)

	REFLECT_START(ClassName)
		PROPERTY(Radius)
		PROPERTY(RadiusFalloff)
		REFLECT_END()

public:
	UFireBallComponent() = default;
	virtual ~UFireBallComponent() override;
	void InitializeComponent();

	float GetRadius() const { return Radius; }
	float GetRadiusFalloff() const { return RadiusFalloff; }

	ELightType GetLightType() const { return ELightType::Fireball; }
	void FillLightData(FLightRenderData& Out) const override;

private:
	float Intensity = 1.8f;
	float Radius = 6.5f;
	float RadiusFalloff = 3.0f;
	FVector4 LightColor = FVector4(1.0f, 0.0f, 0.8f, 1.0f);
};
