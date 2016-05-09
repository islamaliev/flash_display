find_package(JPEG REQUIRED)

include_directories(${JPEG_INCLUDE_DIR})

function(link_jpeg target)
    target_link_libraries(${target} ${JPEG_LIBRARIES})
endfunction()