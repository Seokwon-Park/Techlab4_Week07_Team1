#include "EnginePCH.h"
#include "LightProxy.h"

FLightRenderParameters& LightProxy::GetRenderParameters() {
	return mRenderParameters;
}

ELightType LightProxy::GetLightType() const {
	return mLightType;
}

void LightProxy::SetLightType(ELightType InLightType) {
	mLightType = InLightType;
}

float LightProxy::GetIntensity() const {
	return mIntensity;
}

void LightProxy::SetIntensity(float InIntensity) {
	mIntensity = InIntensity;
}

bool LightProxy::IsVisible() const {
	return mVisible;
}

void LightProxy::SetVisible(bool InVisible) {
	mVisible = InVisible;
}
