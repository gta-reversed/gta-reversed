#pragma once

class CPed;
class CEventGroup;

class CCollisionEventScanner {
public:
    bool m_bAlreadyHitByCar;

    void ScanForCollisionEvents(CPed* victim, CEventGroup* eventGroup);
};
VALIDATE_SIZE(CCollisionEventScanner, 0x1);
