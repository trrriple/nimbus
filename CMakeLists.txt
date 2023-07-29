cmake_minimum_required(VERSION 3.6)

# #########################
# General Setup
# #########################
project(nimbus)

if(NOT CMAKE_CXX_COMPILER)
    if(CMAKE_SYSTEM_NAME MATCHES Windows)
        set(CMAKE_CXX_COMPILER "clang++")
    else()
        set(CMAKE_CXX_COMPILER "g++")
    endif()
endif()

if(NOT CMAKE_C_COMPILER)
    if(CMAKE_SYSTEM_NAME MATCHES Windows)
        set(CMAKE_C_COMPILER "clang")
    else()
        set(CMAKE_C_COMPILER "gcc")
    endif()
endif()

if(NOT CMAKE_AR)
    if(CMAKE_SYSTEM_NAME MATCHES Windows)
        set(CMAKE_AR "llvm-ar")
    else()
        set(CMAKE_AR "ar")
    endif()
endif()


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_PARALLEL_LEVEL 32)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)


# #########################
# Vendor Libraries
# #########################
set(VENDOR_PATH "${CMAKE_CURRENT_LIST_DIR}/vendor")

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
set(SDL_OPENGLES OFF CACHE BOOL "" FORCE)

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
if(TARGET zlibstatic)
    target_compile_options(zlibstatic PRIVATE -Wno-deprecated-non-prototype
        -Wno-deprecated-declarations
        -D_CRT_SECURE_NO_WARNINGS)
endif()

   target_compile_options(assimp PRIVATE -Wno-microsoft-enum-value)


# #########################
# MSDF atlas gen
# #########################
FetchContent_Declare(
    msdf-atlas-gen
    GIT_REPOSITORY https://github.com/Chlumsky/msdf-atlas-gen.git

    GIT_TAG b1af88cfcaa8a02dc5c4e7de593b6af70bb73ae8
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)


set(MSDF_ATLAS_USE_SKIA OFF CACHE BOOL "" FORCE)
set(MSDF_ATLAS_NO_ARTERY_FONT ON CACHE BOOL "" FORCE)
set(MSDF_ATLAS_BUILD_STANDALONE OFF CACHE BOOL "" FORCE)
set(MSDF_ATLAS_USE_VCPKG ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(msdf-atlas-gen)

target_compile_options(msdf-atlas-gen PRIVATE -Wno-deprecated-declarations 
                        -D_CRT_SECURE_NO_WARNINGS)

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
# Self: Nimbus
# #########################
file(GLOB_RECURSE NIMBUS_LIB_SRC 
    "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")

add_library(nimbus STATIC ${NIMBUS_LIB_SRC})

set(CMAKE_USE_PCH ON)
target_precompile_headers(nimbus PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include/nimbus/nmpch.hpp")

target_link_libraries(nimbus PRIVATE SDL2-static)
target_link_libraries(nimbus PRIVATE assimp)
target_link_libraries(nimbus PRIVATE msdf-atlas-gen)
target_link_libraries(nimbus PRIVATE glad)
target_link_libraries(nimbus PRIVATE stb_image)
target_link_libraries(nimbus PRIVATE imgui)
target_link_libraries(nimbus PUBLIC TracyClient)


# #########################
# Add Include paths
# #########################

# self
target_include_directories(nimbus PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include")

#sdl
target_include_directories(nimbus PRIVATE "${sdl2_SOURCE_DIR}/include")

#assimp
target_include_directories(nimbus PRIVATE "${assimp_SOURCE_DIR}/include")
# this is needed because assimp builds a file callled config.h here
target_include_directories(nimbus PRIVATE "${assimp_BINARY_DIR}/include")

#msdf-atlas-gen
target_include_directories(nimbus PRIVATE "${msdf-atlas-gen}/include")


#glm
target_include_directories(nimbus PUBLIC "${glm_SOURCE_DIR}/glm")

#imgui
target_include_directories(nimbus PUBLIC "${imgui_SOURCE_DIR}")

#tracy
target_include_directories(nimbus PUBLIC "${tracy_SOURCE_DIR}/public/tracy")


# raw vendor path
target_include_directories(nimbus PRIVATE "${VENDOR_PATH}")

#glad 
target_include_directories(nimbus PRIVATE "${VENDOR_PATH}/glad")
target_include_directories(nimbus PRIVATE "${VENDOR_PATH}/KHR")

#stb_mage
target_include_directories(nimbus PRIVATE "${VENDOR_PATH}/stb_image")


# #########################
# Self Compiler/Linker flags
# #########################
if(NIMBUS_NO_CONSOLE)
        target_compile_options(nimbus PRIVATE -DNIMBUS_NO_CONSOLE)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(nimbus PRIVATE
        -Wall
        -Wextra)

    # from assimp header
    target_compile_options(nimbus PRIVATE 
        -Wno-microsoft-enum-value 
        -Wno-deprecated-copy)
    
    target_link_options(nimbus PRIVATE
        -fuse-ld=lld)
    endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Add debug-specific flags
    target_compile_options(nimbus PRIVATE -g)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # clang lets us get an pdb file for debugging with remedybg/vs
        target_compile_options(nimbus PRIVATE -gcodeview)
        target_link_options(nimbus PRIVATE -g -gcodeview -Wl,/debug,/pdb:)
    endif()

elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Add release-specific flags
    target_compile_options(nimbus PRIVATE -O3 -march=native -DGLM_FORCE_INTRINSICS)
endif()