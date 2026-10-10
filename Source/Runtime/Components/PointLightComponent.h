#pragma once

#include "LocalLightComponent.h"

class UPointLightComponent : public ULocalLightComponent
{
	DECLARE_CLASS(UPointLightComponent, ULocalLightComponent)
	REFLECT_START(UPointLightComponent)
		REFLECT_END()
public:
	UPointLightComponent();
	virtual ~UPointLightComponent() override;
protected:
private:
	float AttenuationRadius;
	float LightFalloffExponent;
};