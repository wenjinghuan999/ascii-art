# copy all resource files to the build directory after build
add_custom_command(OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/font
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources/font ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/font
    COMMENT "Copying resources 'font' to build directory"
)
add_custom_command(OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/text
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources/text ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/text
    COMMENT "Copying resources 'text' to build directory"
)
add_custom_target(copy-resources DEPENDS
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/font
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/text)