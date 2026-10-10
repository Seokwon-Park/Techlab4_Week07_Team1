#include "EnginePCH.h"
#include "RectLightComponent.h"

URectLightComponent::URectLightComponent() {}
URectLightComponent::~URectLightComponent() {}

void URectLightComponent::FillLightData(FLightRenderData& Out) const
{
    const FVector Position = GetWorldLocation();
    const FVector4 Color = GetLightColor().ToVector4();
    const FVector Right = FTransform(GetWorldRotation(), GetWorldLocation()).GetRight();
    const FVector Dir = FTransform(GetWorldRotation(), GetWorldLocation()).GetForward();


    Out.PositionRadius = FVector4(
        Position.X,
        Position.Y,
        Position.Z,
        std::max<float>(GetAttenuationRadius(), 0.001f)
    );

    Out.ColorIntensity = FVector4(
        Color.X,
        Color.Y,
        Color.Z,
        std::max<float>(GetIntensity(), 0.0f)
    );

    Out.DirectionType = FVector4(
        Dir.X,
        Dir.Y,
        Dir.Z,
        (float)ELightType::Rect
    );

    Out.Params = FVector4(
        1.0f,
        SourceHeight * 0.5f,       // 반높이
        0.0f,
        0.0f
    );

    Out.Extent = FVector4(
        Right.X,
        Right.Y,
        Right.Z,
        SourceWidth * 0.5f         // 반폭
    );
}
