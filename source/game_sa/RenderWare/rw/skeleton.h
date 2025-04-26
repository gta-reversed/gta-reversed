/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once
/****************************************************************************
 *
 * skeleton.h
 *
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 1999, 2000 Criterion Software Ltd.
 * All Rights Reserved.
 *
 */
#include "rwcore.h"
#include <Windows.h>

/* Default arena size depending on platform. */

#if (defined(SKY))
#define rsRESOURCESDEFAULTARENASIZE (8 << 20)
#elif (defined(_XBOX))
#define rsRESOURCESDEFAULTARENASIZE (8 << 20)
#elif (defined(DOLPHIN))
#define rsRESOURCESDEFAULTARENASIZE (4 << 20)
#elif (defined(D3D8_DRVMODEL_H))
#define rsRESOURCESDEFAULTARENASIZE (4 << 20)
#elif (defined(D3D9_DRVMODEL_H))
#define rsRESOURCESDEFAULTARENASIZE (4 << 20)
#elif (defined(OPENGL_DRVMODEL_H))
#if (defined(macintosh))
#define rsRESOURCESDEFAULTARENASIZE (2 << 20)
#else
#define rsRESOURCESDEFAULTARENASIZE (16 << 20)
#endif
#else
#define rsRESOURCESDEFAULTARENASIZE (16 << 20)
#endif

enum RsInputDeviceType
{
    rsKEYBOARD,
    rsMOUSE,
    rsPAD
};
typedef enum RsInputDeviceType RsInputDeviceType;

enum RsEventStatus
{
    rsEVENTERROR,
    rsEVENTPROCESSED,
    rsEVENTNOTPROCESSED
};
typedef enum RsEventStatus RsEventStatus;

enum RsEvent {
    rsCAMERASIZE            =  0,
    rsCOMMANDLINE           =  1,
    rsFILELOAD              =  2,
    rsINITDEBUG             =  3,
    rsINPUTDEVICEATTACH     =  4,
    rsLEFTBUTTONDOWN        =  5,
    rsLEFTBUTTONUP          =  6,
    rsMOUSEMOVE             =  7,
    rsMOUSEWHEELMOVE        =  8,
    rsPLUGINATTACH          =  9,
    rsREGISTERIMAGELOADER   = 10,

    rsRIGHTBUTTONDOWN       = 11,
    rsRIGHTBUTTONUP         = 12,
    rsMIDDLEBUTTONDOWN      = 13,
    rsMIDDLEBUTTONUP        = 14,
    rsMOUSEWHEELMOVEDUP     = 15,
    rsMOUSEWHEELMOVEDDOWN   = 16,
    rsFIRST_XBUTTONUP       = 17,
    rsSECOND_XBUTTONUP      = 18,
    rsFIRST_XBUTTONDOWN     = 19,
    rsSECOND_XBUTTONDOWN    = 20,

    rsRWINITIALIZE          = 21,
    rsRWTERMINATE           = 22,
    rsSELECTDEVICE          = 23,
    rsINITIALIZE            = 24,
    rsTERMINATE             = 25,
    rsIDLE                  = 26,
    rsFRONTENDIDLE          = 27,
    rsKEYDOWN               = 28,
    rsKEYUP                 = 29,
    rsQUITAPP               = 30,

    rsPADBUTTONDOWN         = 31,
    rsPADBUTTONUP           = 32,
    rsPADANALOGUELEFT       = 33,
    rsPADANALOGUELEFTRESET  = 34,
    rsPADANALOGUERIGHT      = 35,
    rsPADANALOGUERIGHTRESET = 36,
    rsPREINITCOMMANDLINE    = 37,
    rsACTIVATE              = 38,
    rsSETMEMORYFUNCS        = 39,
};
typedef enum RsEvent RsEvent;

typedef RsEventStatus (*RsInputEventHandler)(RsEvent event, void* param);

typedef struct RsInputDevice RsInputDevice;
struct RsInputDevice {
    RsInputDeviceType inputDeviceType;
    RwBool used;
    RsInputEventHandler inputEventHandler;
};

/**
 * platform-specific global data
 */
struct psGlobalType {
    HWND      window;
    HINSTANCE instance;
    RwBool    fullScreen;
    RwV2d     lastMousePos;
    uint32    lastRenderTime;

    LPDIRECTINPUT8 diInterface;
    LPDIRECTINPUTDEVICE8 diMouse;
    LPDIRECTINPUTDEVICE8 diDevice1;
    LPDIRECTINPUTDEVICE8 diDevice2;

#ifdef NOTSA_USE_SDL3
    void* sdlWindow; // SDL_Window*
#endif
};

typedef struct RsGlobalType RsGlobalType;
struct RsGlobalType {
    const RwChar* appName;
    RwInt32 maximumWidth;
    RwInt32 maximumHeight;
    RwInt32 frameLimit;
    RwBool quit;

    psGlobalType* ps; /* platform specific data */

    RsInputDevice keyboard;
    RsInputDevice mouse;
    RsInputDevice pad;
};

typedef struct RsMouseStatus RsMouseStatus;
struct RsMouseStatus
{
    RwV2d   pos;
    RwV2d   delta;
    RwBool  shift;
    RwBool  control;
#if defined(macintosh)
    RwBool  alt;
#endif /* defined(macintosh) */
};

enum RsKeyCodes : int32 {
    /* ASCII chars up to 0xFF are also valid */

    rsESC            = 1000,

    rsF1             = 1001,
    rsF2             = 1002,
    rsF3             = 1003,
    rsF4             = 1004,
    rsF5             = 1005,
    rsF6             = 1006,
    rsF7             = 1007,
    rsF8             = 1008,
    rsF9             = 1009,
    rsF10            = 1010,
    rsF11            = 1011,
    rsF12            = 1012,

    rsINS            = 1013,
    rsDEL            = 1014,
    rsHOME           = 1015,
    rsEND            = 1016,
    rsPGUP           = 1017,
    rsPGDN           = 1018,

    rsUP             = 1019,
    rsDOWN           = 1020,
    rsLEFT           = 1021,
    rsRIGHT          = 1022,

    rsDIVIDE         = 1023,
    rsTIMES          = 1024,
    rsPLUS           = 1025,
    rsMINUS          = 1026,
    rsPADDEL         = 1027,
    rsPADEND         = 1028,
    rsPADDOWN        = 1029,
    rsPADPGDN        = 1030,
    rsPADLEFT        = 1031,
    rsPAD5           = 1032,
    rsNUMLOCK        = 1033,
    rsPADRIGHT       = 1034,
    rsPADHOME        = 1035,
    rsPADUP          = 1036,
    rsPADPGUP        = 1037,
    rsPADINS         = 1038,
    rsPADENTER       = 1039,

    rsSCROLL         = 1040,
    rsPAUSE          = 1041,

    rsBACKSP         = 1042,
    rsTAB            = 1043,
    rsCAPSLK         = 1044,
    rsENTER          = 1045,
    rsLSHIFT         = 1046,
    rsRSHIFT         = 1047,
    rsSHIFT          = 1048,
    rsLCTRL          = 1049,
    rsRCTRL          = 1050,
    rsLALT           = 1051,
    rsRALT           = 1052,
    rsLWIN           = 1053,
    rsRWIN           = 1054,
    rsAPPS           = 1055,

    rsNULL           = 1056,

    rsMOUSE_LEFT_BUTTON       = 1,
    rsMOUSE_MIDDLE_BUTTON     = 2,
    rsMOUSE_RIGHT_BUTTON      = 3,
    rsMOUSE_WHEEL_UP_BUTTON   = 4,
    rsMOUSE_WHEEL_DOWN_BUTTON = 5,
    rsMOUSE_X1_BUTTON         = 6,
    rsMOUSE_X2_BUTTON         = 7,
};
typedef enum RsKeyCodes RsKeyCodes;

typedef struct RsKeyStatus RsKeyStatus;
struct RsKeyStatus
{
    RwInt32     keyScanCode;
    RwInt32     keyCharCode;
#if defined(macintosh)
    RwBool      shift;
    RwBool      control;
    RwBool      alt;
#endif /* defined(macintosh) */
};

typedef struct RsPadButtonStatus RsPadButtonStatus;
struct RsPadButtonStatus
{
    RwInt32     padID;
    RwUInt32    padButtons;
};

enum RsPadButtons
{
    rsPADDPADLEFT   = 0x00000001,
    rsPADDPADRIGHT  = 0x00000002,
    rsPADDPADUP     = 0x00000004,
    rsPADDPADDOWN   = 0x00000008,
    rsPADSTART      = 0x00000010,
    rsPADSELECT     = 0x00000020,
    rsPADBUTTON1    = 0x00000040,
    rsPADBUTTON2    = 0x00000080,
    rsPADBUTTON3    = 0x00000100,
    rsPADBUTTON4    = 0x00000200,
    rsPADBUTTON5    = 0x00000400,
    rsPADBUTTON6    = 0x00000800,
    rsPADBUTTON7    = 0x00001000,
    rsPADBUTTON8    = 0x00002000,
    rsPADBUTTONA1   = 0x00004000,
    rsPADBUTTONA2   = 0x00008000
};
typedef enum RsPadButtons RsPadButtons;

enum RsPrintPos
{
    rsPRINTPOSMIDDLE    = 0,

    rsPRINTPOSLEFT   = 1,
    rsPRINTPOSRIGHT  = 2,
    rsPRINTPOSTOP    = 4,
    rsPRINTPOSBOTTOM = 8,

    rsPRINTPOSTOPLEFT     = rsPRINTPOSTOP    | rsPRINTPOSLEFT,
    rsPRINTPOSTOPRIGHT    = rsPRINTPOSTOP    | rsPRINTPOSRIGHT,
    rsPRINTPOSBOTTOMLEFT  = rsPRINTPOSBOTTOM | rsPRINTPOSLEFT,
    rsPRINTPOSBOTTOMRIGHT = rsPRINTPOSBOTTOM | rsPRINTPOSRIGHT,

    rsPRINTPOSFORCENUMSIZEINT = 0x7FFFFFFF
};
typedef enum RsPrintPos RsPrintPos;

/* Screen margins in charset width/height units, used w/ RsPrintPos */
enum RsPrintMargin
{
    rsPRINTMARGINMIDDLE = 0,

#if ((defined(SKY2_DRVMODEL_H)) || (defined(GCN_DRVMODEL_H)) || (defined(XBOX_DRVMODEL_H)))
    rsPRINTMARGINTOP    = 2,
    rsPRINTMARGINBOTTOM = 4,
    rsPRINTMARGINLEFT   = 5,
    rsPRINTMARGINRIGHT  = 5,
#else /* ((defined(SKY2_DRVMODEL_H)) || (defined(GCN_DRVMODEL_H)) || (defined(XBOX_DRVMODEL_H)) */
    rsPRINTMARGINTOP    = 1,
    rsPRINTMARGINBOTTOM = 1,
    rsPRINTMARGINLEFT   = 1,
    rsPRINTMARGINRIGHT  = 1,
#endif /* ((defined(SKY2_DRVMODEL_H)) || (defined(GCN_DRVMODEL_H)) || (defined(XBOX_DRVMODEL_H)) */
    rsPRINTMARGINFORCENUMSIZEINT = 0x7FFFFFFF
};
typedef enum RsPrintMargin RsPrintMargin;
