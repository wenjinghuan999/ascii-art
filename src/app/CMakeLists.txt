add_executable(ascii-art ascii-art.cpp)
target_link_libraries(ascii-art 
    PUBLIC font
    PUBLIC argparse
    PUBLIC utf8cpp)
add_dependencies(ascii-art copy-resources)