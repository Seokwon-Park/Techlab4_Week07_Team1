#pragma once

#include "LightComponent.h"

class UDirectionalLightComponent : public ULightComponent {
	DECLARE_CLASS(UDirectionalLightComponent, ULightComponent)

	REFLECT_START(UDirectionalLightComponent)
		PROPERTY(mSourceAngle)
		PROPERTY(mSoftSourceAngle)
	REFLECT_END()

public:
	UDirectionalLightComponent();
	virtual ~UDirectionalLightComponent() override;

public:
	virtual void OnRegister() override;
	virtual void PostEditChangeProperty(const FProperty& Property) override;
	float GetSourceAngle() const;
	void SetSourceAngle(float InSourceAngle);
	float GetSoftSourceAngle() const;
	void SetSoftSourceAngle(float InSoftSourceAngle);
	virtual ELightType GetLightType() const override;
	virtual void DrawDebug(FLineBatcher* LineBatcher) const override;

private:
	float mSourceAngle{ 0.5357f };
	float mSoftSourceAngle{ 0.0f };
};
