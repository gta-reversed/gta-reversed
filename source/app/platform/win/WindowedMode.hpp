#pragma once

namespace notsa {
void InjectWindowedModeHooks();
bool SetNoVSync(bool enabled);

namespace WindowedMode {
void AdjustVideoModeOnResize(int32 w, int32 h);
};
};
