#include "StdInc.h"
#include "platform.h"

#include <cstdio>
#include <cctype>
#include <cstring>

struct PresetView {
    CVector     m_Translation;
    float       m_RotX;
    float       m_RotY;
    float       m_NearClip;
    float       m_FarClip;
    char*       m_Description;
    PresetView* m_Next;
};

// 3x3 identity matrix
constexpr CVector PresetViewAxisX = { 1.0f, 0.0f, 0.0f }; // 0x8D2E00
constexpr CVector PresetViewAxisY = { 0.0f, 1.0f, 0.0f }; // 0x8D2E0C
constexpr CVector PresetViewAxisZ = { 0.0f, 0.0f, 1.0f }; // 0x8D2E18

constexpr auto ViewsFileName      = "./views.txt"; // 0x8D2E24;

PresetView*& PresetViews          = *(PresetView**)0xC1707C;
int32&       NumPresetViews       = *(int32*)0xC17080;
int32        CurrentPresetView    = *(int32*)0x8D2E30; // -1

// 0x619780
bool RsSetPresetView(RwCamera* camera, int32 viewNum) {
    if (!camera || !NumPresetViews || viewNum >= NumPresetViews || viewNum < 0) {
        return false;
    }

    auto* pv = PresetViews;
    int   v3 = NumPresetViews - viewNum - 1u;
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
    if (!camera) {
        return;
    }

    if (!NumPresetViews) {
        return;
    }

    auto viewNum = ++CurrentPresetView;
    if (CurrentPresetView >= NumPresetViews) {
        viewNum           = 0;
        CurrentPresetView = 0;
    }

    RsSetPresetView(camera, viewNum);
}

// 0x619880
void RsSetPreviousPresetView(RwCamera* camera) {
    if (!camera || !NumPresetViews) {
        return;
    }

    if (--CurrentPresetView < 0) {
        CurrentPresetView = NumPresetViews - 1;
    }

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
    PresetViews    = nullptr;
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
    RsDestroyPresetViews(); // Release old views
    NumPresetViews = 0;     // Reset counter
    FILE* file     = fopen(ViewsFileName, "rt");
    if (!file) {
        return;
    }
    PresetView* head = nullptr;
    PresetView* tail = nullptr;
    char buffer[1'024];
    while (fgets(buffer, sizeof(buffer), file)) {
        char filtered[1'024];
        int  idx = 0;
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (isprint(static_cast<unsigned char>(buffer[i]))) {
                filtered[idx++] = buffer[i];
            }
        }
        filtered[idx] = '\0';
        float x = 0.0f, y = 0.0f, z = 0.0f;
        float rotX = 0.0f, rotY = 0.0f;
        float nearClip = 0.0f, farClip = 0.0f;
        char descBuffer[512] = { 0 };
        int  scanned         = sscanf(filtered, "%f%f%f%f%f%f%f %511[^\n]", &x, &y, &z, &rotX, &rotY, &nearClip, &farClip, descBuffer);

        if (scanned < 7) {
            continue;
        }
        char* desc = new char[strlen(descBuffer) + 1];
        strcpy(desc, descBuffer);
        PresetView* newView    = new PresetView;
        newView->m_Translation = CVector(x, y, z);
        newView->m_RotX        = rotX;
        newView->m_RotY        = rotY;
        newView->m_NearClip    = nearClip;
        newView->m_FarClip     = farClip;
        newView->m_Description = desc;
        newView->m_Next        = nullptr;
        if (!head) {
            head = newView;
            tail = newView;
        } else {
            tail->m_Next = newView;
            tail         = newView;
        }
        NumPresetViews++;
    }
    fclose(file);
    PresetViews       = head;
    CurrentPresetView = -1;
}

// 0x619FA0
void RsSavePresetView() {
    if (!PresetViews || NumPresetViews == 0) {
        return; // There are no views to save
    }
    FILE* file = fopen(ViewsFileName, "wt"); // Open for writing (text)
    if (!file) {
        // Optional: show error or log
        return;
    }
    PresetView* current = PresetViews;
    for (int i = 0; i < NumPresetViews && current != nullptr; i++, current = current->m_Next) {
        // Format the line the same as in the original file, with precision and spacing
        fprintf(file, "%.6f %.6f %.6f %.6f %.6f %.6f %.6f %s\n", current->m_Translation.x, current->m_Translation.y, current->m_Translation.z, current->m_RotX, current->m_RotY, current->m_NearClip, current->m_FarClip, current->m_Description ? current->m_Description : "");
    }
    fclose(file);
}
