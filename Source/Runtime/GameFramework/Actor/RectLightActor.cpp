#include "EnginePCH.h"
#include "RectLightActor.h"

ARectLightActor::ARectLightActor()
{
    RectLightComponent = CreateDefaultSubobject<URectLightComponent>("URectLightComponent");
	SetRootComponent(RectLightComponent);
}