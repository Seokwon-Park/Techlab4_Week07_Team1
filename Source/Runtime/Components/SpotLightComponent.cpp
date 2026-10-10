#include "EnginePCH.h"
#include "SpotLightComponent.h"
#include "Render/LineBatcher.h"
#include <cmath>

namespace {
	void AddWireCone(FLineBatcher* LineBatcher, const FVector& Apex, const FVector& Forward, const FVector& Right, const FVector& Up, float HalfAngleDegrees, float Range, const FVector4& Color) {
		constexpr int32 SegmentCount{ 24 };
		const float BaseRadius{ std::tan(FMath::DegreesToRadians(HalfAngleDegrees)) * Range };
		const FVector BaseCenter{ Apex + Forward * Range };
		FVector PreviousPoint{ BaseCenter + Right * BaseRadius };
		for (int32 Index{ 1 }; Index <= SegmentCount; ++Index) {
			const float Angle{ 2.0f * PI * static_cast<float>(Index) / static_cast<float>(SegmentCount) };
			const FVector Point{ BaseCenter + Right * (std::cos(Angle) * BaseRadius) + Up * (std::sin(Angle) * BaseRadius) };
			LineBatcher->AddLine(PreviousPoint, Point, Color);
			LineBatcher->AddLine(Apex, Point, Color);
			PreviousPoint = Point;
		}
	}
}

USpotLightComponent::USpotLightComponent() {
	SetLightColor(FVector4{ 1.0f, 1.0f, 0.6f, 1.0f });
}

USpotLightComponent::~USpotLightComponent() = default;

void USpotLightComponent::RegisterProperties(UClass* InClass) {
	InClass->AddProperty<float>("InnerConeAngle", offsetof(USpotLightComponent, mInnerConeAngle));
	InClass->AddProperty<float>("OuterConeAngle", offsetof(USpotLightComponent, mOuterConeAngle));
}

void USpotLightComponent::PostEditChangeProperty(const FProperty& Property) {
	Super::PostEditChangeProperty(Property);
	SetInnerConeAngle(mInnerConeAngle);
	SetOuterConeAngle(mOuterConeAngle);
}

float USpotLightComponent::GetInnerConeAngle() const {
	return mInnerConeAngle;
}

void USpotLightComponent::SetInnerConeAngle(float InInnerConeAngle) {
	mInnerConeAngle = FMath::Clamp(InInnerConeAngle, 0.0f, mOuterConeAngle);
}

float USpotLightComponent::GetOuterConeAngle() const {
	return mOuterConeAngle;
}

void USpotLightComponent::SetOuterConeAngle(float InOuterConeAngle) {
	mOuterConeAngle = FMath::Clamp(InOuterConeAngle, 0.0f, 89.0f);
	mInnerConeAngle = FMath::Clamp(mInnerConeAngle, 0.0f, mOuterConeAngle);
}

ELightType USpotLightComponent::GetLightType() const {
	return ELightType::Spot;
}

void USpotLightComponent::DrawDebug(FLineBatcher* LineBatcher) const {
	if (LineBatcher == nullptr || !IsVisible() || GetAttenuationRadius() <= 0.0f) {
		return;
	}

	FTransform WorldTransform{};
	WorldTransform.Location = GetWorldLocation();
	WorldTransform.Rotation = GetWorldRotation();
	const FVector Apex{ WorldTransform.Location };
	const FVector Forward{ WorldTransform.GetForward() };
	const FVector Right{ WorldTransform.GetRight() };
	const FVector Up{ WorldTransform.GetUp() };
	const float OuterAngle{ FMath::Clamp(mOuterConeAngle, 0.0f, 89.0f) };
	const float InnerAngle{ FMath::Clamp(mInnerConeAngle, 0.0f, OuterAngle) };
	const float Radius{ FMath::Max(GetAttenuationRadius(), 0.0f) };
	const FVector4& Color{ GetLightColor() };
	const FVector4 InnerColor{ Color.X * 0.5f, Color.Y * 0.5f, Color.Z * 0.5f, Color.W };
	AddWireCone(LineBatcher, Apex, Forward, Right, Up, OuterAngle, Radius, Color);
	AddWireCone(LineBatcher, Apex, Forward, Right, Up, InnerAngle, Radius, InnerColor);
}
