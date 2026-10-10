#pragma once

#include "LightComponentBase.h"

class ULightComponent : public ULightComponentBase
{
	DECLARE_CLASS(ULightComponent, ULightComponentBase)
	REFLECT_START(ULightComponent)
	REFLECT_END()
public:
	ULightComponent();
	virtual ~ULightComponent() override;
protected:
private:
	float MaxDrawDistance;
	float SpecularScale;
	float DiffuseScale;
	

};