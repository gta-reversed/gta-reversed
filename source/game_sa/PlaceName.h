#pragma once

class CZone;

class CPlaceName {
public:
    CZone* m_pZone;
    uint16 m_nAdditionalTimer;

public:
    static void InjectHooks();

    CPlaceName();

    void Init();
    const char* GetForMap(float x, float y);
    void Process();
    inline void Display() const;
    void ProcessAfterFrontEndShutDown();
    inline static CVector CalcPosition(); // NOTSA, inlined
};

VALIDATE_SIZE(CPlaceName, 0x8);
