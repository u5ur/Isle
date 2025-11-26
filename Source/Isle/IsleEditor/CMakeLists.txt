file(GLOB_RECURSE EDITOR_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_executable(IsleEditor ${EDITOR_SRC})

target_include_directories(IsleEditor PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/Source/Isle/IsleEngine"
    ${THIRDPARTY_INCLUDES}
)

target_link_libraries(IsleEditor PRIVATE IsleEngine ${THIRD_PARTY_LIBS})

set_target_properties(IsleEditor PROPERTIES
    OUTPUT_NAME "$<IF:$<CONFIG:Debug>,IsleEditor_Debug,IsleEditor>"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)