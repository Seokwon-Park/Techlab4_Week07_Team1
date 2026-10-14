#include "EnginePCH.h"
#include "RectLightComponent.h"
#include "Render/LineBatcher.h"

#include "Engine/RectLightSceneProxy.h"

URectLightComponent::URectLightComponent() {}
URectLightComponent::~URectLightComponent() {}

void URectLightComponent::DrawDebug(FLineBatcher* LineBatcher) const
{
	if (LineBatcher == nullptr) return;

	FTransform WorldTransform;
	WorldTransform.Location = GetWorldLocation();
	WorldTransform.Rotation = GetWorldRotation();

	const FVector Center = WorldTransform.Location;
	const FVector Forward = WorldTransform.GetForward();   // +X, 빛이 나가는 방향
	const FVector Right = WorldTransform.GetRight();
	const FVector Up = WorldTransform.GetUp();

	const float HW = std::max(HalfWidth, 0.0f);
	const float HH = std::max(HalfHeight, 0.0f);
	const float Angle = FMath::DegreesToRadians(FMath::Clamp(BarnDoorAngle, 0.0f, 90.0f));
	const float Len = std::max(BarnDoorLength, 0.0f);

	// 발광면 (우상, 좌상, 좌하, 우하)
    const FVector C0 = Center + Right * HW + Up * HH;
    const FVector C1 = Center - Right * HW + Up * HH;
    const FVector C2 = Center - Right * HW - Up * HH;
    const FVector C3 = Center + Right * HW - Up * HH;

    const FVector4 Color = FColor(255.0f, 255.0f, 255.0f, 255.0f).ToVector4();

    // Door은 흐리게
	const FColor DoorFColor(127.5f, 127.5f, 127.5f, 127.5f);
	const FVector4 DoorColor = DoorFColor.ToVector4();

    // 개구부 사각형: 앞으로 F만큼 나가고, 사방으로 S만큼 벌어진다
    const float F = std::cosf(Angle) * Len;     // 앞으로 나간 양
    const float S = std::sinf(Angle) * Len;     // 바깥으로 벌어진 양
    const FVector AC = Center + Forward * F;

    const FVector A0 = AC + Right * (HW + S) + Up * (HH + S);
    const FVector A1 = AC - Right * (HW + S) + Up * (HH + S);
    const FVector A2 = AC - Right * (HW + S) - Up * (HH + S);
    const FVector A3 = AC + Right * (HW + S) - Up * (HH + S);

    // 사각형
    LineBatcher->AddLine(C0, C1, Color);
    LineBatcher->AddLine(C1, C2, Color);
    LineBatcher->AddLine(C2, C3, Color);
    LineBatcher->AddLine(C3, C0, Color);

    // 개구부 사각형
    LineBatcher->AddLine(A0, A1, DoorColor);
    LineBatcher->AddLine(A1, A2, DoorColor);
    LineBatcher->AddLine(A2, A3, DoorColor);
    LineBatcher->AddLine(A3, A0, DoorColor);

    // 발광면 모서리와 개구부 모서리 연결
    LineBatcher->AddLine(C0, A0, DoorColor);
    LineBatcher->AddLine(C1, A1, DoorColor);
    LineBatcher->AddLine(C2, A2, DoorColor);
    LineBatcher->AddLine(C3, A3, DoorColor);

}

FLightSceneProxy* URectLightComponent::CreateSceneProxy() const
{
    return new FRectLightSceneProxy(this);
}
