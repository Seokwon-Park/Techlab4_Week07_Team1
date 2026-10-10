#include "EnginePCH.h"
#include "PointLightComponent.h"

#include "Render/LineBatcher.h"

UPointLightComponent::UPointLightComponent() : LightFalloffExponent(1.0f) {}
UPointLightComponent::~UPointLightComponent() {}

namespace
{
    constexpr int32 SPHERE_SEGMENT_COUNT = 32;

    void AddWireSphere(
        FLineBatcher* LineBatcher,
        const FVector& Center,
        const FVector& AxisA,
        const FVector& AxisB,
        float Radius,
        const FVector4& Color
    )
    {
        FVector Prev = Center + AxisA * Radius;
        for (int32 i = 1; i < SPHERE_SEGMENT_COUNT; ++i)
        {
            const float Angle = 2 * PI * i / SPHERE_SEGMENT_COUNT;
            const FVector Point = Center + AxisA * (std::cosf(Angle)*Radius) + AxisB * (std::sinf(Angle)*Radius);
            LineBatcher->AddLine(Prev, Point, Color);
            Prev = Point;
        }
    }
}

void UPointLightComponent::DrawDebug(FLineBatcher* LineBatcher) const 
{
    // AttenuationRadius 반경의 원 3개(XY, XZ, YZ)
    if (LineBatcher == nullptr) return;

    const FVector Center = GetWorldLocation();
    float Radius = (GetAttenuationRadius() > 0.0f)? GetAttenuationRadius() : 0.0f;

    AddWireSphere(LineBatcher, Center, FVector(1,0,0), FVector(0,0,1), Radius, GetLightColor().ToVector4());
    AddWireSphere(LineBatcher, Center, FVector(0,1,0), FVector(0,0,1), Radius, GetLightColor().ToVector4());
    AddWireSphere(LineBatcher, Center, FVector(1,0,0), FVector(0,1,0), Radius, GetLightColor().ToVector4());
}

void UPointLightComponent::FillLightData(FLightRenderData& Out) const
{
    const FVector Position = GetWorldLocation();
    const FVector4 Color = GetLightColor().ToVector4();

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
        0.0f, 0.0f, 0.0f, (float)ELightType::Point
    );

    Out.Params = FVector4(
        std::max<float>(GetLightFalloffExponent(), 0.001f),
        0.0f, 0.0f, 0.0f);

    Out.Extent = FVector4(0.0f, 0.0f, 0.0f, 0.0f);
}
