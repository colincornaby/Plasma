set(pfPasswordStore_HEADERS
    pfPasswordStore.h
    pfPasswordStore_impl.h
)

set(pfPasswordStore_SOURCES
    pfPasswordStore.cpp
)

if(WIN32 AND NOT CYGWIN)
    set(pfPasswordStore_SOURCES ${pfPasswordStore_SOURCES}
        pfPasswordStore_Win.cpp
    )
endif(WIN32 AND NOT CYGWIN)

if(APPLE)
    set(pfPasswordStore_SOURCES ${pfPasswordStore_SOURCES}
        pfPasswordStore_Apple.cpp
    )
endif(APPLE)

add_library(pfPasswordStore STATIC ${pfPasswordStore_HEADERS} ${pfPasswordStore_SOURCES})
target_link_libraries(pfPasswordStore CoreLib plFile pnNetBase)

if(APPLE)
    find_library(SECURITY_LIBRARY Security)
    target_link_libraries(pfPasswordStore ${SECURITY_LIBRARY})
endif(APPLE)

if(HAVE_LIBSECRET)
    target_compile_definitions(pfPasswordStore PUBLIC HAVE_LIBSECRET)
    target_include_directories(pfPasswordStore PRIVATE ${LIBSECRET_INCLUDE_DIRS})
    target_link_libraries(pfPasswordStore ${LIBSECRET_LIBRARIES})
    target_sources(pfPasswordStore PRIVATE pfPasswordStore_Unix.cpp)
endif(HAVE_LIBSECRET)

source_group("Header Files" FILES ${pfPasswordStore_HEADERS})
source_group("Source Files" FILES ${pfPasswordStore_SOURCES})

