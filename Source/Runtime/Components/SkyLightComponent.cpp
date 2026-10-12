#include "EnginePCH.h"
#include "SkyLightComponent.h"

USkyLightComponent::USkyLightComponent() {}
USkyLightComponent::~USkyLightComponent() {}

void USkyLightComponent::FillLightData(FLightRenderData& Out) const
{
    const FVector4 Color = GetLightColor().ToVector4();
    Out = {};

    Out.ColorIntensity = FVector4(
        Color.X,
        Color.Y,
        Color.Z,
        std::max<float>(GetIntensity(), 0.0f)
    );

    Out.DirectionType = FVector4(
        0.0f, 0.0f, 0.0f, (float)ELightType::Sky
    );

}
