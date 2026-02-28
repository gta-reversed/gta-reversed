#include "StdInc.h"

void C2dEffect::InjectHooks()
{
    RH_ScopedClass(C2dEffect);
    RH_ScopedCategory("Plugins");

// Class methods
    RH_ScopedInstall(Shutdown, 0x4C57D0);

// Statics
    RH_ScopedInstall(Roadsign_GetNumLinesFromFlags, 0x6FA640);
    RH_ScopedInstall(Roadsign_GetNumLettersFromFlags, 0x6FA670);
    RH_ScopedInstall(Roadsign_GetPaletteIDFromFlags, 0x6FA6A0);
    RH_ScopedInstall(PluginAttach, 0x6FA970);
    RH_ScopedInstall(DestroyAtomic, 0x4C54E0);

// RW PLUGIN
    RH_ScopedGlobalInstall(RpGeometryGet2dFxCount, 0x4C4340);
    RH_ScopedGlobalInstall(RpGeometryGet2dFxAtIndex, 0x4C4A40);

    RH_ScopedGlobalInstall(t2dEffectPluginConstructor, 0x6F9F90);
    RH_ScopedGlobalInstall(t2dEffectPluginDestructor, 0x6FA880);
    RH_ScopedGlobalInstall(t2dEffectPluginCopyConstructor, 0x6F9FB0);

    RH_ScopedGlobalInstall(Rwt2dEffectPluginDataChunkReadCallBack, 0x6F9FD0, { .reversed = true });
    RH_ScopedGlobalInstall(Rwt2dEffectPluginDataChunkWriteCallBack, 0x6FA620);
    RH_ScopedGlobalInstall(Rwt2dEffectPluginDataChunkGetSizeCallBack, 0x6FA630);
}

void C2dEffect::Shutdown()
{
    if (m_Type == e2dEffectType::EFFECT_ROADSIGN) {
        if (roadsign.m_pText) {
            CMemoryMgr::Free(roadsign.m_pText);
            roadsign.m_pText = nullptr;
        }

        if (roadsign.m_pAtomic) {
            C2dEffect::DestroyAtomic(roadsign.m_pAtomic);
            roadsign.m_pAtomic = nullptr;
        }
    }
    else if (m_Type == e2dEffectType::EFFECT_LIGHT) {
        if (light.m_pCoronaTex) {
            RwTextureDestroy(light.m_pCoronaTex);
            light.m_pCoronaTex = nullptr;
        }

        if (light.m_pShadowTex) {
            RwTextureDestroy(light.m_pShadowTex);
            light.m_pShadowTex = nullptr;
        }
    }
}

int32 C2dEffect::Roadsign_GetNumLinesFromFlags(CRoadsignAttrFlags flags)
{
    switch (flags.m_nNumOfLines) {
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 3;
    }

    return 4;
}

int32 C2dEffect::Roadsign_GetNumLettersFromFlags(CRoadsignAttrFlags flags)
{
    switch (flags.m_nSymbolsPerLine) {
    case 1:
        return 2;
    case 2:
        return 4;
    case 3:
        return 8;
    }

    return 16;
}

int32 C2dEffect::Roadsign_GetPaletteIDFromFlags(CRoadsignAttrFlags flags)
{

    switch (flags.m_nTextColor) {
    case 1:
        return 1;
    case 2:
        return 2;
    case 3:
        return 3;
    }

    return 0;
}

bool C2dEffect::PluginAttach()
{
    C2dEffect::g2dEffectPluginOffset = RpGeometryRegisterPlugin(
        sizeof(t2dEffectPlugin),
        MAKECHUNKID(rwVENDORID_DEVELOPER, 0xF8),
        t2dEffectPluginConstructor,
        t2dEffectPluginDestructor,
        t2dEffectPluginCopyConstructor
    );

    RpGeometryRegisterPluginStream(
        MAKECHUNKID(rwVENDORID_DEVELOPER, 0xF8),
        Rwt2dEffectPluginDataChunkReadCallBack,
        Rwt2dEffectPluginDataChunkWriteCallBack,
        Rwt2dEffectPluginDataChunkGetSizeCallBack
    );

    return C2dEffect::g2dEffectPluginOffset != -1;
}

void C2dEffect::DestroyAtomic(RpAtomic* atomic)
{
    if (!atomic)
        return;

    auto frame = RpAtomicGetFrame(atomic);
    if (frame) {
        RpAtomicSetFrame(atomic, nullptr);
        RwFrameDestroy(frame);
    }
    RpAtomicDestroy(atomic);
}

uint32 RpGeometryGet2dFxCount(RpGeometry* geometry)
{
    auto plugin = C2DEFFECTPLG(geometry, m_pEffectEntries);
    if (!plugin)
        return 0;

    return plugin->m_nObjCount;
}

C2dEffect* RpGeometryGet2dFxAtIndex(RpGeometry* geometry, int32 iEffectInd)
{
    auto plugin = C2DEFFECTPLG(geometry, m_pEffectEntries);
    return &plugin->m_pObjects[iEffectInd];
}

void* t2dEffectPluginConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject)
{
    C2DEFFECTPLG(object, m_pEffectEntries) = nullptr;
    return object;
}

void* t2dEffectPluginDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject)
{
    auto plugin = C2DEFFECTPLG(object, m_pEffectEntries);
    if (!plugin)
        return object;

    // It's the same as CModelInfo::ms_2dFXInfoStore cleaning, maybe the plugin has CStore inside too?
    // Dunno how that would work, as the size is decided at runtime, easy with some manual memory tricks tho.
    for (uint32 i = 0; i < plugin->m_nObjCount; ++i) {
        auto& effect = plugin->m_pObjects[i];
        effect.Shutdown();
    }

    if (C2DEFFECTPLG(object, m_pEffectEntries))
        CMemoryMgr::Free(C2DEFFECTPLG(object, m_pEffectEntries));

    return object;
}

void* t2dEffectPluginCopyConstructor(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject)
{
    C2DEFFECTPLG(dstObject, m_pEffectEntries) = nullptr;
    return dstObject;
}

RwStream* Rwt2dEffectPluginDataChunkReadCallBack(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject)
{
    if (C2dEffect::ms_nTxdSlot == -1) {
        C2dEffect::ms_nTxdSlot = CTxdStore::FindTxdSlot("particle");
    }

    uint32 numEffects{};
    RwStreamRead(stream, &numEffects, sizeof(numEffects));
    if (!numEffects) {
        return stream;
    }

    auto* e = static_cast<t2dEffectPluginEntry*>(CMemoryMgr::Malloc(numEffects * sizeof(C2dEffect) + sizeof(t2dEffectPluginEntry::m_nObjCount)));

    uint32 countedNumFx{numEffects};
    for (auto i = 0u; i < numEffects; i++) {
        auto& fx = e->m_pObjects[i];
        RwStreamRead(stream, &fx.m_Pos, sizeof(CVector));

        uint32 type{}, sectionSize{};
        RwStreamRead(stream, &type, sizeof(uint32));
        RwStreamRead(stream, &sectionSize, sizeof(uint32));
        fx.m_Type = static_cast<e2dEffectType>(type);

        switch (fx.m_Type) {
        case e2dEffectType::EFFECT_LIGHT: {
            auto* light = C2dEffect::Cast<C2dEffectLight>(&fx);

            t2dEffectLightStreamData d{};
            if (sectionSize != 80 && sectionSize != 76) {
                NOTSA_LOG_DEBUG("Invalid light 2dfx section size: {}, skipping...", sectionSize);
                break;
            }

            RwStreamRead(stream, &d, sectionSize);
            light->m_color                   = d.Color;
            light->m_fCoronaSize             = d.CoronaSize;
            light->m_fCoronaFarClip          = d.CoronaFarClip;
            light->m_bCoronaEnableReflection = d.CoronaEnableReflection;
            light->m_fShadowSize             = d.ShadowSize;
            light->m_nCoronaFlareType        = d.CoronaFlareType;
            light->m_fPointlightRange        = d.PointlightRange;
            light->m_nCoronaFlashType        = d.CoronaFlashType;
            light->m_nShadowColorMultiplier  = d.ShadowColorMultiplier;
            light->m_nFlags                  = ((uint16)d.Flags2 << 8) | d.Flags1;
            light->m_nShadowZDistance        = d.ShadowZDistance;

            // Lights may not have light dir XYZ values at the end
            light->offsetX = (sectionSize == 80) ? d.OffsetX : 0;
            light->offsetY = (sectionSize == 80) ? d.OffsetY : 0;
            light->offsetZ = (sectionSize == 80) ? d.OffsetZ : 100;

            {
                CTxdStore::ScopedTXDSlot _{ C2dEffect::ms_nTxdSlot };
                light->m_pCoronaTex = RwTextureRead(d.CoronaName, nullptr);
                light->m_pShadowTex = RwTextureRead(d.ShadowName, nullptr);
            }
            continue;
        }
        case e2dEffectType::EFFECT_ROADSIGN: {
            auto* sign = C2dEffect::Cast<C2dEffectRoadsign>(&fx);

            if (sectionSize != sizeof(t2dEffectRoadsignStreamData)) {
                NOTSA_LOG_DEBUG("Invalid roadsign 2dfx section size: {}", sectionSize);
                break;
            }

            t2dEffectRoadsignStreamData d{};
            RwStreamRead(stream, &d, sizeof(t2dEffectRoadsignStreamData));
            sign->m_vecSize     = d.Size;
            sign->m_vecRotation = d.Rotation;
            sign->m_nFlags      = d.Flags;
            sign->m_pAtomic     = nullptr;
            sign->m_pText       = static_cast<RwChar*>(CMemoryMgr::Malloc(sizeof(d.Text)));
            std::memcpy(sign->m_pText, d.Text, sizeof(d.Text));
            continue;
        }
        case e2dEffectType::EFFECT_ENEX: {
            auto* enex = C2dEffect::Cast<C2dEffectEnEx>(&fx);

            if (sectionSize != 44 && sectionSize != 40) {
                NOTSA_LOG_DEBUG("Invalid enex 2dfx section size: {}", sectionSize);
                break;
            }

            t2dEffectEnExStreamData d{};
            RwStreamRead(stream, &d, sectionSize);
            enex->m_vecRadius   = d.Radius;
            enex->m_fEnterAngle = d.EnterAngle;
            enex->m_vecExitPosn = d.ExitPos;
            enex->m_nInteriorId = d.InteriorId;
            enex->m_fExitAngle  = d.ExitAngle;
            enex->m_nSkyColor   = d.SkyColor;
            enex->m_nFlags1     = d.Flags1;
            std::memcpy(enex->m_szInteriorName, d.InteriorName, sizeof(d.InteriorName));

            enex->m_nTimeOn  = (sectionSize == 44) ? d.TimeOn : 0;
            enex->m_nTimeOff = (sectionSize == 44) ? d.TimeOn : 24;
            enex->m_nFlags2  = d.Flags2; // TODO: ???
            continue;
        }
        case e2dEffectType::EFFECT_COVER_POINT: {
            auto* cp = C2dEffect::Cast<C2dEffectCoverPoint>(&fx);

            if (sectionSize != sizeof(t2dEffectCoverPointStreamData)) {
                NOTSA_LOG_DEBUG("Invalid cp 2dfx section size: {}", sectionSize);
                break;
            }

            t2dEffectCoverPointStreamData d{};
            RwStreamRead(stream, &d, sizeof(t2dEffectCoverPointStreamData));
            cp->m_DirOfCover = d.Dir;
            cp->m_Usage      = d.Usage;
            continue;
        }
        case e2dEffectType::EFFECT_SUN_GLARE:
            NOTSA_UNREACHABLE("sunglare 2dfx unused");
            RwStreamSkip(stream, sectionSize);
            --i; // We haven't added anything, so skip incrementing.
            break;
        case e2dEffectType::EFFECT_TRIGGER_POINT: {
            auto* wheel = C2dEffect::Cast<C2dEffectSlotMachineWheel>(&fx);

            if (sectionSize != sizeof(t2dEffectSlotMachineWheelStreamData)) {
                NOTSA_LOG_DEBUG("Invalid slot machine wheel 2dfx section size: {}", sectionSize);
                break;
            }

            t2dEffectSlotMachineWheelStreamData d{};
            RwStreamRead(stream, &d, sizeof(t2dEffectSlotMachineWheelStreamData));
            wheel->m_nId = d.PointId;
            continue;
        }
        case e2dEffectType::EFFECT_ESCALATOR: {
            auto* e = C2dEffect::Cast<C2dEffectEscalator>(&fx);

            if (sectionSize != sizeof(t2dEffectEscalatorStreamData)) {
                NOTSA_LOG_DEBUG("Invalid escalator 2dfx section size: {}", sectionSize);
                break;
            }

            t2dEffectEscalatorStreamData d{};
            RwStreamRead(stream, &d, sizeof(t2dEffectEscalatorStreamData));
            e->m_nDirection = d.Direction;
            e->m_vecBottom  = d.Bottom;
            e->m_vecEnd     = d.End;
            e->m_vecTop     = d.Top;
            continue;
        }
        case e2dEffectType::EFFECT_ATTRACTOR: {
            auto* pa = C2dEffect::Cast<C2dEffectPedAttractor>(&fx);

            if (sectionSize != sizeof(t2dEffectPedAttractorStreamData)) {
                NOTSA_LOG_DEBUG("Invalid ped attractor 2dfx section size: {}", sectionSize);
                break;
            }

            t2dEffectPedAttractorStreamData d{};
            RwStreamRead(stream, &d, sizeof(t2dEffectPedAttractorStreamData));
            pa->m_vecQueueDir    = d.QueueDir;
            pa->m_vecUseDir      = d.UseDir;
            pa->m_vecForwardDir  = d.ForwardDir;
            pa->m_nAttractorType = d.Type;

            // TODO: unk/not_used fields assign to higher portion of m_nAttractorType, check.
            continue;
        }
        case e2dEffectType::EFFECT_PARTICLE: {
            auto* p = C2dEffect::Cast<C2dEffectParticle>(&fx);

            if (sectionSize != sizeof(t2dEffectParticleStreamData)) {
                NOTSA_LOG_DEBUG("Invalid particle 2dfx section size: {}", sectionSize);
                break;
            }

            t2dEffectParticleStreamData d{};
            RwStreamRead(stream, &d, sizeof(t2dEffectParticleStreamData));
            std::memcpy(p->m_szName, d, sizeof(t2dEffectParticleStreamData));
            continue;
        }
        default:
            NOTSA_LOG_WARN("Unknown 2dfx type: {}", +fx.m_Type);
            continue; // not skipping unintendedly?
        }

        RwStreamSkip(stream, sectionSize);
        --i; // We haven't added anything, so skip incrementing.
        --countedNumFx;
    }

    e->m_nObjCount = countedNumFx; // correct the count without counting the skipped ones.
    RWPLUGINOFFSET(t2dEffectPlugin, object, C2dEffect::g2dEffectPluginOffset)->m_pEffectEntries = e;
    return stream;
}

RwStream* Rwt2dEffectPluginDataChunkWriteCallBack(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject)
{
    return stream;
}

RwInt32 Rwt2dEffectPluginDataChunkGetSizeCallBack(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject)
{
    return -1;
}
