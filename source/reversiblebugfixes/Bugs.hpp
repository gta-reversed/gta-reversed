#pragma once

#include "./ReversibleBugFix.hpp"

/*!
* This is where the bugs are defined.
* The usual naming convention is `ClassName_Function_BugName` (`BugName` is something unique you come up with).
* Unless the same bug is spanning across multiple functions.
* 
* Fixes that prevent OOB should use `GenericOOB`.
* Fixes that prevent crashes (Like additional null ptr checks) can use `GenericCrash`.
* Feel free to add a new generic bug type if there's nothing that fits your needs.
*/
namespace notsa::bugfixes {
//
// Generic
//
inline const ReversibleBugFix GenericCrashing{
    "Generic crashes (null ptr access, etc)",
    "Fixes bugs that cause null ptr access, and similar anomalies crashing the game",
    "Contributors"
};
inline const ReversibleBugFix GenericOOB{
    "Generic out-of-bounds bugs",
    "Fixes generic out-of-bounds bugs across the codebase",
    "Contributors"
};

//
// Other bugs
//
inline const ReversibleBugFix PS2CoronaRendering{
    "PS2 Corona Rendering",
    "Fix corona rendering, so they're like on PS2",
    "SilentPatch Contributors"
};
inline const ReversibleBugFix AnimBlendSequence_SetName_SetBoneTagFlag{
    "BoneTag Name Flag",
    "Correctly set BoneTag flag in `CAnimBlendSequence::SetName`",
    "Pirulax"
};
inline const ReversibleBugFix AESound_UpdatePlayTime_DivisionByZero{
    "UpdatePlayTime Division-By-Zero",
    "Avoid Division-by-zero in CAESound::UpdatePlayTime",
    "Pirulax"
};
};
