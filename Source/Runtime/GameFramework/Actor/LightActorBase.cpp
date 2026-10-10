#include "EnginePCH.h"

#include "LightActorBase.h"

ALightActorBase::ALightActorBase()
{
    BillboardComponent = CreateDefaultSubobject<UBillboardComponent>("UBillboardComponent");
	BillboardComponent->SetupAttachment(GetRootComponent());
}
