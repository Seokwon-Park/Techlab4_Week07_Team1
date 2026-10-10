#pragma once

#include "SceneComponent.h"

class ULightComponentBase : public USceneComponent {
	DECLARE_CLASS(ULightComponentBase, USceneComponent)

public:
	ULightComponentBase();
	virtual ~ULightComponentBase() override;

public:
	static void RegisterProperties(UClass* InClass);
	virtual void PostEditChangeProperty(const FProperty& Property) override;
	float GetIntensity() const;
	void SetIntensity(float InIntensity);
	const FVector4& GetLightColor() const;
	void SetLightColor(const FVector4& InLightColor);
	bool GetVisible() const;
	void SetVisible(bool InVisible);
	bool IsVisible() const;

private:
	float mIntensity{ 1.0f };
	FVector4 mLightColor{ 1.0f, 1.0f, 1.0f, 1.0f };
	bool mVisible{ true };
};
