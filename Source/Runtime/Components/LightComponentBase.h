#pragma once

#include "SceneComponent.h"
#include "Math/Color.h"

class ULightComponentBase : public USceneComponent
{
	DECLARE_CLASS(ULightComponentBase, USceneComponent)
	REFLECT_START(ULightComponentBase)
		PROPERTY(Intensity)
		PROPERTY(LightColor)
		PROPERTY(bVisible)
	REFLECT_END()
public:
	ULightComponentBase();
	virtual ~ULightComponentBase() override;


	float GetIntensity() const { return Intensity; }
	FColor GetLightColor() const { return LightColor; }
	void SetLightColor(const FColor& InColor) { LightColor = InColor; }
	bool IsVisible() const { return bVisible; }

protected:
	float Intensity = 1.0f;
	FColor LightColor = FColor(255, 255, 255, 255);
	bool bVisible = true;

private:

};