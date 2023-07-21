cmake_minimum_required(VERSION 3.6)

# we want a makefile
# set(CMAKE_GENERATOR "MinGW Makefiles" CACHE INTERNAL "" FORCE)
# set(CMAKE_VERBOSE_MAKEFILE ON)

# #########################
# General Setup
# #########################
if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER "clang++")
endif()

if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER "clang")
endif()

if(NOT CMAKE_AR)
    set(CMAKE_AR "llvm-ar")
endif()

project(nimbus)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_PARALLEL_LEVEL 32)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)


# #########################
# Paths
# #########################
set(SRC_PATH "${CMAKE_CURRENT_LIST_DIR}/src")
set(INCLUDE_PATH "${CMAKE_CURRENT_LIST_DIR}/include")
set(VENDOR_PATH "${CMAKE_CURRENT_LIST_DIR}/vendor")

# #########################
# Libraries
# #########################
include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)

set(FETCHCONTENT_BASE_DIR ${VENDOR_PATH} CACHE PATH "Missing description." FORCE)

# #########################
# SDL2
# #########################
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.28.1
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

# configure it
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SDL2)

# #########################
# Assimp
# #########################
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.2.5
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

# configure it
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)

# zlib really doesn't like c2x so correct for that
target_compile_options(zlibstatic PRIVATE -Wno-deprecated-non-prototype
    -Wno-deprecated-declarations
    -D_CRT_SECURE_NO_WARNINGS)
target_compile_options(assimp PRIVATE -Wno-microsoft-enum-value)


# #########################
# Tracy (profiling)
# #########################
FetchContent_Declare(
    tracy
    GIT_REPOSITORY https://github.com/wolfpld/tracy
    GIT_TAG v0.9
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(tracy)
target_compile_options(TracyClient PRIVATE -Wno-format)


# #########################
# GLM (header only)
# #########################
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 0.9.9.8
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(glm)


# #########################
# Glad
# #########################
file(GLOB GLAD_LIB_SRC "${VENDOR_PATH}/glad/*.cpp")
add_library(glad STATIC ${GLAD_LIB_SRC})

# glad lib include paths
target_include_directories(glad PRIVATE "${VENDOR_PATH}")
target_include_directories(glad PRIVATE "${VENDOR_PATH}/glad")



# #########################
# STB image
# #########################
file(GLOB STB_IMAGE_LIB_SRC "${VENDOR_PATH}/stb_image/*.cpp")
add_library(stb_image STATIC ${STB_IMAGE_LIB_SRC})


# #########################
# Imgui
# #########################
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.89.7-docking
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(imgui)

file(GLOB IMGUI_LIB_SRC "${imgui_SOURCE_DIR}/*.cpp")
list(APPEND IMGUI_LIB_SRC "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp")
list(APPEND IMGUI_LIB_SRC "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp")
add_library(imgui STATIC ${IMGUI_LIB_SRC})

# imgui include paths
target_include_directories(imgui PRIVATE "${imgui_SOURCE_DIR}")
target_include_directories(imgui PRIVATE "${sdl2_SOURCE_DIR}/include")


# #########################
# Self: Nimbus
# #########################
file(GLOB_RECURSE NIMBUS_LIB_SRC "${SRC_PATH}/*.cpp")
add_library(nimbus STATIC ${NIMBUS_LIB_SRC})

set(CMAKE_USE_PCH ON)
target_precompile_headers(nimbus PRIVATE "${INCLUDE_PATH}/nmpch.hpp")

target_link_libraries(nimbus PRIVATE assimp)
target_link_libraries(nimbus PRIVATE SDL2-static)
target_link_libraries(nimbus PRIVATE glad)
target_link_libraries(nimbus PRIVATE stb_image)
target_link_libraries(nimbus PRIVATE imgui)

target_link_libraries (nimbus PUBLIC TracyClient )


# #########################
# Add Include paths
# #########################

# self
target_include_directories(nimbus PUBLIC "${INCLUDE_PATH}")

# libraries build with fetchcontent
#assimp
target_include_directories(nimbus PUBLIC "${assimp_SOURCE_DIR}/include")
# this is needed because assimp builds a file callled config.h here
target_include_directories(nimbus PUBLIC "${assimp_BINARY_DIR}/include")

#sdl
target_include_directories(nimbus PUBLIC "${sdl2_SOURCE_DIR}/include")

#glm
target_include_directories(nimbus PUBLIC "${glm_SOURCE_DIR}/glm")

#imgui
target_include_directories(nimbus PUBLIC "${imgui_SOURCE_DIR}")

#tracy
target_include_directories(nimbus PUBLIC "${tracy_SOURCE_DIR}/public/tracy")




# vendors (not fetch content)
target_include_directories(nimbus PUBLIC "${VENDOR_PATH}")
target_include_directories(nimbus PUBLIC "${VENDOR_PATH}/glad")
target_include_directories(nimbus PUBLIC "${VENDOR_PATH}/KHR")
target_include_directories(nimbus PUBLIC "${VENDOR_PATH}/stb_image")


# #########################
# Self Compiler/Linker flags
# #########################
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(nimbus PRIVATE
        -Wall
        -Wextra
        -Wpedantic)

    # from assimp header
    target_compile_options(nimbus PRIVATE 
        -Wno-microsoft-enum-value 
        -Wno-deprecated-copy)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Add debug-specific flags
    target_compile_options(nimbus PRIVATE -g -gcodeview)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # clang lets us get an pdb file for debugging with remedybg/vs
        target_link_options(nimbus PRIVATE -g -gcodeview -Wl,/debug,/pdb:)
    endif()

elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Add release-specific flags
    target_compile_options(nimbus PRIVATE -O3 -march=native -DGLM_FORCE_INTRINSICS)
endif()

target_link_options(nimbus PRIVATE
    -fuse-ld=lld)