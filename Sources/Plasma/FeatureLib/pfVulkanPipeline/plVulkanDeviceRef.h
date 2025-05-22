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

#ifndef _plVulkanDeviceRef_h
#define _plVulkanDeviceRef_h

#include "plPipeline/hsGDeviceRef.h"
#include "plVulkanDevice.h"

class plCubicEnvironmap;
class plMipmap;
class plGBufferGroup;

////////////////////////////////////////////////////////////////////////////////
// Vulkan Vertex Buffer

class plVulkanVertexBufferRef : public hsGDeviceRef
{
protected:
    plGBufferGroup* fOwner;
    uint32_t fIndex;
    uint32_t fCount;
    uint32_t fLockOwner;

    plVulkanDevice::VertexBufferRef fRef;

public:
    plVulkanVertexBufferRef(plVulkanDevice* dev);
    virtual ~plVulkanVertexBufferRef();

    CLASSNAME_REGISTER(plVulkanVertexBufferRef);
    GETINTERFACE_ANY(plVulkanVertexBufferRef, hsGDeviceRef);

    // hsGDeviceRef overrides
    bool Volatile() override { return false; }

    // Get/set functions
    plVulkanDevice::VertexBufferRef* GetRef() { return &fRef; }

    void SetOwner(plGBufferGroup* o) { fOwner = o; }
    void SetIndex(uint32_t i) { fIndex = i; }
    void SetCount(uint32_t c) { fCount = c; }

    plGBufferGroup* GetOwner() const { return fOwner; }
    uint32_t GetIndex() const { return fIndex; }
    uint32_t GetCount() const { return fCount; }

    // Lock/unlock
    void Lock() { fLockOwner = hsThread::ThisThreadHash(); }
    void Unlock() { fLockOwner = 0; }
    bool IsLocked() const { return fLockOwner != 0; }
};

////////////////////////////////////////////////////////////////////////////////
// Vulkan Index Buffer

class plVulkanIndexBufferRef : public hsGDeviceRef
{
protected:
    plGBufferGroup* fOwner;
    uint32_t fIndex;
    uint32_t fCount;
    uint32_t fLockOwner;

    plVulkanDevice::IndexBufferRef fRef;

public:
    plVulkanIndexBufferRef(plVulkanDevice* dev);
    virtual ~plVulkanIndexBufferRef();

    CLASSNAME_REGISTER(plVulkanIndexBufferRef);
    GETINTERFACE_ANY(plVulkanIndexBufferRef, hsGDeviceRef);

    // hsGDeviceRef overrides
    bool Volatile() override { return false; }

    // Get/set functions
    plVulkanDevice::IndexBufferRef* GetRef() { return &fRef; }

    void SetOwner(plGBufferGroup* o) { fOwner = o; }
    void SetIndex(uint32_t i) { fIndex = i; }
    void SetCount(uint32_t c) { fCount = c; }

    plGBufferGroup* GetOwner() const { return fOwner; }
    uint32_t GetIndex() const { return fIndex; }
    uint32_t GetCount() const { return fCount; }

    // Lock/unlock
    void Lock() { fLockOwner = hsThread::ThisThreadHash(); }
    void Unlock() { fLockOwner = 0; }
    bool IsLocked() const { return fLockOwner != 0; }
};

////////////////////////////////////////////////////////////////////////////////
// Vulkan Texture Reference

class plVulkanTextureRef : public hsGDeviceRef
{
protected:
    plVulkanDevice::TextureRef fRef;
    bool fIsCompressed;
    uint8_t fCompressionType;
    uint32_t fNumLevels;
    
public:
    plVulkanTextureRef(plVulkanDevice* dev);
    virtual ~plVulkanTextureRef();

    CLASSNAME_REGISTER(plVulkanTextureRef);
    GETINTERFACE_ANY(plVulkanTextureRef, hsGDeviceRef);

    // hsGDeviceRef overrides
    bool Volatile() override { return false; }

    // Get/set functions
    plVulkanDevice::TextureRef* GetRef() { return &fRef; }
    
    void SetIsCompressed(bool c) { fIsCompressed = c; }
    void SetCompressionType(uint8_t t) { fCompressionType = t; }
    void SetNumLevels(uint32_t l) { fNumLevels = l; }
    
    bool GetIsCompressed() const { return fIsCompressed; }
    uint8_t GetCompressionType() const { return fCompressionType; }
    uint32_t GetNumLevels() const { return fNumLevels; }

    // Create functions
    void Create(plMipmap* texture);
    void Create(plCubicEnvironmap* texture);
};

#endif // _plVulkanDeviceRef_h