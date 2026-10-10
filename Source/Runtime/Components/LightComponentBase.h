#pragma once

#include "SceneComponent.h"
#include "../Engine/LightProxy.h"

class ULightComponentBase : public USceneComponent {
	DECLARE_CLASS(ULightComponentBase, USceneComponent)

	REFLECT_START(ULightComponentBase)
		PROPERTY(mIntensity)
		PROPERTY_TYPE(mLightColor, Color)
		PROPERTY(mVisible)
	REFLECT_END()

public:
	ULightComponentBase();
	virtual ~ULightComponentBase() override;

	virtual void OnRegister() override;
	virtual void OnUnregister() override;
public:
	virtual void PostEditChangeProperty(const FProperty& Property) override;
	float GetIntensity() const;
	void SetIntensity(float InIntensity);
	const FVector4& GetLightColor() const;
	void SetLightColor(const FVector4& InLightColor);
	bool GetVisible() const;
	void SetVisible(bool InVisible);
	bool IsVisible() const;

protected:
	float mIntensity{ 1.0f };
	FVector4 mLightColor{};
	bool mVisible{ true };

	LightProxy mLightProxy; 
};
