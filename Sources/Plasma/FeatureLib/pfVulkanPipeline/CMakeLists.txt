set(pfVulkanPipeline_SOURCES
    plVulkanDevice.cpp
    plVulkanDeviceRefs.cpp
    plVulkanEnumerate.cpp
    plVulkanMaterialShaderRef.cpp
    plVulkanPipeline.cpp
    plVulkanPipelineState.cpp
    plVulkanPlateManager.cpp
    plVulkanShader.cpp
    plVulkanFragmentShader.cpp
    plVulkanVertexShader.cpp
    plVulkanTextFont.cpp
)

set(pfVulkanPipeline_HEADERS
    plVulkanDevice.h
    plVulkanDeviceRef.h
    plVulkanMaterialShaderRef.h
    plVulkanPipeline.h
    plVulkanPipelineState.h
    plVulkanPlateManager.h
    plVulkanShader.h
    plVulkanTextFont.h
    plVulkanFragmentShader.h
    plVulkanVertexShader.h
    ShaderSrc/ShaderTypes.h
    ShaderSrc/ShaderVertex.h
    pfVulkanPipelineCreatable.h
)

set(pfVulkanPipeline_SHADERS
    ShaderSrc/FixedPipelineShaders.glsl
    ShaderSrc/PlateShaders.glsl
    ShaderSrc/BiasNormals.glsl
    ShaderSrc/CompCosines.glsl
    ShaderSrc/WaveSet7.glsl
    ShaderSrc/Grass.glsl
    ShaderSrc/WaveDecEnv.glsl
    ShaderSrc/Avatar.glsl
    ShaderSrc/WaveDec1Lay_7.glsl
    ShaderSrc/WaveRip.glsl
    ShaderSrc/Clear.glsl
    ShaderSrc/GammaCorrection.glsl
    ShaderSrc/TextFontShader.glsl
)

plasma_library(pfVulkanPipeline
    FOLDER FeatureLib
    SOURCES ${pfVulkanPipeline_SOURCES} ${pfVulkanPipeline_HEADERS}
)

target_link_libraries(pfVulkanPipeline
    PUBLIC
        CoreLib
        pnNucleusInc
        plPipeline
        Vulkan::Vulkan
    PRIVATE
        plStatusLog
    INTERFACE
        pnFactory
)

target_include_directories(pfVulkanPipeline PUBLIC "ShaderSrc")

source_group("Source Files" FILES ${pfVulkanPipeline_SOURCES})
source_group("Header Files" FILES ${pfVulkanPipeline_HEADERS})