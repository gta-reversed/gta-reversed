#include "StdInc.h"

#include "MotionBlurStreaks.h"

void CMotionBlurStreaks::InjectHooks()
{
    RH_ScopedClass(CMotionBlurStreaks);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x721D90);
    RH_ScopedInstall(Update, 0x7240C0);
    RH_ScopedInstall(Render, 0x7240E0);
    RH_ScopedInstall(RegisterStreak, 0x721DC0);
}

// 0x721D90
void CMotionBlurStreaks::Init()
{
    for (CRegisteredMotionBlurStreak& streak : CMotionBlurStreaks::aStreaks) {
        streak.m_nId = 0;
    }
}

// 0x7240C0
void CMotionBlurStreaks::Update()
{
    for (CRegisteredMotionBlurStreak& streak : CMotionBlurStreaks::aStreaks) {
        if (streak.m_nId)
            streak.Update();
    }
}

// 0x7240E0
void CMotionBlurStreaks::Render() {
    ZoneScoped;

    bool bRenderParamsSet = false;
    for (CRegisteredMotionBlurStreak& streak : CMotionBlurStreaks::aStreaks) {
        if (streak.m_nId) {
            if (!bRenderParamsSet) {
                bRenderParamsSet = true;
                RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
                RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
                RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));
                RwRenderStateSet(rwRENDERSTATEFOGCOLOR,          RWRSTATE(CTimeCycle::GetCurrentSkyBottomColor().ToIntARGB()));
                RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
                RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
                RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));
            }
            streak.Render();
        }
    }

    if (bRenderParamsSet) {
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    }
}

// 0x721DC0
void CMotionBlurStreaks::RegisterStreak(uint32 id, uint8 red, uint8 green, uint8 blue, uint8 alpha, CVector leftPoint, CVector rightPoint)
{
    for (CRegisteredMotionBlurStreak& streak : CMotionBlurStreaks::aStreaks) {
        if (streak.m_nId != id)
            continue;

        streak.m_color.Set(red, green, blue, alpha);
        streak.m_avecLeftPoints[0] = leftPoint;
        streak.m_avecRightPoints[0] = rightPoint;
        streak.m_acRenderHistory[0] = true;
        return;
    }


    for (CRegisteredMotionBlurStreak& streak : CMotionBlurStreaks::aStreaks) {
        if (streak.m_nId)
            continue;

        streak.m_nId = id;
        streak.m_color.Set(red, green, blue, alpha);
        streak.m_avecLeftPoints[0] = leftPoint;
        streak.m_avecRightPoints[0] = rightPoint;
        streak.m_acRenderHistory[0] = true;
        streak.m_acRenderHistory[1] = false;
        return;
    }
}
