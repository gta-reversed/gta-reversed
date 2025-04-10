#pragma once

#include "eEventType.h"
#include <Vector.h>
#include "Base.h"

class CPed;
class CEntity;
class CPedGroup;

class NOTSA_EXPORT_VTABLE CEvent {
public:
    int32 m_nTimeActive;
    bool  m_bValid;

public:
    static void* operator new(unsigned size);
    static void operator delete(void* object);

    CEvent();

    virtual ~CEvent();
    virtual eEventType GetEventType() const = 0;
    virtual int32 GetEventPriority() const = 0;
    virtual int32 GetLifeTime() = 0;
    virtual CEvent* Clone() const noexcept = 0;
    virtual bool AffectsPed(CPed* ped) { return true; };
    virtual bool AffectsPedGroup(CPedGroup* pedGroup) { return true; };
    virtual bool IsCriminalEvent() { return false; }
    virtual void ReportCriminalEvent(CPed* ped) { }; // empty
    virtual bool HasEditableResponse() const { return false; }
    virtual CEntity* GetSourceEntity() const { return nullptr; }
    virtual bool TakesPriorityOver(const CEvent& refEvent) { return GetEventPriority() >= refEvent.GetEventPriority(); }
    virtual float GetLocalSoundLevel() { return 0.0f; }
    virtual bool DoInformVehicleOccupants(CPed* ped) { return false; }
    virtual bool IsValid(CPed* ped) { return m_bValid || m_nTimeActive <= GetLifeTime(); }
    virtual bool CanBeInterruptedBySameEvent() { return false; };

    float GetSoundLevel(const CEntity* entity, CVector& position);
    static float CalcSoundLevelIncrement(float level1, float level2);

    void UnTick() { m_nTimeActive--; }
    void Tick() { m_nTimeActive++; }

public: // Casting.hpp support //
    template<typename From, typename Self>
    static constexpr bool classof(const From* f) { return f->GetEventType() == Self::Type; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEvent* Constructor();
};
VALIDATE_SIZE(CEvent, 0xC);
