cmake_minimum_required(VERSION 3.16)
project(Isle LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive- /MP)
else()
    add_compile_options(-msse -msse2 -mavx -Wall -Wextra -Wpedantic -fcolor-diagnostics -Wno-unused-parameter)
endif()

add_compile_definitions(_CRT_SECURE_NO_WARNINGS GLEW_STATIC IMGUI_DEFINE_MATH_OPERATORS)

set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/src/ThirdParty")

file(GLOB_RECURSE ALL_THIRDPARTY_ITEMS "${THIRDPARTY_DIR}/*")
set(THIRDPARTY_INCLUDES "")
foreach(item ${ALL_THIRDPARTY_ITEMS})
    get_filename_component(dir ${item} DIRECTORY)
    list(APPEND THIRDPARTY_INCLUDES ${dir})
endforeach()
list(REMOVE_DUPLICATES THIRDPARTY_INCLUDES)

file(GLOB_RECURSE THIRD_PARTY_LIBS "${THIRDPARTY_DIR}/Libs/Windows/*.lib")
list(FILTER THIRD_PARTY_LIBS EXCLUDE REGEX "glew32\\.lib|glew32\\.dll\\.lib")

list(APPEND THIRD_PARTY_LIBS opengl32 kernel32 user32 gdi32 shell32 ole32)
if(NOT MSVC)
    list(APPEND THIRD_PARTY_LIBS -lucrtd -lvcruntimed)
endif()

file(GLOB IMGUI_SOURCES
    "${THIRDPARTY_DIR}/Includes/imgui/*.cpp"
    "${THIRDPARTY_DIR}/Includes/imgui/backends/imgui_impl_glfw.cpp"
    "${THIRDPARTY_DIR}/Includes/imgui/backends/imgui_impl_opengl3.cpp"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

add_subdirectory(src/Isle/IsleEngine)
add_subdirectory(src/Isle/IsleGame)
add_subdirectory(src/Isle/IsleEditor)