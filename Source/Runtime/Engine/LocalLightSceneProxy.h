#pragma once

#include "LightSceneProxy.h"

class FLocalLightSceneProxy : public FLightSceneProxy
{
public:
    using FLightSceneProxy::FLightSceneProxy;
    // 컴포넌트 값을 읽어 GPU용 공통 구조체를 채운다. Unreal의 GetLightShaderParameters 역할.
    virtual void GetLocalLightInfo(FLocalLightInfo& Out) const = 0;
};
