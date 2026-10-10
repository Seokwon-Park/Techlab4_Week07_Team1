#include "EnginePCH.h"
#include "LightComponentBase.h"

ULightComponentBase::ULightComponentBase() = default;

ULightComponentBase::~ULightComponentBase() = default;

void ULightComponentBase::RegisterProperties(UClass* InClass) {
	InClass->AddProperty<float>("Intensity", offsetof(ULightComponentBase, mIntensity));
	InClass->AddProperty<FVector4>("LightColor", offsetof(ULightComponentBase, mLightColor), EPropertyType::Color);
	InClass->AddProperty<bool>("bVisible", offsetof(ULightComponentBase, mVisible));
}

void ULightComponentBase::PostEditChangeProperty(const FProperty& Property) {
	Super::PostEditChangeProperty(Property);
	SetIntensity(mIntensity);
	SetLightColor(mLightColor);
	SetVisible(mVisible);
}

float ULightComponentBase::GetIntensity() const {
	return mIntensity;
}

void ULightComponentBase::SetIntensity(float InIntensity) {
	mIntensity = FMath::Max(InIntensity, 0.0f);
}

const FVector4& ULightComponentBase::GetLightColor() const {
	return mLightColor;
}

void ULightComponentBase::SetLightColor(const FVector4& InLightColor) {
	mLightColor = FVector4{ FMath::Max(InLightColor.X, 0.0f), FMath::Max(InLightColor.Y, 0.0f), FMath::Max(InLightColor.Z, 0.0f), FMath::Clamp(InLightColor.W, 0.0f, 1.0f) };
}

bool ULightComponentBase::GetVisible() const {
	return mVisible;
}

void ULightComponentBase::SetVisible(bool InVisible) {
	mVisible = InVisible;
}

bool ULightComponentBase::IsVisible() const {
	return mVisible;
}
