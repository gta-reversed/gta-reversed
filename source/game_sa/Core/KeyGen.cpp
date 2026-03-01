/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "KeyGen.h"

void CKeyGen::InjectHooks() {
    RH_ScopedClass(CKeyGen);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(AppendStringToKey, 0x53CF70);
    RH_ScopedOverloadedInstall(GetKey, "null-terminated", 0x53CF00, uint32(*)(const char*));
    RH_ScopedOverloadedInstall(GetKey, "sized", 0x53CED0, uint32(*)(const char*, int32));
    RH_ScopedOverloadedInstall(GetUppercaseKey, "", 0x53CF30, uint32(*)(const char*));
}
