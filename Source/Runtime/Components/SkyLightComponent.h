#pragma once

#include "LocalLightComponent.h"

class USkyLightComponent : public ULocalLightComponent
{
	DECLARE_CLASS(USkyLightComponent, ULocalLightComponent)
	REFLECT_START(USkyLightComponent)
	REFLECT_END()
public:
	USkyLightComponent();
	virtual ~USkyLightComponent() override;

	ELightType GetLightType() const { return ELightType::Sky; }
	
	// spherical wireframe
	void DrawDebug(FLineBatcher* LineBatcher) const override;

	void FillLightData(FLightRenderData& Out) const override;

protected:
private:
};