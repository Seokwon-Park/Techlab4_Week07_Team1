#pragma once

#include "SceneComponent.h"
#include "Math/Color.h"

class ULightComponentBase : public USceneComponent
{
	DECLARE_CLASS(ULightComponentBase, USceneComponent)
	REFLECT_START(ULightComponentBase)
		REFLECT_END()
public:
	ULightComponentBase();
	virtual ~ULightComponentBase() override;

private:
	float Intensity;
	FColor LightColor;
	bool bVisible;

};