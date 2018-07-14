cmake_minimum_required(VERSION 2.8.4)
project(flash_display)

if (CMAKE_SOURCE_DIR STREQUAL CMAME_CURRENT_SOURCE_DIR)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
endif()

find_package(OpenGL REQUIRED)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SOURCES
    "${SOURCE_DIR}/Color.h"
    "${SOURCE_DIR}/ComponentContainer.cpp"
    "${SOURCE_DIR}/ComponentContainer.h"
    "${SOURCE_DIR}/Context.cpp"
    "${SOURCE_DIR}/Context.h"
    "${SOURCE_DIR}/DisplayObject.cpp"
    "${SOURCE_DIR}/DisplayObject.h"
    "${SOURCE_DIR}/DisplayObjectContainer.cpp"
    "${SOURCE_DIR}/DisplayObjectContainer.h"
    "${SOURCE_DIR}/FileLoader.cpp"
    "${SOURCE_DIR}/FileLoader.h"
    "${SOURCE_DIR}/Image.cpp"
    "${SOURCE_DIR}/Image.h"
    "${SOURCE_DIR}/Program.cpp"
    "${SOURCE_DIR}/Program.h"
    "${SOURCE_DIR}/Rectangle.h"
    "${SOURCE_DIR}/RenderBufferOrganizer.cpp"
    "${SOURCE_DIR}/RenderBufferOrganizer.h"
    "${SOURCE_DIR}/RenderState.h"
    "${SOURCE_DIR}/Shape.h"
    "${SOURCE_DIR}/StackAllocator.cpp"
    "${SOURCE_DIR}/StackAllocator.h"
    "${SOURCE_DIR}/Stage.cpp"
    "${SOURCE_DIR}/Stage.h"
    "${SOURCE_DIR}/Texture.cpp"
    "${SOURCE_DIR}/Texture.h")

set(CMAKE_DEBUG_POSTFIX d)

set(ALL_LIBS ${OPENGL_LIBRARIES} CONAN_PKG::flash_math CONAN_PKG::glew CONAN_PKG::glfw CONAN_PKG::libpng 
    CONAN_PKG::libjpeg)

add_library(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${PROJECT_NAME} ${ALL_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC ${SOURCE_DIR} ${OPENGL_INCLUDE_DIRS})

#set(OFFSCREEN_TARGET_NAME ${PROJECT_NAME}_offscreen)
#add_library(${OFFSCREEN_TARGET_NAME} ${SOURCES})
#set_target_properties(${OFFSCREEN_TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
#target_compile_definitions(${OFFSCREEN_TARGET_NAME} PRIVATE OFFSCREEN=1)
#target_link_libraries(${OFFSCREEN_TARGET_NAME} ${ALL_LIBS})
#target_include_directories(${OFFSCREEN_TARGET_NAME} PUBLIC ${SOURCE_DIR} ${OPENGL_INCLUDE_DIRS})

#add_executable(try main.cpp)
#target_link_libraries(try ${OFFSCREEN_TARGET_NAME} ${ALL_LIBS})
#target_link_libraries(try ${PROJECT_NAME} ${ALL_LIBS})

#set(DLL_FILES "")

#foreach(BIN_DIR ${CONAN_BIN_DIRS})
#file(GLOB files "${BIN_DIR}/*.dll")
#if (files)
#list(APPEND DLL_FILES ${files})
#endif(files)
#endforeach(BIN_DIR)

#foreach(DLL_FILE ${DLL_FILES})
#add_custom_command(TARGET try POST_BUILD
#COMMAND ${CMAKE_COMMAND} -E copy_if_different 
#${DLL_FILE} 
#$<TARGET_FILE_DIR:try>)
#endforeach(DLL_FILE)
