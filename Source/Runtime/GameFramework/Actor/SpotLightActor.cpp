
#include "EnginePCH.h"
#include "SpotLightActor.h"

ASpotLightActor::ASpotLightActor()
	: ALightActor{ ELightType::Spot } {
}

ASpotLightActor::~ASpotLightActor() = default;

USpotLightComponent* ASpotLightActor::GetSpotLightComponent() const {
	return Cast<USpotLightComponent>(GetLightComponent());
}
