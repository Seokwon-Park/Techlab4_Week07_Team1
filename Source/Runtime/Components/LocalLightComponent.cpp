#include "EnginePCH.h"
#include "LocalLightComponent.h"

ULocalLightComponent::ULocalLightComponent() = default;

ULocalLightComponent::~ULocalLightComponent() = default;

void ULocalLightComponent::RegisterProperties(UClass* InClass) {
	InClass->AddProperty<float>("AttenuationRadius", offsetof(ULocalLightComponent, mAttenuationRadius));
}

void ULocalLightComponent::PostEditChangeProperty(const FProperty& Property) {
	Super::PostEditChangeProperty(Property);
	SetAttenuationRadius(mAttenuationRadius);
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
