#pragma once

#include "DebugModule.h"

class PostEffectsDebugModule final : public DebugModule {
public:
    void RenderMenuEntry() override final;
};
