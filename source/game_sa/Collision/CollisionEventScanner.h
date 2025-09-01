#pragma once

class CPed;
class CEventGroup;

class CCollisionEventScanner {
public:
    static void InjectHooks();

    void ScanForCollisionEvents(CPed* victim, CEventGroup* eventGroup);

public:
    bool m_bAlreadyHitByCar;
};
VALIDATE_SIZE(CCollisionEventScanner, 0x1);
