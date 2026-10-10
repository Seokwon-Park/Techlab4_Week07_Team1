#include "EnginePCH.h"
#include "PointLightComponent.h"

UPointLightComponent::UPointLightComponent() : LightFalloffExponent(1.0f) {}
UPointLightComponent::~UPointLightComponent() {}

void UPointLightComponent::DrawDebug(FLineBatcher* LineBatcher) const 
{
    // 월드 위치 중심, AttenuationRadius 반경의 원 3개(XY, XZ, YZ)
    // LineBatcher->AddLine SpotLight 참고
    
}
