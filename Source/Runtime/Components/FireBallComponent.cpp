#include "EnginePCH.h"
#include "FireBallComponent.h"
#include "Engine/World.h"

UFireBallComponent::UFireBallComponent()
{
    SetIntensity(1.8f);
    SetLightColor(FColor(1.0f, 0.0f, 0.8f, 1.0f));
    SetAttenuationRadius(6.5f);
}

UFireBallComponent::~UFireBallComponent() {}

void UFireBallComponent::FillLightData(FLightRenderData& Out) const
{
    
    const FVector Position = GetWorldLocation();
    const FVector4 Color = GetLightColor().ToVector4();

    Out.PositionRadius = FVector4(
        Position.X,
        Position.Y,
        Position.Z,
        std::max<float>(GetRadius(), 0.001f)
    );

    Out.ColorIntensity = FVector4(
        Color.X,
        Color.Y,
        Color.Z,
        std::max<float>(GetIntensity(), 0.0f)
    );

    Out.DirectionType = FVector4(
        0, 0, 0, (float)ELightType::Fireball
    );

    Out.Params = FVector4(
        std::max<float>(GetRadiusFalloff(), 0.001f),
        0, 0, 0
    );

    Out.Extent = FVector4(0, 0, 0, 0);
}