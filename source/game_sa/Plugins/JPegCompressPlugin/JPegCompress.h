#pragma once

#include "RenderWare.h"
#include "MenuManager.h"

struct jpeg_destination_mgr;
struct jpeg_source_mgr;

namespace JPegPlugin {
void InjectHooks();
}

/**
 * Compress camera screen common
 *
 * @addr  0x5D0470
 */
void JPegCompressScreen(RwCamera* camera, struct jpeg_destination_mgr& dst);

/**
 * Compress camera screen and save at the given file path
 *
 * @addr  0x5D0820
 */
void JPegCompressScreenToFile(RwCamera* camera, const char* path);

/**
 * Compress camera screen to the given buffer
 *
 * @addr  0x5D0740
 */
void JPegCompressScreenToBuffer(RwCamera* cam, uint8** buffer, size_t& bufferSizeInOut);

/**
 * Decompress a JPEG to a raster for displaying in-game. (PS2 gallery?)
 *
 * @addr  0x5D05F0
 */
void JPegDecompressToRaster(RwRaster* raster, struct jpeg_source_mgr& src);

/**
 * Decompress a JPEG to memory (GS?) for displaying in-game. (PS2 gallery?)
 *
 * @addr  0x5D05F0
 */
void JPegDecompressToVramFromBuffer(RwRaster* raster, bool enable);
