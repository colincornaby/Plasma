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

#include "plVulkanPipeline.h"
#include "plPipeline/plPipeDebugFlags.h"
#include "plPipeline/plPipelineCreatable.h"
#include "plPipeline/hsG3DDeviceSelector.h"
#include "plVulkanDevice.h"
#include "plVulkanPlateManager.h"
#include "plVulkanFragmentShader.h"
#include "plVulkanVertexShader.h"
#include "plVulkanMaterialShaderRef.h"

#include "plGImage/plMipmap.h"
#include "plSurface/plLayer.h"
#include "pnKeyedObject/plUoid.h"
#include "plDrawable/plDrawableSpans.h"
#include "hsResMgr.h"
#include "plStatusLog/plStatusLog.h"
#include "hsTimer.h"

#include <vulkan/vulkan.h>

plVulkanPipeline::plVulkanPipeline(hsWindowHndl display, hsWindowHndl window, const hsG3DDeviceModeRecord* devMode) 
    : pl3DPipeline(devMode),
      fRenderTargetRefList(),
      fMatRefList(),
      fCurrentRenderPassUniforms(),
      fPShaderRefList(),
      fVShaderRefList(),
      fCurrRenderLayer()
{
    fPlateManager = nullptr;
    fDevice.InitDevice();
    
    // For now - set this once at startup. If the underlying device is allow to change on
    // the fly (eGPU, display change, etc) - revisit.
    SetFramebufferFormat(VK_FORMAT_B8G8R8A8_UNORM);

    IInitShaders();
    IInitDisplayDevice(devMode->GetMode()->IsFullScreen(), devMode->GetMode()->GetColorDepth());
}

plVulkanPipeline::~plVulkanPipeline()
{
    delete fPlateManager;
}

void plVulkanPipeline::SetViewport(const float x, const float y, const float width, const float height)
{
    // Implementation will set the viewport in Vulkan
}

plTextFont* plVulkanPipeline::MakeTextFont(ST::string face, uint16_t size)
{
    return nullptr; // To be implemented
}

void plVulkanPipeline::CheckVertexBufferRef(plGBufferGroup* owner, uint32_t idx)
{
    // To be implemented
}

void plVulkanPipeline::CheckIndexBufferRef(plGBufferGroup* owner, uint32_t idx)
{
    // To be implemented  
}

bool plVulkanPipeline::OpenAccess(plAccessSpan& dst, plDrawableSpans* d, const plVertexSpan* span, bool readOnly)
{
    return false; // To be implemented
}

bool plVulkanPipeline::CloseAccess(plAccessSpan& acc)
{
    return false; // To be implemented
}

void plVulkanPipeline::CheckTextureRef(plLayerInterface* lay)
{
    // To be implemented
}

void plVulkanPipeline::PushRenderRequest(plRenderRequest* req)
{
    // To be implemented
}

void plVulkanPipeline::PopRenderRequest(plRenderRequest* req)
{
    // To be implemented
}

void plVulkanPipeline::ClearRenderTarget(plDrawable* d)
{
    // To be implemented
}

void plVulkanPipeline::ClearRenderTarget(const hsColorRGBA* col, const float* depth)
{
    // To be implemented
}

hsGDeviceRef* plVulkanPipeline::MakeRenderTargetRef(plRenderTarget* owner)
{
    return nullptr; // To be implemented
}

bool plVulkanPipeline::BeginRender()
{
    return fDevice.BeginRender();
}

bool plVulkanPipeline::EndRender()
{
    return false; // To be implemented
}

void plVulkanPipeline::RenderScreenElements()
{
    // To be implemented
}

bool plVulkanPipeline::IsFullScreen() const
{
    return false; // To be implemented
}

uint32_t plVulkanPipeline::ColorDepth() const
{
    return 32; // To be implemented with actual color depth
}

void plVulkanPipeline::Resize(uint32_t width, uint32_t height)
{
    // To be implemented
}

bool plVulkanPipeline::CheckResources()
{
    return true; // To be implemented
}

void plVulkanPipeline::LoadResources()
{
    // To be implemented
}

void plVulkanPipeline::SetZBiasScale(float scale)
{
    // To be implemented
}

float plVulkanPipeline::GetZBiasScale() const
{
    return 0.0f; // To be implemented
}

void plVulkanPipeline::SetWorldToCamera(const hsMatrix44& w2c, const hsMatrix44& c2w)
{
    pl3DPipeline::SetWorldToCamera(w2c, c2w);
    // Additional Vulkan-specific implementation
}

void plVulkanPipeline::RefreshScreenMatrices()
{
    pl3DPipeline::RefreshScreenMatrices();
    // Additional Vulkan-specific implementation
}

plDrawableSpans* plVulkanPipeline::CreateDrawableSpans(const hsTArray<plDrawableSpans*>& drawable, plRenderTarget* target, uint32_t blendFlags)
{
    return nullptr; // To be implemented
}

plDrawableSpans* plVulkanPipeline::CreateDrawableSpans(plDrawableSpans* drawable)
{
    return nullptr; // To be implemented
}

void plVulkanPipeline::RenderSpans(plDrawableSpans* ice, const std::vector<int16_t>& visList)
{
    // To be implemented
}

void plVulkanPipeline::UpdateRenderTargetFormat(uint32_t format)
{
    // To be implemented
}

void plVulkanPipeline::SetFramebufferFormat(uint32_t format)
{
    // To be implemented
}

plMipmap* plVulkanPipeline::ExtractMipMap(plRenderTarget* targ)
{
    return nullptr; // To be implemented
}

ST::string plVulkanPipeline::GetErrorString()
{
    return fDevice.GetErrorString();
}

void plVulkanPipeline::GetSupportedColorDepths(std::vector<int>& ColorDepths)
{
    ColorDepths.push_back(32); // Default to 32-bit color depth
}

void plVulkanPipeline::GetSupportedDisplayModes(std::vector<plDisplayMode>* res, int ColorDepth)
{
    // To be implemented - collect available display modes
}

int plVulkanPipeline::GetMaxAnisotropicSamples()
{
    return 16; // Default value, to be implemented
}

int plVulkanPipeline::GetMaxAntiAlias(int Width, int Height, int ColorDepth)
{
    return 8; // Default value, to be implemented
}

void plVulkanPipeline::ResetDisplayDevice(int Width, int Height, int ColorDepth, bool Windowed, int NumAASamples, int MaxAnisotropicSamples, bool vSync)
{
    // To be implemented
}

int plVulkanPipeline::NumDisplays() const
{
    return 1; // Default value, to be implemented
}

void plVulkanPipeline::InspectDisplays()
{
    // To be implemented
}

void plVulkanPipeline::ICreateRenderPipeline()
{
    // To be implemented - create Vulkan pipeline objects
}

void plVulkanPipeline::IInitShaders()
{
    // To be implemented - initialize built-in shaders
}

bool plVulkanPipeline::IInitDisplayDevice(bool fullscreen, int colorDepth)
{
    // To be implemented - setup Vulkan device, swapchain, etc.
    return true;
}

void plVulkanPipeline::IResizeDrawable(float width, float height)
{
    // To be implemented - handle window resize
}