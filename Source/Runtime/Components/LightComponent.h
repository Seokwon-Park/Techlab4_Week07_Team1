#pragma once

#include "LightComponentBase.h"

class FLineBatcher;

enum class ELightType {
	Directional,
	Point,
	Spot,
	Rect
};

class ULightComponent : public ULightComponentBase {
	DECLARE_CLASS(ULightComponent, ULightComponentBase)

	REFLECT_START(ULightComponent)
		PROPERTY(mCastShadows)
		PROPERTY(mDiffuseScale)
		PROPERTY(mSpecularScale)
	REFLECT_END()

public:
	ULightComponent();
	virtual ~ULightComponent() override;

public:
	virtual void OnRegister() override;
	virtual void PostEditChangeProperty(const FProperty& Property) override;
	bool GetCastShadows() const;
	void SetCastShadows(bool InCastShadows);
	float GetDiffuseScale() const;
	void SetDiffuseScale(float InDiffuseScale);
	float GetSpecularScale() const;
	void SetSpecularScale(float InSpecularScale);
	FVector GetLightDirection() const;
	virtual ELightType GetLightType() const = 0;
	virtual void DrawDebug(FLineBatcher* LineBatcher) const = 0;

private:
	bool mCastShadows{ true };
	float mDiffuseScale{ 1.0f };
	float mSpecularScale{ 1.0f };
};
