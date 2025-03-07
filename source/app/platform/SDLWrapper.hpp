#pragma once

class CMouseControllerState;
class CKeyboardState;

namespace notsa {
namespace SDLWrapper {
bool Initialize();
void Terminate();
void ProcessEvents();

const CMouseControllerState& GetMouseState();
const CKeyboardState& GetKeyboardState();
};
};
