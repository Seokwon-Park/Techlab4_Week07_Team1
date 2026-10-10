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
	UFireBallComponent();
	virtual ~UFireBallComponent() override;

	float GetRadius() const { return Radius; }
	float GetRadiusFalloff() const { return RadiusFalloff; }

	ELightType GetLightType() const { return ELightType::Fireball; }
	void FillLightData(FLightRenderData& Out) const override;

private:
	float RadiusFalloff = 3.0f;
	float Radius = 5.0f;
};
