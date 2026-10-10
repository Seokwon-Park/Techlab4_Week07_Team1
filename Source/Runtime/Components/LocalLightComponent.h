#pragma once

#include "LightComponent.h"

class ULocalLightComponent : public ULightComponent {
	DECLARE_CLASS(ULocalLightComponent, ULightComponent)

public:
	ULocalLightComponent();
	virtual ~ULocalLightComponent() override;

public:
	static void RegisterProperties(UClass* InClass);
	virtual void PostEditChangeProperty(const FProperty& Property) override;
	float GetAttenuationRadius() const;
	void SetAttenuationRadius(float InAttenuationRadius);
	float GetInvRadius() const;

private:
	float mAttenuationRadius{ 5.0f };
};
