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

#include "plVulkanPlateManager.h"
#include "plVulkanDevice.h"

plVulkanPlateManager::plVulkanPlateManager(plVulkanDevice* device)
    : plPlateManager(),
      fDevice(device),
      fVertexBuffer(VK_NULL_HANDLE),
      fVertexBufferMemory(VK_NULL_HANDLE),
      fPipeline(VK_NULL_HANDLE),
      fPipelineLayout(VK_NULL_HANDLE),
      fDescriptorSetLayout(VK_NULL_HANDLE),
      fDescriptorPool(VK_NULL_HANDLE)
{
    // To be implemented - initialization of plate rendering resources
}

plVulkanPlateManager::~plVulkanPlateManager()
{
    // Clean up Vulkan resources
    if (fDevice && fDevice->GetDevice() != VK_NULL_HANDLE) {
        if (fVertexBuffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(fDevice->GetDevice(), fVertexBuffer, nullptr);
            fVertexBuffer = VK_NULL_HANDLE;
        }
        
        if (fVertexBufferMemory != VK_NULL_HANDLE) {
            vkFreeMemory(fDevice->GetDevice(), fVertexBufferMemory, nullptr);
            fVertexBufferMemory = VK_NULL_HANDLE;
        }
        
        if (fPipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(fDevice->GetDevice(), fPipeline, nullptr);
            fPipeline = VK_NULL_HANDLE;
        }
        
        if (fPipelineLayout != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(fDevice->GetDevice(), fPipelineLayout, nullptr);
            fPipelineLayout = VK_NULL_HANDLE;
        }
        
        if (fDescriptorSetLayout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(fDevice->GetDevice(), fDescriptorSetLayout, nullptr);
            fDescriptorSetLayout = VK_NULL_HANDLE;
        }
        
        if (fDescriptorPool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(fDevice->GetDevice(), fDescriptorPool, nullptr);
            fDescriptorPool = VK_NULL_HANDLE;
        }
    }
}

void plVulkanPlateManager::Draw()
{
    // To be implemented - render plates
}