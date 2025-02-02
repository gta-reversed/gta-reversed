#include "StdInc.h"

#include "AudioZonesDebugModule.h"
#include "Lines.h"

void AudioZonesDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({"Visualization", "Audio zones"}, [&] { ImGui::Checkbox("Show all", &m_ShowAudioZones); });
}

void AudioZonesDebugModule::Render3D() {
    if (!m_ShowAudioZones) {
        return;
    }

    for (auto& box : CAudioZones::GetActiveAuZoBoxes()) {
        box.DrawWireFrame(CRGBA(255, 0, 0, 255), m_Transform);
    }
}
