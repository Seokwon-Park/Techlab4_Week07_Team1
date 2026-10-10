
#pragma once

#include "LightActor.h"
#include "Components/PointLightComponent.h"

class APointLightActor : public ALightActor {
	DECLARE_CLASS(APointLightActor, ALightActor)

public:
	APointLightActor();
	virtual ~APointLightActor() override;

public:
	UPointLightComponent* GetPointLightComponent() const;
};
