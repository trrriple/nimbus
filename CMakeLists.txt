cmake_minimum_required(VERSION 3.8)

# #########################
# General Setup
# #########################
project(nimbus)

option(NIMBUS_BUILD_FELIX "Build the Felix Editor" ON)


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
# Entt (header only)
# #########################
FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.12.2
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)


FetchContent_MakeAvailable(entt)


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
    GIT_REPOSITORY https://github.com/trrriple/imgui.git
    GIT_TAG trrriple-mods
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
# ImGuizmo
# #########################
FetchContent_Declare(
    imguizmo
    GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
    GIT_TAG 822be7b44c37dbe98d328739ebe0d5a1ea87ecfc
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(imguizmo)

file(GLOB IMGUIZMO_LIB_SRC "${imguizmo_SOURCE_DIR}/*.cpp")
add_library(imguizmo STATIC ${IMGUIZMO_LIB_SRC})

target_include_directories(imguizmo PRIVATE "${imgui_SOURCE_DIR}")



# #########################
# Box2D
# #########################
FetchContent_Declare(box2d
    GIT_REPOSITORY https://github.com/erincatto/Box2D.git
    GIT_TAG v2.4.1
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE)


set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(box2d)


# #########################
# Toml++
# #########################
FetchContent_Declare(toml++
    GIT_REPOSITORY https://github.com/trrriple/tomlplusplus
    GIT_TAG master
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE)


FetchContent_MakeAvailable(toml++)

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
    "${CMAKE_CURRENT_LIST_DIR}/nimbus/*.cpp")

add_library(nimbus STATIC ${NIMBUS_LIB_SRC})

set(CMAKE_USE_PCH ON)
target_precompile_headers(nimbus PRIVATE "${CMAKE_CURRENT_LIST_DIR}/nimbus/include/nimbus/core/nmpch.hpp")

target_link_libraries(nimbus PRIVATE SDL2-static)
target_link_libraries(nimbus PRIVATE assimp)
target_link_libraries(nimbus PRIVATE msdf-atlas-gen)
target_link_libraries(nimbus PRIVATE glad)
target_link_libraries(nimbus PRIVATE stb_image)
target_link_libraries(nimbus PRIVATE imgui)
target_link_libraries(nimbus PRIVATE imguizmo)
target_link_libraries(nimbus PRIVATE box2d)
target_link_libraries(nimbus PUBLIC TracyClient)

# Mono
# TODO : Not just windows release
if(WIN32)
    target_link_libraries(nimbus PRIVATE "${VENDOR_PATH}/mono/win64/lib/Release/libmono-static-sgen.lib")
    target_link_libraries(nimbus PRIVATE ws2_32)
    target_link_libraries(nimbus PRIVATE Bcrypt.lib)
endif()

# #########################
# Add Include paths
# #########################

# self
target_include_directories(nimbus PRIVATE "${CMAKE_CURRENT_LIST_DIR}/nimbus/include")

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

#entt
target_include_directories(nimbus PUBLIC "${entt_SOURCE_DIR}/src/")


#imgui
target_include_directories(nimbus PUBLIC "${imgui_SOURCE_DIR}")
target_include_directories(nimbus PUBLIC "${imguizmo_SOURCE_DIR}")


#box2d
target_include_directories(nimbus PUBLIC "${box2d_SOURCE_DIR}/include")

#yamlcpp
target_include_directories(nimbus PUBLIC "${toml++_SOURCE_DIR}/include")

#tracy
target_include_directories(nimbus PUBLIC "${tracy_SOURCE_DIR}/public/tracy")


# raw vendor path
target_include_directories(nimbus PRIVATE "${VENDOR_PATH}")

#glad 
target_include_directories(nimbus PRIVATE "${VENDOR_PATH}/glad")
target_include_directories(nimbus PRIVATE "${VENDOR_PATH}/KHR")

#stb_mage
target_include_directories(nimbus PRIVATE "${VENDOR_PATH}/stb_image")

#symbol font header
target_include_directories(nimbus PUBLIC "${VENDOR_PATH}/IconFontCppHeaders")

#portable-file-dialogs
target_include_directories(nimbus PRIVATE "${VENDOR_PATH}/portable-file-dialogs")

#mono 
target_include_directories(nimbus PRIVATE "${VENDOR_PATH}/mono/include")


# #########################
# Self Compiler/Linker flags
# #########################
if(NIMBUS_NO_CONSOLE)
        target_compile_options(nimbus PRIVATE -DNIMBUS_NO_CONSOLE)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(nimbus PRIVATE
        -Wall
        -Wextra
        -fno-exceptions)

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
    target_compile_definitions(nimbus PRIVATE NIMBUS_GL_DEBUG)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # clang lets us get an pdb file for debugging with remedybg/vs
        target_compile_options(nimbus PRIVATE -gcodeview)
        target_link_options(nimbus PRIVATE -g -gcodeview -Wl,/debug,/pdb:)
    endif()

elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Add release-specific flags

    target_compile_options(nimbus PRIVATE -O3 -flto -march=native -DGLM_FORCE_INTRINSICS)
    target_link_options(nimbus PRIVATE -O3 -flto -march=native -DGLM_FORCE_INTRINSICS)
endif()


# #########################
# ScriptCore
# #########################
set(SCRIPTCORE_PRJ_PATH "${CMAKE_SOURCE_DIR}/resources/scriptCore/scriptCore.csproj")

# Custom target to build the C# project
add_custom_target(scriptCore
  COMMAND dotnet build ${SCRIPTCORE_PRJ_PATH} -c Release -o "${CMAKE_SOURCE_DIR}/resources/scriptCore/bin"
  COMMENT "Building C# project ${SCRIPTCORE_PRJ_PATH}"
)

# make nimbus dependent on scriptCore so it builds with nimbus
add_dependencies(nimbus scriptCore)

if(NIMBUS_BUILD_FELIX)
    # #########################
    # Felix (Editor)
    # #########################
    file(GLOB FELIX_SRC "felix/src/*.cpp")
    add_executable(felix WIN32 ${FELIX_SRC})
    set_target_properties(felix PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin"
    )

    target_link_libraries(felix PRIVATE nimbus)

    target_include_directories(felix PRIVATE "${CMAKE_CURRENT_LIST_DIR}/nimbus/include")
    target_include_directories(felix PRIVATE "${CMAKE_CURRENT_LIST_DIR}/felix/include")


    # #########################
    # Felix Compiler/Linker flags
    # #########################
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(felix PRIVATE
            -Wall
            -Wextra
            -fno-exceptions)

        target_link_options(felix PRIVATE
            -fuse-ld=lld)

        target_link_options(felix PRIVATE -Xlinker /subsystem:console)
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        # Add debug-specific flags
        target_compile_options(felix PRIVATE -g)

        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            # clang lets us get an pdb file for debugging with remedybg/vs
            target_compile_options(felix PRIVATE -gcodeview)
            target_link_options(felix PRIVATE -g -gcodeview -Wl,/debug,/pdb:)
        endif()

    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        # Add release-specific flags
        target_compile_options(felix PRIVATE -O3 -flto -march=native -DGLM_FORCE_INTRINSICS)
        target_link_options(felix PRIVATE -O3 -flto -march=native -DGLM_FORCE_INTRINSICS)

    endif()
endif()