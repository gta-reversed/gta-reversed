/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CAnimBlock {
public:
    char         Name[16];
    bool         IsLoaded;
    int16        RefCnt;
    int32        FirstAnimIdx;
    uint32       NumAnims;
    AssocGroupId GroupId;
};
VALIDATE_SIZE(CAnimBlock, 0x20);
