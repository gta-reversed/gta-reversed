#pragma once

#include <General.h>
#include "TaskComplex.h"
#include "TaskSimpleFacial.h"

class CPed;

/*!
* @brief An infinitely running task for controlling facial expression animations
* @brief Facial expressions can be requested using `SetRequest()`
*/
class NOTSA_EXPORT_VTABLE CTaskComplexFacial : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_FACIAL;

    CTaskComplexFacial() = default;
    ~CTaskComplexFacial() override = default; // 0x690D90

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskComplexFacial(); }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

    /*!
     * @addr 0x691250
     * @brief Stop all facial animations
    */
    void StopAll();

    /*!
     * @addr 0x691230
     * @brief Set requests for facial expression animations
     * @param       typeA First type to play
     * @param       durA First type's duration [ms]
     * @param [opt] typeB Type to play after the first one has finished
     * @param [opt] durB Second type's duration [ms]
    */
    void SetRequest(eFacialExpression typeA, int32 durA, eFacialExpression typeB = eFacialExpression::NONE, int32 durB = -1);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexFacial* Constructor() { this->CTaskComplexFacial::CTaskComplexFacial(); return this; }

private:
    bool              m_IsChewing{CGeneral::RandomBool(30.f)};
    bool              m_TalkingLastFrame{};
    bool              m_IsAborting{};

    eFacialExpression m_RequestA{eFacialExpression::NONE};
    int32             m_DurationA{};

    eFacialExpression m_RequestB{eFacialExpression::NONE};
    int32             m_DurationB{};
};
VALIDATE_SIZE(CTaskComplexFacial, 0x20);
