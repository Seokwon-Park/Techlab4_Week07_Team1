#pragma once

#pragma once

#include "LocalLightComponent.h"

class URectLightComponent :public ULocalLightComponent
{
	DECLARE_CLASS(URectLightComponent, ULocalLightComponent)
	REFLECT_START(URectLightComponent)
		REFLECT_END()
public:
	URectLightComponent();
	virtual ~URectLightComponent() override;
private:
};