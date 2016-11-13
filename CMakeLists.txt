cmake_minimum_required(VERSION 2.8.4)
project(flash_display)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(TARGET_NAME fdisplay)
set(OFFSCREEN_TARGET_NAME ${TARGET_NAME}_offscreen)

set(${PROJECT_NAME}_TARGET_NAME ${TARGET_NAME}
        CACHE INTERNAL "${PROJECT_NAME}: Lib Binary Name" FORCE)
set(${PROJECT_NAME}_OFFSCREEN_TARGET_NAME ${OFFSCREEN_TARGET_NAME}
        CACHE INTERNAL "${PROJECT_NAME}: Lib Binary Name for offscreen render tests" FORCE)

set(DEPENDENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)

include_directories(/System/Library/Frameworks)
find_library(COCOA_LIBRARY Cocoa)
find_library(COREVIDEO_LIBRARY CoreVideo)
find_library(OpenGL_LIBRARY OpenGL)
mark_as_advanced(COCOA_LIBRARY GLUT_LIBRARY OpenGL_LIBRARY)
set(EXTRA_LIBS ${COCOA_LIBRARY} ${COREVIDEO_LIBRARY} ${GLUT_LIBRARY} ${OpenGL_LIBRARY})

include(jpeg)
include(png)

set(MATH_LIB flash_math)
add_subdirectory(${DEPENDENCIES_DIR}/${MATH_LIB})
set(MATH_LIB_TARGET_NAME ${${MATH_LIB}_TARGET_NAME})

set(${PROJECT_NAME}_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${${MATH_LIB}_INCLUDE_DIRS}
        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

include_directories(${${PROJECT_NAME}_INCLUDE_DIRS})

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.mm
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.m)

add_library(${TARGET_NAME} ${SOURCES})
set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
link_jpeg(${TARGET_NAME})
target_link_libraries(${TARGET_NAME} ${MATH_LIB_TARGET_NAME} ${EXTRA_LIBS})

add_library(${OFFSCREEN_TARGET_NAME} ${SOURCES})
set_target_properties(${OFFSCREEN_TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_compile_definitions(${OFFSCREEN_TARGET_NAME} PRIVATE OFFSCREEN=1)
link_jpeg(${OFFSCREEN_TARGET_NAME} ${EXTRA_LIBS})
link_png(${OFFSCREEN_TARGET_NAME} ${EXTRA_LIBS})
target_link_libraries(${OFFSCREEN_TARGET_NAME} ${MATH_LIB_TARGET_NAME} ${EXTRA_LIBS})
