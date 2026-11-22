#pragma once

bool IsWideScreenRatio(float ratio);
bool IsFullScreenRatio(float ratio);
#define IS_WIDESCREEN_RATIO(ratio) IsWideScreenRatio(ratio)
#define IS_FULLSCREEN_RATIO(ratio) IsFullScreenRatio(ratio)

void RsInjectHooks();

uint8 RsKeyFromScanCode(uint8 scan, int32 shiftKeyDown);

uint32 RsTimer();
void RsWindowSetText(const RwChar* str);
char RsPathGetSeparator();
bool RsAlwaysOnTop(bool alwaysOnTop);
bool RsRegisterImageLoader();
void RsSetDebug();

void RsMouseSetVisibility(bool visibility);
void RsMouseSetPos(RwV2d* pos);

bool RsSelectDevice();
int32 RsInputDeviceAttach(RsInputDeviceType deviceType, RsInputEventHandler handler);

bool rsCommandLine(void* param);
bool rsPreInitCommandLine(RwChar* arg);

RsEventStatus RsEventHandler(RsEvent event, void* param);
RsEventStatus RsKeyboardEventHandler(RsEvent event, void* param);
RsEventStatus RsMouseEventHandler(RsEvent event, void* param);
RsEventStatus RsPadEventHandler(RsEvent event, void* param);

RwChar* RsPathnameCreate(const RwChar* buffer);
void RsPathnameDestroy(RwChar* buffer);
void RsRwTerminate();
void RsTerminate();
bool RsInitialize();
bool RsRwInitialize(void* param);

bool RsSetPresetView(RwCamera* camera, int32 viewNum);
void RsSetNextPresetView(RwCamera* camera);
void RsSetPreviousPresetView(RwCamera* camera);
void RsDestroyPresetViews();
void* RsGetPresetViewDescription();
void RsLoadPresetViews();
void RsSavePresetView();

RwImage* RsGrabScreen(RwCamera* camera);

void RsErrorMessage(const RwChar* msg);
void RsWarningMessage(const RwChar* msg);

bool RsCameraBeginUpdate(RwCamera* camera);
RwCamera* RsCameraShowRaster(RwCamera* camera);

/**
 * Platform Specific
 */
RwBool psInitialize();
void psTerminate();

void psWindowSetText(const char* str);
void psErrorMessage(const char* str);
void psWarningMessage(const char* str);

bool psCameraBeginUpdate(RwCamera* camera);
RwCamera* psCameraShowRaster(RwCamera* camera);

uint32 psTimer();

RwImage* psGrabScreen(RwCamera* camera);

void psMouseSetVisibility(bool visible);
void psMouseSetPos(RwV2d* pos);

char* psPathnameCreate(const char* buffer);
void psPathnameDestroy(char* buffer);
char psPathGetSeparator();

bool psInstallFileSystem();
bool psNativeTextureSupport();
RsEventStatus psDebugMessageHandler(RsEvent event, void* param);
bool psAlwaysOnTop(bool alwaysOnTop);
bool psSelectDevice();
