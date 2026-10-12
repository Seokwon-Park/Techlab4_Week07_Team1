#pragma once 
#include "Math/Color.h"

enum class ELightType {
	Directional,
	Point,
	Spot,
	Rect
};

struct FLightRenderParameters {
	FVector4 LightColor;
	FVector WorldPosition;
	float InvRadius;
	
	FVector WorldDirection;
	float FalloffExponent;

	float SourceRadius;
	float SoftSourceRadius;
	float SourceLength;
	float DiffuseScale;

	FVector2 SpotAngles;
	float SpecularScale;
	float Padding;
};

class LightProxy {
public:
	LightProxy() = default;
	~LightProxy() = default;

public:
	FLightRenderParameters& GetRenderParameters();
	ELightType GetLightType() const;
	void SetLightType(ELightType InLightType);
	float GetIntensity() const;
	void SetIntensity(float InIntensity);
	bool IsVisible() const;
	void SetVisible(bool InVisible);

private:
	FLightRenderParameters mRenderParameters{};
	ELightType mLightType{ ELightType::Directional };
	float mIntensity{ 1.0f };
	bool mVisible{ true };
};
