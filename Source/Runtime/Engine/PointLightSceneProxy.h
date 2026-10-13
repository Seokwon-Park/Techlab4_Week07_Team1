#pragma once

#include "LocalLightSceneProxy.h"

class FPointLightSceneProxy : public FLocalLightSceneProxy
{
public:
    explicit FPointLightSceneProxy(UPointLightComponent* InComponent);
    void GetLocalLightInfo(FLocalLightInfo& Out) const override;
};