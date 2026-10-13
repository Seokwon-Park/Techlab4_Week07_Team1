#pragma once
#include "PrimitiveComponent.h"

class ULightArrowComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(ULightArrowComponent, UPrimitiveComponent)
    REFLECT_START(ULightArrowComponent)
    REFLECT_END()

public:
    ULightArrowComponent();
    virtual ~ULightArrowComponent();

    const FStaticMeshData* GetMeshData() const { return }
    bool LineTraceComponent(const FRay& WorldRay, FHitResult& OutHit);

    
} 