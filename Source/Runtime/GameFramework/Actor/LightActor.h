#pragma once

#include "GameFramework/Actor.h"
#include "Components/BillboardComponent.h"
#include "Components/SpotLightComponent.h"

class ALightActor : public AActor {
	DECLARE_CLASS(ALightActor, AActor)

public:
	ALightActor();
	explicit ALightActor(ELightType InLightType);
	virtual ~ALightActor() override;

public:
	static void RegisterProperties(UClass* InClass);
	UBillboardComponent* GetBillboardComponent() const;
	ULightComponent* GetLightComponent() const;
	ULightComponent* GetSpotLightComponent() const;

private:
	UBillboardComponent* mBillboardComponent{};
	ULightComponent* mLightComponent{};
};
