#include "EnginePCH.h"

#include "LightActorBase.h"

ALightActorBase::ALightActorBase() {}

void ALightActorBase::SetupLightRoot(ULightComponent* LightComponent)
{
    SetRootComponent(LightComponent);

    BillboardComponent = CreateDefaultSubobject<UBillboardComponent>("UBillboardComponent");
    BillboardComponent->SetupAttachment(LightComponent);
}
