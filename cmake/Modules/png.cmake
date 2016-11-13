find_package(PNG REQUIRED)

include_directories(${PNG_INCLUDE_DIR})

function(link_png target)
    target_link_libraries(${target} ${PNG_LIBRARIES})
endfunction()