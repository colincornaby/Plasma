include(FetchContent)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

FetchContent_Declare(
    metalcpp
    GIT_REPOSITORY https://github.com/apple/metal-cpp.git
    #release/metal-cpp_macOS13.3_iOS16.4
    GIT_TAG        d72d46fccb230b53b28ff65fddbbe07ec693964b
)

FetchContent_MakeAvailable(metalcpp)

set(pfMetalPipeline_SOURCES
    plMetalArgumentBuffer.cpp
    plMetalDevice.cpp
    plMetalDeviceRefs.cpp
    plMetalEnumerate.mm
    plMetalMaterialShaderRef.cpp
    plMetalPipeline.cpp
    plMetalPipelineState.cpp
    plMetalPlateManager.cpp
    plMetalShader.cpp
    plMetalFragmentShader.cpp
    plMetalVertexShader.cpp
    plMetalTextFont.cpp
    plMetalDevicePerformanceShaders.mm
    plMetalDeviceLibraryLoader.mm
)

set(pfMetalPipeline_HEADERS
    plMetalArgumentBuffer.h
    plMetalDevice.h
    plMetalDeviceRef.h
    plMetalLightRef.h
    plMetalMaterialShaderRef.h
    plMetalPipeline.h
    plMetalPipelineState.h
    plMetalPlateManager.h
    plMetalShader.h
    plMetalTextFont.h
    plMetalFragmentShader.h
    plMetalVertexShader.h
    ShaderSrc/ShaderTypes.h
    ShaderSrc/ShaderVertex.h
    pfMetalPipelineCreatable.h
)

set(pfMetalPipeline_SHADERS
    ShaderSrc/FixedPipelineShaders.metal
    ShaderSrc/PlateShaders.metal
    ShaderSrc/BiasNormals.metal
    ShaderSrc/CompCosines.metal
    ShaderSrc/WaveSet7.metal
    ShaderSrc/Grass.metal
    ShaderSrc/WaveDecEnv.metal
    ShaderSrc/Avatar.metal
    ShaderSrc/WaveDec1Lay_7.metal
    ShaderSrc/WaveRip.metal
    ShaderSrc/Clear.metal
    ShaderSrc/GammaCorrection.metal
    ShaderSrc/TextFontShader.metal
    ShaderSrc/Materials.metal
    ShaderSrc/MetalShaderCommon.h
)

plasma_library(pfMetalPipeline
    FOLDER FeatureLib
    SOURCES ${pfMetalPipeline_SOURCES} ${pfMetalPipeline_HEADERS}
)

target_link_libraries(pfMetalPipeline
    PUBLIC
        CoreLib
        pnNucleusInc
        plPipeline
        Metal::Metal
        "-framework MetalPerformanceShaders"
    PRIVATE
        plStatusLog
    INTERFACE
        pnFactory
)
add_dependencies(
    pfMetalPipeline
    pfMetalPipelineShadersMSL21T1
    pfMetalPipelineShadersMSL21T2
    pfMetalPipelineShadersMSL23T1
    pfMetalPipelineShadersMSL23T2
)

target_include_directories(pfMetalPipeline PUBLIC ${metalcpp_SOURCE_DIR})
target_include_directories(pfMetalPipeline PUBLIC "ShaderSrc")

source_group("Source Files" FILES ${pfMetalPipeline_SOURCES})
source_group("Header Files" FILES ${pfMetalPipeline_HEADERS})

# All supported Macs for Plasma/Metal support MSL 2.1
add_metal_shader_library(pfMetalPipelineShadersMSL21T1
    STANDARD macos-metal2.1
    ${pfMetalPipeline_SHADERS}
)
target_include_directories(pfMetalPipelineShadersMSL21T1
    PRIVATE
    $<TARGET_PROPERTY:pnNucleusInc,INTERFACE_INCLUDE_DIRECTORIES>
)
set_target_properties(pfMetalPipelineShadersMSL21T1 PROPERTIES
    XCODE_ATTRIBUTE_MTL_PREPROCESSOR_DEFINITIONS
        "SHADER_TIER=1"
)
set_target_properties(pfMetalPipelineShadersMSL21T1 PROPERTIES
    FOLDER FeatureLib
)

add_metal_shader_library(pfMetalPipelineShadersMSL21T2
    STANDARD macos-metal2.1
    ${pfMetalPipeline_SHADERS}
)
target_include_directories(pfMetalPipelineShadersMSL21T2
    PRIVATE
    $<TARGET_PROPERTY:pnNucleusInc,INTERFACE_INCLUDE_DIRECTORIES>
)
set_target_properties(pfMetalPipelineShadersMSL21T2 PROPERTIES
    XCODE_ATTRIBUTE_MTL_PREPROCESSOR_DEFINITIONS
        "SHADER_TIER=2"
)
set_target_properties(pfMetalPipelineShadersMSL21T2 PROPERTIES
    FOLDER FeatureLib
)

# Build a 2.3 varient for macOS 11 and/or Apple Silicon specific features
add_metal_shader_library(pfMetalPipelineShadersMSL23T1
    STANDARD macos-metal2.3
    ${pfMetalPipeline_SHADERS}
)
target_include_directories(pfMetalPipelineShadersMSL23T1
    PRIVATE
    $<TARGET_PROPERTY:pnNucleusInc,INTERFACE_INCLUDE_DIRECTORIES>
)
set_target_properties(pfMetalPipelineShadersMSL23T1 PROPERTIES
    XCODE_ATTRIBUTE_MTL_PREPROCESSOR_DEFINITIONS
        "SHADER_TIER=1"
)
set_target_properties(pfMetalPipelineShadersMSL23T1 PROPERTIES
    FOLDER FeatureLib
)

add_metal_shader_library(pfMetalPipelineShadersMSL23T2
    STANDARD macos-metal2.3
    ${pfMetalPipeline_SHADERS}
)
target_include_directories(pfMetalPipelineShadersMSL23T2
    PRIVATE
    $<TARGET_PROPERTY:pnNucleusInc,INTERFACE_INCLUDE_DIRECTORIES>
)
set_target_properties(pfMetalPipelineShadersMSL23T2 PROPERTIES
    XCODE_ATTRIBUTE_MTL_PREPROCESSOR_DEFINITIONS
        "SHADER_TIER=2"
)
set_target_properties(pfMetalPipelineShadersMSL23T2 PROPERTIES
    FOLDER FeatureLib
)

if(NOT DEFINED CMAKE_XCODE_VERSION_MAJOR OR
    CMAKE_XCODE_VERSION_MAJOR VERSION_GREATER_EQUAL 13)

    add_metal_shader_library(pfMetalPipelineShadersMSL30
        STANDARD metal3.0
        ${pfMetalPipeline_SHADERS}
    )
    target_include_directories(pfMetalPipelineShadersMSL30
        PRIVATE
        $<TARGET_PROPERTY:pnNucleusInc,INTERFACE_INCLUDE_DIRECTORIES>
    )
    set_target_properties(pfMetalPipelineShadersMSL30 PROPERTIES
        FOLDER FeatureLib
    )

set_target_properties(pfMetalPipelineShadersMSL30 PROPERTIES
    XCODE_ATTRIBUTE_MTL_PREPROCESSOR_DEFINITIONS
        "SHADER_TIER=2"
)

    target_compile_options(pfMetalPipelineShadersMSL30 PRIVATE
        "-D SHADER_TIER=2"
    )

    target_compile_definitions(pfMetalPipeline
        PRIVATE
        METAL_3_SDK
    )

    add_dependencies(
        pfMetalPipeline
        pfMetalPipelineShadersMSL30
    )
endif()
