include_directories(${GTEST_INCLUDE_DIR})
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/Sources/Plasma/CoreLib)
include_directories(${CMAKE_SOURCE_DIR}/Sources/Plasma/NucleusLib)
include_directories(${CMAKE_SOURCE_DIR}/Sources/Plasma/FeatureLib)

set(pfPasswordStoreTest_SOURCES
    test_pfPasswordStore.cpp
)

add_executable(test_pfPasswordStore ${pfPasswordStoreTest_SOURCES})

target_link_libraries(test_pfPasswordStore gtest gtest_main)
target_link_libraries(test_pfPasswordStore CoreLib)
target_link_libraries(test_pfPasswordStore pnUUID)
target_link_libraries(test_pfPasswordStore pfPasswordStore)
target_link_libraries(test_pfPasswordStore ${STRING_THEORY_LIBRARIES})

add_test(NAME test_pfPasswordStore COMMAND test_pfPasswordStore)
add_dependencies(check test_pfPasswordStore)

