add_executable(ascii-art ascii-art.cpp)
target_link_libraries(ascii-art 
    PUBLIC freetype)
add_dependencies(ascii-art copy-resources)