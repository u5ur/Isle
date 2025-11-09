cmake_minimum_required(VERSION 3.16)
project(Isle LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive- /MP)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS GLEW_STATIC IMGUI_DEFINE_MATH_OPERATORS)
else()
    add_compile_options(-msse -msse2 -mavx -Wall -Wextra -Wpedantic -fcolor-diagnostics -Wno-unused-parameter)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS GLEW_STATIC IMGUI_DEFINE_MATH_OPERATORS)
endif()

set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/src/ThirdParty")

file(GLOB_RECURSE ALL_THIRDPARTY_DIRS LIST_DIRECTORIES true "${THIRDPARTY_DIR}/*")
set(THIRDPARTY_INCLUDES "")
foreach(dir ${ALL_THIRDPARTY_DIRS})
    if(IS_DIRECTORY ${dir})
        list(APPEND THIRDPARTY_INCLUDES ${dir})
    endif()
endforeach()

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

if(MSVC)
    list(APPEND THIRD_PARTY_LIBS opengl32.lib kernel32.lib user32.lib gdi32.lib shell32.lib ole32.lib)
else()
    list(APPEND THIRD_PARTY_LIBS opengl32.lib kernel32.lib user32.lib gdi32.lib shell32.lib ole32.lib)
    list(APPEND THIRD_PARTY_LIBS -lucrtd -lvcruntimed)
endif()

file(GLOB IMGUI_SOURCES
    "${THIRDPARTY_DIR}/Includes/imgui/*.cpp"
    "${THIRDPARTY_DIR}/Includes/imgui/backends/imgui_impl_glfw.cpp"
    "${THIRDPARTY_DIR}/Includes/imgui/backends/imgui_impl_opengl3.cpp"
)

# ============= IsleEngine =============
file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleEngine/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleEngine/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleEngine/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleEngine/*.hpp"
)
list(FILTER ENGINE_SRC EXCLUDE REGEX ".*/(bin|build|toolchain|CMakeFiles)/.*")

add_library(IsleEngine SHARED ${ENGINE_SRC} ${IMGUI_SOURCES})

target_compile_definitions(IsleEngine PRIVATE ISLEENGINE_EXPORTS)

target_include_directories(IsleEngine PUBLIC
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleEngine"
    "${THIRDPARTY_DIR}/Includes/imgui"
    "${THIRDPARTY_DIR}/Includes/imgui/backends"
    ${THIRDPARTY_INCLUDES}
)

target_link_libraries(IsleEngine PRIVATE ${THIRD_PARTY_LIBS})

set_target_properties(IsleEngine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
    OUTPUT_NAME "$<IF:$<CONFIG:Debug>,IsleEngine_Debug,IsleEngine>"
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)

# ============= IsleGame (Hot-Reloadable) =============
file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleGame/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleGame/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleGame/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleGame/*.hpp"
)
list(FILTER GAME_SRC EXCLUDE REGEX ".*/(bin|build|toolchain|CMakeFiles)/.*")

add_library(IsleGame SHARED ${GAME_SRC})

target_compile_definitions(IsleGame PRIVATE ISLEGAME_EXPORTS)

target_include_directories(IsleGame PUBLIC
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleGame"
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleEngine"
    ${THIRDPARTY_INCLUDES}
)

target_link_libraries(IsleGame PRIVATE IsleEngine ${THIRD_PARTY_LIBS})

# Output to build_tmp during compilation, custom command will copy to build/
set_target_properties(IsleGame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build_tmp"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build_tmp"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build_tmp"
    OUTPUT_NAME "$<IF:$<CONFIG:Debug>,IsleGame_Debug,IsleGame>"
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    PDB_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build_tmp"
)

# Disable incremental linking and use minimal debug info to avoid PDB locks
if(MSVC)
    target_link_options(IsleGame PRIVATE
        /INCREMENTAL:NO
        $<$<CONFIG:Debug>:/DEBUG:NONE>
    )
    target_compile_options(IsleGame PRIVATE
        $<$<CONFIG:Debug>:/Z7>
    )
endif()

# Copy DLL to build folder after successful build
add_custom_command(TARGET IsleGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/build"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:IsleGame>"
        "${CMAKE_SOURCE_DIR}/build/$<TARGET_FILE_NAME:IsleGame>"
    COMMENT "Copying IsleGame DLL to build folder"
)

# ============= IsleEditor =============
file(GLOB_RECURSE EDITOR_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleEditor/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleEditor/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleEditor/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleEditor/*.hpp"
)
list(FILTER EDITOR_SRC EXCLUDE REGEX ".*/(bin|build|toolchain|CMakeFiles)/.*")

add_executable(IsleEditor ${EDITOR_SRC})

target_include_directories(IsleEditor PUBLIC
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleEditor"
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleEngine"
    ${THIRDPARTY_INCLUDES}
)

target_link_libraries(IsleEditor PRIVATE IsleEngine ${THIRD_PARTY_LIBS})

set_target_properties(IsleEditor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
    OUTPUT_NAME "$<IF:$<CONFIG:Debug>,IsleEditor_Debug,IsleEditor>"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)