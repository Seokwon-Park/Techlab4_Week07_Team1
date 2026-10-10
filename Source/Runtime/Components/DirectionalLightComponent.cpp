#include "EnginePCH.h"
#include "DirectionalLightComponent.h"
#include "Render/LineBatcher.h"

UDirectionalLightComponent::UDirectionalLightComponent() = default;

UDirectionalLightComponent::~UDirectionalLightComponent() = default;

void UDirectionalLightComponent::RegisterProperties(UClass* InClass) {
	InClass->AddProperty<float>("SourceAngle", offsetof(UDirectionalLightComponent, mSourceAngle));
	InClass->AddProperty<float>("SoftSourceAngle", offsetof(UDirectionalLightComponent, mSoftSourceAngle));
}

void UDirectionalLightComponent::PostEditChangeProperty(const FProperty& Property) {
	Super::PostEditChangeProperty(Property);
	SetSourceAngle(mSourceAngle);
	SetSoftSourceAngle(mSoftSourceAngle);
}

float UDirectionalLightComponent::GetSourceAngle() const {
	return mSourceAngle;
}

void UDirectionalLightComponent::SetSourceAngle(float InSourceAngle) {
	mSourceAngle = FMath::Clamp(InSourceAngle, 0.0f, 180.0f);
}

float UDirectionalLightComponent::GetSoftSourceAngle() const {
	return mSoftSourceAngle;
}

void UDirectionalLightComponent::SetSoftSourceAngle(float InSoftSourceAngle) {
	mSoftSourceAngle = FMath::Clamp(InSoftSourceAngle, 0.0f, 180.0f);
}

ELightType UDirectionalLightComponent::GetLightType() const {
	return ELightType::Directional;
}

void UDirectionalLightComponent::DrawDebug(FLineBatcher* LineBatcher) const {
	if (LineBatcher == nullptr || !IsVisible()) {
		return;
	}

	const FVector Start{ GetWorldLocation() };
	const FVector Forward{ GetLightDirection() };
	const FVector Right{ GetWorldRotation().Quaternion().GetRightVector() };
	const FVector End{ Start + Forward };
	const FVector4& Color{ GetLightColor() };
	LineBatcher->AddLine(Start, End, Color);
	LineBatcher->AddLine(End, End - Forward * 0.25f + Right * 0.15f, Color);
	LineBatcher->AddLine(End, End - Forward * 0.25f - Right * 0.15f, Color);
}
