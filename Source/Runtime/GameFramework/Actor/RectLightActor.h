
#pragma once

#include "LightActor.h"
#include "Components/RectLightComponent.h"

class ARectLightActor : public ALightActor {
	DECLARE_CLASS(ARectLightActor, ALightActor)

public:
	ARectLightActor();
	virtual ~ARectLightActor() override;

public:
	URectLightComponent* GetRectLightComponent() const;
};
