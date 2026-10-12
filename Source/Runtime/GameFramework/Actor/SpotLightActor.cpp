#include "EnginePCH.h"
#include "SpotLightActor.h"
#include "Asset/AssetManager.h"


ASpotLightActor::ASpotLightActor()
{
    SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>("USpotLightComponent");
	SetupLightRoot(SpotLightComponent, "Assets/Icons/Spot.png");

    ArrowComponent = CreateDefaultSubobject<UStaticMeshComponent>("UArrowComponent");
    ArrowComponent->SetupAttachment(SpotLightComponent);
    ArrowComponent->SetStaticMesh(UAssetManager::GetAssetByPath<UStaticMesh>("Arrow"));
    ArrowComponent->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
    ArrowComponent->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.5f));
    ArrowComponent->SetHiddenInDetails(true);

}
