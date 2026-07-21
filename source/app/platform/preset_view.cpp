#include "StdInc.h"

#include "platform.h"

struct PresetView {
    CVector m_Translation;
    float m_RotX;
    float m_RotY;
    float m_NearClip;
    float m_FarClip;
    char* m_Description;
    PresetView* m_Next;
};

// 3x3 identity matrix
constexpr CVector PresetViewAxisX = {1.0f, 0.0f, 0.0f}; // 0x8D2E00
constexpr CVector PresetViewAxisY = {0.0f, 1.0f, 0.0f}; // 0x8D2E0C
constexpr CVector PresetViewAxisZ = {0.0f, 0.0f, 1.0f}; // 0x8D2E18

constexpr auto ViewsFileName = "./views.txt"; // 0x8D2E24;

auto& PresetViews = StaticRef<PresetView*>(0xC1707C);
auto& NumPresetViews = StaticRef<int32>(0xC17080);
auto& CurrentPresetView = StaticRef<int32>(0x8D2E30); // -1

// 0x619780
bool RsSetPresetView(RwCamera* camera, int32 viewNum) {
    if (!camera || !NumPresetViews || viewNum >= NumPresetViews || viewNum < 0)
        return false;

    auto* pv = PresetViews;
    int v3 = NumPresetViews - viewNum - 1u;
    for (CurrentPresetView = viewNum; v3 > 0; --v3) {
        if (!pv) {
            NOTSA_UNREACHABLE();
            break;
        }

        pv = pv->m_Next;
    }

    RwFrame* parent = RwCameraGetFrame(camera);
    RwFrameSetIdentity(parent);
    RwFrameRotate(parent, &PresetViewAxisX, -pv->m_RotX, rwCOMBINEREPLACE);
    RwFrameRotate(parent, &PresetViewAxisY, pv->m_RotY, rwCOMBINEPOSTCONCAT);
    RwFrameTranslate(parent, &pv->m_Translation, rwCOMBINEPOSTCONCAT);
    RwFrameUpdateObjects(parent);
    RwCameraSetNearClipPlane(camera, pv->m_NearClip);
    RwCameraSetFarClipPlane(camera, pv->m_FarClip);

    return true;
}

// 0x619840
void RsSetNextPresetView(RwCamera* camera) {
    if (!camera)
        return;

    if (!NumPresetViews)
        return;

    auto viewNum = ++CurrentPresetView;
    if (CurrentPresetView >= NumPresetViews) {
        viewNum = 0;
        CurrentPresetView = 0;
    }

    RsSetPresetView(camera, viewNum);
}

// 0x619880
void RsSetPreviousPresetView(RwCamera* camera) {
    if (!camera || !NumPresetViews)
        return;

    if (--CurrentPresetView < 0)
        CurrentPresetView = NumPresetViews - 1;

    RsSetPresetView(camera, CurrentPresetView);
}

// 0x6198C0
void RsDestroyPresetViews() {
    auto pv = PresetViews;
    if (PresetViews) {
        PresetView* m_Next;
        do {
            m_Next = pv->m_Next;
            if (pv->m_Description) {
                CMemoryMgr::Free(pv->m_Description);
            }
            CMemoryMgr::Free(pv);
            pv = m_Next;
        } while (m_Next);
    }
    PresetViews = nullptr;
    NumPresetViews = 0;
}

// 0x619AB0
void* RsGetPresetViewDescription() {
    if (!NumPresetViews || CurrentPresetView == -1) {
        return nullptr;
    }

    auto* pv = PresetViews;
    for (auto i = NumPresetViews - CurrentPresetView - 1; i > 0; --i) {
        if (!pv) {
            break;
        }
        pv = pv->m_Next;
    }
    if (!pv) {
        NOTSA_UNREACHABLE();
    }

    return pv->m_Description;
}

// 0x619D60
void RsLoadPresetViews() {
    plugin::Call<0x619D60>();
}

// 0x619FA0
void RsSavePresetView() {
    plugin::Call<0x619FA0>();
}
