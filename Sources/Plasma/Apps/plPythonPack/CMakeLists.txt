include_directories(${PYTHON_INCLUDE_DIR})

set(plPythonPack_SOURCES
    main.cpp
    PythonInterface.cpp
)

set(plPythonPack_HEADERS
    PythonInterface.h
)

add_executable(plPythonPack ${plPythonPack_SOURCES} ${plPythonPack_HEADERS})
target_link_libraries(plPythonPack CoreLib plFile)
target_link_libraries(plPythonPack Python3::Python)
target_link_libraries(plPythonPack ${STRING_THEORY_LIBRARIES})

if(USE_VLD)
    target_link_libraries(plPythonPack ${VLD_LIBRARY})
endif()

install(
    TARGETS plPythonPack
    DESTINATION tools_cli
)

source_group("Source Files" FILES ${plPythonPack_SOURCES})
source_group("Header Files" FILES ${plPythonPack_HEADERS})
