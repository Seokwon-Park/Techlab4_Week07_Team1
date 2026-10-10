#include "EnginePCH.h"
#include "SpotLightActor.h"

ASpotLightActor::ASpotLightActor()
{
    SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>("USpotLightComponent");
	SetupLightRoot(SpotLightComponent);
}