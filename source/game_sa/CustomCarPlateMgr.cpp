#include "StdInc.h"

#include "CustomCarPlateMgr.h"

void CCustomCarPlateMgr::InjectHooks() {
    RH_ScopedClass(CCustomCarPlateMgr);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x6FD500);
    RH_ScopedInstall(GeneratePlateText, 0x6FD5B0);
    RH_ScopedInstall(Shutdown, 0x6FD720);
    RH_ScopedInstall(GetMapRegionPlateDesign, 0x6FD7A0);
    RH_ScopedInstall(SetupMaterialPlatebackTexture, 0x6FDE50);
    RH_ScopedInstall(CreatePlateTexture, 0x6FDEA0);
    RH_ScopedInstall(SetupClumpAfterVehicleUpgrade, 0x6FDFE0);
    RH_ScopedInstall(MaterialUpgradeSetCarplateTextureCB, 0x6FDF50);
    RH_ScopedInstall(AtomicUpgradeSetCarplateTextureCB, 0x6FDFC0);
    RH_ScopedInstall(SetupMaterialPlateTexture, 0x6FE020);
    // RH_ScopedInstall(GetCharacterPositionInCharSet, 0x6FD7C0, { .enabled = false }); // Incompatible with original function (As it uses __usercall and passes the first arg in `al`)
    RH_ScopedInstall(MaterialSetCarplateTextureCB, 0x6FE060);
    RH_ScopedInstall(AtomicSetCarplateTextureCB, 0x6FE0D0);
    RH_ScopedInstall(SetupClump, 0x6FE0F0);
    RH_ScopedInstall(RenderLicenseplateTextToRaster, 0x6FDD70);
}

// 0x6FD500
bool CCustomCarPlateMgr::Initialise() {
    {
        const auto txd = CTxdStore::FindTxdSlot("vehicle");
        CTxdStore::PushCurrentTxd();
        CTxdStore::SetCurrentTxd(txd);
    }

    const auto FindTXDAndSetFlags = [](const char* name, RwTextureFilterMode filterMode) {
        const auto tex =RwTextureRead(name, nullptr);
        RwTextureSetAddressingU(tex, rwFILTERMIPNEAREST);
        RwTextureSetAddressingV(tex, rwFILTERMIPNEAREST);
        RwTextureSetFilterMode(tex, filterMode);
        return tex;
    };

    pCharsetTex = FindTXDAndSetFlags("platecharset", RwTextureFilterMode::rwFILTERNEAREST);

    const char* texNames[] = { "plateback1", "plateback2", "plateback3" };
    for (uint32 i = 0; i < std::size(texNames); i++) {
        pPlatebackTexTab[i] = FindTXDAndSetFlags(texNames[i], RwTextureFilterMode::rwFILTERLINEAR);
    }

    CTxdStore::PopCurrentTxd();

    pCharsetLockedData = RwRasterLock(RwTextureGetRaster(pCharsetTex), 0, rwRASTERLOCKREAD);
    return pCharsetLockedData != nullptr;
}

// 0x6FD720
void CCustomCarPlateMgr::Shutdown() {
    if (pCharsetTex) {
        RwRasterUnlock(RwTextureGetRaster(pCharsetTex));
        pCharsetLockedData = nullptr;

        RwTextureDestroy(pCharsetTex);
        pCharsetTex = nullptr;
    }

    for (auto& tex : pPlatebackTexTab) {
        if (tex) {
            RwTextureDestroy(tex);
            tex = nullptr;
        }
    }

    CTxdStore::SafeRemoveTxdSlot("vehicle");
}

// Returns texture index in pPlatebackTexTab. See texture names in ::Initialise.
// 0x6FD7A0
int8 CCustomCarPlateMgr::GetMapRegionPlateDesign() {
    switch (CWeather::WeatherRegion) {
    case eWeatherRegion::WEATHER_REGION_LA:
        return 2;
    case eWeatherRegion::WEATHER_REGION_LV:
    case eWeatherRegion::WEATHER_REGION_DESERT:
        return 1;
    case eWeatherRegion::WEATHER_REGION_DEFAULT:
    case eWeatherRegion::WEATHER_REGION_SF:
        return 0;
    default:
        assert(false);
        return -1;
    }
}

// 0x6FDC00
// unused
int8 CCustomCarPlateMgr::LoadPlatecharsetDat(const char* filename, uint8* data) {
    return plugin::CallAndReturn<int8, 0x6FDC00, const char*, uint8*>(filename, data);
}

auto ResolvePlateType(uint8 plateType) {
    return plateType == (uint8)-1 ? CCustomCarPlateMgr::GetMapRegionPlateDesign() : plateType;
}

// 0x6FDE50
void CCustomCarPlateMgr::SetupMaterialPlatebackTexture(RpMaterial* material, uint8 plateType) {
    assert(material);

    RpMaterialSetTexture(material, pPlatebackTexTab[ResolvePlateType(plateType)]);
}

// 0x6FD7C0 - Can't be hooked - Calling conventions differ - So we're allowed to use `std::pair` :D
// Returns <col, row> of character in the charset
std::pair<uint32, uint32> GetCharacterPositionInCharSet(char c) {
    switch (c)
    {
    case '0':
        return { 2, 6 };
    case '1':
        return { 3, 6 };
    case '2':
        return { 0, 7 };
    case '3':
        return { 1, 7 };
    case '4':
        return { 2, 7 };
    case '5':
        return { 3, 7 };
    case '6':
        return { 0, 8 };
    case '7':
        return { 1, 8 };
    case '8':
        return { 2, 8 };
    case '9':
        return { 3, 8 };
    case 'A':
    case 'a':
        return { 0, 0 };
    case 'B':
    case 'b':
        return { 1, 0 };
    case 'C':
    case 'c':
        return { 2, 0 };
    case 'D':
    case 'd':
        return { 3, 0 };
    case 'E':
    case 'e':
        return { 0, 1 };
    case 'F':
    case 'f':
        return { 1, 1 };
    case 'G':
    case 'g':
        return { 2, 1 };
    case 'H':
    case 'h':
        return { 3, 1 };
    case 'I':
    case 'i':
        return { 0, 2 };
    case 'J':
    case 'j':
        return { 1, 2 };
    case 'K':
    case 'k':
        return { 2, 2 };
    case 'L':
    case 'l':
        return { 3, 2 };
    case 'M':
    case 'm':
        return { 0, 3 };
    case 'N':
    case 'n':
        return { 1, 3 };
    case 'O':
    case 'o':
        return { 2, 3 };
    case 'P':
    case 'p':
        return { 3, 3 };
    case 'Q':
    case 'q':
        return { 0, 4 };
    case 'R':
    case 'r':
        return { 1, 4 };
    case 'S':
    case 's':
        return { 2, 4 };
    case 'T':
    case 't':
        return { 3, 4 };
    case 'U':
    case 'u':
        return { 0, 5 };
    case 'V':
    case 'v':
        return { 1, 5 };
    case 'W':
    case 'w':
        return { 2, 5 };
    case 'X':
    case 'x':
        return { 3, 5 };
    case 'Y':
    case 'y':
        return { 0, 6 };
    case 'Z':
    case 'z':
        return { 1, 6 };
    default:
        return { 0, 9 };
    }
}

// 0x6FDD70
// FIX ME
bool CCustomCarPlateMgr::RenderLicenseplateTextToRaster(const char* text, RwRaster * charsRaster, _IGNORED_ void* palette, RwRaster* plateRaster) {
    assert(text);
    assert(charsRaster);
    assert(plateRaster);

    const auto lockedPlateRaster = RwRasterLock(plateRaster, 0, rwRASTERLOCKNOFETCH | rwRASTERLOCKWRITE);
    if (!lockedPlateRaster)
        return false;

    if (!pCharsetLockedData)
        return false;

    const auto plateRasterStride = RwRasterGetStride(plateRaster);
    if (!plateRasterStride)
        return false;

    const auto charsRasterStride = RwRasterGetStride(charsRaster);
    if (!charsRasterStride)
        return false;

    // Copy each character from charset raster to plate raster
    // Going from left to right

    auto plateRasterCharIter = lockedPlateRaster; // Always points to the top left corner of each character
    for (auto letter = 0; letter < MAX_TEXT_LENGTH; letter++) {
        const auto [charCol, charRow] = GetCharacterPositionInCharSet(text[letter]);

        // Copy specific character from charset raster to plate raster

        // Size of a pixel (texel) in `pCharsetLockedData`. It's in 32 bit BGRA format
        constexpr auto texelSize = 4;

        // Character's top left corner in charset raster
        auto charRasterIt = &pCharsetLockedData[(CHARSET_COL_WIDTH * CHARSET_ROW_HEIGHT * charRow + CHARSET_CHAR_WIDTH * charCol) * texelSize];

        // Character's top left corner in target (plate) raster
        auto plateRasterIt = plateRasterCharIter;

        // Copy character row by row (going from top to bottom) to target (plate) raster
        for (auto r = 0u; r < CHARSET_CHAR_HEIGHT; r++) {
            memcpy(plateRasterIt, charRasterIt, CHARSET_CHAR_WIDTH * texelSize); // Copy row

            // Advance to next row
            plateRasterIt += plateRasterStride;
            charRasterIt += charsRasterStride;
        }

        // Advance to next character's column
        plateRasterCharIter += CHARSET_CHAR_WIDTH * texelSize;
    }

    RwRasterUnlock(plateRaster);

    return true;
}

// 0x6FDEA0
RwTexture* CCustomCarPlateMgr::CreatePlateTexture(const char* text, uint8 plateType) {
    assert(text);

    const auto plateRaster = RwRasterCreate(64, 16, 32, rwRASTERFORMAT888 | rwRASTERPIXELLOCKEDWRITE);
    if (!plateRaster)
        return nullptr;

    if (!RwTextureGetRaster(pCharsetTex)) {
        RwRasterDestroy(plateRaster);
        return nullptr;
    }

    if (!RenderLicenseplateTextToRaster(text, RwTextureGetRaster(pCharsetTex), pPalette1555Tab[ResolvePlateType(plateType)], plateRaster)) {
        RwRasterDestroy(plateRaster);
        return nullptr;
    }

    if (const auto plateTex = RwTextureCreate(plateRaster)) {
        RwTextureSetName(plateTex, text);
        RwTextureSetFilterMode(plateTex, rwFILTERNEAREST);
        return plateTex;
    }

    return nullptr;
}

// 0x6FDF50
RpMaterial* CCustomCarPlateMgr::MaterialUpgradeSetCarplateTextureCB(RpMaterial* material, void* geometry) {
    //printf("[Debug - Trace]: MaterialUpgradeSetCarplateTextureCB");

    assert(material);
    assert(geometry);

    if (const auto tex = RpMaterialGetTexture(material)) {
        if (const auto name{ RwTextureGetName(tex) }) { // Can this ever be null?
            if (_stricmp(name, "careplate") == 0) {
                RpGeometryReplaceOldMaterialWithNewMaterial((RpGeometry*)geometry, material, CurrentLicensePlateMaterial);
            } else if (_stricmp(name, "carpback") == 0) {
                SetupMaterialPlatebackTexture(material, CurrentLicensePlateType);
            }
        }
    }
    return material;
}

// 0x6FDFC0
RpAtomic* CCustomCarPlateMgr::AtomicUpgradeSetCarplateTextureCB(RpAtomic* atomic, _IGNORED_ void* data) {
    DEV_LOG("[Debug - Trace]: AtomicUpgradeSetCarplateTextureCB");
    assert(atomic);

    RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), MaterialUpgradeSetCarplateTextureCB, RpAtomicGetGeometry(atomic));
    return atomic;
}

// 0x6FDFE0
int8 CCustomCarPlateMgr::SetupClumpAfterVehicleUpgrade(RpClump* clump, RpMaterial* plateMaterial, uint8 plateType) {
    assert(plateMaterial); // NOTSA: Don't think this should happen?
    assert(clump);

    if (!plateMaterial)
        return false;

    DEV_LOG("[Debug - Trace]: SetupClumpAfterVehicleUpgrade");

    CurrentLicensePlateType = plateType;
    CurrentLicensePlateMaterial = plateMaterial;
    RpClumpForAllAtomics(clump, AtomicUpgradeSetCarplateTextureCB, nullptr);

    return true;
}

// 0x6FE020
void CCustomCarPlateMgr::SetupMaterialPlateTexture(RpMaterial* material, const char* plateText, uint8 plateType) {
    assert(material);
    assert(plateText);

    if (material) {
        if (const auto plateTex = CreatePlateTexture(plateText, plateType)) {
            RpMaterialSetTexture(material, plateTex);
            RwTextureDestroy(plateTex);
        }
    }
}

// 0x6FE060
RpMaterial* CCustomCarPlateMgr::MaterialSetCarplateTextureCB(RpMaterial * material, void* plateText) {
    assert(material);
    assert(plateText);

    if (const auto tex = RpMaterialGetTexture(material)) {
        if (const auto name{ RwTextureGetName(tex) }) { // Can this ever be null?
            if (_stricmp(name, "carplate") == 0) {
                CurrentLicensePlateMaterial = material;
                SetupMaterialPlateTexture(material, (const char*)plateText, CurrentLicensePlateType);
            } else if (_stricmp(name, "carpback") == 0) {
                SetupMaterialPlatebackTexture(material, CurrentLicensePlateType);
            }
        }
    }
    return material;
}

// 0x6FE0D0
RpAtomic* CCustomCarPlateMgr::AtomicSetCarplateTextureCB(RpAtomic* atomic, void* plateText) {
    assert(atomic);
    assert(plateText);

    RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), MaterialSetCarplateTextureCB, plateText);
    return atomic;
}

// 0x6FE0F0
RpMaterial* CCustomCarPlateMgr::SetupClump(RpClump* clump, char* plateText, uint8 plateType) {
    CurrentLicensePlateType = plateType;
    CurrentLicensePlateMaterial = nullptr;
    RpClumpForAllAtomics(clump, AtomicSetCarplateTextureCB, plateText);
    return CurrentLicensePlateMaterial;
}

// 0x6FD5B0
bool CCustomCarPlateMgr::GeneratePlateText(char* out, uint32 length) {
    if (length < 4)
        return false;

    constexpr auto RandomAlphanumeric = [] {
        return (char)CGeneral::GetRandomNumberInRange('A', 'Z');
    };

    constexpr auto RandomNumeric = [] {
        return (char)CGeneral::GetRandomNumberInRange('0', '9');
    };

    uint32 charIdx{};
    const auto Put = [&](char c) {
        out[charIdx++] = c;
    };

    Put(RandomAlphanumeric());
    Put(RandomAlphanumeric());
    Put(RandomNumeric());
    Put(RandomNumeric());

    for (auto i = charIdx; i < length; i += 4) {
        Put(' ');
        Put(RandomNumeric());
        Put(RandomAlphanumeric());
        Put(RandomAlphanumeric());
    }

    return true;
}
