#pragma once

class CPed;
class CVector;

// Android
class CPedDebugVisualiser {
    class CTextDisplayFlags {
    public:
        bool b1 = false;
        bool b2 = false;
        bool b3 = false;
        bool b4 = false;

        CTextDisplayFlags() = default;
        ~CTextDisplayFlags() = default;
    };

    static void SetRenderStates();
    //static void VisualiseAttractorsInPtrList(PtrListType& list, const CPed& ped);
    static void VisualiseBoundingVolumes(CPed& ped);
    static void VisualiseEvents(const CPed& ped);
    static void VisualiseFOV(const CPed& ped);
    static void VisualiseHitSidesToPlayer(CPed& ped);
    static void VisualiseNearbyAttractors(const CPed& ped);
    static void VisualiseOneAttractor(const CVector& p1, const CVector& p2, const CVector& p3, const CVector& p4);
    static void VisualisePedsNearPlayer();
    static void VisualisePosition(const CPed& ped);
    static void VisualiseTasks(const CPed& ped);
    static void VisualiseText(const CPed& ped);
    static void VisualiseTextViaMenu(const CPed& ped);
};
