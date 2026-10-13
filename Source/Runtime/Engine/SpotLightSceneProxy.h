#pragma once

#include "PointLightSceneProxy.h"

class FSpotLightSceneProxy : public FPointLightSceneProxy
{
public:
    explicit FSpotLightSceneProxy(USpotLightComponent* InComponent);
    void GetLocalLightInfo(FLocalLightInfo& Out) const override;
};