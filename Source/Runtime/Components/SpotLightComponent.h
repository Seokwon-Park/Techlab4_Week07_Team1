#pragma once

#include "PointLightComponent.h"

class USpotLightComponent : public UPointLightComponent {
	DECLARE_CLASS(USpotLightComponent, UPointLightComponent)

	REFLECT_START(USpotLightComponent)
		PROPERTY(mInnerConeAngle)
		PROPERTY(mOuterConeAngle)
	REFLECT_END()

public:
	USpotLightComponent();
	virtual ~USpotLightComponent() override;

public:
	virtual void OnRegister() override;
	virtual void PostEditChangeProperty(const FProperty& Property) override;
	float GetInnerConeAngle() const;
	void SetInnerConeAngle(float InInnerConeAngle);
	float GetOuterConeAngle() const;
	void SetOuterConeAngle(float InOuterConeAngle);
	virtual ELightType GetLightType() const override;
	virtual void DrawDebug(FLineBatcher* LineBatcher) const override;

private:
	float mInnerConeAngle{ 15.0f };
	float mOuterConeAngle{ 30.0f };
};
