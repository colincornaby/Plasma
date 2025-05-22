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

#include "plVulkanPipelineState.h"
#include "plVulkanDevice.h"
#include "plVulkanFragmentShader.h"
#include "plVulkanVertexShader.h"

bool plVulkanPipelineState::BlendState::operator==(const plVulkanPipelineState::BlendState& other) const
{
    return blendEnable == other.blendEnable &&
           srcColorBlendFactor == other.srcColorBlendFactor &&
           dstColorBlendFactor == other.dstColorBlendFactor &&
           colorBlendOp == other.colorBlendOp &&
           srcAlphaBlendFactor == other.srcAlphaBlendFactor &&
           dstAlphaBlendFactor == other.dstAlphaBlendFactor &&
           alphaBlendOp == other.alphaBlendOp &&
           colorWriteMask == other.colorWriteMask;
}

bool plVulkanPipelineState::RasterState::operator==(const plVulkanPipelineState::RasterState& other) const
{
    return cullMode == other.cullMode &&
           frontFace == other.frontFace &&
           depthBiasEnable == other.depthBiasEnable &&
           depthBiasConstantFactor == other.depthBiasConstantFactor &&
           depthBiasSlopeFactor == other.depthBiasSlopeFactor &&
           depthClampEnable == other.depthClampEnable &&
           polygonMode == other.polygonMode &&
           lineWidth == other.lineWidth;
}

bool plVulkanPipelineState::DepthStencilState::operator==(const plVulkanPipelineState::DepthStencilState& other) const
{
    return depthTestEnable == other.depthTestEnable &&
           depthWriteEnable == other.depthWriteEnable &&
           depthCompareOp == other.depthCompareOp &&
           depthBoundsTestEnable == other.depthBoundsTestEnable &&
           minDepthBounds == other.minDepthBounds &&
           maxDepthBounds == other.maxDepthBounds &&
           stencilTestEnable == other.stencilTestEnable &&
           memcmp(&front, &other.front, sizeof(front)) == 0 &&
           memcmp(&back, &other.back, sizeof(back)) == 0;
}

plVulkanPipelineState::plVulkanPipelineState(plVulkanDevice* device)
    : fDevice(device),
      fVertexShader(nullptr),
      fFragmentShader(nullptr)
{
    // Initialize with default states
    fBlendState = {};
    fBlendState.blendEnable = false;
    fBlendState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    fBlendState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    fBlendState.colorBlendOp = VK_BLEND_OP_ADD;
    fBlendState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    fBlendState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    fBlendState.alphaBlendOp = VK_BLEND_OP_ADD;
    fBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | 
                                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    
    fRasterState = {};
    fRasterState.cullMode = VK_CULL_MODE_BACK_BIT;
    fRasterState.frontFace = VK_FRONT_FACE_CLOCKWISE;
    fRasterState.depthBiasEnable = false;
    fRasterState.depthBiasConstantFactor = 0.0f;
    fRasterState.depthBiasSlopeFactor = 0.0f;
    fRasterState.depthClampEnable = false;
    fRasterState.polygonMode = VK_POLYGON_MODE_FILL;
    fRasterState.lineWidth = 1.0f;
    
    fDepthStencilState = {};
    fDepthStencilState.depthTestEnable = true;
    fDepthStencilState.depthWriteEnable = true;
    fDepthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
    fDepthStencilState.depthBoundsTestEnable = false;
    fDepthStencilState.minDepthBounds = 0.0f;
    fDepthStencilState.maxDepthBounds = 1.0f;
    fDepthStencilState.stencilTestEnable = false;
    fDepthStencilState.front = {};
    fDepthStencilState.back = {};
}

plVulkanPipelineState::~plVulkanPipelineState()
{
    // No resources to clean up
}

void plVulkanPipelineState::SetBlendState(const BlendState& state)
{
    fBlendState = state;
}

void plVulkanPipelineState::SetRasterState(const RasterState& state)
{
    fRasterState = state;
}

void plVulkanPipelineState::SetDepthStencilState(const DepthStencilState& state)
{
    fDepthStencilState = state;
}

void plVulkanPipelineState::SetVertexShader(plVulkanVertexShader* shader)
{
    fVertexShader = shader;
}

void plVulkanPipelineState::SetFragmentShader(plVulkanFragmentShader* shader)
{
    fFragmentShader = shader;
}

VkPipeline plVulkanPipelineState::CreatePipeline(VkRenderPass renderPass, VkFormat colorFormat, VkFormat depthFormat)
{
    // To be implemented
    return VK_NULL_HANDLE;
}