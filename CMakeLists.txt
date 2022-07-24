cmake_minimum_required(VERSION 3.16)

project(fbx_sdk_wrapper)

set(CMAKE_CXX_STANDARD 17)

# add fbx include path at here
# in practice, copy all files from fbxsdk/include to fbxsdk/, and use linux command in wsl:
# find fbxsdk -type d
# to list all path of include directories 
set(FBX_SDK_INCLUDES
fbxsdk
fbxsdk/fbxsdk
fbxsdk/fbxsdk/core
fbxsdk/fbxsdk/core/arch
fbxsdk/fbxsdk/core/base
fbxsdk/fbxsdk/core/math
fbxsdk/fbxsdk/core/sync
fbxsdk/fbxsdk/fileio
fbxsdk/fbxsdk/fileio/collada
fbxsdk/fbxsdk/fileio/fbx
fbxsdk/fbxsdk/scene
fbxsdk/fbxsdk/scene/animation
fbxsdk/fbxsdk/scene/constraint
fbxsdk/fbxsdk/scene/geometry
fbxsdk/fbxsdk/scene/shading
fbxsdk/fbxsdk/utils
fbxsdk/libxml2
fbxsdk/libxml2/libxml
)

option(USE_DEBUG_FBX "Link with fbx sdk debug library" OFF)

if (USE_DEBUG_FBX)
    file(GLOB FBX_SDK_LIBS lib/debug/*.lib)
else()
    file(GLOB FBX_SDK_LIBS lib/release/*.lib)
endif()

file(GLOB WRAPPER_FILES src/*.cpp src/*.h)
add_library(${PROJECT_NAME} STATIC ${WRAPPER_FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_include_directories(${PROJECT_NAME} PUBLIC ${FBX_SDK_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${FBX_SDK_LIBS})

# test executable
add_executable(test test/main.cpp)
set_property(TARGET test PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_include_directories(test PUBLIC src)
target_link_libraries(test PUBLIC ${PROJECT_NAME})