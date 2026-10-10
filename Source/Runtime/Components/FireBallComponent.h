#pragma once
#include "PointLightComponent.h"

class UFireBallComponent : public UPointLightComponent
{
	DECLARE_CLASS(UFireBallComponent, UPointLightComponent)

	REFLECT_START(ClassName)
	REFLECT_END()

public:
	UFireBallComponent();
	virtual ~UFireBallComponent() override;

	ELightType GetLightType() const { return ELightType::Fireball; }
	void FillLightData(FLightRenderData& Out) const override;

private:
};
