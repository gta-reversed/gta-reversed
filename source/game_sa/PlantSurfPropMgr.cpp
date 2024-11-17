#include "StdInc.h"

#include "PlantSurfPropMgr.h"

void CPlantSurfPropMgr::InjectHooks() {
    RH_ScopedClass(CPlantSurfPropMgr);
    RH_ScopedCategory("Plant");

    RH_ScopedInstall(Initialise, 0x5DD6C0);
    RH_ScopedInstall(AllocSurfProperties, 0x5DD370);
    RH_ScopedInstall(GetSurfProperties, 0x6F9DE0);
    RH_ScopedInstall(LoadPlantsDat, 0x5DD3B0);
}

// 0x5DD6C0
bool CPlantSurfPropMgr::Initialise() {
    m_countSurfPropsAllocated = 0;
    rng::fill(m_SurfPropPtrTab, nullptr);
    for (auto& props : m_SurfPropTab) {
        rng::fill(props.m_PlantData, CPlantSurfPropPlantData{});
    }

    return LoadPlantsDat("PLANTS.DAT");
}

void CPlantSurfPropMgr::Shutdown() {
    // NOP
}

// 0x5DD370
CPlantSurfProp* CPlantSurfPropMgr::AllocSurfProperties(uint16 surfaceId, bool clearAllocCount) {
    if (clearAllocCount) {
        m_countSurfPropsAllocated = 0;
        return nullptr;
    }

    if (m_countSurfPropsAllocated >= MAX_SURFACE_PROPERTIES) {
        return nullptr;
    }

    m_SurfPropPtrTab[surfaceId] = &m_SurfPropTab[m_countSurfPropsAllocated++];
    return m_SurfPropPtrTab[surfaceId];
}

// 0x6F9DE0
CPlantSurfProp* CPlantSurfPropMgr::GetSurfProperties(uint16 index) {
    return index < MAX_SURFACE_PTR_PROPERTIES ? m_SurfPropPtrTab[index] : nullptr;
}

// 0x5DD3B0
bool CPlantSurfPropMgr::LoadPlantsDat(const char* filename) {
    char errorMsg[128];
    uint16 pcdId;
    uint32 surfaceId;

    CFileMgr::SetDir("DATA");
    auto file = CFileMgr::OpenFile(filename, "r");
    CFileMgr::SetDir("");
    int32 lineId = 0;
    for (char* line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        ++lineId;
        if (!strcmp(line, ";the end"))
            break;

        if (*line == ';')
            continue;

        ePlantField field = ePlantField::NAME;
        CPlantSurfProp* surfProperties = nullptr;
        char* lastToken{};
        char* surfaceName = strtok_s(line, " \t", &lastToken);

        CPlantSurfPropPlantData* plant = nullptr;
        do {
            switch (field) {
            case ePlantField::NAME:
                surfaceId = g_surfaceInfos.GetSurfaceIdFromName(surfaceName);
                if (surfaceId != SURFACE_DEFAULT) {
                    surfProperties = GetSurfProperties(surfaceId);
                    if (surfProperties || (surfProperties = AllocSurfProperties(surfaceId, false))) {
                        field = ePlantField::NAME;
                        break;
                    }
                } else {
                    sprintf_s(errorMsg, "Unknown surface name '%s' in 'Plants.dat' (line %d)! See Andrzej to fix this.", surfaceName, lineId);
                }
                return false;
            case ePlantField::PCD_ID:
                pcdId = atoi(surfaceName);
                if (pcdId > 2) {
                    pcdId = 0;
                }
                assert(surfProperties);

                plant = &surfProperties->m_PlantData[pcdId];
                break;
            case ePlantField::SLOT_ID:
                surfProperties->m_nPlantSlotID = atoi(surfaceName);
                break;
            case ePlantField::MODEL_ID:
                plant->m_nModelID = atoi(surfaceName);
                break;
            case ePlantField::UV_OFFSET:
                plant->m_nTextureID = atoi(surfaceName);
                break;
            case ePlantField::COLOR_R:
                plant->m_rgbaColor.r = atoi(surfaceName);
                break;
            case ePlantField::COLOR_G:
                plant->m_rgbaColor.g = atoi(surfaceName);
                break;
            case ePlantField::COLOR_B:
                plant->m_rgbaColor.b = atoi(surfaceName);
                break;
            case ePlantField::INTENSITY:
                plant->m_nIntensity = atoi(surfaceName);
                break;
            case ePlantField::INTENSITY_VARIATION:
                plant->m_nIntensityVar = atoi(surfaceName);
                break;
            case ePlantField::COLOR_ALPHA:
                plant->m_rgbaColor.a = atoi(surfaceName);
                break;
            case ePlantField::SCALE_XY:
                plant->m_fScaleXY = static_cast<float>(atof(surfaceName));
                break;
            case ePlantField::SCALE_Z:
                plant->m_fScaleZ = static_cast<float>(atof(surfaceName));
                break;
            case ePlantField::SCALE_VARIATION_XY:
                plant->m_fScaleVarXY = static_cast<float>(atof(surfaceName));
                break;
            case ePlantField::SCALE_VARIATION_Z:
                plant->m_fScaleVarZ = static_cast<float>(atof(surfaceName));
                break;
            case ePlantField::WIND_BENDING_SCALE:
                plant->m_fWindBendScale = static_cast<float>(atof(surfaceName));
                break;
            case ePlantField::WIND_BENDING_VARIATION:
                plant->m_fWindBendVar = static_cast<float>(atof(surfaceName));
                break;
            case ePlantField::DENSITY:
                plant->m_fDensity = static_cast<float>(atof(surfaceName));
                break;
            default:
                break;
            }
            surfaceName = strtok_s(nullptr, " \t", &lastToken);
            field = static_cast<ePlantField>(static_cast<int32>(field) + 1);
        } while (surfaceName);

        if (field < ePlantField::ALL_FIELDS_READED)
            return false;
    }
    CFileMgr::CloseFile(file);
    return true;
}
