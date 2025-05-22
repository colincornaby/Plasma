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

#include "plVulkanDeviceRef.h"
#include "plDrawable/plGBufferGroup.h"
#include "plGImage/plMipmap.h"
#include "plGImage/plCubicEnvironmap.h"
#include "hsResMgr.h"

/// Vertex Buffer Ref ////////////////////////////////////////////////////////

plVulkanVertexBufferRef::plVulkanVertexBufferRef(plVulkanDevice* dev)
    : hsGDeviceRef(dev),
      fOwner(),
      fIndex(),
      fCount(),
      fLockOwner()
{
    memset(&fRef, 0, sizeof(fRef));
}

plVulkanVertexBufferRef::~plVulkanVertexBufferRef()
{
    // Clean up Vulkan resources
    if (fRef.buffer != VK_NULL_HANDLE) {
        VkDevice device = reinterpret_cast<plVulkanDevice*>(fDevice)->GetDevice();
        vkDestroyBuffer(device, fRef.buffer, nullptr);
        vkFreeMemory(device, fRef.memory, nullptr);
    }
}

/// Index Buffer Ref /////////////////////////////////////////////////////////

plVulkanIndexBufferRef::plVulkanIndexBufferRef(plVulkanDevice* dev)
    : hsGDeviceRef(dev),
      fOwner(),
      fIndex(),
      fCount(),
      fLockOwner()
{
    memset(&fRef, 0, sizeof(fRef));
}

plVulkanIndexBufferRef::~plVulkanIndexBufferRef()
{
    // Clean up Vulkan resources
    if (fRef.buffer != VK_NULL_HANDLE) {
        VkDevice device = reinterpret_cast<plVulkanDevice*>(fDevice)->GetDevice();
        vkDestroyBuffer(device, fRef.buffer, nullptr);
        vkFreeMemory(device, fRef.memory, nullptr);
    }
}

/// Texture Ref //////////////////////////////////////////////////////////////

plVulkanTextureRef::plVulkanTextureRef(plVulkanDevice* dev)
    : hsGDeviceRef(dev),
      fIsCompressed(false),
      fCompressionType(0),
      fNumLevels(0)
{
    memset(&fRef, 0, sizeof(fRef));
}

plVulkanTextureRef::~plVulkanTextureRef()
{
    // Clean up Vulkan resources
    if (fRef.image != VK_NULL_HANDLE) {
        VkDevice device = reinterpret_cast<plVulkanDevice*>(fDevice)->GetDevice();
        if (fRef.sampler != VK_NULL_HANDLE)
            vkDestroySampler(device, fRef.sampler, nullptr);
        if (fRef.view != VK_NULL_HANDLE)
            vkDestroyImageView(device, fRef.view, nullptr);
        vkDestroyImage(device, fRef.image, nullptr);
        vkFreeMemory(device, fRef.memory, nullptr);
    }
}

void plVulkanTextureRef::Create(plMipmap* texture)
{
    plVulkanDevice* dev = reinterpret_cast<plVulkanDevice*>(fDevice);
    dev->MakeTextureRef(&fRef, texture);
}

void plVulkanTextureRef::Create(plCubicEnvironmap* texture)
{
    plVulkanDevice* dev = reinterpret_cast<plVulkanDevice*>(fDevice);
    dev->MakeCubicTextureRef(&fRef, texture);
}