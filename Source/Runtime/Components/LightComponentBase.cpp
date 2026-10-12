#include "EnginePCH.h"
#include "LightComponentBase.h"
#include "../Engine/World.h"

ULightComponentBase::ULightComponentBase() = default;

ULightComponentBase::~ULightComponentBase() = default;

void ULightComponentBase::OnRegister() {
	Super::OnRegister();

	FLightRenderParameters& RenderParameters{ mLightProxy.GetRenderParameters() };
	RenderParameters = FLightRenderParameters{};
	RenderParameters.LightColor = mLightColor;
	RenderParameters.WorldPosition = GetWorldLocation();
	mLightProxy.SetIntensity(mIntensity);
	mLightProxy.SetVisible(mVisible);

	Super::GetWorld()->GetScene().RegisterLightProxy(&mLightProxy);
}

void ULightComponentBase::OnUnregister() {
	Super::OnUnregister();

	Super::GetWorld()->GetScene().UnregisterLightProxy(&mLightProxy);
}

void ULightComponentBase::Serialize(FStructuredArchive::FRecord Record) {
	Super::Serialize(Record);

	if (Record.GetUnderlyingArchive().IsLoading()) {
		const FProperty Property{};
		PostEditChangeProperty(Property);
		MarkTransformDirty();
	}
}

void ULightComponentBase::PostEditChangeProperty(const FProperty& Property) {
	Super::PostEditChangeProperty(Property);
	SetIntensity(mIntensity);
	SetLightColor(mLightColor);
	SetVisible(mVisible);

	mLightProxy.GetRenderParameters().LightColor = FVector4{ mLightColor.X, mLightColor.Y, mLightColor.Z, mLightColor.W };
	mLightProxy.GetRenderParameters().WorldPosition = GetWorldLocation();
	mLightProxy.SetIntensity(mIntensity);
	mLightProxy.SetVisible(mVisible);

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
