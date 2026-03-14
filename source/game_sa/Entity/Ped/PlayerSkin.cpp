#include "StdInc.h"

auto& gpPlayerClump = StaticRef<RpClump*>(0xC3F040);

void CPlayerSkin::InjectHooks() {
    RH_ScopedClass(CPlayerSkin);
    RH_ScopedCategory("Entity/Ped");

    RH_ScopedInstall(Initialise, 0x6FF8A0);
    RH_ScopedInstall(Shutdown, 0x6FF8D0);
    RH_ScopedInstall(RenderFrontendSkinEdit, 0x6FF900);
    RH_ScopedInstall(GetSkinTexture, 0x6FFA10);
}

// 0x6FF8A0
void CPlayerSkin::Initialise() {
    m_txdSlot = CTxdStore::AddTxdSlot("skin");
    CTxdStore::Create(m_txdSlot);
    CTxdStore::AddRef(m_txdSlot);
}

// 0x6FF8D0
void CPlayerSkin::Shutdown() {
    CTxdStore::RemoveTxdSlot(m_txdSlot);
}

// unused
// 0x6FF900
void CPlayerSkin::RenderFrontendSkinEdit() {
    static uint32 s_LastFlash; // 0xC3F04C
    static float  s_Angle;     // 0xC3F048

    RwRGBAReal color{ 255, 255, 255, 1.0f };
    if (CTimer::GetTimeInMSPauseMode() - s_LastFlash > 7) {
        s_Angle = s_Angle + 2.0f;
        if (s_Angle > 360.0) {
            s_Angle -= 360.0f;
        }
        s_LastFlash = CTimer::GetTimeInMSPauseMode();
    }
    auto* frame = RpClumpGetFrame(gpPlayerClump);
    RwFrameTransform(frame, RwFrameGetMatrix(RwCameraGetFrame(Scene.m_pRwCamera)), rwCOMBINEREPLACE);
    RwV3d vector = { 1.35f, 0.1f, 7.725f };
    RwFrameTranslate(frame, &vector, rwCOMBINEPRECONCAT);
    RwV3d axis = { 0.0f, 1.0f, 0.0f };
    RwFrameRotate(frame, &axis, s_Angle, rwCOMBINEPRECONCAT);
    RwFrameUpdateObjects(frame);
    SetAmbientColours(&color);
    RpClumpRender(gpPlayerClump);
}

// 0x6FFA10
RwTexture* CPlayerSkin::GetSkinTexture(const char* name) {
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(m_txdSlot);
    RwTexture* skinTexture = RwTextureRead(name, nullptr);
    CTxdStore::PopCurrentTxd();
    if (skinTexture) {
        return skinTexture;
    }

    const char* imagePath;
    if (!name[0] || !strcmp(name, "$$\"\"")) {
        imagePath = "models\\generic\\player.bmp";
    } else {
        imagePath = "skins\\%s.bmp";
    }
    sprintf_s(gString, imagePath, name);

    RwImage* image = RtBMPImageRead(gString);
    if (!image) {
        return skinTexture;
    }

    int32 width, height, depth, flags;
    RwImageFindRasterFormat(image, rwRASTERTYPETEXTURE, &width, &height, &depth, &flags);
    RwRaster* raster = RwRasterCreate(width, height, depth, flags);
    RwRasterSetFromImage(raster, image);
    RwTexture* texture = RwTextureCreate(raster);
    RwTextureSetName(texture, name);
    RwTextureSetFilterMode(texture, rwFILTERLINEAR);
    RwTexDictionaryAddTexture(CTxdStore::ms_pTxdPool->GetAtRef(m_txdSlot)->m_pRwDictionary, texture);
    RwImageDestroy(image);
    return texture;
}
