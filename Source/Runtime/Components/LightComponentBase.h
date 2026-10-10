#pragma once
#include "SceneComponent.h"

class ULightComponentBase : public USceneComponent
{
public:
    DECLARE_CLASS(ULightComponentBase, USceneComponent)
	REFLECT_START(ULightComponentBase)
		PROPERTY(Intensity)
		PROPERTY(LightColor, Color)
	REFLECT_END()

    float GetIntensity() { return Intensity; }
    FVector4 GetLightColor() { return LightColor; }
    void SetIntensity(float InIntensity) { Intensity = InIntensity; }
    void SetLightColor(FVector4 InLightColor) { LightColor = InLightColor; }

private:
    float Intensity;
    FVector4 LightColor;
};