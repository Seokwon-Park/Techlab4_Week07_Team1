#pragma once

#include "Math/EngineMath.h"
#include "Math/Color.h"

struct FLightRenderParameters
{
	FVector Position;
	float  InvRadius;
	FLinearColor Color; // Color * Intensity
	float FalloffExponent;
	FVector Direction;
	float SourceRadius; // Radius Or Width
	FVector Tangent; // Right
	float SourceLength; // Height
	FVector2 SpotAngles;
	FVector2 Padding;
};

