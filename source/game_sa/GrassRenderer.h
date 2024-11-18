#pragma once

#include "PPTriPlantBuffer.h"

class CVector;

class CGrassRenderer {
public:
    static inline float& m_windBending = *(float*)0xC02DB8;
    static inline float& m_closeDist = *(float*)0xC02DBC;
    static inline CVector& m_vecCameraPos = *(CVector*)0xC02DDC;
    static inline float& m_farDist = *(float*)0x8D132C; // 10.0f

public:
    static void InjectHooks();

    CGrassRenderer() = default;
    ~CGrassRenderer() = default;

    static bool Initialise();
    static void Shutdown();

    static void AddTriPlant(PPTriPlant* srcPlant, uint32 plantModelSet);
    static void DrawTriPlants(PPTriPlant* triPlants, int32 numTriPlants, RpAtomic** plantModelsTab);
    static void FlushTriPlantBuffer();

    static RpAtomic** GetPlantModelsTab(uint32 type);
    static void SetPlantModelsTab(uint32 type, RpAtomic** atomics);

    static void SetCloseFarAlphaDist(float fCloseDist, float fFarDist);
    static void SetCurrentScanCode(uint16 scanCode);
    static void SetGlobalCameraPos(const CVector& cameraPosn);
    static void SetGlobalWindBending(float bending);

    static CVector& GenPointInTriangle(CVector& outPosn, const CVector& v1, const CVector& v2, const CVector& v3, float randA, float randB);
};

extern uint16& g_GrassCurrentScanCode;
extern CPPTriPlantBuffer& gTriPlantBuf;
