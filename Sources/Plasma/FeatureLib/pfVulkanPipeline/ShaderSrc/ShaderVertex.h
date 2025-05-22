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

#ifndef SHADERVERTEX_H
#define SHADERVERTEX_H

#include "ShaderTypes.h"

// Define vertex data layouts for Vulkan shaders

// Standard vertex layout
typedef struct {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
    vec4 color;
} StandardVertex;

// Skinned vertex layout
typedef struct {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
    vec4 color;
    vec4 weights;
    vec4 indices;
} SkinnedVertex;

// Simple vertex for 2D drawing
typedef struct {
    vec2 position;
    vec2 texCoord;
    vec4 color;
} SimpleVertex;

// Vertex binding descriptions for Vulkan
#ifdef __cplusplus
#include <vulkan/vulkan.h>

// Standard vertex binding
static VkVertexInputBindingDescription standardVertexBindingDescription = {
    0,                              // binding
    sizeof(StandardVertex),         // stride
    VK_VERTEX_INPUT_RATE_VERTEX     // inputRate
};

// Standard vertex attribute descriptions
static VkVertexInputAttributeDescription standardVertexAttributeDescriptions[] = {
    {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(StandardVertex, position)}, // Position
    {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(StandardVertex, normal)},   // Normal
    {2, 0, VK_FORMAT_R32G32_SFLOAT,    offsetof(StandardVertex, texCoord)}, // TexCoord
    {3, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(StandardVertex, color)}  // Color
};

// Simple vertex binding
static VkVertexInputBindingDescription simpleVertexBindingDescription = {
    0,                              // binding
    sizeof(SimpleVertex),           // stride
    VK_VERTEX_INPUT_RATE_VERTEX     // inputRate
};

// Simple vertex attribute descriptions
static VkVertexInputAttributeDescription simpleVertexAttributeDescriptions[] = {
    {0, 0, VK_FORMAT_R32G32_SFLOAT,       offsetof(SimpleVertex, position)}, // Position
    {1, 0, VK_FORMAT_R32G32_SFLOAT,       offsetof(SimpleVertex, texCoord)}, // TexCoord
    {2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(SimpleVertex, color)}     // Color
};

#endif // __cplusplus

#endif // SHADERVERTEX_H