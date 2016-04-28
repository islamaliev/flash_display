cmake_minimum_required(VERSION 2.8.4)
project(flash_display)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(TARGET_NAME fdisplay)

set(${PROJECT_NAME}_TARGET_NAME ${TARGET_NAME}
        CACHE INTERNAL "${PROJECT_NAME}: Lib Binary Name" FORCE)

set(DEPENDENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)
include(glfw)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_library(${TARGET_NAME} ${SOURCES})
set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
link_glfw(${TARGET_NAME})