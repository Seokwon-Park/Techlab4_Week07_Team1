#pragma once

#include "LightComponent.h"

class ULocalLightComponent : public ULightComponent {
	DECLARE_CLASS(ULocalLightComponent, ULightComponent)

	REFLECT_START(ULocalLightComponent)
		PROPERTY(mAttenuationRadius)
	REFLECT_END()

public:
	ULocalLightComponent();
	virtual ~ULocalLightComponent() override;

public:
	virtual void OnRegister() override;
	virtual void PostEditChangeProperty(const FProperty& Property) override;
	float GetAttenuationRadius() const;
	void SetAttenuationRadius(float InAttenuationRadius);
	float GetInvRadius() const;

private:
	float mAttenuationRadius{ 5.0f };
};
