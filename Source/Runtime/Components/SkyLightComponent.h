#pragma once

#include "LightComponent.h"

class USkyLightComponent : public ULightComponent
{
	DECLARE_CLASS(USkyLightComponent, ULightComponent)
	REFLECT_START(USkyLightComponent)
	REFLECT_END()
public:
	USkyLightComponent();
	virtual ~USkyLightComponent() override;

	ELightType GetLightType() const { return ELightType::Sky; }
	
	void FillLightData(FLightRenderData& Out) const override;

protected:
private:
};