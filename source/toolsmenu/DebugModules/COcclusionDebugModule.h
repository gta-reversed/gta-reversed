#ifdef EXTRA_DEBUG_FEATURES
#pragma once

class COcclusionDebugModule
{
public:
    static bool DrawActiveOcclusions;
    static int NumEntitiesSkipped;

public:
    static void ProcessImgui();
    static void ProcessRender();
};
#endif
