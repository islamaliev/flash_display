cmake_minimum_required(VERSION 2.8.4)
project(flash_display)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include
        CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(TARGET_NAME flash_display)
set(OFFSCREEN_TARGET_NAME ${TARGET_NAME}_offscreen)

include_directories(${INCLUDE_DIR})

set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SOURCES
    ${INCLUDE_DIR}/Color.h
    ${SOURCES_DIR}/ComponentContainer.cpp
    ${INCLUDE_DIR}/ComponentContainer.h
    ${SOURCES_DIR}/Context.cpp
    ${INCLUDE_DIR}/Context.h
    ${SOURCES_DIR}/DisplayObject.cpp
    ${INCLUDE_DIR}/DisplayObject.h
    ${SOURCES_DIR}/DisplayObjectContainer.cpp
    ${INCLUDE_DIR}/DisplayObjectContainer.h
    ${SOURCES_DIR}/FileLoader.cpp
    ${INCLUDE_DIR}/FileLoader.h
    ${SOURCES_DIR}/Image.cpp
    ${INCLUDE_DIR}/Image.h
    ${SOURCES_DIR}/Program.cpp
    ${INCLUDE_DIR}/Program.h
    ${INCLUDE_DIR}/Rectangle.h
    ${SOURCES_DIR}/RenderBufferOrganizer.cpp
    ${INCLUDE_DIR}/RenderBufferOrganizer.h
    ${INCLUDE_DIR}/RenderState.h
    ${INCLUDE_DIR}/Shape.h
    ${SOURCES_DIR}/StackAllocator.cpp
    ${INCLUDE_DIR}/StackAllocator.h
    ${SOURCES_DIR}/Stage.cpp
    ${INCLUDE_DIR}/Stage.h
    ${SOURCES_DIR}/Texture.cpp
    ${INCLUDE_DIR}/Texture.h
)

add_library(${TARGET_NAME} ${SOURCES})
set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${TARGET_NAME} ${CONAN_LIBS})

add_library(${OFFSCREEN_TARGET_NAME} ${SOURCES})
set_target_properties(${OFFSCREEN_TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_compile_definitions(${OFFSCREEN_TARGET_NAME} PRIVATE OFFSCREEN=1)
target_link_libraries(${OFFSCREEN_TARGET_NAME} ${CONAN_LIBS})
