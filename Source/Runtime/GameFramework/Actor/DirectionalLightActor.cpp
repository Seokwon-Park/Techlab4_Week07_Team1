
#include "EnginePCH.h"
#include "DirectionalLightActor.h"

ADirectionalLightActor::ADirectionalLightActor()
	: ALightActor{ ELightType::Directional } {
}

ADirectionalLightActor::~ADirectionalLightActor() = default;

UDirectionalLightComponent* ADirectionalLightActor::GetDirectionalLightComponent() const {
	return Cast<UDirectionalLightComponent>(GetLightComponent());
}
