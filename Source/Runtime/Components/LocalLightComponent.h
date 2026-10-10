#pragma once

#include "LightComponent.h"

class ULocalLightComponent :public ULightComponent
{
	DECLARE_CLASS(ULocalLightComponent, ULightComponent)
	REFLECT_START(ULocalLightComponent)
	REFLECT_END()
public:
	ULocalLightComponent();
	virtual ~ULocalLightComponent() override;

private:
};