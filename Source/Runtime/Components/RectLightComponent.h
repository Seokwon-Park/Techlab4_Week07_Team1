#pragma once

#include "LocalLightComponent.h"

class URectLightComponent :public ULocalLightComponent
{
	DECLARE_CLASS(URectLightComponent, ULocalLightComponent)
	REFLECT_START(URectLightComponent)
		PROPERTY(HalfWidth)
		PROPERTY(HalfHeight)
	REFLECT_END()
public:
	URectLightComponent();
	virtual ~URectLightComponent() override;
	ELightType GetLightType() const { return ELightType::Rect; }
	float GetWidth() const;
	float GetHeight() const;

private:
	float HalfWidth;
	float HalfHeight;

};