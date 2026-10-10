#pragma once 
#include "Math/Color.h"


struct FLightRenderParameters {
	FColor LightColor;
	FVector WorldPosition;
	FVector WorldDirection;

	float InvRadius;
	float FalloffExponent;

	float SourceRadius;
	float SoftSourceRadius;
	float SourceLength;

	FVector2 SpotAngles;

	float DiffuseScale;
	float SpecularScale;
};

class LightProxy {
public:
	LightProxy() = default;
	~LightProxy() = default;

public:
	FLightRenderParameters& GetRenderParameters() { return RenderParameters; }

private:
	FLightRenderParameters RenderParameters;
};