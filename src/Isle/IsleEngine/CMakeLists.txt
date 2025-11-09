file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_library(IsleEngine SHARED ${ENGINE_SRC} ${IMGUI_SOURCES})

target_compile_definitions(IsleEngine PRIVATE ISLEENGINE_EXPORTS)

target_include_directories(IsleEngine PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${THIRDPARTY_DIR}/Includes/imgui"
    "${THIRDPARTY_DIR}/Includes/imgui/backends"
    ${THIRDPARTY_INCLUDES}
)

target_link_libraries(IsleEngine PRIVATE ${THIRD_PARTY_LIBS})

set_target_properties(IsleEngine PROPERTIES
    OUTPUT_NAME "$<IF:$<CONFIG:Debug>,IsleEngine_Debug,IsleEngine>"
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)