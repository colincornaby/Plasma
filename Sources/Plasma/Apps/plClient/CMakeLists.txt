set(external_SCRIPTS
    external/create_resource_dat.py
    external/makeres.py
    external/render_svg.py
    external/scalergba.py
)

set(external_SOURCES
    external/Cursor_Base.svg
    external/Linking_Book.svg
    external/Loading_Text_rasterfont.svg
    external/Voice_Chat.svg
)
find_program(
    PNGCRUSH_EXECUTABLE pngcrush
    DOC "Path to pngcrush"
)

# Can we generate the resource.dat file?
python_test_modules(
    MODULES cairosvg PIL
    REQUIREMENTS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/external/requirements.txt"
)

cmake_dependent_option(PLASMA_BUILD_RESOURCE_DAT "Do we want to build resource.dat?" ON "cairosvg_FOUND AND PIL_FOUND" OFF)
cmake_dependent_option(RESOURCE_OPTIMIZE "Optimize the images in resource.dat" ON "PLASMA_BUILD_RESOURCE_DAT AND PNGCRUSH_EXECUTABLE" OFF)
cmake_dependent_option(RESOURCE_BRUTE "Allow pngcrush brute-force optimization" OFF "PLASMA_BUILD_RESOURCE_DAT AND PNGCRUSH_EXECUTABLE" OFF)

if(PLASMA_BUILD_RESOURCE_DAT)
    set(external_DAT "${CMAKE_CURRENT_BINARY_DIR}/resource.dat")
    if(RESOURCE_OPTIMIZE)
        string(APPEND OPTIMIZE_ARGUMENT "--pngcrush \"${PNGCRUSH_EXECUTABLE}\" ")
    endif()
    if(RESOURCE_BRUTE)
        string(APPEND OPTIMIZE_ARGUMENT "--brute ")
    endif()

    add_custom_command(
        OUTPUT ${external_DAT}
        COMMAND "${Python3_EXECUTABLE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/external/makeres.py"
                --render --package
                ${OPTIMIZE_ARGUMENT}
                -i "${CMAKE_CURRENT_SOURCE_DIR}/external"
                -w "${CMAKE_CURRENT_BINARY_DIR}"
                -o "${CMAKE_CURRENT_BINARY_DIR}"
        DEPENDS ${external_SCRIPTS} ${external_SOURCES}
    )

    install(
        FILES "${external_DAT}"
        DESTINATION client
    )
endif()

set(plClient_HEADERS
    plClient.h
    plClientCreatable.h
    plClientLoader.h
    plClientUpdateFormat.h
)

set(plClient_SOURCES
    pfAllCreatables.cpp
    plAllCreatables.cpp
    plClient.cpp
    plClientLoader.cpp
    pnAllCreatables.cpp
)

set(plClient_RESOURCES
)

set(plClient_TEXT
    ${Plasma_SOURCE_DIR}/Docs/ReleaseNotes/ReleaseNotes.txt
)

if(WIN32)
    list(APPEND plClient_SOURCES
        win32/plClient_Win.cpp
        win32/winmain.cpp
    )
    list(APPEND plClient_RESOURCES
        win32/res/plClient.exe.manifest
        win32/res/plClient.rc
        win32/res/resource.h
        win32/res/banner.bmp
        win32/res/cnsl1.bin
        win32/res/Dirt.ICO
        win32/res/headspin.ico
    )
elseif(APPLE)
    include_directories("../../PubUtilLib/plPipeline/GL/OSX")
    set(plClient_SOURCES ${plClient_SOURCES}
        OSX/main.mm
        OSX/PLSKeyboardEventMonitor.mm
        OSX/PLSView.mm
        OSX/PLSLoginWindowController.mm
        OSX/PLSPatcherWindowController.mm
        OSX/PLSPatcher.mm
        OSX/PLSServerStatus.mm
    )
    set(plClient_HEADERS ${plClient_HEADERS}
        OSX/PLSKeyboardEventMonitor.h
        OSX/PLSView.h
        OSX/PLSLoginWindowController.h
        OSX/PLSPatcherWindowController.h
        OSX/PLSPatcher.h
        OSX/PLSServerStatus.h
    )
    set(RESOURCES 
        OSX/MainMenu.xib
        OSX/PLSLoginWindowController.xib
        OSX/PLSPatcherWindowController.xib
        OSX/banner@2x.jpeg
        win32/res/Dirt.ICO
    )
    #shaders need to be compiled as part of the app
    #this could change in the future, but for now the Metal code expects the library to be compiled in the app
    set(plClient_SHADERS
	../../FeatureLib/pfMetalPipeline/ShaderSrc/FixedPipelineShaders.metal
	../../FeatureLib/pfMetalPipeline/ShaderSrc/PlateShaders.metal
	../../FeatureLib/pfMetalPipeline/ShaderSrc/BiasNormals.metal
	../../FeatureLib/pfMetalPipeline/ShaderSrc/CompCosines.metal
	../../FeatureLib/pfMetalPipeline/ShaderSrc/WaveSet7.metal
	../../FeatureLib/pfMetalPipeline/ShaderSrc/Grass.metal
	../../FeatureLib/pfMetalPipeline/ShaderSrc/WaveDecEnv.metal
    ../../FeatureLib/pfMetalPipeline/ShaderSrc/Avatar.metal
    ../../FeatureLib/pfMetalPipeline/ShaderSrc/WaveDec1Lay_7.metal
    ../../FeatureLib/pfMetalPipeline/ShaderSrc/WaveRip.metal
    ../../FeatureLib/pfMetalPipeline/ShaderSrc/Clear.metal
    ../../FeatureLib/pfMetalPipeline/ShaderSrc/GammaCorrection.metal
    ../../FeatureLib/pfMetalPipeline/ShaderSrc/TextFontShader.metal
    )
    set_source_files_properties(${plClient_SHADERS} PROPERTIES LANGUAGE METAL)
    source_group("Metal Shaders" FILES ${plClient_SHADERS})
    set(plClient_SOURCES ${plClient_SOURCES} ${plClient_SHADERS})
    include_directories("../../FeatureLib/pfMetalPipeline/metal-cpp"
                        "../../FeatureLib/pfMetalPipeline/ShaderSrc")
    list(APPEND plClient_RESOURCES
        win32/res/resource.h
        win32/res/banner.bmp
        win32/res/Dirt.ICO
        win32/res/headspin.ico
    )
else()
    list(APPEND plClient_SOURCES
        main.cpp
    )
endif()

if(APPLE)
    add_executable(plClient MACOSX_BUNDLE ${plClient_SOURCES} ${plClient_HEADERS} ${RESOURCES})
    set_target_properties(plClient PROPERTIES XCODE_ATTRIBUTE_MTL_FAST_MATH "YES")
    set(MAIN_NIBFILE "MainMenu")           
    set_target_properties(plClient PROPERTIES
        BUNDLE True
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/OSX/Info.plist.in"
        MACOSX_BUNDLE_GUI_IDENTIFIER org.openuru.plClient
        MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
        XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon;
    )
    set_property (TARGET plClient APPEND_STRING PROPERTY COMPILE_FLAGS "-fobjc-arc")
    target_sources(plClient PRIVATE OSX/Assets.xcassets)
    set_source_files_properties(OSX/Assets.xcassets ${RESOURCES} PROPERTIES
         MACOSX_PACKAGE_LOCATION Resources
    )
    target_link_libraries(plClient PRIVATE "-framework Cocoa")
    target_link_libraries(plClient PRIVATE "-framework QuartzCore")
    target_link_libraries(plClient PRIVATE "-framework MetalPerformanceShaders")
    install(
        TARGETS plClient
        DESTINATION client
    )
    set (searchDirs
        "${CMAKE_PREFIX_PATH}/lib"
        "${CMAKE_PREFIX_PATH}/usr/local/lib"
        "${CMAKE_PREFIX_PATH}/mac64/physx/bin/mac64.xcode/release"
    )
else()
    plasma_executable(plClient WIN32 CLIENT INSTALL_PDB
        SOURCES
            ${plClient_SOURCES} ${plClient_HEADERS}
            ${plClient_TEXT} ${plClient_RESOURCES}
    )
endif()

if(PLASMA_BUILD_RESOURCE_DAT)
    target_sources(plClient PRIVATE ${external_SCRIPTS} ${external_SOURCES} ${external_DAT})
endif()

target_link_libraries(
    plClient
    PRIVATE
        CoreLib

        # For the "all creatables"
        pnNucleusInc
        plPubUtilInc
        pfFeatureInc

        # Everything else used in this target.
        pnDispatch
        pnFactory
        pnKeyedObject
        pnMessage
        pnSceneObject
        plAgeLoader
        plAnimation
        plAudio
        plAvatar
        plClientResMgr
        plDrawable
        plFile
        plGImage
        plGLight
        plInputCore
        plMessage
        plModifier
        plNetClient
        plNetCommon
        plNetGameLib
        plPhysX
        plPipeline
        plProgressMgr
        plResMgr
        plScene
        plSDL
        plStatGather
        plStatusLog
        plUnifiedTime
        pfAnimation
        pfAudio
        pfCharacter
        pfConsole
        pfConsoleCore
        $<$<PLATFORM_ID:Windows>:pfCrashHandler>
        pfGameGUIMgr
        pfJournalBook
        pfLocalizationMgr
        pfMoviePlayer
        pfPasswordStore
        pfPatcher
        pfPython
        $<$<BOOL:${PLASMA_PIPELINE_DX}>:pfDXPipeline>
        $<$<BOOL:${PLASMA_PIPELINE_GL}>:pfGLPipeline>
        $<$<BOOL:${PLASMA_PIPELINE_METAL}>:pfMetalPipeline>
        CURL::libcurl
)
target_include_directories(plClient PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

if(PLASMA_EXTERNAL_RELEASE)
    set_target_properties(plClient PROPERTIES OUTPUT_NAME "UruExplorer")
endif(PLASMA_EXTERNAL_RELEASE)

if(APPLE)
    get_target_property(OUTPUT_NAME plClient OUTPUT_NAME)
    if(OUTPUT_NAME MATCHES "OUTPUT_NAME-NOTFOUND")
        get_target_property(OUTPUT_NAME plClient NAME)
    endif()
    install(
        CODE "include(\"BundleUtilities\")
            fixup_bundle(\"${CMAKE_INSTALL_PREFIX}/client/${OUTPUT_NAME}.app\" \"\" \"${searchDirs}\" IGNORE_ITEM Python)
            execute_process(COMMAND \"codesign\" \"--remove-signature\" \"--deep\" \"-f\" \"${CMAKE_INSTALL_PREFIX}/client/${OUTPUT_NAME}.app\")
            "
        DESTINATION client
    )
endif()

if(PLASMA_BUILD_RESOURCE_DAT)
    source_group("Client Resource Scripts" FILES ${external_SCRIPTS})
    source_group("Client Resource Images" FILES ${external_SOURCES})
endif()
source_group("Source Files" FILES ${plClient_SOURCES})
source_group("Header Files" FILES ${plClient_HEADERS})
source_group("Text" FILES ${plClient_TEXT})
source_group("Windows Resource Files" FILES ${plClient_RESOURCES})
