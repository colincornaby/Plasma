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

#ifndef _plVulkanDevice_h
#define _plVulkanDevice_h

#include <string_theory/string>
#include <unordered_map>

#include "hsGeometry3.h"
#include "hsMatrix44.h"

#include <vulkan/vulkan.h>

class plRenderTarget;
class hsGMaterial;
class plCubicEnvironmap;
class plMipmap;
class plGBufferGroup;
class plLayer;

class plVulkanDevice
{
public:
    struct plVulkanPipelineRecord
    {
        VkFormat depthFormat;
        VkFormat colorFormat;
        uint32_t sampleCount;
        
        struct PipelineState {
            // Pipeline state details
            bool operator==(const PipelineState& p) const;
        };
        
        PipelineState* state;
        
        bool operator==(const plVulkanPipelineRecord& p) const;
    };

    struct VertexBufferRef
    {
        VkBuffer buffer;
        VkDeviceMemory memory;
        size_t size;
        uint32_t stride;
        uint32_t count;
    };

    struct IndexBufferRef
    {
        VkBuffer buffer;
        VkDeviceMemory memory;
        size_t size;
        uint32_t count;
    };

    struct TextureRef
    {
        VkImage image;
        VkImageView view;
        VkDeviceMemory memory;
        VkSampler sampler;
        uint32_t width;
        uint32_t height;
        uint32_t format;
        uint32_t levels;
    };

    plVulkanDevice();
    ~plVulkanDevice();

    bool InitDevice();
    void Shutdown();

    /**
     * Set rendering to the specified render target.
     *
     * Null rendertarget is the primary. Invalidates the state as required by
     * experience, not documentation.
     */
    void SetRenderTarget(plRenderTarget* target);

    /** Translate our viewport into a Vulkan viewport. */
    void SetViewport();

    bool BeginRender();

    /* Device Ref Functions **************************************************/
    void SetupVertexBufferRef(plGBufferGroup* owner, uint32_t idx, VertexBufferRef* vRef);
    void CheckStaticVertexBuffer(VertexBufferRef* vRef, plGBufferGroup* owner, uint32_t idx);
    void FillVertexBufferRef(VertexBufferRef* ref, plGBufferGroup* group, uint32_t idx);
    void FillVolatileVertexBufferRef(VertexBufferRef* ref, plGBufferGroup* group, uint32_t idx);
    void SetupIndexBufferRef(plGBufferGroup* owner, uint32_t idx, IndexBufferRef* iRef);
    void CheckStaticIndexBuffer(IndexBufferRef* iRef, plGBufferGroup* owner, uint32_t idx);
    void FillIndexBufferRef(IndexBufferRef* ref, plGBufferGroup* group, uint32_t idx);

    // Texture upload functions
    uint32_t ConfigureAllowedLevels(TextureRef* tRef, plMipmap* mipmap);
    void PopulateTexture(TextureRef* tRef, plMipmap* img, uint32_t slice);
    void MakeTextureRef(TextureRef* tRef, plMipmap* img);
    void MakeCubicTextureRef(TextureRef* tRef, plCubicEnvironmap* img);

    // Matrix management
    void SetProjectionMatrix(const hsMatrix44& src);
    void SetWorldToCameraMatrix(const hsMatrix44& src);
    void SetLocalToWorldMatrix(const hsMatrix44& src);

    // Draw commands
    void CreateCommandBuffer();
    void SubmitCommandBuffer();
    VkCommandBuffer GetCurrentCommandBuffer() const;

    // Error handling
    ST::string GetErrorString() const { return fErrorMsg; }

    // Sampler state
    VkSampler SamplerStateForClampFlags(uint32_t sampleState) const;

    // Device capabilities
    bool SupportsTileMemory() const { return fSupportsTileMemory; }

private:
    // Device and instance
    VkInstance fVkInstance;
    VkPhysicalDevice fPhysicalDevice;
    VkDevice fVkDevice;
    VkQueue fGraphicsQueue;
    VkCommandPool fCommandPool;
    
    // Current command buffer and target
    VkCommandBuffer fCurrentCommandBuffer;
    VkRenderPass fCurrentRenderPass;
    VkFramebuffer fCurrentFramebuffer;
    
    // Current state
    VkPipeline fCurrentPipeline;
    VkPipelineLayout fCurrentPipelineLayout;
    VkDescriptorSet fCurrentDescriptorSet;
    
    // Getter for device
    VkDevice GetDevice() const { return fVkDevice; }
    
    // Swapchain
    VkSwapchainKHR fSwapchain;
    VkFormat fSwapchainFormat;
    std::vector<VkImage> fSwapchainImages;
    std::vector<VkImageView> fSwapchainImageViews;
    
    // Samplers 
    VkSampler fSamplerStates[16]; // Different sampler states
    
    // Error message
    ST::string fErrorMsg;

    // Render target clear values
    VkClearValue fClearRenderTargetColor;
    VkClearValue fClearDrawableColor;
    float fClearRenderTargetDepth;
    float fClearDrawableDepth;
    bool fShouldClearRenderTarget;
    bool fShouldClearDrawable;
    
    // Threading
    uint32_t fActiveThread;

    // Capabilities
    bool fSupportsTileMemory;
    
    // Pipeline cache
    std::unordered_map<size_t, VkPipeline> fPipelineStateCache;
    
    // Helper methods
    void CreateInstance();
    void SelectPhysicalDevice();
    void CreateLogicalDevice();
    bool CheckValidationLayerSupport();
    void CreateSwapchain();
    void CleanupSwapchain();
    void CreateSamplers();
};

#endif // _plVulkanDevice_h