file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_library(IsleGame SHARED ${GAME_SRC})

target_compile_definitions(IsleGame PRIVATE ISLEGAME_EXPORTS)

target_include_directories(IsleGame PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/src/Isle/IsleEngine"
    ${THIRDPARTY_INCLUDES}
)

target_link_libraries(IsleGame PRIVATE IsleEngine ${THIRD_PARTY_LIBS})

if(MSVC)
    target_link_options(IsleGame PRIVATE /INCREMENTAL:NO $<$<CONFIG:Debug>:/DEBUG:NONE>)
    target_compile_options(IsleGame PRIVATE $<$<CONFIG:Debug>:/Z7>)
endif()

set_target_properties(IsleGame PROPERTIES
    OUTPUT_NAME "$<IF:$<CONFIG:Debug>,IsleGame_Debug,IsleGame>"
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)