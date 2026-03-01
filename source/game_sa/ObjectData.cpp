#include "StdInc.h"

#include "ObjectData.h"

CObjectData(&CObjectData::ms_aObjectInfo)[NUM_OBJECT_INFOS] = *(CObjectData(*)[NUM_OBJECT_INFOS])0xBB4A90;

void CObjectData::InjectHooks()
{
    RH_ScopedClass(CObjectData);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5B5360);
    RH_ScopedInstall(SetObjectData, 0x5A2D00);
}

// 0x5B5360
void CObjectData::Initialise(const char* fileName) {
    ZoneScoped;
    ZoneText(fileName, strlen(fileName));

    auto& default0 = CObjectData::GetDefault();
    default0.m_fMass = 99999.0F;
    default0.m_fTurnMass = 99999.0F;
    default0.m_fAirResistance = 0.99F;
    default0.m_fElasticity = 0.1F;
    default0.m_fBuoyancyConstant = 1599.9841F;
    default0.m_fUprootLimit = 0.0F;
    default0.m_fColDamageMultiplier = 1.0F;
    default0.m_nColDamageEffect = COL_DAMAGE_EFFECT_NONE;
    default0.m_nSpecialColResponseCase = COL_SPECIAL_RESPONSE_NONE;
    default0.m_nCameraAvoidObject = eObjectCameraAvoidType::CAMERA_UNKNOWN;

    auto& default1 = CObjectData::GetAtIndex(1);
    default1 = default0;
    default1.m_nCameraAvoidObject = eObjectCameraAvoidType::CAMERA_IGNORE;

    auto& default2 = CObjectData::GetAtIndex(2);
    default2 = default0;
    default2.m_nSpecialColResponseCase = COL_SPECIAL_RESPONSE_FENCEPART;

    auto& default3 = CObjectData::GetAtIndex(3);
    default3 = default0;
    default3.m_nCameraAvoidObject = eObjectCameraAvoidType::CAMERA_IGNORE;
    default3.m_nSpecialColResponseCase = COL_SPECIAL_RESPONSE_FENCEPART;

    auto& default4 = CObjectData::GetAtIndex(4);
    default4 = default0;
    default4.m_nSpecialColResponseCase = COL_SPECIAL_RESPONSE_GRENADE;

    CFileMgr::SetDir("");
    auto* file = CFileMgr::OpenFile(fileName, "rb");
    char* line;
    auto iFirstFreeInd = 5;
    while((line = CFileLoader::LoadLine(file)))
    {
        if (!line[0] || line[0] == ';')
            continue;

        if (line[0] == '*')
            break;

        if (line[0] == '#') // NOTSA: Skip comments so we can use a sanity check for sscanf.
            continue;

        auto& curInfo = CObjectData::GetAtIndex(iFirstFreeInd);
        memset(&curInfo, 0, sizeof(CObjectData));
        char modelName[256], effectName[256];
        float fPercentSubmerged;
        int32 iColDamEffect, iSpecialColResp, iCameraAvoid, iCausesExplosion, iFxType; // Have to be read as 32-bit integers and later assigned to 8 bit int32
        VERIFY(sscanf_s(line, "%s %f %f %f %f %f %f %f %d %d %d %d %d %f %f %f %s %f %f %f %f %f %d %d", // FIX_BUGS: Sized string read
            SCANF_S_STR(modelName),
            &curInfo.m_fMass,
            &curInfo.m_fTurnMass,
            &curInfo.m_fAirResistance,
            &curInfo.m_fElasticity,
            &fPercentSubmerged,
            &curInfo.m_fUprootLimit,
            &curInfo.m_fColDamageMultiplier,
            &iColDamEffect,
            &iSpecialColResp,
            &iCameraAvoid,
            &iCausesExplosion,
            &iFxType,
            &curInfo.m_vFxOffset.x,        // optional
            &curInfo.m_vFxOffset.y,        // optional
            &curInfo.m_vFxOffset.z,        // optional
            SCANF_S_STR(effectName),      // optional
            &curInfo.m_fSmashMultiplier,   // optional (Breakable Info)
            &curInfo.m_vecBreakVelocity.x, // optional (Breakable Info)
            &curInfo.m_vecBreakVelocity.y, // optional (Breakable Info)
            &curInfo.m_vecBreakVelocity.z, // optional (Breakable Info)
            &curInfo.m_fBreakVelocityRand, // optional (Breakable Info)
            &curInfo.m_nGunBreakMode,      // optional (Breakable Info)
            &curInfo.m_nSparksOnImpact     // optional (Breakable Info)
        ) >= 13);

        curInfo.m_pFxSystemBP = nullptr;
        curInfo.m_nFxType = iFxType;
        if (iFxType > 0)
            curInfo.m_pFxSystemBP = g_fxMan.FindFxSystemBP(effectName);

        const auto fBuoyancy = (100.0F / fPercentSubmerged) * (curInfo.m_fMass / 125.0F);
        curInfo.m_nSpecialColResponseCase = iSpecialColResp;
        curInfo.m_nColDamageEffect = iColDamEffect;
        curInfo.m_nCameraAvoidObject = iCameraAvoid;
        curInfo.m_bCausesExplosion = iCausesExplosion;
        curInfo.m_fBuoyancyConstant = fBuoyancy;

        auto *mi = CModelInfo::GetModelInfo(modelName, nullptr);
        if (!mi)
            continue;

        if (curInfo.m_fMass == default0.m_fMass
            && curInfo.m_fColDamageMultiplier == default0.m_fColDamageMultiplier
            && curInfo.m_nColDamageEffect == default0.m_nColDamageEffect)
        {
            if (curInfo.m_nSpecialColResponseCase == default0.m_nSpecialColResponseCase)
            {
                mi->m_nObjectInfoIndex = curInfo.m_nCameraAvoidObject == 0 ? 0 : 1;
                continue;
            }

            if (curInfo.m_nSpecialColResponseCase == default2.m_nSpecialColResponseCase)
            {
                mi->m_nObjectInfoIndex = curInfo.m_nCameraAvoidObject == 0 ? 2 : 3;
                continue;
            }
        }

        int32 iCheckedInd = 0;
        bool bFound = false;
        while (iCheckedInd < iFirstFreeInd)
        {
            if (curInfo == CObjectData::GetAtIndex(iCheckedInd))
            {
                mi->m_nObjectInfoIndex = iCheckedInd;
                bFound = true;
                break;
            }

            ++iCheckedInd;
        }

        if (bFound)
            continue;

        mi->m_nObjectInfoIndex = iFirstFreeInd;
        ++iFirstFreeInd;
    }

    CFileMgr::CloseFile(file);
}

// 0x5A2D00
void CObjectData::SetObjectData(int32 dwModelIndex, CObject& obj)
{
    auto* const mi = CModelInfo::GetModelInfo(dwModelIndex);
    if (mi->m_nObjectInfoIndex == -1)
    {
        obj.m_fMass = 99999.0F;
        obj.m_fTurnMass = 99999.0F;

        obj.physicalFlags.bApplyGravity = false;
        obj.physicalFlags.bExplosionProof = true;
        obj.physicalFlags.bCollidable = true;
        obj.physicalFlags.bDisableCollisionForce = true;

        obj.m_pObjectInfo = &CObjectData::GetDefault();
        return;
    }

    auto& objInfo = CObjectData::GetAtIndex(mi->m_nObjectInfoIndex);
    obj.m_pObjectInfo = &objInfo;
    obj.m_fMass = objInfo.m_fMass;
    obj.m_fTurnMass = objInfo.m_fTurnMass;
    obj.m_fAirResistance = objInfo.m_fAirResistance;
    obj.m_fElasticity = objInfo.m_fElasticity;
    obj.m_fBuoyancyConstant = objInfo.m_fBuoyancyConstant;
    obj.m_nColDamageEffect = objInfo.m_nColDamageEffect;

    if (objInfo.m_fMass >= 99998.0F)
    {
        obj.physicalFlags.bApplyGravity = false;
        obj.physicalFlags.bCollidable = true;
        obj.physicalFlags.bDisableCollisionForce = true;

        if (objInfo.m_nColDamageEffect == COL_DAMAGE_EFFECT_NONE)
            obj.physicalFlags.bExplosionProof = true;
    }

    switch(objInfo.m_nSpecialColResponseCase)
    {
    case COL_SPECIAL_RESPONSE_SWINGDOOR:
        obj.physicalFlags.bDisableMoveForce = true;
        obj.objectFlags.bIsDoorOpen = false;
        obj.objectFlags.bIsDoorMoving = false;
        break;
    case COL_SPECIAL_RESPONSE_LOCKDOOR:
        obj.physicalFlags.bDisableMoveForce = true;
        obj.physicalFlags.bCollidable = true;
        obj.physicalFlags.bDisableCollisionForce = true;
        obj.objectFlags.bIsDoorOpen = false;
        obj.objectFlags.bIsDoorMoving = true;
        break;
    case COL_SPECIAL_RESPONSE_HANGING:
        obj.physicalFlags.bInfiniteMass = true;
        obj.m_vecCentreOfMass.Set(0.0F, 0.0F, mi->GetColModel()->GetBoundingBox().m_vecMin.z * 0.8F);
        break;
    case COL_SPECIAL_RESPONSE_OB_COL_POOLBALL:
        obj.physicalFlags.bApplyGravity = false;
        obj.physicalFlags.bDisableZ = true;
        break;
    }
}

bool CObjectData::operator==(const CObjectData& right) const
{
    return m_fMass == right.m_fMass
        && m_fTurnMass == right.m_fTurnMass
        && m_fAirResistance == right.m_fAirResistance
        && m_fElasticity == right.m_fElasticity
        && m_fBuoyancyConstant == right.m_fBuoyancyConstant
        && m_fUprootLimit == right.m_fUprootLimit
        && m_fColDamageMultiplier == right.m_fColDamageMultiplier
        && m_nColDamageEffect == right.m_nColDamageEffect
        && m_nSpecialColResponseCase == right.m_nSpecialColResponseCase
        && m_nCameraAvoidObject == right.m_nCameraAvoidObject
        && m_bCausesExplosion == right.m_bCausesExplosion
        && m_nFxType == right.m_nFxType
        && m_vFxOffset == right.m_vFxOffset
        && m_pFxSystemBP == right.m_pFxSystemBP
        && m_fSmashMultiplier == right.m_fSmashMultiplier
        && m_vecBreakVelocity == right.m_vecBreakVelocity
        && m_fBreakVelocityRand == right.m_fBreakVelocityRand
        && m_nGunBreakMode == right.m_nGunBreakMode
        && m_nSparksOnImpact == right.m_nSparksOnImpact;

}

