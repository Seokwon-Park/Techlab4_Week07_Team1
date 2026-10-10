#include "EnginePCH.h"
#include "LightComponent.h"
#include "Engine/World.h"

ULightComponent::ULightComponent() {}
ULightComponent::~ULightComponent() {}
void ULightComponent::DrawDebug(FLineBatcher* LineBatcher) const {}

void ULightComponent::CreateRenderState()
{
    Super::CreateRenderState();
    GetWorld()->GetScene().RegisterLight(this);
}

void ULightComponent::DestroyRenderState()
{
    GetWorld()->GetScene().UnregisterLight(this);
	Super::DestroyRenderState();
}