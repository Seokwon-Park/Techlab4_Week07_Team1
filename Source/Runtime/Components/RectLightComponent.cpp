#include "EnginePCH.h"
#include "RectLightComponent.h"

#include "Engine/RectLightSceneProxy.h"

URectLightComponent::URectLightComponent() {}
URectLightComponent::~URectLightComponent() {}

FLightSceneProxy* URectLightComponent::CreateSceneProxy() const
{
    return new FRectLightSceneProxy(this);
}
