#pragma once

#include "LightComponent.h"

class UDirctionalLightComponent :public ULightComponent
{
	DECLARE_CLASS(UDirctionalLightComponent, ULightComponent)
	REFLECT_START(UDirctionalLightComponent)
	REFLECT_END()
public:
	UDirctionalLightComponent();
	virtual ~UDirctionalLightComponent() override;

private:
};