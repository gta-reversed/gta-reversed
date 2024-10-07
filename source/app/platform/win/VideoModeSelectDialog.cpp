#include "StdInc.h"

#include "VideoModeSelectDialog.h"
#include "VideoMode.h"
#include "platform/platform.h"

//
// TODO:
// For the final exe we have to add win.rc (A resource file that contains the definition of this dialogbox)
// It will also generate a resource.h that will have these defines in it
//

//! Resource ID of the videomode select dialog
#define IDD_DIALOG1                     104

//! Device select combobox item id
#define IDC_DEVICESEL                   1000

//! Video mode select combobox item id
#define IDC_VIDMODE                     1001

//! Exit button item id
#define IDEXIT                          1002

//
// Helper functions
//

RwInt32 GetCBCurSel(HWND hComboBox) {
    return SendMessage(hComboBox, CB_GETCURSEL, NULL, NULL);
}

void SetCBCurSel(HWND hComboBox, RwInt32 entryIdx) {
    SendMessage(hComboBox, CB_SETCURSEL, entryIdx, NULL);
}

RwInt32 GetCBItemData(HWND hComboBox, RwInt32 entryIdx) {
    return SendMessage(hComboBox, CB_GETITEMDATA, entryIdx, NULL);
}

RwInt32 GetCBCurSelData(HWND hComboBox) {
    return GetCBItemData(hComboBox, GetCBCurSel(hComboBox));
}

void SetSelectedVM(HWND hDlg, RwInt32 vm) {
    GcurSelVM = vm;
    SetCBCurSel(GetDlgItem(hDlg, IDC_VIDMODE), vm);
}

// 0x745920 - Fill in the available VMs to the dialog box vm select item
void FillAvailableVMs(HWND hVMSel) {
    const auto numVM = RwEngineGetNumVideoModes();
    for (auto i = 0; i < numVM; i++) {
        const auto vmi = RwEngineGetVideoModeInfo(i);

        if ((vmi.flags & rwVIDEOMODEEXCLUSIVE) == 0 || vmi.width < APP_MINIMAL_WIDTH || vmi.height < APP_MINIMAL_HEIGHT) {
            continue;
        }

        const auto aspectr = (float)vmi.height / (float)vmi.width;
        // printf("%f\n", aspectr) // annoying printf that spams the console was here

        const auto gcd = std::gcd(vmi.width, vmi.height);
        const auto ratioW = vmi.width / gcd, ratioH = vmi.height / gcd;

        // NOTSA: Provide aspect ratio info (W:H) instead of 'FULLSCREEN' and 'WIDESCREEN'.
        if (IsFullScreenRatio(aspectr) || IsWideScreenRatio(aspectr)) {
            static char vmName[1024];
            *std::format_to(vmName, "{} x {} x {} ({}:{})", vmi.width, vmi.height, vmi.depth, ratioW, ratioH) = 0;
            const auto idx = SendMessage(hVMSel, CB_ADDSTRING, NULL, (LPARAM)vmName); // Add entry, and get it's index
            SendMessage(hVMSel, CB_SETITEMDATA, idx, i);                              // Set index of that entry to correspond to `i`
        } else {
            DEV_LOG("Not listing video mode ({}) to device select! [{} x {} ({}:{})]", i, vmi.width, vmi.height, ratioW, ratioH);
        }
    }
}

// Fill in the available devices (subystems)
void FillAvailableDevices(HWND hDevSel) {
    // Fill in the available subystems
    for (auto ss = GnumSubSystems; ss-- > 0;) {
        SendMessage(hDevSel, CB_ADDSTRING, NULL, (LPARAM)(&GsubSysInfo[ss]));
        SendMessage(hDevSel, CB_SETITEMDATA, (WPARAM)ss, (LPARAM)ss);
    }

    // Set current SS as the selected one
    SendMessage(hDevSel, CB_SETCURSEL, GcurSelSS, NULL);
}

void ComboBoxClear(HWND hComboBox) {
    SendMessage(hComboBox, CB_RESETCONTENT, NULL, NULL);
}

// 0x745CC0 - Initialize the dialogbox
INT_PTR InitDialog(HWND hDlg) {
    const auto hDevSel = GetDlgItem(hDlg, IDC_DEVICESEL),
               hVMSel  = GetDlgItem(hDlg, IDC_VIDMODE);

    FillAvailableDevices(hDevSel);
    FillAvailableVMs(hVMSel);

    // Figure out currently selected videomode
    const auto numVM    = RwEngineGetNumVideoModes();
    const auto lastUsedVM = FrontEndMenuManager.m_nDisplayVideoMode;
    if (GcurSelVM == -1 && lastUsedVM < numVM && GetVideoModeList()[lastUsedVM]) {
        for (auto i = 0; i < numVM; i++) {
            if (GetCBItemData(hVMSel, i) == FrontEndMenuManager.m_nDisplayVideoMode) {
                SetSelectedVM(hDlg, i);
                return NULL;
            }
        }

        // Failed
        SetSelectedVM(hDlg, -1);
    } else {
        SetSelectedVM(hDlg, RwEngineGetCurrentVideoMode());
    }
    
    return NULL;
}

// 0x745E50
INT_PTR CALLBACK DialogFunc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam) {    
    switch (Msg) {
    case WM_INITDIALOG:
        return InitDialog(hDlg);

    case WM_COMMAND: {
        const auto hDevSel = GetDlgItem(hDlg, IDC_DEVICESEL),
                   hVMSel  = GetDlgItem(hDlg, IDC_VIDMODE);

        switch (LOWORD(wParam)) {
        case IDEXIT: {
            if (HIWORD(wParam) == 0) {
                EndDialog(hDlg, NULL);
            }
            return TRUE;
        }
        case IDC_VIDMODE: {
            if (HIWORD(wParam) == 1) {
                GcurSelVM = GetCBCurSelData(hVMSel);
            }
            return TRUE;
        }
        case IDOK: {
            if (HIWORD(wParam) == 0) {
                GcurSelVM = GetCBCurSelData(hVMSel);
                EndDialog(hDlg, TRUE);
            }
            return TRUE;
        }
        case IDC_DEVICESEL: { // Function from 0x745DC0
            const auto hDevSel = GetDlgItem(hDlg, IDC_DEVICESEL),
                       hVMSel  = GetDlgItem(hDlg, IDC_VIDMODE);

            const auto currEntry = GetCBCurSel(hDevSel);
            if (currEntry != GcurSelSS) {
                GcurSelSS = GetCBItemData(hDevSel, currEntry);

                VERIFY(RwEngineSetSubSystem(GcurSelSS));

                // Deal with VM select ComboBox
                ComboBoxClear(hVMSel);
                FillAvailableVMs(hVMSel);
                SetSelectedVM(hDlg, RwEngineGetCurrentVideoMode());
            }
            return TRUE;
        }
        break;
    }
    }
    }
    return FALSE;
}

LRESULT CreateVidModeSelectDialog(HINSTANCE hInst, HWND hWnd) {
    return DialogBoxParam(hInst, MAKEINTRESOURCEA(IDD_DIALOG1), hWnd, DialogFunc, 0);
}
