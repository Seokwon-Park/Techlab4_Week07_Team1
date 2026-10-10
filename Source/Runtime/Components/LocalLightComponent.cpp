#include "EnginePCH.h"
#include "LocalLightComponent.h"

ULocalLightComponent::ULocalLightComponent() = default;

ULocalLightComponent::~ULocalLightComponent() = default;

void ULocalLightComponent::OnRegister() {
	Super::OnRegister();

	mLightProxy.GetRenderParameters().InvRadius = GetInvRadius();
}

void ULocalLightComponent::PostEditChangeProperty(const FProperty& Property) {
	Super::PostEditChangeProperty(Property);
	SetAttenuationRadius(mAttenuationRadius);

	mLightProxy.GetRenderParameters().InvRadius = GetInvRadius();
}

float ULocalLightComponent::GetAttenuationRadius() const {
	return mAttenuationRadius;
}

void ULocalLightComponent::SetAttenuationRadius(float InAttenuationRadius) {
	mAttenuationRadius = FMath::Max(InAttenuationRadius, 0.0f);
}

float ULocalLightComponent::GetInvRadius() const {
	return mAttenuationRadius > 0.0f ? 1.0f / mAttenuationRadius : 0.0f;
}
