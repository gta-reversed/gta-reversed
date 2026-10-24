#include "StdInc.h"

#include "ProjectileInfo.h"

#include "Entity/Object/Projectile.h"
#include "Radar.h"
#include "World.h"
#include "Pools/Pools.h"
#include "Collision/Box.h"
#include "Timer.h"
#include "General.h"
#include "Camera.h"
#include "Streaming.h"
#include "WeaponInfo.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Physical.h"
#include "Fx/FxManager.h"
#include "Fx/FxSystem.h"
#include "Audio/AudioEngine.h"
#include "ObjectData.h"
#include "MemoryMgr.h"
#include "Collision/ColModel.h"
#include "Collision/ColSphere.h"
#include "Models/ModelInfo.h"
#include "Enums/eSurfaceType.h"

void CProjectileInfo::InjectHooks() {
    RH_ScopedClass(CProjectileInfo);
    RH_ScopedCategoryGlobal();

    // Install("CProjectileInfo", "", , &CProjectileInfo::);
    RH_ScopedInstall(Initialise, 0x737B40, { .Reversed = false });
    RH_ScopedInstall(Shutdown, 0x737BC0, { .Reversed = false });
    RH_ScopedInstall(GetProjectileInfo, 0x737BF0, { .Reversed = false });
    RH_ScopedInstall(RemoveNotAdd, 0x737C00, { .Reversed = false });
    RH_ScopedInstall(AddProjectile, 0x737C80);
    RH_ScopedInstall(RemoveDetonatorProjectiles, 0x738860, { .Reversed = false });
    RH_ScopedInstall(RemoveProjectile, 0x7388F0, { .Reversed = false });
    RH_ScopedInstall(Update, 0x738B20, { .Reversed = false });
    RH_ScopedInstall(IsProjectileInRange, 0x739860);
    RH_ScopedInstall(RemoveAllProjectiles, 0x7399B0, { .Reversed = false });
    RH_ScopedInstall(RemoveIfThisIsAProjectile, 0x739A40, { .Reversed = false });
    RH_ScopedInstall(RemoveFXSystem, 0x737B80, { .Reversed = false });
}

// 0x737B40
void CProjectileInfo::Initialise() {
    plugin::Call<0x737B40>();
}

// 0x737BC0
void CProjectileInfo::Shutdown() {
    plugin::Call<0x737BC0>();
}

// 0x737BF0
CProjectileInfo* CProjectileInfo::GetProjectileInfo(int32 infoId) {
    return plugin::CallAndReturn<CProjectileInfo*, 0x737BF0, int32>(infoId);
}

// 0x737C00
void CProjectileInfo::RemoveNotAdd(CEntity* creator, eWeaponType weaponType, CVector pos) {
    plugin::Call<0x737C00, CEntity*, eWeaponType, CVector>(creator, weaponType, pos);
}

// 0x737C80
bool CProjectileInfo::AddProjectile(CEntity* creator, eWeaponType weaponType, CVector origin, float force, const CVector* direction, CEntity* victim) {
    CMatrix transform;           // Transform the projectile will be created with
    CVector velocity;            // Initial velocity
    float   elasticity{ 0.75f }; // 0x3F400000
    bool    bSetFxBrain{};       // Original: local_a6 == 5 -> m_pObjectInfo = &CObjectData::ms_aObjectInfo[4] (0xBB4BD0)
    bool    bApplyGravity{ true }; // Original: local_a5 (only rockets are created without gravity)
    int32   destroyTime{};

    const auto setThrowTransform = [&](float heading) {
        transform.SetTranslate(CVector{});
        const auto sine = static_cast<float>(std::sin(static_cast<double>(heading)));
        const auto cosine = std::cos(static_cast<double>(heading));
        // Original RotateZ changes only XY; a generic 3D product loses signed zero.
        for (auto* row : { &transform.GetRight(), &transform.GetForward(), &transform.GetUp(), &transform.GetPosition() }) {
            const float cosineX = static_cast<float>(cosine * row->x);
            const float sineY = sine * row->y;
            const float cosineY = static_cast<float>(cosine * row->y);
            const float sineX = sine * row->x;
            row->x = cosineX - sineY;
            row->y = cosineY + sineX;
        }
        transform.GetPosition() += origin;
    };

    const auto creatorType = creator ? creator->GetType() : ENTITY_TYPE_NOTHING;

    switch (weaponType) {
    case WEAPON_GRENADE:
    case WEAPON_REMOTE_SATCHEL_CHARGE: {
        destroyTime = CTimer::GetTimeInMS() + 2000;

        float speed = 0.0f;
        if (force != StaticRef<float>(0x858B50)) { // 0.0f
            speed = force * StaticRef<float>(0x8717AC) + StaticRef<float>(0x858FCC); // 0.22f + 0.15f
        }
        if (weaponType == WEAPON_REMOTE_SATCHEL_CHARGE) {
            speed *= StaticRef<float>(0x858B8C); // 0.5f
        }

        const float heading = creator
            ? (creatorType == ENTITY_TYPE_VEHICLE
                ? CGeneral::LimitRadianAngle(creator->GetHeading() + StaticRef<float>(0x858CB8)) // PI
                : creator->GetHeading())
            : 0.0f;

        // Original: SetTranslate(0, 0, 0) (0x59AF40) + RotateZ(heading) (0x59B390), then adds the origin
        setThrowTransform(heading);

        velocity.x = static_cast<float>(std::sin(static_cast<double>(heading)) * speed * StaticRef<float>(0x858C1C)); // -1.0f (ped forward is -Y)
        velocity.y = static_cast<float>(std::cos(static_cast<double>(heading)) * speed);
        velocity.z = (force + StaticRef<float>(0x858624)) * StaticRef<float>(0x858EE8) * speed; // (force + 1.0f) * 0.4f

        // Grenades thrown from a Sentinel inherit the car's velocity
        if (creator && creator->GetModelIndex() == MODEL_SENTINEL) { // 0x195
            velocity += static_cast<CPhysical*>(creator)->m_vecMoveSpeed;
        }

        bSetFxBrain  = true;
        bApplyGravity = true;
        elasticity   = 0.5f; // 0x3F000000
        if (weaponType == WEAPON_REMOTE_SATCHEL_CHARGE) {
            elasticity = 0.03f; // 0x3CF5C28F
        }
        break;
    }
    case WEAPON_TEARGAS: {
        destroyTime = CTimer::GetTimeInMS() + 20000;

        float speed = 0.0f;
        if (force != StaticRef<float>(0x858B50)) { // 0.0f
            speed = force * StaticRef<float>(0x8717AC) + StaticRef<float>(0x858FCC);
        }

        const float heading = creator ? creator->GetHeading() : 0.0f;

        setThrowTransform(heading);

        velocity.x = static_cast<float>(std::sin(static_cast<double>(heading)) * speed * StaticRef<float>(0x858C1C));
        velocity.y = static_cast<float>(std::cos(static_cast<double>(heading)) * speed);
        velocity.z = (force + StaticRef<float>(0x858624)) * StaticRef<float>(0x858EE8) * speed;

        bSetFxBrain  = true;
        bApplyGravity = true;
        elasticity   = 0.5f;
        break;
    }
    case WEAPON_MOLOTOV: {
        destroyTime = CTimer::GetTimeInMS() + 2000;

        float speed = force * StaticRef<float>(0x8717AC) + StaticRef<float>(0x858FCC);
        if (speed < StaticRef<float>(0x858CC4)) { // 0.2f
            speed = 0.2f;
        }

        const float heading = creator ? creator->GetHeading() : 0.0f;

        setThrowTransform(heading);

        velocity.x = static_cast<float>(std::sin(static_cast<double>(heading)) * speed * StaticRef<float>(0x858C1C));
        velocity.y = static_cast<float>(std::cos(static_cast<double>(heading)) * speed);
        velocity.z = (force * StaticRef<float>(0x858CC4) + StaticRef<float>(0x858EE8)) * speed; // (force * 0.2f + 0.4f)
        break;
    }
    case WEAPON_ROCKET:
    case WEAPON_ROCKET_HS: {
        destroyTime = CTimer::GetTimeInMS() + (weaponType == WEAPON_ROCKET ? 3000 : 10000);
        bApplyGravity = false; // Original: local_a5 = 0 (only case where it is set to false)

        float speed = weaponType == WEAPON_ROCKET ? 0.4f : 0.2f;

        if (creatorType == ENTITY_TYPE_VEHICLE) {
            transform = creator->GetMatrix(); // 0x411990 + 0x59BBC0
            transform.GetPosition() = origin;
            speed += static_cast<CPhysical*>(creator)->m_vecMoveSpeed.Magnitude();
        } else if (creatorType == ENTITY_TYPE_PED && static_cast<CPed*>(creator)->IsPlayer()) {
            // Fired by the player on foot: use the active camera basis (0x738122).
            // Original reads 0xB6F32C + activeCam * 0x8E => &CCam::m_vecFront, +0x24 => CCam::m_vecUp.
            const auto& cam = TheCamera.GetActiveCam();
            transform.GetRight()    = CrossProduct(cam.m_vecUp, cam.m_vecFront);
            transform.GetForward()  = cam.m_vecFront;
            transform.GetUp()       = cam.m_vecUp;
            transform.GetPosition() = origin;
        } else if (direction) {
            // Explicit direction given: forward = direction; right = creator's right; up = Cross(right, forward)
            transform.GetRight()    = creator->GetMatrix().GetRight();
            transform.GetForward()  = *direction;
            transform.GetUp()       = CrossProduct(transform.GetRight(), *direction);
            transform.GetPosition() = origin;
        } else {
            transform = creator ? creator->GetMatrix() : CMatrix{};
        }

        // Original: Multiply3x3 (0x59C790) (transform, CVector{0, speed, 0}) => speed * m_forward row (+0x10)
        velocity = transform.GetForward() * speed;
        break;
    }
    case WEAPON_FREEFALL_BOMB: {
        destroyTime = CTimer::GetTimeInMS() + 2000000;

        if (creatorType >= ENTITY_TYPE_VEHICLE && creatorType <= ENTITY_TYPE_OBJECT) {
            velocity = static_cast<CPhysical*>(creator)->m_vecMoveSpeed;
        }
        transform = creator ? creator->GetMatrix() : CMatrix{};
        transform.GetPosition() = origin;
        break;
    }
    case WEAPON_FLARE: {
        CStreaming::RequestModel(ModelIndices::MI_FLARE, 0); // 0x8CD75C
        destroyTime = CTimer::GetTimeInMS() + 10000;

        if (creatorType >= ENTITY_TYPE_VEHICLE && creatorType <= ENTITY_TYPE_OBJECT) {
            velocity = static_cast<CPhysical*>(creator)->m_vecMoveSpeed;
        }
        transform = creator ? creator->GetMatrix() : CMatrix{};
        transform.GetPosition() = origin;
        break;
    }
    default:
        break;
    }

    // Find a free slot (0x737F8E: m_bActive scan, 0xC89637 = end of the array's m_bActive fields)
    uint32 slot = 0;
    while (slot < MAX_PROJECTILE_INFOS && gaProjectileInfo[slot].m_bActive) {
        slot++;
    }
    if (slot == MAX_PROJECTILE_INFOS) { // LAB_00738574 - all slots busy
        return false;
    }

    // (LAB_0073839B) Create the projectile object
    CProjectile* obj{};
    switch (weaponType) {
    case WEAPON_GRENADE:
    case WEAPON_TEARGAS:
    case WEAPON_MOLOTOV:
    case WEAPON_REMOTE_SATCHEL_CHARGE:
    case WEAPON_ROCKET:
    case WEAPON_ROCKET_HS: {
        // Original: CObject::operator new(0x17C) (0x5A1EE0) + CProjectile ctor (0x5A4030)
        obj = new CProjectile{ CWeaponInfo::GetWeaponInfo(weaponType, eWeaponSkill::STD)->m_nModelId1 }; // +0xC
        break;
    }
    case WEAPON_FREEFALL_BOMB: {
        obj = new CProjectile{ CWeaponInfo::GetWeaponInfo(weaponType, eWeaponSkill::STD)->m_nModelId1 };
        break;
    }
    case WEAPON_FLARE: {
        // Original: CObject::operator new(0x17C) + CProjectile ctor with ModelIndices::MI_FLARE (0x8CD75C)
        obj = new CProjectile{ ModelIndices::MI_FLARE };
        if (obj) {
            obj->m_fAirResistance = 0.9f; // 0x3F666666
        }
        break;
    }
    default:
        break;
    }
    ms_apProjectile[slot] = obj;
    if (!obj) { // No free pool slot
        return false;
    }

    auto& info = gaProjectileInfo[slot];

    // (0x738405) Grenade-family only: replace the weapon model's col model with a single collision sphere.
    // The original dereferences the col model without a null-check here.
    if (weaponType == WEAPON_GRENADE || weaponType == WEAPON_TEARGAS || weaponType == WEAPON_MOLOTOV || weaponType == WEAPON_REMOTE_SATCHEL_CHARGE) {
        auto& colModel = *CModelInfo::ms_modelInfoPtrs[obj->GetModelIndex()]->GetColModel(); // modelInfo->m_pColModel (+0x14)
        auto* colData  = colModel.m_pColData;                                               // +0x2C
        if (!colData) {
            colModel.AllocateData(1, 0, 0, 0, 0, false);
            colData = colModel.m_pColData; // Re-read after AllocateData (original: mov eax,[esi+2C] after the call)
        } else if (colData->m_nNumSpheres == 0 && !colData->m_pSpheres) {
            colData->m_nNumSpheres = 1;
            colData->m_pSpheres = static_cast<CColSphere*>(CMemoryMgr::Malloc(sizeof(CColSphere), 0)); // 0x14 bytes
        } else {
            colData = nullptr; // Already has collision volumes - left untouched (original: jmp 0x738562)
        }
        if (colData) {
            colData->m_pSpheres[0].Set( // 0x40FD10, ECX = &m_pSpheres[0]
                colModel.GetBoundRadius() * StaticRef<float>(0x858F34), // * 0.75f
                colModel.GetBoundCenter(),
                SURFACE_GIRDER, // Original passes 0x38
                0,
                tColLighting{ 0xFF }
            );
        }
    }

    info.m_nWeaponType = weaponType;
    info.m_pCreator    = creator;
    if (creator) {
        creator->RegisterReference(&info.m_pCreator); // Original calls this unconditionally (0x7385AD)
    }

    obj->SetMatrix(transform);
    obj->m_vecMoveSpeed = velocity;

    // Grenade-like projectiles are affected by gravity (rockets are not).
    // Original: (flags ^ ((gravity << 1) ^ flags)) & 2 - conditional set of bit 0x2
    obj->physicalFlags.bApplyGravity = bApplyGravity;

    info.m_nDestroyTime = destroyTime;
    obj->m_fElasticity  = elasticity; // +0x9C

    if (bSetFxBrain) {
        // Original: object+0x160 (m_pObjectInfo) = 0xBB4BD0 = &CObjectData::ms_aObjectInfo[4]
        // (the special projectile entry from OBJECT.DAT; ms_aObjectInfo @ 0xBB4A90, sizeof == 0x50)
        obj->m_pObjectInfo = &CObjectData::GetAtIndex(4);
    }

    info.m_pVictim = victim;
    if (victim) {
        victim->RegisterReference(&info.m_pVictim);
    }

    info.m_bActive = true;

    CWorld::Add(obj);
    obj->RegisterReference(reinterpret_cast<CEntity**>(&ms_apProjectile[slot]));

    info.m_vecLastPosn = obj->GetPosition();

    if (weaponType == WEAPON_TEARGAS) { // Original: (!(0x13) && !(0x14)) && (0x11)
        if (auto* matrix = obj->GetModellingMatrix()) {
            info.m_pFxSystem = g_fxMan.CreateFxSystem("teargasAD", CVector{}, matrix, false);
            if (info.m_pFxSystem) {
                info.m_pFxSystem->Play();
            }
        }
    }

    obj->m_pEntityIgnoredCollision = creator; // +0x128
    obj->physicalFlags.bCanBeCollidedWith = true; // Original: physicalFlags (+0x40) |= 0x10000000

    // Creator ignores itself (vehicle/ped/object only, and only if not set yet)
    if (creator
        && (creatorType == ENTITY_TYPE_VEHICLE || creatorType == ENTITY_TYPE_PED || creatorType == ENTITY_TYPE_OBJECT)
        && !static_cast<CPhysical*>(creator)->m_pEntityIgnoredCollision
    ) {
        static_cast<CPhysical*>(creator)->m_pEntityIgnoredCollision = creator;
    }

    if (weaponType == WEAPON_ROCKET_HS) {
        // Heat seeking missiles get a radar blip
        const auto  poolRef = GetObjectPool()->GetRef(obj); // (obj - pool->objects) / 0x19C + idx<<8
        const auto  blip    = CRadar::SetEntityBlip(BLIP_OBJECT, poolRef, 0xFF0000FF, BLIP_DISPLAY_BLIPONLY); // 0x5839A0; original passes an unused "CODEPRJ" 5th arg
        CRadar::ChangeBlipScale(blip, 1); // 0x583CC0
        const bool playerOwned = creator == FindPlayerPed(-1) || creator == FindPlayerVehicle(-1, false);
        CRadar::ChangeBlipColour(blip, playerOwned ? (eBlipColour)0xFFFFFFFF : (eBlipColour)0xFF0000FF);
    }

    AudioEngine.ReportWeaponEvent(AE_PROJECTILE_FIRE, weaponType, obj); // 0x506F40 (0x94)

    return true;
}

// 0x738860
void CProjectileInfo::RemoveDetonatorProjectiles() {
    plugin::Call<0x738860>();
}

// 0x7388F0
void CProjectileInfo::RemoveProjectile(CProjectileInfo* info, CProjectile* object) {
    plugin::Call<0x7388F0, CProjectileInfo*, CProjectile*>(info, object);
}

// 0x738B20
void CProjectileInfo::Update() {
    return plugin::Call<0x738B20>();
}

// 0x739860
bool CProjectileInfo::IsProjectileInRange(float x1, float x2, float y1, float y2, float z1, float z2, bool bDestroy) {
    const CBox bb{
        CVector{ x1, y1, z1 },
        CVector{ x2, y2, z2 }
    };
    bool found = false;
    for (auto&& [info, proj] : rngv::zip(gaProjectileInfo, ms_apProjectile)) {
        if (!info.m_bActive) {
            continue;
        }

        if (!IsWeaponTypeProjectile(static_cast<eWeaponType>(info.m_nWeaponType))) {
            continue;
        }

        if (!bb.IsPointInside(proj->GetPosition())) {
            continue;
        }

        found = true;
        if (bDestroy) {
            info.m_bActive = false;
            info.RemoveFXSystem(false);
            CRadar::ClearBlipForEntity(BLIP_OBJECT, GetObjectPool()->GetRef(proj));
            CWorld::Remove(proj);
            delete proj;
        }
    }
    return found;
}

// 0x7399B0
void CProjectileInfo::RemoveAllProjectiles() {
    plugin::Call<0x7399B0>();
}

// 0x739A40
bool CProjectileInfo::RemoveIfThisIsAProjectile(CObject* object) {
    return plugin::CallAndReturn<bool, 0x739A40, CObject*>(object);
}

// 0x737B80
void CProjectileInfo::RemoveFXSystem(bool bInstantly) {
    plugin::CallMethod<0x737B80, CProjectileInfo*, bool>(this, bInstantly);
}
