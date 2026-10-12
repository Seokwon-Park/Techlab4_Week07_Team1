#include "EnginePCH.h"
#include "DirectionalLightActor.h"
#include "Asset/AssetManager.h"

ADirectionalLightActor::ADirectionalLightActor()
{
    DirectionalLightComponent = CreateDefaultSubobject<UDirectionalLightComponent>("UDirectionalLightComponent");
	SetupLightRoot(DirectionalLightComponent, "Assets/Icons/Directional.png");

    ArrowComponent = CreateDefaultSubobject<UStaticMeshComponent>("UArrowComponent");
    ArrowComponent->SetupAttachment(DirectionalLightComponent);
    ArrowComponent->SetStaticMesh(UAssetManager::GetAssetByPath<UStaticMesh>("Arrow"));
    ArrowComponent->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
    ArrowComponent->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.3f));
    ArrowComponent->SetHiddenInDetails(true);

}