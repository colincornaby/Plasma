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

#ifndef _plVulkanPipelineState_h
#define _plVulkanPipelineState_h

#include <vulkan/vulkan.h>

// Forward declarations
class plVulkanDevice;
class plVulkanFragmentShader;
class plVulkanVertexShader;

class plVulkanPipelineState
{
public:
    plVulkanPipelineState(plVulkanDevice* device);
    ~plVulkanPipelineState();
    
    struct BlendState {
        bool blendEnable;
        VkBlendFactor srcColorBlendFactor;
        VkBlendFactor dstColorBlendFactor;
        VkBlendOp colorBlendOp;
        VkBlendFactor srcAlphaBlendFactor;
        VkBlendFactor dstAlphaBlendFactor;
        VkBlendOp alphaBlendOp;
        VkColorComponentFlags colorWriteMask;
        
        bool operator==(const BlendState& other) const;
    };
    
    struct RasterState {
        VkCullModeFlags cullMode;
        VkFrontFace frontFace;
        bool depthBiasEnable;
        float depthBiasConstantFactor;
        float depthBiasSlopeFactor;
        bool depthClampEnable;
        VkPolygonMode polygonMode;
        float lineWidth;
        
        bool operator==(const RasterState& other) const;
    };
    
    struct DepthStencilState {
        bool depthTestEnable;
        bool depthWriteEnable;
        VkCompareOp depthCompareOp;
        bool depthBoundsTestEnable;
        float minDepthBounds;
        float maxDepthBounds;
        bool stencilTestEnable;
        VkStencilOpState front;
        VkStencilOpState back;
        
        bool operator==(const DepthStencilState& other) const;
    };
    
    // Set state functions
    void SetBlendState(const BlendState& state);
    void SetRasterState(const RasterState& state);
    void SetDepthStencilState(const DepthStencilState& state);
    void SetVertexShader(plVulkanVertexShader* shader);
    void SetFragmentShader(plVulkanFragmentShader* shader);
    
    // Get state functions
    const BlendState& GetBlendState() const { return fBlendState; }
    const RasterState& GetRasterState() const { return fRasterState; }
    const DepthStencilState& GetDepthStencilState() const { return fDepthStencilState; }
    plVulkanVertexShader* GetVertexShader() const { return fVertexShader; }
    plVulkanFragmentShader* GetFragmentShader() const { return fFragmentShader; }
    
    // Create pipeline
    VkPipeline CreatePipeline(VkRenderPass renderPass, VkFormat colorFormat, VkFormat depthFormat);
    
private:
    plVulkanDevice* fDevice;
    BlendState fBlendState;
    RasterState fRasterState;
    DepthStencilState fDepthStencilState;
    plVulkanVertexShader* fVertexShader;
    plVulkanFragmentShader* fFragmentShader;
};

#endif // _plVulkanPipelineState_h