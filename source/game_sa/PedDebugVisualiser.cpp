#include "StdInc.h"

#include "PedDebugVisualiser.h"

void CPedDebugVisualiser::SetRenderStates() {
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,       RWRSTATE(rwTEXTUREADDRESSWRAP));
    RwRenderStateSet(rwRENDERSTATETEXTUREPERSPECTIVE,   RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,            RWRSTATE(rwSHADEMODEFLAT));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,        RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,             RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,            RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEBORDERCOLOR,          RWRSTATE(0xFF000000));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,            RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATECULLMODE,             RWRSTATE(rwCULLMODECULLNONE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION,    RWRSTATE(5));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(2));

    CFont::SetFontStyle(eFontStyle::FONT_SUBTITLES);
    CFont::SetBackground(false, false);
    CFont::SetWrapx(640.0f);
    CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
    CFont::SetProportional(true);
    CFont::SetColor({ 255, 255, 0, 200});
    CFont::SetEdge(1);
    CFont::SetDropColor({ 0, 0, 0, 255 });
}

//void CPedDebugVisualiser::VisualiseAttractorsInPtrList(CPtrList& list, const CPed& ped) {
//
//}

void CPedDebugVisualiser::VisualiseBoundingVolumes(CPed& ped) {

}

void CPedDebugVisualiser::VisualiseEvents(const CPed& ped) {

}

void CPedDebugVisualiser::VisualiseFOV(const CPed& ped) {

}

void CPedDebugVisualiser::VisualiseHitSidesToPlayer(CPed& ped) {

}

void CPedDebugVisualiser::VisualiseNearbyAttractors(const CPed& ped) {

}

void CPedDebugVisualiser::VisualiseOneAttractor(const CVector& p1, const CVector& p2, const CVector& p3, const CVector& p4) {

}

void CPedDebugVisualiser::VisualisePedsNearPlayer() {

}

void CPedDebugVisualiser::VisualisePosition(const CPed& ped) {

}

void CPedDebugVisualiser::VisualiseTasks(const CPed& ped) {

}

void CPedDebugVisualiser::VisualiseText(const CPed& ped) {

}

void CPedDebugVisualiser::VisualiseTextViaMenu(const CPed& ped) {

}
