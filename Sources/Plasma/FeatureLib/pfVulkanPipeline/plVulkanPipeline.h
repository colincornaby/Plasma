/*==LICENSE==*

CyanWorlds.com Engine - MMOG client, server and tools
Copyright (C) 2011  Cyan Worlds, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Additional permissions under GNU GPL version 3 section 7

If you modify this Program, or any covered work, by linking or
combining it with any of RAD Game Tools Bink SDK, Autodesk 3ds Max SDK,
NVIDIA PhysX SDK, Microsoft DirectX SDK, OpenSSL library, Independent
JPEG Group JPEG library, Microsoft Windows Media SDK, or Apple QuickTime SDK
(or a modified version of those libraries),
containing parts covered by the terms of the Bink SDK EULA, 3ds Max EULA,
PhysX SDK EULA, DirectX SDK EULA, OpenSSL and SSLeay licenses, IJG
JPEG Library README, Windows Media SDK EULA, or QuickTime SDK EULA, the
licensors of this Program grant you additional
permission to convey the resulting work. Corresponding Source for a
non-source form of such a combination shall include the source code for
the parts of OpenSSL and IJG JPEG Library used as well as that of the covered
work.

You can contact Cyan Worlds, Inc. by email legal@cyan.com
 or by snail mail at:
      Cyan Worlds, Inc.
      14617 N Newport Hwy
      Mead, WA   99021

*==LICENSE==*/

#ifndef _plVulkanPipeline_h
#define _plVulkanPipeline_h

#include "plPipeline/pl3DPipeline.h"

#include "plVulkanDevice.h"
#include "plVulkanDeviceRef.h"
#include "plVulkanFragmentShader.h"
#include "plVulkanVertexShader.h"
#include "plVulkanMaterialShaderRef.h"

class plVulkanPlateManager;

class plVulkanPipeline : public pl3DPipeline<plVulkanDevice>
{
protected:
    std::vector<hsGDeviceRef*> fRenderTargetRefList;
    std::vector<plVulkanMaterialRef*> fMatRefList;
    
    // Uniform buffers for the current render pass
    struct CurrentRenderPassUniforms {
        // Uniform buffers for shaders
        hsMatrix44 fView;
        hsMatrix44 fProj;
        hsMatrix44 fLocal;
    } fCurrentRenderPassUniforms;

    plVulkanFragmentShader* fPShaderRefList;
    plVulkanVertexShader* fVShaderRefList;

    uint32_t fCurrRenderLayer;

public:
    plVulkanPipeline(hsWindowHndl display, hsWindowHndl window, const hsG3DDeviceModeRecord* devMode);
    virtual ~plVulkanPipeline();

    CLASSNAME_REGISTER(plVulkanPipeline);
    GETINTERFACE_ANY(plVulkanPipeline, pl3DPipeline<plVulkanDevice>);

    // Overrides from pl3DPipeline
    void SetViewport(const float x, const float y, const float width, const float height);

    // Virtual overrides
    plTextFont* MakeTextFont(ST::string face, uint16_t size) override;
    void CheckVertexBufferRef(plGBufferGroup* owner, uint32_t idx) override;
    void CheckIndexBufferRef(plGBufferGroup* owner, uint32_t idx) override;
    bool OpenAccess(plAccessSpan& dst, plDrawableSpans* d, const plVertexSpan* span, bool readOnly) override;
    bool CloseAccess(plAccessSpan& acc) override;
    void CheckTextureRef(plLayerInterface* lay) override;
    void PushRenderRequest(plRenderRequest* req) override;
    void PopRenderRequest(plRenderRequest* req) override;
    void ClearRenderTarget(plDrawable* d) override;
    void ClearRenderTarget(const hsColorRGBA* col = nullptr, const float* depth = nullptr) override;
    hsGDeviceRef* MakeRenderTargetRef(plRenderTarget* owner) override;
    bool BeginRender() override;
    bool EndRender() override;
    void RenderScreenElements() override;
    bool IsFullScreen() const override;
    uint32_t ColorDepth() const override;
    void Resize(uint32_t width, uint32_t height) override;
    bool CheckResources() override;
    void LoadResources() override;
    void SetZBiasScale(float scale) override;
    float GetZBiasScale() const override;
    void SetWorldToCamera(const hsMatrix44& w2c, const hsMatrix44& c2w) override;
    void RefreshScreenMatrices() override;

    // Rendering methods
    plDrawableSpans* CreateDrawableSpans(const hsTArray<plDrawableSpans*>& drawable, plRenderTarget* target, uint32_t blendFlags);
    plDrawableSpans* CreateDrawableSpans(plDrawableSpans* drawable);
    void RenderSpans(plDrawableSpans* ice, const std::vector<int16_t>& visList) override;

    void UpdateRenderTargetFormat(uint32_t format);
    void SetFramebufferFormat(uint32_t format);

    plMipmap* ExtractMipMap(plRenderTarget* targ) override;

    /// Error handling
    ST::string GetErrorString() override;

    bool ManagedAlloced() const { return false; }

    void GetSupportedColorDepths(std::vector<int>& ColorDepths);
    void GetSupportedDisplayModes(std::vector<plDisplayMode>* res, int ColorDepth = 32) override;
    int GetMaxAnisotropicSamples() override;
    int GetMaxAntiAlias(int Width, int Height, int ColorDepth) override;

    void ResetDisplayDevice(int Width, int Height, int ColorDepth, bool Windowed, int NumAASamples, int MaxAnisotropicSamples, bool vSync = false) override;

    int NumDisplays() const;
    void InspectDisplays();

    // For Vulkan shaders
    plVulkanVertexShader* GetVertexShader(uint32_t index) { return &fVShaderRefList[index]; }
    plVulkanFragmentShader* GetFragmentShader(uint32_t index) { return &fPShaderRefList[index]; }

protected:
    void ICreateRenderPipeline();

    plVulkanPlateManager* fPlateManager;

private:
    void IInitShaders();
    bool IInitDisplayDevice(bool fullscreen, int colorDepth);
    void IResizeDrawable(float width, float height);
};

#endif // _plVulkanPipeline_h