#include "EnginePCH.h"
#include "LightComponent.h"

ULightComponent::ULightComponent() = default;

ULightComponent::~ULightComponent() = default;

void ULightComponent::RegisterProperties(UClass* InClass) {
	InClass->AddProperty<bool>("CastShadows", offsetof(ULightComponent, mCastShadows));
	InClass->AddProperty<float>("DiffuseScale", offsetof(ULightComponent, mDiffuseScale));
	InClass->AddProperty<float>("SpecularScale", offsetof(ULightComponent, mSpecularScale));
}

void ULightComponent::PostEditChangeProperty(const FProperty& Property) {
	Super::PostEditChangeProperty(Property);
	SetCastShadows(mCastShadows);
	SetDiffuseScale(mDiffuseScale);
	SetSpecularScale(mSpecularScale);
}

bool ULightComponent::GetCastShadows() const {
	return mCastShadows;
}

void ULightComponent::SetCastShadows(bool InCastShadows) {
	mCastShadows = InCastShadows;
}

float ULightComponent::GetDiffuseScale() const {
	return mDiffuseScale;
}

void ULightComponent::SetDiffuseScale(float InDiffuseScale) {
	mDiffuseScale = FMath::Max(InDiffuseScale, 0.0f);
}

float ULightComponent::GetSpecularScale() const {
	return mSpecularScale;
}

void ULightComponent::SetSpecularScale(float InSpecularScale) {
	mSpecularScale = FMath::Max(InSpecularScale, 0.0f);
}

FVector ULightComponent::GetLightDirection() const {
	return GetWorldRotation().Quaternion().GetForwardVector();
}
