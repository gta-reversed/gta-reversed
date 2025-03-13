#pragma once

namespace notsa {
void InjectWindowedModeHooks();

namespace WindowedMode {
void AdjustPresentParams(D3DPRESENT_PARAMETERS* pp);
void AdjustGameToWindowSize();
};
};
