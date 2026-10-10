#pragma once
#include "Core/Types.h"
#include "Math/Vector4.h"

constexpr uint32 MaxLights = 16;

struct alignas(16) FLightRenderData
{
	FVector4 PositionRadius; 	// xyz: 위치, w: 감쇠 반경
	FVector4 ColorIntensity; 	// rgb: 색상, w: 강도
	FVector4 DirectionType;  	// xyz: 빛의 방향, w: ELightType (Dir:0, Point:1, Spot:2, Rect:3)
	FVector4 Params;         	// x: 감쇠지수, y: cos(Inner), z: cos(Outer), w: spot falloff
	FVector4 Extent;         	// Rect용 xyz: 오른쪽 축, w: 반폭
};

struct alignas(16) FLightConstants
{
	FLightRenderData Light[MaxLights];
	uint32 LightCount;
	uint32 Padding[3] = {};
};