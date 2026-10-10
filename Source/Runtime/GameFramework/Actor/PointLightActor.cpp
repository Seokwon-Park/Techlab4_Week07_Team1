#include "EnginePCH.h"
#include "PointLightActor.h"

APointLightActor::APointLightActor()
{
    PointLightComponent = CreateDefaultSubobject<UPointLightComponent>("UPointLightComponent");
	SetRootComponent(PointLightComponent);
}