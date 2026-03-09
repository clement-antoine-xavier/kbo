cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(kbo)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(Vulkan REQUIRED)

file(GLOB SRC_FILES src/*.c)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

add_executable(${PROJECT_NAME} ${SRC_FILES})

if(WIN32)
    include(cmake/platform/windows.cmake)
elseif(APPLE)
    include(cmake/platform/macos.cmake)
elseif(UNIX)
    include(cmake/platform/linux.cmake)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

kbo_apply_platform_config(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})
