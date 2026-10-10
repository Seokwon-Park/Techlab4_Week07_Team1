#include "EnginePCH.h"
#include "RectLightComponent.h"
#include "Render/LineBatcher.h"

URectLightComponent::URectLightComponent() = default;

URectLightComponent::~URectLightComponent() = default;

void URectLightComponent::OnRegister() {
	Super::OnRegister();

	FLightRenderParameters& RenderParameters{ mLightProxy.GetRenderParameters() };
	RenderParameters.SourceRadius = mSourceWidth * 0.5f;
	RenderParameters.SourceLength = mSourceHeight * 0.5f;
}

void URectLightComponent::PostEditChangeProperty(const FProperty& Property) {
	Super::PostEditChangeProperty(Property);
	SetSourceWidth(mSourceWidth);
	SetSourceHeight(mSourceHeight);
	SetBarnDoorAngle(mBarnDoorAngle);
	SetBarnDoorLength(mBarnDoorLength);

	FLightRenderParameters& RenderParameters{ mLightProxy.GetRenderParameters() };
	RenderParameters.SourceRadius = mSourceWidth * 0.5f;
	RenderParameters.SourceLength = mSourceHeight * 0.5f;
}

float URectLightComponent::GetSourceWidth() const {
	return mSourceWidth;
}

void URectLightComponent::SetSourceWidth(float InSourceWidth) {
	mSourceWidth = FMath::Max(InSourceWidth, 0.0f);
}

float URectLightComponent::GetSourceHeight() const {
	return mSourceHeight;
}

void URectLightComponent::SetSourceHeight(float InSourceHeight) {
	mSourceHeight = FMath::Max(InSourceHeight, 0.0f);
}

float URectLightComponent::GetBarnDoorAngle() const {
	return mBarnDoorAngle;
}

void URectLightComponent::SetBarnDoorAngle(float InBarnDoorAngle) {
	mBarnDoorAngle = FMath::Clamp(InBarnDoorAngle, 0.0f, 90.0f);
}

float URectLightComponent::GetBarnDoorLength() const {
	return mBarnDoorLength;
}

void URectLightComponent::SetBarnDoorLength(float InBarnDoorLength) {
	mBarnDoorLength = FMath::Max(InBarnDoorLength, 0.0f);
}

ELightType URectLightComponent::GetLightType() const {
	return ELightType::Rect;
}

void URectLightComponent::DrawDebug(FLineBatcher* LineBatcher) const {
	if (LineBatcher == nullptr || !IsVisible()) {
		return;
	}

	FTransform WorldTransform{};
	WorldTransform.Location = GetWorldLocation();
	WorldTransform.Rotation = GetWorldRotation();
	const FVector Center{ WorldTransform.Location };
	const FVector HalfRight{ WorldTransform.GetRight() * FMath::Max(mSourceWidth, 0.0f) * 0.5f };
	const FVector HalfUp{ WorldTransform.GetUp() * FMath::Max(mSourceHeight, 0.0f) * 0.5f };
	const FVector TopLeft{ Center - HalfRight + HalfUp };
	const FVector TopRight{ Center + HalfRight + HalfUp };
	const FVector BottomLeft{ Center - HalfRight - HalfUp };
	const FVector BottomRight{ Center + HalfRight - HalfUp };
	const FVector4& Color{ GetLightColor() };
	LineBatcher->AddLine(TopLeft, TopRight, Color);
	LineBatcher->AddLine(TopRight, BottomRight, Color);
	LineBatcher->AddLine(BottomRight, BottomLeft, Color);
	LineBatcher->AddLine(BottomLeft, TopLeft, Color);
	LineBatcher->AddLine(Center, Center + WorldTransform.GetForward() * FMath::Max(GetAttenuationRadius(), 0.0f), Color);
}
