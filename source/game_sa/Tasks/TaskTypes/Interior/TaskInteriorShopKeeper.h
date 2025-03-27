#pragma once

#include "TaskComplex.h"

class CPed;
struct InteriorGroup_c;
struct InteriorInfo_t;
struct Interior_c;

class NOTSA_EXPORT_VTABLE CTaskInteriorShopKeeper : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_INTERIOR_SHOPKEEPER;

    static void InjectHooks();

    CTaskInteriorShopKeeper(InteriorGroup_c* intGrp, bool bIsSetup);
    CTaskInteriorShopKeeper(const CTaskInteriorShopKeeper&);
    ~CTaskInteriorShopKeeper();

    CTask*    Clone() const override { return new CTaskInteriorShopKeeper{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;


private: // Wrappers for hooks
    // 0x675660
    CTaskInteriorShopKeeper* Constructor(InteriorGroup_c* a, uint8 b) {
        this->CTaskInteriorShopKeeper::CTaskInteriorShopKeeper(a, b);
        return this;
    }

    // 0x6756A0
    CTaskInteriorShopKeeper* Destructor() {
        this->CTaskInteriorShopKeeper::~CTaskInteriorShopKeeper();
        return this;
    }

private:
    bool             m_bStartServeAnim{};
    InteriorGroup_c* m_IntGrp{};
    bool             m_bIsSetup{};
    InteriorInfo_t*  m_IntInfo{};
    Interior_c*      m_Int{};
};
