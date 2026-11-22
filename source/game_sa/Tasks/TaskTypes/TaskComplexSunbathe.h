/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"

class CAnimBlock;
class CObject;

enum eSunbatherType : uint32
{
    SUNBATHER_MALE_1 = 0,
    SUNBATHER_MALE_2,
    SUNBATHER_FEMALE_1,
    SUNBATHER_FEMALE_2,
    SUNBATHER_FEMALE_3,
};

class NOTSA_EXPORT_VTABLE CTaskComplexSunbathe : public CTaskComplex {
public:
    bool           m_bStartStanding{};
    bool           m_bBathing{};
    bool           m_bBeachAnimsReferenced{};
    bool           m_bSunbatheAnimsReferenced{};
    bool           m_bAborted{};
    CTaskTimer     m_BathingTimer{};
    eSunbatherType m_SunbatherType{};
    CAnimBlock*    m_pBeachAnimBlock{};
    CAnimBlock*    m_pSunbatheAnimBlock{};
    int32          m_BeachAnimBlockIndex{};
    int32          m_SunbatheAnimBlockIndex{};
    CObject*       m_pTowel{};

public:
    static void InjectHooks();

    static constexpr auto Type = TASK_COMPLEX_SUNBATHE;

    CTaskComplexSunbathe(CObject* towel, bool bStartStanding);
    CTaskComplexSunbathe(const CTaskComplexSunbathe&);
    ~CTaskComplexSunbathe();
    
    CTask*    Clone() const override { return new CTaskComplexSunbathe{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private:
    // 0x631F80
    CTaskComplexSunbathe* Constructor(CObject* towel, bool startStanding) {
        this->CTaskComplexSunbathe::CTaskComplexSunbathe(towel, startStanding);
        return this;
    }

    // 0x632050
    CTaskComplexSunbathe* Destructor() {
        this->CTaskComplexSunbathe::~CTaskComplexSunbathe();
        return this;
    }
};
VALIDATE_SIZE(CTaskComplexSunbathe, 0x38);
