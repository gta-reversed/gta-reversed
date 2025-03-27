/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskSimpleIKChain.h"
#include "Enums/eIKChainSlot.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleIKManager final : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_IK_MANAGER;

    CTaskSimpleIKManager() = default;
    CTaskSimpleIKManager(const CTaskSimpleIKManager&);
    ~CTaskSimpleIKManager() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskSimpleIKManager{*this}; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool      ProcessPed(CPed* ped) override;

    /*!
    * @addr 0x633940
    * @brief Store an IK task in the given IK slot.
    * @param slot If slot == UNKNOWN the first unused slot will be used, otherwise the specified slot
    */
    void AddIKChainTask(CTaskSimpleIKChain* task, eIKChainSlot slot);

    /*!
     * @addr 0x6339B0
    */
    CTaskSimpleIKChain* GetTaskAtSlot(eIKChainSlot slot) { return m_IKChainTasks[(size_t)slot]; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimpleIKManager* Constructor() { this->CTaskSimpleIKManager::CTaskSimpleIKManager(); return this; }
    CTaskSimpleIKManager* Destructor() { this->CTaskSimpleIKManager::~CTaskSimpleIKManager(); return this; }

private:
    std::array<CTaskSimpleIKChain*, (size_t)eIKChainSlot::COUNT> m_IKChainTasks{}; //! See `eIKChainSlot`
    bool                                                         m_IsAborting{};
};
VALIDATE_SIZE(CTaskSimpleIKManager, 0x1C);
