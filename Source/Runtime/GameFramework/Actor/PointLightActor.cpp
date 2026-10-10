
#include "EnginePCH.h"
#include "PointLightActor.h"

APointLightActor::APointLightActor()
	: ALightActor{ ELightType::Point } {
}

APointLightActor::~APointLightActor() = default;

UPointLightComponent* APointLightActor::GetPointLightComponent() const {
	return Cast<UPointLightComponent>(GetLightComponent());
}
