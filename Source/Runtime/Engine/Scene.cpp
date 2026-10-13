#include "EnginePCH.h"
#include "Scene.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/SkyLightComponent.h"

void FScene::AddPrimitive(UPrimitiveComponent* Component)
{
	if (!Component || Component->SceneProxy) return;

	FPrimitiveSceneProxy* Proxy = new FPrimitiveSceneProxy(Component);
	Proxy->Scene = this;
	Proxy->PackedIndex = PrimitiveSceneProxies.Num();
	Component->SceneProxy = Proxy;
	MarkDirty(Proxy);
	MarkRenderStateDirty(Proxy);

	PrimitiveSceneProxies.Add(Proxy);
	PrimitiveBounds.Add(FAABB{});
	PrimitiveFlags.Add(0);

	bElementListChanged = true;
}

void FScene::RemovePrimitive(UPrimitiveComponent* Component)
{
	if (!Component || !Component->SceneProxy) return;

	FPrimitiveSceneProxy* Proxy = Component->SceneProxy;
	if (!Proxy) return;

	const uint32 Index = static_cast<uint32>(Proxy->PackedIndex);

	PrimitiveSceneProxies.RemoveAtSwap(Index);
	PrimitiveBounds.RemoveAtSwap(Index);
	PrimitiveFlags.RemoveAtSwap(Index);

	if (Index < static_cast<uint32>(PrimitiveSceneProxies.Num()))
		PrimitiveSceneProxies[Index]->PackedIndex = static_cast<int32>(Index);

	if (Proxy->bQueuedForUpdate)
	{
		const int32 Found = DirtyProxies.Find(Proxy);   // 프로젝트 TArray의 Find 이름에 맞게
		if (Found != INDEX_NONE) DirtyProxies.RemoveAtSwap(Found);
	}

	if (Proxy->bRenderStateQueued)
	{
		const int32 Found = RenderStateDirtyProxies.Find(Proxy);
		if (Found != INDEX_NONE) RenderStateDirtyProxies.RemoveAtSwap(Found);
	}

	Component->SceneProxy = nullptr;
	delete Proxy;

	bElementListChanged = true;
}

void FScene::RemoveAllPrimitives()
{
	// 하나씩 RemovePrimitive하면 대기열 검색(Find)이 물체 수만큼 반복되므로 통째로 비운다.
	for (FPrimitiveSceneProxy* Proxy : PrimitiveSceneProxies)
	{
		if (UPrimitiveComponent* Component = Proxy->GetComponent())
			Component->SceneProxy = nullptr;
		delete Proxy;
	}
	PrimitiveSceneProxies.Reset();
	PrimitiveBounds.Reset();
	PrimitiveFlags.Reset();
	DirtyProxies.Reset();
	RenderStateDirtyProxies.Reset();
	BVH.Clear();
	bElementListChanged = true;
}

void FScene::AddLight(ULightComponent* Component)
{
	if (!Component) return;
	if (LightComponents.Find(Component) != INDEX_NONE) return;
	LightComponents.Add(Component);
}

void FScene::RemoveLight(ULightComponent* Component)
{
	const int32 Index = LightComponents.Find(Component);
	if (Index != INDEX_NONE)
		LightComponents.RemoveAtSwap(Index);
}

void FScene::RemoveAllLights()
{
	LightComponents.Reset();
}

// 등록된 라이트를 셰이더용 상수로 모아 Renderer에 올린다. 컴포넌트 값을 매 프레임 직접 읽으므로 별도 갱신 경로가 필요 없다.
void FScene::UpdateLights(FRenderer* Renderer)
{
	if (!Renderer)
		return;

	FLightingConstants Constants{};

	// 앰비언트 컴포넌트가 생기기 전까지는 고정값을 쓴다.
	Constants.Ambient.Color = FVector(1.0f, 1.0f, 1.0f);
	Constants.Ambient.Intensity = 0.1f;

	PointLights.Reset();
	bool bHasDirectional = false;
	Constants.NumSpotLights = 0;
	for (ULightComponent* Light : LightComponents)
	{
		if (!Light || !Light->IsVisible())
			continue;

		if (UDirectionalLightComponent* Directional = Cast<UDirectionalLightComponent>(Light))
		{
			// 셰이더는 한 개만 받으므로 첫 번째 Directional만 쓴다.
			if (bHasDirectional)
				continue;
			bHasDirectional = true;

			FTransform Transform;
			Transform.Rotation = Directional->GetWorldRotation();
			const FLinearColor Color(Directional->GetLightColor());

			Constants.Directional.Direction = Transform.GetForward();
			Constants.Directional.Color = FVector(Color.R, Color.G, Color.B);
			Constants.Directional.Intensity = Directional->GetIntensity();
		}
		else if (USkyLightComponent* Sky = Cast<USkyLightComponent>(Light))
		{
			const FLinearColor Color(Sky->GetLightColor());
			Constants.Ambient.Color = FVector(Color.R, Color.G, Color.B);
			Constants.Ambient.Intensity = Sky->GetIntensity();
		}
		else if (USpotLightComponent* Spot = Cast<USpotLightComponent>(Light))
		{
			FTransform Transform;
			Transform.Rotation = Spot->GetWorldRotation();
			const FLinearColor Color(Spot->GetLightColor());

			Constants.SpotLights[Constants.NumSpotLights].Color = FVector(Color.R, Color.G, Color.B);
			Constants.SpotLights[Constants.NumSpotLights].Direction = Transform.GetForward();
			Constants.SpotLights[Constants.NumSpotLights].FalloffExponent = Spot->GetFalloff();
			Constants.SpotLights[Constants.NumSpotLights].Intensity = Spot->GetIntensity();
			Constants.SpotLights[Constants.NumSpotLights].Position = Spot->GetWorldLocation();
			Constants.SpotLights[Constants.NumSpotLights].Radius = Spot->GetAttenuationRadius();
			const float SafeOuterAngle = FMath::Clamp(Spot->GetOuterConeAngle(), 0.0f, 89.0f);
			const float SafeInnerAngle = FMath::Clamp(Spot->GetInnerConeAngle(), 0.0f, SafeOuterAngle);
			Constants.SpotLights[Constants.NumSpotLights].InnerConeCos = std::cos(FMath::DegreesToRadians(SafeInnerAngle));
			Constants.SpotLights[Constants.NumSpotLights].OuterConeCos = std::cos(FMath::DegreesToRadians(SafeOuterAngle));
			Constants.NumSpotLights++;
		}
		else if (UPointLightComponent* Point = Cast<UPointLightComponent>(Light))
		{
			const FLinearColor Color(Point->GetLightColor());
			FPointLightInfo Info{};
			Info.Position = Point->GetWorldLocation();
			Info.Radius = Point->GetAttenuationRadius();
			Info.Color = FVector(Color.R, Color.G, Color.B);
			Info.Intensity = Point->GetIntensity();
			Info.FalloffExponent = Point->GetFalloff();
			PointLights.Add(Info);
		}
	}

	Renderer->SetLighting(Constants, PointLights, SpotLights);
}

void FScene::AddFogInfo(uint32 Id, const FFogInfo& FogInfo)
{
	FExponentialHeightFogSceneInfo sceneFogInfo;
	sceneFogInfo.Id = Id;
	sceneFogInfo.FogInfo = FogInfo;


	FogInfos.Add(sceneFogInfo);
}

void FScene::RemoveFogInfo(uint32 Id)
{
	// 렌더러는 FogInfos[0]만 쓰므로, 순서를 유지하려고 RemoveAtSwap 대신 RemoveAt을 쓴다.
	for (int32 i = 0; i < FogInfos.Num(); ++i)
	{
		if (FogInfos[i].Id == Id)
		{
			FogInfos.RemoveAt(i);
			return;
		}
	}
}

void FScene::RemoveAllFogInfo()
{
	FogInfos.Reset();

}

void FScene::UpdateFogInfo(uint32 Id, const FFogInfo& FogInfo)
{
	for (FExponentialHeightFogSceneInfo& SceneFogInfo : FogInfos)
	{
		if (SceneFogInfo.Id == Id)
		{
			SceneFogInfo.FogInfo = FogInfo;
			return;
		}


	}
}

//void FScene::RemoveFogInfo(uint32 Id)
//{
//	for (int32 i = 0; i < FogInfos.Num(); ++i)
//	{
//		if (FogInfos[i].Id == Id)
//		{
//			FogInfos.RemoveAtSwap(i);
//			return;
//		}
//	}
//}


void FScene::UpdateAllTransforms()
{
	for (FPrimitiveSceneProxy* Proxy : RenderStateDirtyProxies)
	{
		Proxy->UpdateRenderState();
		Proxy->bRenderStateQueued = false;
	}
	RenderStateDirtyProxies.Reset();

	for (FPrimitiveSceneProxy* Proxy : DirtyProxies)
	{
		Proxy->UpdateTransform();
		PrimitiveBounds[Proxy->PackedIndex] = Proxy->GetBounds();
		PrimitiveFlags[Proxy->PackedIndex] = Proxy->GetComponent()->IsVisible() ? 1 : 0;
		Proxy->bQueuedForUpdate = false;
	}

	//const int32 Count = Proxies.Num();
	//for (int32 i = 0; i < Count; ++i)
	//{
	//	FPrimitiveSceneProxy* Proxy = Proxies[i];
	//	Proxy->UpdateTransform();

	//	const FAABB& Bounds = Proxy->GetBounds();
	//	if (PrimitiveBounds[i].Center != Bounds.Center || PrimitiveBounds[i].Extent != Bounds.Extent)
	//	{
	//		bBoundsChanged = true;
	//	}

	//	PrimitiveBounds[i] = Proxy->GetBounds();
	//	PrimitiveFlags[i] = Proxy->GetComponent()->IsVisible() ? 1 : 0;
	//}

	const bool bAnyMoved = DirtyProxies.Num() > 0;
	DirtyProxies.Reset();


	if (bElementListChanged)
	{
		BuildBVH();
		bElementListChanged = false;
	}
	else if (bAnyMoved)
	{
		BVH.Refit();
	}
}

void FScene::BuildBVH()
{
	BVH.Clear();
	// BillboardComponents.Reset();

	TArray<FPrimitiveSceneProxy*> Elements;
	Elements.Reserve(PrimitiveSceneProxies.Num());
	for (FPrimitiveSceneProxy* Proxy : PrimitiveSceneProxies)
	{
		if (Proxy && Proxy->GetComponent())
			Elements.Add(Proxy);
	}

	BVH.Build(std::span(Elements.GetData(), Elements.Num()));
}

void FScene::MarkDirty(FPrimitiveSceneProxy* Proxy)
{
	if (Proxy->bQueuedForUpdate) return;            // 중복 추가 방지
	Proxy->bQueuedForUpdate = true;
	DirtyProxies.Add(Proxy);
}

void FScene::MarkRenderStateDirty(FPrimitiveSceneProxy* Proxy)
{
	if (!Proxy || Proxy->bRenderStateQueued) return;
	Proxy->bRenderStateQueued = true;
	RenderStateDirtyProxies.Add(Proxy);
}