#pragma once

#include "AEDataStream.h"

class CAEStreamTransformer {
public:
    void Initialise();
    void TransformBuffer(void* buffer, size_t size, uint32 position);

private:
    uint8 table[16]; // EA 3A C4 A1 9A A8 14 F3 48 B0 D7 23 9D E8 FF F1

    friend class CAEDataStream;
    static void InjectHooks();
};

extern CAEStreamTransformer& AEStreamTransformer;
