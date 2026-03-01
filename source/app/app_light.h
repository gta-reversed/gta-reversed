#pragma once

#include "RenderWare.h"

extern void AppLightInjectHooks();

static inline RpLight*& pAmbient = *reinterpret_cast<RpLight**>(0xC886E8);
static inline RwRGBAReal& AmbientLightColour = *reinterpret_cast<RwRGBAReal*>(0xC886A4);
static inline RwRGBAReal& AmbientLightColourForFrame = *reinterpret_cast<RwRGBAReal*>(0xC886D4);
static inline RwRGBAReal& AmbientLightColourForFrame_PedsCarsAndObjects = *reinterpret_cast<RwRGBAReal*>(0xC886C4);

static inline RpLight*& pDirect = *reinterpret_cast<RpLight**>(0xC886EC);
static inline RwRGBAReal& DirectionalLightColour = *reinterpret_cast<RwRGBAReal*>(0xC88694);
static inline RwRGBAReal& DirectionalLightColourForFrame = *reinterpret_cast<RwRGBAReal*>(0xC886B4);
static inline RwRGBAReal (&DirectAmbientLight)[2] = *reinterpret_cast<RwRGBAReal (*)[2]>(0xC8865C); // Direct, Ambient Light
static inline auto& pExtraDirectionals = StaticRef<std::array<RpLight*, 6>>(0xC886F0);
static inline int32& NumExtraDirectionalLights = *reinterpret_cast<int32*>(0xC88708);
static inline auto& LightStrengths = StaticRef<std::array<float, 6>>(0xC8867C);

extern void ActivateDirectional();
extern void DeActivateDirectional();

extern void LightsCreate(RpWorld* world);
extern void LightsDestroy(RpWorld* world);
extern void LightsEnable(int32 enable);

extern void SetLightsWithTimeOfDayColour(RpWorld* world);

extern void WorldReplaceNormalLightsWithScorched(RpWorld* world, float lighting);
extern void WorldReplaceScorchedLightsWithNormal(RpWorld* world);

extern void AddAnExtraDirectionalLight(RpWorld* world, float x, float y, float z, float red, float green, float blue);
extern void RemoveExtraDirectionalLights(RpWorld* world);

extern void SetAmbientAndDirectionalColours(float fMult);
extern void ReSetAmbientAndDirectionalColours();

extern void SetFlashyColours(float fMult);
extern void SetFlashyColours_Mild(float fMult);

extern void SetBrightMarkerColours(float lighting);
extern void SetDirectionalColours(RwRGBAReal* color);

extern void SetAmbientColoursToIndicateRoadGroup(int32 idx);
extern void SetFullAmbient();
extern void SetAmbientColours();
extern void SetAmbientColours(RwRGBAReal* color);

extern void SetLightColoursForPedsCarsAndObjects(float fMult);

extern void SetLightsForInfraredVisionHeatObjects();
extern void StoreAndSetLightsForInfraredVisionHeatObjects();
extern void RestoreLightsForInfraredVisionHeatObjects();
extern void SetLightsForInfraredVisionDefaultObjects();
extern void SetLightsForNightVision();

float GetDayNightBalance();
