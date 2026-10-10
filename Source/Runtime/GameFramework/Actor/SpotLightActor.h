
#pragma once

#include "LightActor.h"
#include "Components/SpotLightComponent.h"

class ASpotLightActor : public ALightActor {
	DECLARE_CLASS(ASpotLightActor, ALightActor)

public:
	ASpotLightActor();
	virtual ~ASpotLightActor() override;

public:
	USpotLightComponent* GetSpotLightComponent() const;
};
