/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"

enum eCarPlateType : uint8 {
    CARPLATE_DEFAULT = 255,
    CARPLATE_SF = 0,
    CARPLATE_LV = 1,
    CARPLATE_LA = 2,
    MAX_CARPLATE = 3
};

class CCustomCarPlateMgr {
public:
    static constexpr uint32 MAX_TEXT_LENGTH{ 8u };

private:
    // Values below in pixels:
    static constexpr uint32 CHARSET_CHAR_WIDTH{ 8u };
    static constexpr uint32 CHARSET_CHAR_HEIGHT{ 16u };
    static constexpr uint32 CHARSET_COL_WIDTH{ 4 * CHARSET_CHAR_WIDTH }; // 4x chars / row
    static constexpr uint32 CHARSET_ROW_HEIGHT{ 16u };
    
    static inline RwTexture*& pCharsetTex = *(RwTexture**)0xC3EF5C;
    static inline RpMaterial*& CurrentLicensePlateMaterial = *(RpMaterial**)0xC3EF7C;
    static inline uint8& CurrentLicensePlateType = *(uint8*)0xC3EF80;
    static inline RwTexture* (&pPlatebackTexTab)[3] = *(RwTexture * (*)[3])0xC3EF60;
    static inline void* (&pPalette1555Tab)[3] = *(void*(*)[3])0xC3EF6C;
    static inline RwUInt8*& pCharsetLockedData = *(RwUInt8**)0xC3EF78; 
public:
    static void InjectHooks();

    static bool Initialise();
    static bool GeneratePlateText(char* buf, uint32 len);
    static void Shutdown();
    static int8 GetMapRegionPlateDesign();
    static int8 LoadPlatecharsetDat(const char* filename, uint8* data);
    static void SetupMaterialPlatebackTexture(RpMaterial* material, uint8 plateType);
    static RwTexture* CreatePlateTexture(const char* text, uint8 plateType);
    static int8 SetupClumpAfterVehicleUpgrade(RpClump* clump, RpMaterial* plateMaterial, uint8 plateType);
    static void SetupMaterialPlateTexture(RpMaterial* material, const char* plateText, uint8 plateType);
    static RpMaterial* SetupClump(RpClump* clump, char* plateText, uint8 plateType);

private:
    static bool RenderLicenseplateTextToRaster(const char* text, RwRaster* charsetRaster, void* palette, RwRaster* plateRaster);
    // 'data' is a pointer to RpGeometry
    static RpMaterial* MaterialUpgradeSetCarplateTextureCB(RpMaterial* material, void* data);
    // 'data' is unused
    static RpAtomic* AtomicUpgradeSetCarplateTextureCB(RpAtomic* atomic, _IGNORED_ void* data);
    // 'plateText' is a plate text (char *)
    static RpMaterial* MaterialSetCarplateTextureCB(RpMaterial* material, void* plateText);
    // 'plateText' is a plate text (char *)
    static RpAtomic* AtomicSetCarplateTextureCB(RpAtomic* atomic, void* plateText);
};


