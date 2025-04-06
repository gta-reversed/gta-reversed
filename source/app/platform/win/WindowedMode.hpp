#pragma once

namespace notsa {
void InjectWindowedModeHooks();

namespace WindowedMode {
void AdjustVideoModeOnResize(int32 w, int32 h);
};
};
