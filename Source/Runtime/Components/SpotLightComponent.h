#pragma once

#include "PointLightComponent.h"

class USpotLightComponent : public UPointLightComponent {
	DECLARE_CLASS(USpotLightComponent, UPointLightComponent)

public:
	USpotLightComponent();
	virtual ~USpotLightComponent() override;

public:
	static void RegisterProperties(UClass* InClass);
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
