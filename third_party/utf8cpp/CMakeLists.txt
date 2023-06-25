# utfcpp
FetchContent_Declare(
  utf8cpp
  GIT_REPOSITORY https://github.com/nemtrif/utfcpp
  GIT_TAG        v3.2.3
)
FetchContent_MakeAvailable(utf8cpp)
target_include_directories(utf8cpp INTERFACE ${utf8cpp_SOURCE_DIR}/source)