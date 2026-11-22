#pragma once

struct IMediaControl;

#define WM_GRAPHNOTIFY 1037

namespace VideoPlayer {

void InjectHooks();

void Shutdown();
void WaitState(int32 state);
void OnGraphNotify();
void UpdateWindow();
void Release();
void Play(int32 nCmdShow, const char* path);
auto GetMediaControl() -> IMediaControl*;

} // namespace VideoPlayer
