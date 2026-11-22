#pragma once
#include "DebugModule.h"
class CPlantLocTri;
class CPlantColEntEntry;

class ProcObjectDebugModule final : public DebugModule {
public:
    void RenderWindow() override final;
    void RenderMenuEntry() override final;
    uint32 IteratePlantLocTriList(CPlantLocTri* plantTri);
    uint32 IteratePlantColLocList(CPlantColEntEntry* colLoc);

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(ProcObjectDebugModule, m_IsOpen);

private:
    bool m_IsOpen{};
};
