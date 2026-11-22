/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <Base.h>
#include "RenderWare.h"

class CShadowCamera { // AKA CShadowImage
public:
    RwCamera*  m_pRwCamera{};
    RwTexture* m_pRwRenderTexture{};

public:
    static void InjectHooks();

    CShadowCamera() = default;
    ~CShadowCamera() { Destroy(); }

    /*!
    * @addr 0x705400
    */
    void Destroy();

    /*!
    * @addr 0x7054C0
    */
    void SetFrustum(float viewWindow);

    /*!
    * @brief 0x705520
    */
    void SetLight(RpLight* light);

    /*!
    * @addr 0x705590
    */
    void SetCenter(const CVector& center);

    /*!
    * @addr 0x705660
    * @brief Render an inverted color version of the camera's raster onto itself
    */
    void InvertRaster();

    /*!
    * @addr 0x705770
    * @brief Return the camera's raster
    */
    RwRaster* GetRwRenderRaster() const;

    /*!
    * @addr 0x705780
    * @brief Return a texture that is using the same raster as the camera
    */
    RwTexture* GetRwRenderTexture() const;

    /*!
    * @addr 0x705790
    * @brief Draw a 1px wide outline on the edges of the camera's rastrer
    * @return The raster the outline was rendered to
    */
    RwRaster* DrawOutlineBorder(const CRGBA& color);

    /*!
    * @addr 0x705B60
    * @brief Create camera and render texture
    * 
    * @param rasterSizePower Size of the raster, final size in pixels will be `pow(2, rasterSizePower)`
    * 
    * @return The camera created
    */
    RwCamera* Create(int32 rasterSize);

    /*!
    * @addr  0x705BF0
    * @brief Render a clump and all its atomics
    */
    RwCamera* Update(RpClump* clump);

    /*!
    * @addr 0x705C80
    * @brief Render an atomic
    */
    RwCamera* Update(RpAtomic* atomic);

    /*!
    * @addr 0x705D20
    * @brief Make a monocolor intensity gardient
    * @return The camera the gardient was rendered on if successful, `nullptr` otherwise.
    */
    RwCamera* MakeGradientRaster();

    RwImage* CreateRwImage();
    RwRaster* DrawBlurInRect(RwRect* rect);

    /*!
    * @addr 0x706070
    */
    RwRaster* RasterResample(RwRaster* raster);

    /*!
    * @addr 0x706170
    * @brief Render a blurry version of the camera's raster onto `blurRaster`
    * @return The camera's raster
    */
    RwRaster* RasterBlur(RwRaster* blurRaster, int32 numPasses);

    /*!
    * @addr 0x706330
    *
    * @param srcRaster The raster to render to
    * 
    * @return The raster we've rendered to (`srcRaster`)
    */
    RwRaster* DrawBlurryRaster2(RwRaster* srcRaster);
private:
    /*!
    * @notsa
    * @brief Internal function to render to reduce copy-paste code
    *
    * @param geo             The geometry to be rendered
    * @param geoFlagsToClear Flags to clear from geometry for rendering this time (all flags will be restored afterwards)
    * @param Render          The function that should render everything necessary
    */
    template<typename RenderFnT>
    void Update_Internal(RpAtomic* atomic, uint32 geoFlagsToClear, RenderFnT&& Render);

private:
    CShadowCamera* Constructor() {
        this->CShadowCamera::CShadowCamera();
        return this;
    }

    CShadowCamera* Destructor() {
        this->CShadowCamera::~CShadowCamera();
        return this;
    }
};
VALIDATE_SIZE(CShadowCamera, 0x8);
