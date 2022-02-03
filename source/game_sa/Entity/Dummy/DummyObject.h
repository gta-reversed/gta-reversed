/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Dummy.h"

class CDummyObject : public CDummy {
public:
    CDummyObject() : CDummy() {}
    explicit CDummyObject(CObject* obj);

public:
    static void InjectHooks();

    CObject* CreateObject();
    void     UpdateFromObject(CObject* obj);
};

VALIDATE_SIZE(CDummyObject, 0x38);
