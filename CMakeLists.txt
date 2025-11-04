cmake_minimum_required(VERSION 3.16)
project(Isle LANGUAGES C CXX)

set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_SHARED_LINKER_FLAGS "")
set(CMAKE_MODULE_LINKER_FLAGS "")
set(CMAKE_CXX_FLAGS "")

if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/arch:SSE2 /W4 /permissive- /MP)
    add_link_options(/arch:SSE2 /ignore:4099)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<IF:$<CONFIG:Debug,RelWithDebInfo>,EditAndContinue,ProgramDatabase>")
else()
    add_compile_options(-msse -msse2 -mavx -Wall -Wextra -Wpedantic -fcolor-diagnostics -Wno-unused-parameter)
    add_link_options(-msse -msse2 -mavx)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

foreach(flag_var
    CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
    CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
    if(${flag_var})
        string(REPLACE "-nostartfiles" "" ${flag_var} "${${flag_var}}")
        string(REPLACE "-nostdlib" "" ${flag_var} "${${flag_var}}")
    endif()
endforeach()

set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/src/ThirdParty")

file(GLOB_RECURSE ALL_THIRDPARTY_DIRS LIST_DIRECTORIES true "${THIRDPARTY_DIR}/*")
set(THIRDPARTY_INCLUDES "")
foreach(dir ${ALL_THIRDPARTY_DIRS})
    if(IS_DIRECTORY ${dir})
        list(APPEND THIRDPARTY_INCLUDES ${dir})
    endif()
endforeach()
include_directories(${THIRDPARTY_INCLUDES})

set(THIRDPARTY_LIBS_DIR "${THIRDPARTY_DIR}/Libs/Windows")

file(GLOB_RECURSE THIRD_PARTY_LIB_FILES "${THIRDPARTY_LIBS_DIR}/*.lib")

set(THIRD_PARTY_LIBS "")
foreach(lib ${THIRD_PARTY_LIB_FILES})
    string(TOLOWER "${lib}" lib_lower)
    if(lib_lower MATCHES "glew32s.lib")
        list(APPEND THIRD_PARTY_LIBS "${lib}")
    elseif(lib_lower MATCHES "glew32.lib" OR lib_lower MATCHES "glew32.dll.lib")
    else()
        list(APPEND THIRD_PARTY_LIBS "${lib}")
    endif()
endforeach()

list(APPEND THIRD_PARTY_LIBS opengl32.lib kernel32.lib user32.lib gdi32.lib shell32.lib ole32.lib)

file(GLOB IMGUI_SOURCES
    "${THIRDPARTY_DIR}/Includes/imgui/*.cpp"
    "${THIRDPARTY_DIR}/Includes/imgui/backends/imgui_impl_glfw.cpp"
    "${THIRDPARTY_DIR}/Includes/imgui/backends/imgui_impl_opengl3.cpp"
)

function(configure_isle_target target_name)
    file(GLOB_RECURSE SRC CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/${target_name}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/${target_name}/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/${target_name}/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/${target_name}/*.hpp"
    )
    list(FILTER SRC EXCLUDE REGEX ".*/bin/.*")
    list(FILTER SRC EXCLUDE REGEX ".*/build/.*")
    list(FILTER SRC EXCLUDE REGEX ".*/toolchain/.*")
    list(FILTER SRC EXCLUDE REGEX ".*/CMakeFiles/.*")
    add_library(${target_name} STATIC ${SRC})
    target_compile_definitions(${target_name} PRIVATE "ISLE_MODULE_${target_name}" GLEW_STATIC IMGUI_DEFINE_MATH_OPERATORS)
    target_include_directories(${target_name} PUBLIC
        "${CMAKE_SOURCE_DIR}/src/Isle/${target_name}"
        "${CMAKE_SOURCE_DIR}/src/Isle/IsleEngine"
        "${CMAKE_SOURCE_DIR}/src/Isle/IsleGame"
        "${CMAKE_SOURCE_DIR}/src/Isle/IsleEditor"
        ${THIRDPARTY_INCLUDES}
    )
    target_link_libraries(${target_name} PRIVATE ${THIRD_PARTY_LIBS})
    set_target_properties(${target_name} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
        OUTPUT_NAME "$<IF:$<CONFIG:Debug>,${target_name}_Debug,${target_name}>"
    )
endfunction()

configure_isle_target(IsleEngine)
target_sources(IsleEngine PRIVATE ${IMGUI_SOURCES})
target_include_directories(IsleEngine PUBLIC
    "${THIRDPARTY_DIR}/Includes/imgui"
    "${THIRDPARTY_DIR}/Includes/imgui/backends"
)

file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleGame/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleGame/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleGame/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleGame/*.hpp"
)
list(FILTER GAME_SRC EXCLUDE REGEX ".*/(bin|build|toolchain|CMakeFiles)/.*")

add_library(IsleGame SHARED ${GAME_SRC})
target_compile_definitions(IsleGame PRIVATE ISLEGAME_EXPORTS GLEW_STATIC IMGUI_DEFINE_MATH_OPERATORS)
target_include_directories(IsleGame PUBLIC
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleGame"
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleEngine"
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleEditor"
    ${THIRDPARTY_INCLUDES}
)
target_link_libraries(IsleGame PRIVATE IsleEngine ${THIRD_PARTY_LIBS})
set_target_properties(IsleGame PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
    OUTPUT_NAME "$<IF:$<CONFIG:Debug>,IsleGame_Debug,IsleGame>"
)

file(GLOB_RECURSE EDITOR_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleEditor/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleEditor/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleEditor/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleEditor/*.hpp"
)
list(FILTER EDITOR_SRC EXCLUDE REGEX ".*/(bin|build|toolchain|CMakeFiles)/.*")

add_executable(IsleEditor ${EDITOR_SRC})
target_compile_definitions(IsleEditor PRIVATE GLEW_STATIC IMGUI_DEFINE_MATH_OPERATORS)
target_include_directories(IsleEditor PUBLIC
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleEditor"
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleEngine"
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleGame"
    ${THIRDPARTY_INCLUDES}
)
target_link_libraries(IsleEditor PRIVATE
    debug "${CMAKE_SOURCE_DIR}/build/IsleEngine_Debug.lib"
    optimized "${CMAKE_SOURCE_DIR}/build/IsleEngine.lib"
    debug "${CMAKE_SOURCE_DIR}/build/IsleGame_Debug.lib"
    optimized "${CMAKE_SOURCE_DIR}/build/IsleGame.lib"
    ${THIRD_PARTY_LIBS}
)
set_target_properties(IsleEditor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
    OUTPUT_NAME "$<IF:$<CONFIG:Debug>,IsleEditor_Debug,IsleEditor>"
)