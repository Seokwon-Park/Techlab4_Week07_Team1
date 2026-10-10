#include "EnginePCH.h"
#include "FireBallComponent.h"
#include "Engine/World.h"

void UFireBallComponent::InitializeComponent()
{
    Super::InitializeComponent();

    if (AActor* Owner = GetOwner())
    {
        if (UWorld* World = Owner->GetWorld())
        {
            World->GetScene().RegisterFireBall(this);
        }
    }
}

UFireBallComponent::~UFireBallComponent()
{
    if (AActor* Owner = GetOwner())
    {
        if (UWorld* World = Owner->GetWorld())
        {
            World->GetScene().UnregisterFireBall(this);
        }
    }
}

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
        std::max<float>(GetRadiusFalloff(), 0.001f),
        1.0f,
        0.0f,
        (float)ELightType::Fireball
    );

    Out.Params = FVector4(
        std::max<float>(GetRadiusFalloff(), 0.001f),
        0, 0, 0
    );

    Out.Extent = FVector4(0, 0, 0, 0);
}