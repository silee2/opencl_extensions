cmake_minimum_required(VERSION 3.20.0)

# CMP0116: Ninja generators transform `DEPFILE`s from `add_custom_command()`
# New in CMake 3.20. https://cmake.org/cmake/help/latest/policy/CMP0116.html
if(POLICY CMP0116)
    cmake_policy(SET CMP0116 OLD)
endif()

project(opencl-extensions LANGUAGES CXX C)

# Must go below project(..)
include(GNUInstallDirs)
include(FetchContent)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to conform to")
#list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")

# CMAKE_SYSTEM_NAME needs to be checked after "project"
if(${CMAKE_VERSION} VERSION_LESS 3.25)
    # CMake starting with version 3.25 sets var LINUX to true if target system is linux
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(LINUX TRUE)
    endif()
endif()

# Try FindOpenCL.cmake first
find_package(OpenCL)

if(NOT OpenCL_FOUND)
    # Intel does not distribute OpenCL SDK separately since 2020 and bundles it with DPC++ Compiler.
    # Header and Libraries are not standard location needs the custom searching.
    if(DEFINED ENV{CMPLR_ROOT})
        # Set SDK root path for Linux and Windows
        if(LINUX OR (${CMAKE_SYSTEM_NAME} MATCHES "Linux"))
            set(DEV_ROOT "$ENV{CMPLR_ROOT}/linux")
        elseif(WIN32)
            set(DEV_ROOT "$ENV{CMPLR_ROOT}/windows")
        endif()

        # Find custom path for OpenCL_INCLUDE_DIR and OpenCL_LIBRARY
        find_path(OpenCL_INCLUDE_DIR REQUIRED
            NAMES
                CL/cl.h OpenCL/cl.h
            PATHS
                ${DEV_ROOT}
            PATH_SUFFIXES
                include/sycl
            )

        find_library(OpenCL_LIBRARY REQUIRED
            NAMES OpenCL
            PATHS
                ${DEV_ROOT}
            PATH_SUFFIXES
                lib)

        # Clear cached variables for OpenCL version checking.
        foreach(VERSION "3_0" "2_2" "2_1" "2_0" "1_2" "1_1" "1_0")
            unset(OPENCL_VERSION_${VERSION} CACHE)
        endforeach()

        find_package(OpenCL)
    endif()
endif()

# If FindOpenCL.cmake failed. Use Khronos OpenCL headers and ICD loader
if(NOT OpenCL_FOUND)
    FetchContent_Declare(OpenCL-Headers
        GIT_REPOSITORY  "https://github.com/KhronosGroup/OpenCL-Headers.git"
        GIT_TAG         main
        GIT_SHALLOW     TRUE
    )
    FetchContent_MakeAvailable(OpenCL-Headers)
    FetchContent_GetProperties(OpenCL-Headers
        SOURCE_DIR OpenCL_INCLUDE_DIR
    )
    FetchContent_Declare(OpenCL-ICD-Loader
        GIT_REPOSITORY  "https://github.com/KhronosGroup/OpenCL-ICD-Loader.git"
        GIT_TAG         main
        GIT_SHALLOW     TRUE
    )
    FetchContent_MakeAvailable(OpenCL-ICD-Loader)
    set(OpenCL_LIBRARY ${PROJECT_BINARY_DIR}/lib/libOpenCL.so)

    # Emulate CMake variables defined by FindOpenCL.cmake
    set(OpenCL_FOUND TRUE)
    set(OpenCL_LIBRARIES ${OpenCL_LIBRARY})
    set(OpenCL_INCLUDE_DIRS ${OpenCL_INCLUDE_DIR})
    set(OpenCL_VERSION_STRING "3.0")
    set(OpenCL_VERSION_MAJOR "3")
    set(OpenCL_VERSION_MINOR "0")
endif()

message(STATUS "${OpenCL_FOUND}")
message(STATUS "${OpenCL_INCLUDE_DIRS}")
message(STATUS "${OpenCL_LIBRARIES}")
message(STATUS "${OpenCL_VERSION_STRING}")
message(STATUS "${OpenCL_VERSION_MAJOR}")
message(STATUS "${OpenCL_VERSION_MINOR}")

message(STATUS "${OpenCL_INCLUDE_DIR}")
message(STATUS "${OpenCL_LIBRARY}")

add_executable(opencl_extensions opencl_extensions.cpp)
target_link_libraries(opencl_extensions OpenCL::OpenCL)

