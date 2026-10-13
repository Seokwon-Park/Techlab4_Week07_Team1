#include "EnginePCH.h"
#include "RectLightActor.h"

ARectLightActor::ARectLightActor()
{
    RectLightComponent = CreateDefaultSubobject<URectLightComponent>("URectLightComponent");
	SetupLightRoot(RectLightComponent, "Assets/Icons/Point.png");
}