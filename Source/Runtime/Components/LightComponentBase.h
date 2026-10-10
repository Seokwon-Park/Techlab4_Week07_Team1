#pragma once

#include "SceneComponent.h"
#include "Math/Color.h"

enum class ELightType { Directional, Point, Spot, Rect, Fireball };

class ULightComponentBase : public USceneComponent
{
	DECLARE_CLASS(ULightComponentBase, USceneComponent)
	REFLECT_START(ULightComponentBase)
		PROPERTY(Intensity)
		// PROPERTY_TYPE(LightColor, Color)
		PROPERTY(bVisible)
	REFLECT_END()

public:
	ULightComponentBase();
	virtual ~ULightComponentBase() override;

	// Getter
	float GetIntensity() const { return Intensity; }
	FColor GetLightColor() const { return LightColor; }
	bool IsVisible() const { return bVisible; }

	// Setter
	void SetIntensity(float InIntensity) { Intensity = InIntensity; }
	void SetLightColor(FColor InLightColor) { LightColor = InLightColor; }
	void SetVisibility(bool bNewVisibility) { bVisible = bNewVisibility; }
	
private:
	float Intensity;
	FColor LightColor;
	bool bVisible;

};