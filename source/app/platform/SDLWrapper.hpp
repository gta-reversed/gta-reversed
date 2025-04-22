#pragma once

#ifdef NOTSA_USE_SDL3
namespace notsa {
namespace SDLWrapper {
bool Initialize();
void Terminate();
void ProcessEvents();
};
};
#endif
