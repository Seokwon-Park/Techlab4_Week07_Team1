
#include "EnginePCH.h"
#include "RectLightActor.h"

ARectLightActor::ARectLightActor()
	: ALightActor{ ELightType::Rect } {
}

ARectLightActor::~ARectLightActor() = default;

URectLightComponent* ARectLightActor::GetRectLightComponent() const {
	return Cast<URectLightComponent>(GetLightComponent());
}
