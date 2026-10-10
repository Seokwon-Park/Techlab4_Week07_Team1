#include "EnginePCH.h"
#include "PointLightComponent.h"
#include "Render/LineBatcher.h"
#include <cmath>

UPointLightComponent::UPointLightComponent() = default;

UPointLightComponent::~UPointLightComponent() = default;

void UPointLightComponent::OnRegister() {
	Super::OnRegister();

	FLightRenderParameters& RenderParameters{ mLightProxy.GetRenderParameters() };
	RenderParameters.FalloffExponent = mLightFalloffExponent;
	RenderParameters.SourceRadius = mSourceRadius;
	RenderParameters.SoftSourceRadius = mSoftSourceRadius;
	RenderParameters.SourceLength = mSourceLength;
}

void UPointLightComponent::PostEditChangeProperty(const FProperty& Property) {
	Super::PostEditChangeProperty(Property);
	SetLightFalloffExponent(mLightFalloffExponent);
	SetSourceRadius(mSourceRadius);
	SetSoftSourceRadius(mSoftSourceRadius);
	SetSourceLength(mSourceLength);

	FLightRenderParameters& RenderParameters{ mLightProxy.GetRenderParameters() };
	RenderParameters.FalloffExponent = mLightFalloffExponent;
	RenderParameters.SourceRadius = mSourceRadius;
	RenderParameters.SoftSourceRadius = mSoftSourceRadius;
	RenderParameters.SourceLength = mSourceLength;
}

float UPointLightComponent::GetLightFalloffExponent() const {
	return mLightFalloffExponent;
}

void UPointLightComponent::SetLightFalloffExponent(float InLightFalloffExponent) {
	mLightFalloffExponent = FMath::Max(InLightFalloffExponent, 0.0f);
}

float UPointLightComponent::GetSourceRadius() const {
	return mSourceRadius;
}

void UPointLightComponent::SetSourceRadius(float InSourceRadius) {
	mSourceRadius = FMath::Max(InSourceRadius, 0.0f);
}

float UPointLightComponent::GetSoftSourceRadius() const {
	return mSoftSourceRadius;
}

void UPointLightComponent::SetSoftSourceRadius(float InSoftSourceRadius) {
	mSoftSourceRadius = FMath::Max(InSoftSourceRadius, 0.0f);
}

float UPointLightComponent::GetSourceLength() const {
	return mSourceLength;
}

void UPointLightComponent::SetSourceLength(float InSourceLength) {
	mSourceLength = FMath::Max(InSourceLength, 0.0f);
}

float UPointLightComponent::GetFalloff() const {
	return GetLightFalloffExponent();
}

void UPointLightComponent::SetFalloff(float InFalloff) {
	SetLightFalloffExponent(InFalloff);
}

ELightType UPointLightComponent::GetLightType() const {
	return ELightType::Point;
}

void UPointLightComponent::DrawDebug(FLineBatcher* LineBatcher) const {
	if (LineBatcher == nullptr || !IsVisible() || GetAttenuationRadius() <= 0.0f) {
		return;
	}

	const FVector Center{ GetWorldLocation() };
	const float Radius{ GetAttenuationRadius() };
	const FVector4& Color{ GetLightColor() };
	constexpr int32 SegmentCount{ 24 };
	FVector PreviousXY{ Center + FVector{ Radius, 0.0f, 0.0f } };
	FVector PreviousXZ{ PreviousXY };
	FVector PreviousYZ{ Center + FVector{ 0.0f, Radius, 0.0f } };
	for (int32 Index{ 1 }; Index <= SegmentCount; ++Index) {
		const float Angle{ 2.0f * PI * static_cast<float>(Index) / static_cast<float>(SegmentCount) };
		const float Cosine{ std::cos(Angle) * Radius };
		const float Sine{ std::sin(Angle) * Radius };
		const FVector PointXY{ Center + FVector{ Cosine, Sine, 0.0f } };
		const FVector PointXZ{ Center + FVector{ Cosine, 0.0f, Sine } };
		const FVector PointYZ{ Center + FVector{ 0.0f, Cosine, Sine } };
		LineBatcher->AddLine(PreviousXY, PointXY, Color);
		LineBatcher->AddLine(PreviousXZ, PointXZ, Color);
		LineBatcher->AddLine(PreviousYZ, PointYZ, Color);
		PreviousXY = PointXY;
		PreviousXZ = PointXZ;
		PreviousYZ = PointYZ;
	}
}
