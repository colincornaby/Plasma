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

#include "plVulkanDevice.h"
#include "hsThread.h"

#include "plDrawable/plGBufferGroup.h"
#include "plGImage/plMipmap.h"
#include "plGImage/plCubicEnvironmap.h"
#include "plPipeline/pl3DPipeline.h"
#include "plPipeline/plRenderTarget.h"
#include "plStatusLog/plStatusLog.h"

#include "hsStream.h"
#include "hsUtils.h"
#include "hsMatrix44.h"
#include "hsFastMath.h"
#include "hsResMgr.h"

#include <vector>
#include <algorithm>

static const char* VALIDATION_LAYERS[] = {
    "VK_LAYER_KHRONOS_validation"
};

static const char* REQUIRED_DEVICE_EXTENSIONS[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

bool plVulkanDevice::plVulkanPipelineRecord::operator==(const plVulkanPipelineRecord& p) const
{
    return depthFormat == p.depthFormat &&
           colorFormat == p.colorFormat &&
           sampleCount == p.sampleCount &&
           state->operator==(*p.state);
}

bool plVulkanDevice::plVulkanPipelineRecord::PipelineState::operator==(const plVulkanDevice::plVulkanPipelineRecord::PipelineState& p) const
{
    // Compare pipeline state elements
    return true; // Placeholder, to be implemented with actual state comparison
}

plVulkanDevice::plVulkanDevice()
    : fErrorMsg(),
      fActiveThread(hsThread::ThisThreadHash()),
      fVkInstance(VK_NULL_HANDLE),
      fPhysicalDevice(VK_NULL_HANDLE),
      fVkDevice(VK_NULL_HANDLE),
      fGraphicsQueue(VK_NULL_HANDLE),
      fCommandPool(VK_NULL_HANDLE),
      fCurrentCommandBuffer(VK_NULL_HANDLE),
      fCurrentRenderPass(VK_NULL_HANDLE),
      fCurrentFramebuffer(VK_NULL_HANDLE),
      fCurrentPipeline(VK_NULL_HANDLE),
      fCurrentPipelineLayout(VK_NULL_HANDLE),
      fCurrentDescriptorSet(VK_NULL_HANDLE),
      fSwapchain(VK_NULL_HANDLE),
      fSwapchainFormat(VK_FORMAT_UNDEFINED),
      fSwapchainImages(),
      fSwapchainImageViews(),
      fShouldClearRenderTarget(false),
      fShouldClearDrawable(false),
      fClearRenderTargetDepth(1.0f),
      fClearDrawableDepth(1.0f),
      fSupportsTileMemory(false),
      fPipelineStateCache()
{
    // Initialize samplers to nullptr
    for (int i = 0; i < 16; i++) {
        fSamplerStates[i] = VK_NULL_HANDLE;
    }
}

plVulkanDevice::~plVulkanDevice()
{
    Shutdown();
}

bool plVulkanDevice::InitDevice()
{
    try {
        CreateInstance();
        SelectPhysicalDevice();
        CreateLogicalDevice();
        CreateCommandPool();
        CreateSamplers();
        return true;
    }
    catch (const std::runtime_error& e) {
        fErrorMsg = e.what();
        return false;
    }
}

void plVulkanDevice::Shutdown()
{
    if (fVkDevice != VK_NULL_HANDLE) {
        // Wait for any pending operations
        vkDeviceWaitIdle(fVkDevice);
        
        // Cleanup swapchain
        CleanupSwapchain();
        
        // Cleanup samplers
        for (int i = 0; i < 16; i++) {
            if (fSamplerStates[i] != VK_NULL_HANDLE) {
                vkDestroySampler(fVkDevice, fSamplerStates[i], nullptr);
                fSamplerStates[i] = VK_NULL_HANDLE;
            }
        }
        
        // Destroy command pool
        if (fCommandPool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(fVkDevice, fCommandPool, nullptr);
            fCommandPool = VK_NULL_HANDLE;
        }
        
        // Destroy device
        vkDestroyDevice(fVkDevice, nullptr);
        fVkDevice = VK_NULL_HANDLE;
    }
    
    // Destroy instance
    if (fVkInstance != VK_NULL_HANDLE) {
        vkDestroyInstance(fVkInstance, nullptr);
        fVkInstance = VK_NULL_HANDLE;
    }
}

void plVulkanDevice::SetRenderTarget(plRenderTarget* target)
{
    // To be implemented
}

void plVulkanDevice::SetViewport()
{
    // To be implemented
}

bool plVulkanDevice::BeginRender()
{
    if (fActiveThread == hsThread::ThisThreadHash()) {
        return true;
    }

    fActiveThread = hsThread::ThisThreadHash();
    return true;
}

void plVulkanDevice::CreateInstance()
{
    // Application info
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Plasma";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Plasma";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    
    // Extension and validation layers
    std::vector<const char*> extensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif defined(__APPLE__)
        VK_MVK_MACOS_SURFACE_EXTENSION_NAME
#else // Linux
        VK_KHR_XCB_SURFACE_EXTENSION_NAME
#endif
    };
    
    std::vector<const char*> validationLayers;
    
#ifdef HS_DEBUGGING
    if (CheckValidationLayerSupport()) {
        validationLayers.push_back(VALIDATION_LAYERS[0]);
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
#endif
    
    // Create instance
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
    
    VkResult result = vkCreateInstance(&createInfo, nullptr, &fVkInstance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance");
    }
}

bool plVulkanDevice::CheckValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    
    for (const char* layerName : VALIDATION_LAYERS) {
        bool layerFound = false;
        
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        
        if (!layerFound) {
            return false;
        }
    }
    
    return true;
}

void plVulkanDevice::SelectPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(fVkInstance, &deviceCount, nullptr);
    
    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support");
    }
    
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(fVkInstance, &deviceCount, devices.data());
    
    // Find a suitable device (could be more sophisticated)
    for (const auto& device : devices) {
        // Check for required extensions and features
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
        
        bool allExtensionsSupported = true;
        for (const char* requiredExtension : REQUIRED_DEVICE_EXTENSIONS) {
            bool extensionFound = false;
            for (const auto& extension : availableExtensions) {
                if (strcmp(requiredExtension, extension.extensionName) == 0) {
                    extensionFound = true;
                    break;
                }
            }
            if (!extensionFound) {
                allExtensionsSupported = false;
                break;
            }
        }
        
        if (allExtensionsSupported) {
            // Check for graphics queue
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
            
            for (uint32_t i = 0; i < queueFamilyCount; i++) {
                if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    fPhysicalDevice = device;
                    break;
                }
            }
            
            if (fPhysicalDevice != VK_NULL_HANDLE) {
                break;
            }
        }
    }
    
    if (fPhysicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a suitable GPU");
    }
    
    // Check tile memory capabilities
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(fPhysicalDevice, &deviceFeatures);
    // Determine if device supports tile-based rendering optimizations
    // This varies by vendor, so this is just a placeholder
    fSupportsTileMemory = false;
}

void plVulkanDevice::CreateLogicalDevice()
{
    // Find graphics queue family
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(fPhysicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(fPhysicalDevice, &queueFamilyCount, queueFamilies.data());
    
    uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueFamilyIndex = i;
            break;
        }
    }
    
    if (graphicsQueueFamilyIndex == UINT32_MAX) {
        throw std::runtime_error("Failed to find graphics queue family");
    }
    
    // Create device queue
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    
    // Request device features
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.fillModeNonSolid = VK_TRUE;
    
    // Create device
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    
    std::vector<const char*> deviceExtensions(std::begin(REQUIRED_DEVICE_EXTENSIONS), std::end(REQUIRED_DEVICE_EXTENSIONS));
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    
    // Add validation layers if in debug mode
#ifdef HS_DEBUGGING
    std::vector<const char*> validationLayers;
    if (CheckValidationLayerSupport()) {
        validationLayers.push_back(VALIDATION_LAYERS[0]);
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
#endif
    
    VkResult result = vkCreateDevice(fPhysicalDevice, &createInfo, nullptr, &fVkDevice);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device");
    }
    
    // Get queue
    vkGetDeviceQueue(fVkDevice, graphicsQueueFamilyIndex, 0, &fGraphicsQueue);
}

void plVulkanDevice::CreateCommandPool()
{
    // Find graphics queue family
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(fPhysicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(fPhysicalDevice, &queueFamilyCount, queueFamilies.data());
    
    uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueFamilyIndex = i;
            break;
        }
    }
    
    // Create command pool
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    
    VkResult result = vkCreateCommandPool(fVkDevice, &poolInfo, nullptr, &fCommandPool);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool");
    }
}

void plVulkanDevice::CreateSamplers()
{
    // Create different samplers for texture filtering modes
    // This is just a basic example - we'll need to create different samplers based on texture properties
    
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    
    if (vkCreateSampler(fVkDevice, &samplerInfo, nullptr, &fSamplerStates[0]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture sampler");
    }
    
    // More samplers would be created for different filtering modes
}

void plVulkanDevice::CreateSwapchain()
{
    // To be implemented
}

void plVulkanDevice::CleanupSwapchain()
{
    // Destroy image views
    for (auto imageView : fSwapchainImageViews) {
        vkDestroyImageView(fVkDevice, imageView, nullptr);
    }
    
    // Destroy swapchain
    if (fSwapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(fVkDevice, fSwapchain, nullptr);
        fSwapchain = VK_NULL_HANDLE;
    }
}

void plVulkanDevice::CreateCommandBuffer()
{
    // Create command buffer
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = fCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    
    VkResult result = vkAllocateCommandBuffers(fVkDevice, &allocInfo, &fCurrentCommandBuffer);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffer");
    }
    
    // Begin command buffer
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    vkBeginCommandBuffer(fCurrentCommandBuffer, &beginInfo);
}

void plVulkanDevice::SubmitCommandBuffer()
{
    // End command buffer
    vkEndCommandBuffer(fCurrentCommandBuffer);
    
    // Submit command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &fCurrentCommandBuffer;
    
    vkQueueSubmit(fGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(fGraphicsQueue);
    
    // Reset commands
    vkFreeCommandBuffers(fVkDevice, fCommandPool, 1, &fCurrentCommandBuffer);
    fCurrentCommandBuffer = VK_NULL_HANDLE;
    
    // Reset clear flags
    fShouldClearRenderTarget = false;
    fShouldClearDrawable = false;
}

VkCommandBuffer plVulkanDevice::GetCurrentCommandBuffer() const
{
    return fCurrentCommandBuffer;
}

void plVulkanDevice::SetupVertexBufferRef(plGBufferGroup* owner, uint32_t idx, VertexBufferRef* vRef)
{
    // To be implemented
}

void plVulkanDevice::CheckStaticVertexBuffer(VertexBufferRef* vRef, plGBufferGroup* owner, uint32_t idx)
{
    // To be implemented
}

void plVulkanDevice::FillVertexBufferRef(VertexBufferRef* ref, plGBufferGroup* group, uint32_t idx)
{
    // To be implemented
}

void plVulkanDevice::FillVolatileVertexBufferRef(VertexBufferRef* ref, plGBufferGroup* group, uint32_t idx)
{
    // To be implemented
}

void plVulkanDevice::SetupIndexBufferRef(plGBufferGroup* owner, uint32_t idx, IndexBufferRef* iRef)
{
    // To be implemented
}

void plVulkanDevice::CheckStaticIndexBuffer(IndexBufferRef* iRef, plGBufferGroup* owner, uint32_t idx)
{
    // To be implemented
}

void plVulkanDevice::FillIndexBufferRef(IndexBufferRef* ref, plGBufferGroup* group, uint32_t idx)
{
    // To be implemented
}

uint32_t plVulkanDevice::ConfigureAllowedLevels(TextureRef* tRef, plMipmap* mipmap)
{
    // To be implemented
    return 0;
}

void plVulkanDevice::PopulateTexture(TextureRef* tRef, plMipmap* img, uint32_t slice)
{
    // To be implemented
}

void plVulkanDevice::MakeTextureRef(TextureRef* tRef, plMipmap* img)
{
    // To be implemented
}

void plVulkanDevice::MakeCubicTextureRef(TextureRef* tRef, plCubicEnvironmap* img)
{
    // To be implemented
}

void plVulkanDevice::SetProjectionMatrix(const hsMatrix44& src)
{
    // To be implemented
}

void plVulkanDevice::SetWorldToCameraMatrix(const hsMatrix44& src)
{
    // To be implemented
}

void plVulkanDevice::SetLocalToWorldMatrix(const hsMatrix44& src)
{
    // To be implemented
}

VkSampler plVulkanDevice::SamplerStateForClampFlags(uint32_t sampleState) const
{
    // Choose appropriate sampler based on clamp flags
    return fSamplerStates[sampleState & 0xF];
}