set (glfw_DIR ${DEPENDENCIES_DIR}/glfw)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Don't build example")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Don't build tests")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Don't build docs")
add_subdirectory(${glfw_DIR} ${CMAKE_BINARY_DIR}/glfw)
set_target_properties(glfw PROPERTIES COMPILE_FLAGS " -w")

find_package(OpenGL REQUIRED)
set(GL_LIBRARIES ${OPENGL_LIBRARIES})

find_package(GLEW REQUIRED)
list(APPEND GL_LIBRARIES ${GLEW_LIBRARIES})

include_directories(${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${glfw_DIR}/include)

function(link_glfw target)
    target_link_libraries(${target} glfw ${GL_LIBRARIES})
endfunction()