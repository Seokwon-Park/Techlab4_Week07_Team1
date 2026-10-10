#pragma once

#include "LocalLightComponent.h"

class UPointLightComponent : public ULocalLightComponent {
	DECLARE_CLASS(UPointLightComponent, ULocalLightComponent)

	REFLECT_START(UPointLightComponent)
		PROPERTY(mLightFalloffExponent)
		PROPERTY(mSourceRadius)
		PROPERTY(mSoftSourceRadius)
		PROPERTY(mSourceLength)
	REFLECT_END()

public:
	UPointLightComponent();
	virtual ~UPointLightComponent() override;

public:
	virtual void OnRegister() override;
	virtual void PostEditChangeProperty(const FProperty& Property) override;
	float GetLightFalloffExponent() const;
	void SetLightFalloffExponent(float InLightFalloffExponent);
	float GetSourceRadius() const;
	void SetSourceRadius(float InSourceRadius);
	float GetSoftSourceRadius() const;
	void SetSoftSourceRadius(float InSoftSourceRadius);
	float GetSourceLength() const;
	void SetSourceLength(float InSourceLength);
	float GetFalloff() const;
	void SetFalloff(float InFalloff);
	virtual ELightType GetLightType() const override;
	virtual void DrawDebug(FLineBatcher* LineBatcher) const override;

private:
	float mLightFalloffExponent{ 1.0f };
	float mSourceRadius{ 0.0f };
	float mSoftSourceRadius{ 0.0f };
	float mSourceLength{ 0.0f };
};
