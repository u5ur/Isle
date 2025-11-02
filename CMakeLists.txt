cmake_minimum_required(VERSION 3.16)
project(Isle LANGUAGES C CXX)

if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive- /MP)
    add_link_options(/ignore:4099) # suppress missing PDB warnings
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<IF:$<CONFIG:Debug,RelWithDebInfo>,EditAndContinue,ProgramDatabase>")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -fcolor-diagnostics -Wno-unused-parameter)
    if(WIN32)
        add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/src/ThirdParty")
set(THIRDPARTY_INCLUDES "${THIRDPARTY_DIR}/Includes")

if(WIN32)
    set(THIRDPARTY_LIBS_DIR "${THIRDPARTY_DIR}/Libs/Windows")
elseif(APPLE)
    set(THIRDPARTY_LIBS_DIR "${THIRDPARTY_DIR}/Libs/Mac")
elseif(UNIX)
    set(THIRDPARTY_LIBS_DIR "${THIRDPARTY_DIR}/Libs/Linux")
else()
    message(FATAL_ERROR "Unsupported platform for Isle build")
endif()

if(WIN32)
    set(GLEW_LIB "${THIRDPARTY_LIBS_DIR}/glew32s.lib")
    set(GLFW_DEBUG_LIB "${THIRDPARTY_LIBS_DIR}/glfw3_mt.lib")
    set(GLFW_RELEASE_LIB "${THIRDPARTY_LIBS_DIR}/glfw3.lib")

    set(THIRD_PARTY_LIBS
        ${GLEW_LIB}
        debug ${GLFW_DEBUG_LIB} optimized ${GLFW_RELEASE_LIB}
        opengl32.lib
    )

elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)

    set(THIRD_PARTY_LIBS
        "${THIRDPARTY_LIBS_DIR}/libglfw3.a"
        "${THIRDPARTY_LIBS_DIR}/libGLEW.a"
        ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY}
    )

elseif(UNIX)
    set(THIRD_PARTY_LIBS
        "${THIRDPARTY_LIBS_DIR}/libGLFW.a"
        "${THIRDPARTY_LIBS_DIR}/libGLEW.a"
        GL X11 pthread
    )
endif()

include_directories("${THIRDPARTY_INCLUDES}")

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
    target_compile_definitions(${target_name} PRIVATE "ISLE_MODULE_${target_name}" GLEW_STATIC)

    target_include_directories(${target_name} PUBLIC
        "${CMAKE_SOURCE_DIR}/src/Isle/${target_name}"
        "${CMAKE_SOURCE_DIR}/src/Isle/IsleEngine"
        "${CMAKE_SOURCE_DIR}/src/Isle/IsleGame"
        "${CMAKE_SOURCE_DIR}/src/Isle/IsleEditor"
        "${THIRDPARTY_INCLUDES}"
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

file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleGame/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleGame/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleGame/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Isle/IsleGame/*.hpp"
)
list(FILTER GAME_SRC EXCLUDE REGEX ".*/bin/.*")
list(FILTER GAME_SRC EXCLUDE REGEX ".*/build/.*")
list(FILTER GAME_SRC EXCLUDE REGEX ".*/toolchain/.*")
list(FILTER GAME_SRC EXCLUDE REGEX ".*/CMakeFiles/.*")

add_library(IsleGame SHARED ${GAME_SRC})
target_compile_definitions(IsleGame PRIVATE ISLEGAME_EXPORTS GLEW_STATIC)

target_include_directories(IsleGame PUBLIC
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleGame"
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleEngine"
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleEditor"
    "${THIRDPARTY_INCLUDES}"
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
list(FILTER EDITOR_SRC EXCLUDE REGEX ".*/bin/.*")
list(FILTER EDITOR_SRC EXCLUDE REGEX ".*/build/.*")
list(FILTER EDITOR_SRC EXCLUDE REGEX ".*/toolchain/.*")
list(FILTER EDITOR_SRC EXCLUDE REGEX ".*/CMakeFiles/.*")

add_executable(IsleEditor ${EDITOR_SRC})
target_compile_definitions(IsleEditor PRIVATE GLEW_STATIC)

target_include_directories(IsleEditor PUBLIC
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleEditor"
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleEngine"
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleGame"
    "${THIRDPARTY_INCLUDES}"
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
