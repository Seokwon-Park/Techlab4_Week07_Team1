#include "EnginePCH.h"
#include "DirectionalLightActor.h"

ADirectionalLightActor::ADirectionalLightActor()
{
    DirectionalLightComponent = CreateDefaultSubobject<UDirectionalLightComponent>("UDirectionalLightComponent");
	SetupLightRoot(DirectionalLightComponent, "Assets/Icons/Directional.png");
}