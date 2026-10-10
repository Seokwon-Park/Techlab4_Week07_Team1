#pragma once

#include "LocalLightComponent.h"

class URectLightComponent : public ULocalLightComponent {
	DECLARE_CLASS(URectLightComponent, ULocalLightComponent)

	REFLECT_START(URectLightComponent)
		PROPERTY(mSourceWidth)
		PROPERTY(mSourceHeight)
		PROPERTY(mBarnDoorAngle)
		PROPERTY(mBarnDoorLength)
	REFLECT_END()

public:
	URectLightComponent();
	virtual ~URectLightComponent() override;

public:
	virtual void OnRegister() override;
	virtual void PostEditChangeProperty(const FProperty& Property) override;
	float GetSourceWidth() const;
	void SetSourceWidth(float InSourceWidth);
	float GetSourceHeight() const;
	void SetSourceHeight(float InSourceHeight);
	float GetBarnDoorAngle() const;
	void SetBarnDoorAngle(float InBarnDoorAngle);
	float GetBarnDoorLength() const;
	void SetBarnDoorLength(float InBarnDoorLength);
	virtual ELightType GetLightType() const override;
	virtual void DrawDebug(FLineBatcher* LineBatcher) const override;

private:
	float mSourceWidth{ 1.0f };
	float mSourceHeight{ 1.0f };
	float mBarnDoorAngle{ 88.0f };
	float mBarnDoorLength{ 0.2f };
};
