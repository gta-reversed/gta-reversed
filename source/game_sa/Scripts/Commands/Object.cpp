#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>

#include "Object.h"
#include "SetPieces.h"
#include "Fx.h"
#include "PointLights.h"
#include "CommandParser/Parser.hpp"

using namespace notsa;
using namespace notsa::script;
/*!
* Various object commands
*/

namespace Object {
CObject& CreateObject(CRunningScript& S, script::Model model, CVector posn) {
    const auto mi = CModelInfo::GetModelInfo(model);
    auto* object = CObject::Create(model, false);
    CWorld::PutToGroundIfTooLow(posn);
    posn.z += object->GetDistanceFromCentreOfMassToBaseOfModel();
    object->SetPosn(posn);
    object->SetOrientation(CVector{0.0f});
    object->UpdateRW();
    object->UpdateRwFrame();
    if (mi->AsLodAtomicModelInfoPtr()) {
        object->SetupBigBuilding();
    }

    CTheScripts::ClearSpaceForMissionEntity(posn, object);
    CWorld::Add(object);

    if (S.m_bUseMissionCleanup) {
        CTheScripts::MissionCleanUp.AddEntityToList(*object);
    }
    return *object;
}

void RemoveObject(CRunningScript& S, CObject& object) {
    CWorld::Remove(&object);
    CWorld::RemoveReferencesToDeletedObject(&object);
    delete &object;

    if (S.m_bUseMissionCleanup) {
        CTheScripts::MissionCleanUp.RemoveEntityFromList(object);
    }
}

bool DoesObjectExists(CObject* object) {
    return object != nullptr;
}

void MarkObjectNoLongerNeeded(CRunningScript& S, CObject* object) {
    CTheScripts::CleanUpThisObject(object);

    if (object && S.m_bUseMissionCleanup) {
        CTheScripts::MissionCleanUp.RemoveEntityFromList(*object);
    }
}

void DoNotRemoveObject(CObject& object) {
    CTheScripts::MissionCleanUp.RemoveEntityFromList(object);
}

CVector GetObjectCoordinates(CObject& object) {
    return object.GetPosition();
}

void SetObjectCoordinates(CObject& object, CVector posn) {
    object.SetPosn(posn);
}

CVector GetObjectVelocity(CObject& object) {
    return object.GetMoveSpeed() * 50.0f;
}

void SetObjectVelocity(CObject& object, CVector velocity) {
    object.SetVelocity(velocity / 50.0f);
}

float GetObjectHeading(CObject& object) {
    return FixAngleDegrees(RadiansToDegrees(object.GetHeading()));
}

void SetObjectHeading(CObject& object, float heading) {
    object.SetHeading(DegreesToRadians(FixAngleDegrees(heading)));
}

void SetObjectScale(CObject& object, float scale) {
    object.m_fScale = scale;
}

void SetObjectCollision(CObject& object, bool enable) {
    object.m_bUsesCollision = enable;
}

bool DoesObjectHaveThisModel(CObject& object, script::Model model) {
    return object.m_nModelIndex == model;
}
} // namespace Object

namespace Model {
void LoadModel(CRunningScript* S, script::Model model) {
    CStreaming::RequestModel(model, STREAMING_MISSION_REQUIRED | STREAMING_KEEP_IN_MEMORY);
    CTheScripts::ScriptResourceManager.AddToResourceManager(model, RESOURCE_TYPE_MODEL_OR_SPECIAL_CHAR, S);
}

bool HasModelLoaded(script::Model model) {
    return CStreaming::IsModelLoaded(model);
}

void MarkModelNotNeeded(CRunningScript* S, script::Model model) {
    if (CTheScripts::ScriptResourceManager.RemoveFromResourceManager(model, RESOURCE_TYPE_MODEL_OR_SPECIAL_CHAR, S)) {
        CStreaming::SetMissionDoesntRequireModel(model);
    }
}

void LoadAllModelsNow() {
    CTimer::Suspend();
    CStreaming::LoadAllRequestedModels(false);
    CTimer::Resume();
}

bool IsModelAvailable(eModelID model) {
    return CModelInfo::GetModelInfo(model) != nullptr;
}
} // namespace Model

namespace Fx {
void DrawLight(CVector pos, CVector color) {
    color /= 255.0f;
    CPointLights::AddLight(ePointLightType::PLTYPE_POINTLIGHT, pos, {}, 12.0f, color.x, color.y, color.z, 0, false, nullptr);
}

void AddBigGunFlash(CVector origin, CVector target) {
    CPointLights::AddLight(ePointLightType::PLTYPE_POINTLIGHT, origin, {}, 5.0f, 1.0f, 0.8f, 0.0f, 0, false, nullptr);
    g_fx.TriggerGunshot(nullptr, origin, (target - origin).Normalized(), true);
}
} // namespace Fx

namespace Attractor {
void EnableDisabledAttractorOnObject(CObject& object, bool enabled) {
    object.objectFlags.bEnableDisabledAttractors = enabled; // todo: rename obj->objectFlags.b0x1000000
}
} // namespace Attractor

namespace Animation {
void SetObjectAnimSpeed(CObject& obj, const char* animName, float speed) {
    if (const auto anim = RpAnimBlendClumpGetAssociation(obj.m_pRwClump, animName)) {
        anim->m_Speed = speed;
    }
}

bool IsObjectPlayingAnim(CObject& obj, const char* animName) {
    if (!obj.m_pRwClump) {
        return false;
    }
    if (RwObjectGetType(obj.m_pRwClump) != rpCLUMP) {
        return false;
    }
    if (!RpAnimBlendClumpIsInitialized(obj.m_pRwClump)) {
        return false;
    }
    return RpAnimBlendClumpGetAssociation(obj.m_pRwClump, animName) != nullptr;
}

auto GetObjectAnimCurrentTime(CObject& obj, const char* animName) {
    if (const auto anim = RpAnimBlendClumpGetAssociation(obj.m_pRwClump, animName)) {
        return anim->m_CurrentTime / anim->m_BlendHier->m_fTotalTime;
    }
    return 0.f;
}

auto SetObjectAnimCurrentTime(CObject& obj, const char* animName, float progress) {
    if (const auto anim = RpAnimBlendClumpGetAssociation(obj.m_pRwClump, animName)) {
        anim->SetCurrentTime(anim->m_Speed * progress);
    }
}
} // namespace Animation

void notsa::script::commands::object::RegisterHandlers() {
    using namespace Object;
    using namespace Model;
    using namespace Fx;
    using namespace Attractor;
    using namespace Animation;

    REGISTER_COMMAND_HANDLER(COMMAND_CREATE_OBJECT, CreateObject);
    REGISTER_COMMAND_HANDLER(COMMAND_DELETE_OBJECT, RemoveObject);
    REGISTER_COMMAND_HANDLER(COMMAND_DOES_OBJECT_EXIST, DoesObjectExists);
    REGISTER_COMMAND_HANDLER(COMMAND_MARK_OBJECT_AS_NO_LONGER_NEEDED, MarkObjectNoLongerNeeded);
    REGISTER_COMMAND_HANDLER(COMMAND_DONT_REMOVE_OBJECT, DoNotRemoveObject);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_OBJECT_COORDINATES, GetObjectCoordinates);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_OBJECT_COORDINATES, SetObjectCoordinates);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_OBJECT_VELOCITY, GetObjectVelocity);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_OBJECT_VELOCITY, SetObjectVelocity);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_OBJECT_HEADING, GetObjectHeading);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_OBJECT_HEADING, SetObjectHeading);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_OBJECT_SCALE, SetObjectScale);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_OBJECT_COLLISION, SetObjectCollision);
    REGISTER_COMMAND_HANDLER(COMMAND_DOES_OBJECT_HAVE_THIS_MODEL, DoesObjectHaveThisModel);

    REGISTER_COMMAND_HANDLER(COMMAND_REQUEST_MODEL, LoadModel);
    REGISTER_COMMAND_HANDLER(COMMAND_HAS_MODEL_LOADED, HasModelLoaded);
    REGISTER_COMMAND_HANDLER(COMMAND_MARK_MODEL_AS_NO_LONGER_NEEDED, MarkModelNotNeeded);
    REGISTER_COMMAND_HANDLER(COMMAND_LOAD_ALL_MODELS_NOW, LoadAllModelsNow);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_MODEL_AVAILABLE, IsModelAvailable);

    REGISTER_COMMAND_HANDLER(COMMAND_ADD_SET_PIECE, CSetPieces::AddOne);
    REGISTER_COMMAND_HANDLER(COMMAND_DRAW_LIGHT, DrawLight);
    REGISTER_COMMAND_HANDLER(COMMAND_ADD_BIG_GUN_FLASH, AddBigGunFlash);

    REGISTER_COMMAND_HANDLER(COMMAND_ENABLE_DISABLED_ATTRACTORS_ON_OBJECT, EnableDisabledAttractorOnObject);

    REGISTER_COMMAND_HANDLER(COMMAND_SET_OBJECT_ANIM_SPEED, SetObjectAnimSpeed);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_OBJECT_PLAYING_ANIM, IsObjectPlayingAnim);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_OBJECT_ANIM_CURRENT_TIME, GetObjectAnimCurrentTime);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_OBJECT_ANIM_CURRENT_TIME, SetObjectAnimCurrentTime);

    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_FLASH_OBJECT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_NO_OBJ);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_WAIT_ON_FOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_FLEE_ON_FOOT_TILL_SAFE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_GUARD_SPOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_GUARD_AREA);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_WAIT_IN_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_KILL_CHAR_ON_FOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_KILL_PLAYER_ON_FOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_KILL_CHAR_ANY_MEANS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_KILL_PLAYER_ANY_MEANS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_FLEE_CHAR_ON_FOOT_TILL_SAFE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_FLEE_PLAYER_ON_FOOT_TILL_SAFE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_FLEE_CHAR_ON_FOOT_ALWAYS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_FLEE_PLAYER_ON_FOOT_ALWAYS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_GOTO_CHAR_ON_FOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_GOTO_PLAYER_ON_FOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_LEAVE_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_ENTER_CAR_AS_PASSENGER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_ENTER_CAR_AS_DRIVER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_FOLLOW_CAR_IN_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_FIRE_AT_OBJECT_FROM_VEHICLE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_DESTROY_OBJECT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_DESTROY_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_GOTO_AREA_ON_FOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_GOTO_AREA_IN_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_FOLLOW_CAR_ON_FOOT_WITH_OFFSET);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_GUARD_ATTACK);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_FOLLOW_ROUTE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_GOTO_COORD_ON_FOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_GOTO_COORD_IN_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_RUN_TO_AREA);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_RUN_TO_COORD);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ADD_PEDS_IN_AREA_TO_COLL);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ADD_PEDS_IN_VEHICLE_TO_COLL);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CLEAR_COLL);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_COLL_IN_CARS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_COLL_ANY_MEANS_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_COLL_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_COLL_IN_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_STOPPED_COLL_ANY_MEANS_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_STOPPED_COLL_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_STOPPED_COLL_IN_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_COLL_ANY_MEANS_CHAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_COLL_ON_FOOT_CHAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_COLL_IN_CAR_CHAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_COLL_ANY_MEANS_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_COLL_ON_FOOT_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_COLL_IN_CAR_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_COLL_ANY_MEANS_PLAYER_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_COLL_ON_FOOT_PLAYER_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_COLL_IN_CAR_PLAYER_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_COLL_IN_AREA_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_COLL_IN_AREA_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_COLL_IN_AREA_IN_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_COLL_STOPPED_IN_AREA_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_COLL_STOPPED_IN_AREA_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_COLL_STOPPED_IN_AREA_IN_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_NUMBER_OF_PEDS_IN_COLL);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_GOTO_AREA_ANY_MEANS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ADD_PARTICLE_EFFECT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ADD_SPRITE_BLIP_FOR_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ADD_SPRITE_BLIP_FOR_CHAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ADD_SPRITE_BLIP_FOR_OBJECT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_DROP_MINE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_DROP_NAUTICAL_MINE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOAD_SPECIAL_MODEL);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CREATE_CUTSCENE_HEAD);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CUTSCENE_HEAD_ANIM);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_CATCH_TRAIN);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_COLL_OBJ_STEAL_ANY_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ADD_MOVING_PARTICLE_EFFECT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_REMOVE_PARTICLE_EFFECTS_IN_AREA);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_COLLISION_IN_MEMORY);
}
