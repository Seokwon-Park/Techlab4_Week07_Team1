
#pragma once

#include "LightActor.h"
#include "Components/DirectionalLightComponent.h"

class ADirectionalLightActor : public ALightActor {
	DECLARE_CLASS(ADirectionalLightActor, ALightActor)

public:
	ADirectionalLightActor();
	virtual ~ADirectionalLightActor() override;

public:
	UDirectionalLightComponent* GetDirectionalLightComponent() const;
};
