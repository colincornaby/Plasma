set(pfMetalPipeline_SOURCES
    plMetalDevice.cpp
    plMetalDeviceRefs.cpp
    plMetalMaterialShaderRef.cpp
    plMetalPipeline.cpp
    plMetalPipelineState.cpp
    plMetalPlateManager.cpp
    plMetalShader.cpp
    plMetalFragmentShader.cpp
    plMetalVertexShader.cpp
)

set(pfMetalPipeline_HEADERS
    plMetalDevice.h
    plMetalDeviceRef.h
    plMetalMaterialShaderRef.h
    plMetalPipeline.h
    plMetalPipelineState.h
    plMetalPlateManager.h
    plMetalShader.h
    plMetalFragmentShader.h
    plMetalVertexShader.h
    ShaderSrc/ShaderTypes.h
    ShaderSrc/ShaderVertex.h
    pfMetalPipelineCreatable.h
)

plasma_library(pfMetalPipeline SOURCES ${pfMetalPipeline_SOURCES} ${pfMetalPipeline_HEADERS})
target_link_libraries(pfMetalPipeline
    PUBLIC
        CoreLib
        pnNucleusInc
        plPipeline
        "-framework Metal"
    PRIVATE
        plStatusLog
    INTERFACE
        pnFactory
)
target_include_directories(pfMetalPipeline PRIVATE "metal-cpp" "ShaderSrc")

source_group("Source Files" FILES ${pfMetalPipeline_SOURCES})
source_group("Header Files" FILES ${pfMetalPipeline_HEADERS})
