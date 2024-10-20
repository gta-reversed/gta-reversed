/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#pragma once

#include "RenderWare.h"
#include "MenuManager.h"

namespace JPegPlugin
{
void InjectHooks();
}


void JPegCompressScreen(RwCamera* camera, IStream* stream);

/**
 * Compress camera screen and save at the given file path
 *
 * @addr  0x5D0820
 */
void JPegCompressScreenToFile(RwCamera* camera, const char* path);

/**
 * Compress camera screen to the given buffer
 *
 * @addr 0x5D0740
 */
void JPegCompressScreenToBuffer(RwCamera* camera, uint8*& buffer, uint32& size);

// 0x5D05F0
void JPegDecompressToRaster(RwRaster* raster, IStream* stream);

// 0x5D07A0
bool JPegDecompressToVramFromBuffer(RwRaster* raster, uint8** unk);
