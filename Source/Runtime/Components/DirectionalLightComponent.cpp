#include "EnginePCH.h"
#include "DirectionalLightComponent.h"

UDirectionalLightComponent::UDirectionalLightComponent() {}
UDirectionalLightComponent::~UDirectionalLightComponent() {}

void UDirectionalLightComponent::FillLightData(FLightRenderData& Out) const
{
    const FVector Dir = FTransform(GetWorldRotation(), GetWorldLocation()).GetForward();
    const FVector4 Color = GetLightColor().ToVector4();

    Out.PositionRadius = FVector4(0, 0, 0, 0);
    Out.ColorIntensity = FVector4(
        Color.X,
        Color.Y,
        Color.Z,
        std::max<float>(GetIntensity(), 0.0f)
    );

    Out.DirectionType = FVector4(Dir.X, Dir.Y, Dir.Z, (float)ELightType::Directional);
    Out.Params = FVector4(0, 0, 0, 0);
    Out.Extent = FVector4(0, 0, 0, 0);
}