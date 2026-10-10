#include "EnginePCH.h"
#include "LightActor.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/RectLightComponent.h"

ALightActor::ALightActor()
	: ALightActor{ ELightType::Spot } {
}

ALightActor::ALightActor(ELightType InLightType) {
	mBillboardComponent = CreateDefaultSubobject<UBillboardComponent>("UBillboardComponent");
	mBillboardComponent->SetupAttachment(GetRootComponent());

	switch (InLightType) {
	case ELightType::Directional:
		mLightComponent = CreateDefaultSubobject<UDirectionalLightComponent>("UDirectionalLightComponent");
		break;
	case ELightType::Point:
		mLightComponent = CreateDefaultSubobject<UPointLightComponent>("UPointLightComponent");
		break;
	case ELightType::Rect:
		mLightComponent = CreateDefaultSubobject<URectLightComponent>("URectLightComponent");
		break;
	case ELightType::Spot:
	default:
		mLightComponent = CreateDefaultSubobject<USpotLightComponent>("USpotLightComponent");
		break;
	}

	mLightComponent->SetupAttachment(GetRootComponent());
}

ALightActor::~ALightActor() = default;

void ALightActor::RegisterProperties(UClass* InClass) {
	InClass->AddProperty<UBillboardComponent*>("BillboardComponent", offsetof(ALightActor, mBillboardComponent), CPF_VisibleAnywhere);
	InClass->AddProperty<ULightComponent*>("LightComponent", offsetof(ALightActor, mLightComponent), CPF_VisibleAnywhere);
}

UBillboardComponent* ALightActor::GetBillboardComponent() const {
	return mBillboardComponent;
}

ULightComponent* ALightActor::GetLightComponent() const {
	return mLightComponent;
}

ULightComponent* ALightActor::GetSpotLightComponent() const {
	return mLightComponent;
}
