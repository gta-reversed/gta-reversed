#pragma once

#include "Ped.h"

class NOTSA_EXPORT_VTABLE CCivilianPed : public CPed {
public:
    CCivilianPed(ePedType pedType, uint32 modelIndex);
    ~CCivilianPed() override = default; // 0x5DDBE0

    void ProcessControl() override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CCivilianPed* Constructor(ePedType pedType, uint32 modelIndex);
    CCivilianPed* Destructor();
};

VALIDATE_SIZE(CCivilianPed, 0x79C);
