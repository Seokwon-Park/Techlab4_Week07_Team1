#pragma once
#include "PointLightComponent.h"

class UFireBallComponent : public UPointLightComponent
{
	DECLARE_CLASS(UFireBallComponent, UPointLightComponent)

	REFLECT_START(ClassName)
		PROPERTY(Intensity)
		PROPERTY(Radius)
		PROPERTY(RadiusFalloff)
		PROPERTY_TYPE(LightColor, LinearColor)
		REFLECT_END()

public:
	UFireBallComponent();
	virtual ~UFireBallComponent() override;

	ELightType GetLightType() const { return ELightType::Fireball; }

private:
};
