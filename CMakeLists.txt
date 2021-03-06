cmake_minimum_required(VERSION 3.14)
project(Plasma)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CMakeDependentOption)
include(FeatureSummary)

# Set up Product Identification parameters
set(PRODUCT_BRANCH_ID   "1"         CACHE STRING "Branch ID")
set(PRODUCT_BUILD_ID    "918"       CACHE STRING "Build ID")
set(PRODUCT_BUILD_TYPE  "50"        CACHE STRING "Build Type")
set(PRODUCT_CORE_NAME   "UruLive"   CACHE STRING "Product Core Name")
set(PRODUCT_SHORT_NAME  "UruLive"   CACHE STRING "Product Short Name")
set(PRODUCT_LONG_NAME   "Uru Live"  CACHE STRING "Product Long Name")
set(PRODUCT_UUID        "ea489821-6c35-4bd0-9dae-bb17c585e680"
                                    CACHE STRING "Product UUID")

# HeadSpin Configuration
if(WIN32 AND NOT CYGWIN)
    add_definitions(-DHS_BUILD_FOR_WIN32)
endif(WIN32 AND NOT CYGWIN)

if(UNIX)
    # This is set for both Linux and Mac builds
    add_definitions(-DHS_BUILD_FOR_UNIX)
endif(UNIX)

if(APPLE)
    add_definitions(-DHS_BUILD_FOR_APPLE)

    if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
        add_definitions(-DHS_BUILD_FOR_MACOS)
    endif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
endif(APPLE)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-DHS_BUILD_FOR_LINUX)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
# End HeadSpin Configuration

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Find all 3rd-party libraries that are required
find_package(string_theory 2.0 REQUIRED)
set_package_properties(string_theory PROPERTIES
    URL "http://github.com/zrax/string_theory"
    DESCRIPTION "Flexible C++11 string library and type-safe formatter"
    TYPE REQUIRED
)
include_directories(${STRING_THEORY_INCLUDE_DIRS})

find_package(OpenSSL REQUIRED)
set_package_properties(OpenSSL PROPERTIES
    URL "https://www.openssl.org/"
    DESCRIPTION "OpenSSL libraries for SSL/TLS"
    TYPE REQUIRED
)

find_package(Python3 3.8 REQUIRED Interpreter Development)
set_package_properties(Python3 PROPERTIES
    URL "http://www.python.org"
    DESCRIPTION "Python Scripting language C bindings"
    TYPE REQUIRED
)

find_package(EXPAT REQUIRED)
set_package_properties(EXPAT PROPERTIES
    URL "http://expat.sourceforge.net/"
    DESCRIPTION "Expat XML Parser for C"
    TYPE REQUIRED
)

find_package(ZLIB REQUIRED)
set_package_properties(ZLIB PROPERTIES
    URL "http://www.zlib.net"
    DESCRIPTION "Fast (de)compression library"
    TYPE REQUIRED
)

find_package(JPEG REQUIRED)
set_package_properties(JPEG PROPERTIES
    URL "http://libjpeg-turbo.virtualgl.org/"
    DESCRIPTION "JPEG encoding and decoding library.  libjpeg-turbo is recommended for better performance"
    TYPE REQUIRED
)

find_package(PNG REQUIRED)
set_package_properties(PNG PROPERTIES
    URL "http://www.libpng.org/pub/png/libpng.html"
    DESCRIPTION "Efficient lossless image library"
    TYPE REQUIRED
)

#TODO: Not required if we aren't building the client
find_package(Ogg REQUIRED)
set_package_properties(Ogg PROPERTIES
    URL "https://xiph.org/ogg/"
    DESCRIPTION "Ogg multimedia container library for various audio and video codecs"
    TYPE REQUIRED
    PURPOSE "Required for audio support in the client"
)

#TODO: Not required if we aren't building the client
find_package(Vorbis REQUIRED)
set_package_properties(Vorbis PROPERTIES
    URL "https://xiph.org/vorbis/"
    DESCRIPTION "Free and open audio codec for mid-to-high quality audio"
    TYPE REQUIRED
    PURPOSE "Required for audio support in the client"
)

find_package(Speex)
set_package_properties(Speex PROPERTIES
    URL "http://www.speex.org/"
    DESCRIPTION "Free and open audio codec specifically designed for speech"
    TYPE OPTIONAL
    PURPOSE "Required for legacy in-game voice chat in the client"
)

find_package(VPX)
set_package_properties(VPX PROPERTIES
    URL "http://www.webmproject.org/"
    DESCRIPTION "VP8 and VP9 video codec"
    TYPE OPTIONAL
    PURPOSE "Required for in-game video playback support"
)

find_package(Opus)
set_package_properties(Opus PROPERTIES
    URL "https://www.opus-codec.org/"
    DESCRIPTION "Versatile free and open audio codec"
    TYPE OPTIONAL
    PURPOSE "Required for in-game video playback and voice chat support"
)

find_package(CURL REQUIRED)
set_package_properties(CURL PROPERTIES
    URL "http://curl.haxx.se/libcurl/"
    DESCRIPTION "Multi-protocol file transfer library"
    TYPE REQUIRED
)

find_package(Freetype)
set_package_properties(Freetype PROPERTIES
    URL "http://www.freetype.org/"
    DESCRIPTION "Library for rendering fonts"
    TYPE OPTIONAL
    PURPOSE "Required in order to build the plFontConverter tool"
)

find_package(BZip2)
set_package_properties(BZip2 PROPERTIES
    URL "https://www.sourceware.org/bzip2/"
    DESCRIPTION "Data compression library"
    TYPE OPTIONAL
    PURPOSE "Required only if FreeType is built statically against bzip2"
)

if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBSECRET libsecret-1)
    if(LIBSECRET_FOUND)
        set(HAVE_LIBSECRET TRUE)
    endif(LIBSECRET_FOUND)
endif(UNIX AND NOT APPLE)

include(PrecompiledHeader)    #Precompiled Header helper macros

if(WIN32)
    # libCurl isn't smart enough to detect this for us, so we have to configure it ourselves
    option(CURL_IS_STATIC "Using the static version of libcurl?" ON)
    if(CURL_IS_STATIC)
        add_definitions(-DCURL_STATICLIB)
    endif(CURL_IS_STATIC)

    # Same story for expat
    option(EXPAT_IS_STATIC "Using the static version of libexpat" ON)
endif(WIN32)

option(PLASMA_EXTERNAL_RELEASE "Is this release intended for the general public?" OFF)
if(PLASMA_EXTERNAL_RELEASE)
    add_definitions(-DPLASMA_EXTERNAL_RELEASE)
endif(PLASMA_EXTERNAL_RELEASE)

if(WIN32)
    set(PLASMA_PIPELINE "DirectX"
                        CACHE STRING "Which graphics backend to use")
else(WIN32)
    set(PLASMA_PIPELINE "OpenGL"
                        CACHE STRING "Which graphics backend to use")
endif(WIN32)
set_property(CACHE PLASMA_PIPELINE PROPERTY STRINGS "DirectX" "OpenGL")

if(PLASMA_PIPELINE STREQUAL "DirectX")
    find_package(DirectX REQUIRED)
    set_package_properties(DirectX PROPERTIES
        DESCRIPTION "Framework for hardware-accelerated 3D graphics on Microsoft platforms"
        TYPE REQUIRED
    )

    add_definitions(-DPLASMA_PIPELINE_DX)
endif(PLASMA_PIPELINE STREQUAL "DirectX")

if(PLASMA_PIPELINE STREQUAL "OpenGL")
    find_package(OpenGL REQUIRED)
    add_definitions(-DPLASMA_PIPELINE_GL)
endif(PLASMA_PIPELINE STREQUAL "OpenGL")

#Allow us to disable certain parts of the build
option(PLASMA_BUILD_RESOURCE_DAT "Do we want to build resource.dat?" ON)
option(PLASMA_BUILD_LAUNCHER "Do we want to build plUruLauncher?" ON)
option(PLASMA_BUILD_TOOLS "Do we want to build the Plasma tools?" ON)
option(PLASMA_BUILD_TESTS "Do we want to build the unit tests?" OFF)

if(WIN32)
    find_package(3dsm)
    set_package_properties(3dsm PROPERTIES
        URL "http://www.autodesk.com/"
        DESCRIPTION "SDK for integrating with the 3DS Max modelling software"
        TYPE OPTIONAL
        PURPOSE "Required for building the Plasma plugins for 3DS Max"
    )

    option(3dsm_BUILD_PLUGIN "Do we want to build the 3ds Max plugin?" OFF)
    if (3dsm_BUILD_PLUGIN AND 3dsm_FOUND)
        add_definitions(-DBUILDING_MAXPLUGIN)
    endif()
else()
    set(3dsm_BUILD_PLUGIN OFF)
endif()

if(WIN32)
    find_package(VLD)
    set_package_properties(VLD PROPERTIES
        URL "https://vld.codeplex.com/"
        DESCRIPTION "Visual Leak Debugger for software compiled with Microsoft Visual C++"
        TYPE OPTIONAL
        PURPOSE "Useful for detecting memory leaks (MSVC only)"
    )

    option(USE_VLD "Build and link with Visual Leak Detector" OFF)
    if(USE_VLD AND VLD_FOUND)
        add_definitions(-DUSE_VLD)
        include_directories(${VLD_INCLUDE_DIR})
    endif()
endif()

# Configure compile-time compiler-specific flags and other config files
include(cmake/CompilerChecks.cmake)
include(cmake/Config.cmake)

configure_file("${PROJECT_SOURCE_DIR}/cmake/hsConfig.h.cmake" "${PROJECT_BINARY_DIR}/hsConfig.h")
include_directories(${PROJECT_BINARY_DIR})

if(PLASMA_BUILD_TOOLS)
    # Custom dummy target for compiling all tools
    add_custom_target(tools)
endif()

include(TestBigEndian)
test_big_endian(BIG_ENDIAN)
if(BIG_ENDIAN)
    add_definitions(-DBIG_ENDIAN)
else()
    add_definitions(-DLITTLE_ENDIAN)
endif()

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_SCL_SECURE_NO_DEPRECATE)
endif(MSVC)

#TODO: Make the OpenSSL includes less promiscuous so this isn't needed
include_directories(${OPENSSL_INCLUDE_DIR})

set(PLASMA_SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Plasma")
set(PLASMA_TOOLS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/Sources/Tools")

add_subdirectory(Scripts)
add_subdirectory(Sources/Plasma)
add_subdirectory(Sources/Tools)

if(PLASMA_BUILD_TESTS)
    enable_testing()
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
    add_subdirectory(Sources/Tests EXCLUDE_FROM_ALL)
endif(PLASMA_BUILD_TESTS)

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)
