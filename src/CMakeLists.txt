cmake_minimum_required(VERSION 3.10)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()
if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

include(FetchContent)

FetchContent_Declare(
    minhook
    URL https://github.com/TsudaKageyu/minhook/archive/refs/tags/v1.3.3.zip
)
FetchContent_GetProperties(minhook) 
if(NOT minhook_POPULATED)
    FetchContent_Populate(minhook)
    set(MINHOOK_SOURCES
        ${minhook_SOURCE_DIR}/src/buffer.c
        ${minhook_SOURCE_DIR}/src/hook.c
        ${minhook_SOURCE_DIR}/src/trampoline.c
        ${minhook_SOURCE_DIR}/src/hde/hde32.c
        ${minhook_SOURCE_DIR}/src/hde/hde64.c
    )
endif()

FetchContent_Declare(
    imgui
    URL https://github.com/ocornut/imgui/archive/refs/heads/docking.zip
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
    
    set(IMGUI_SOURCES
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    )
endif()

set(COMMON_SOURCES 
    ../useless.hpp 
    hook.cpp 
    api.cpp 
    ${IMGUI_SOURCES} 
    ${MINHOOK_SOURCES}
)

set(COMMON_INCLUDES
    ${minhook_SOURCE_DIR}/include
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# DX9 Target
set(DX9_SOURCES ${COMMON_SOURCES} ${imgui_SOURCE_DIR}/backends/imgui_impl_dx9.cpp)
add_library(UselessImGUI SHARED ${DX9_SOURCES})
target_include_directories(UselessImGUI PRIVATE ${COMMON_INCLUDES})
target_link_libraries(UselessImGUI PRIVATE d3d9)

# DX8 Target
set(DX8_SOURCES ${COMMON_SOURCES} imgui_impl_dx8.cpp)
add_library(UselessImGUI_DX8 SHARED ${DX8_SOURCES})
target_include_directories(UselessImGUI_DX8 PRIVATE 
    ${COMMON_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}/dx8_include
)
target_link_directories(UselessImGUI_DX8 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dx8_lib/x86)
target_compile_definitions(UselessImGUI_DX8 PRIVATE USE_DX8)

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/api.gml"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/api.gml"
        "${CMAKE_CURRENT_BINARY_DIR}/api.gml"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/api.gml"
)

# DX9 GEX
add_custom_command(TARGET UselessImGUI POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/UselessImGUI.gej"
        "${CMAKE_CURRENT_BINARY_DIR}/UselessImGUI.gej"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/api.gml"
        "${CMAKE_CURRENT_BINARY_DIR}/api.gml"
    COMMAND python "${CMAKE_SOURCE_DIR}/gm82gex.py" "UselessImGUI.gej"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Created UselessImGUI.gex"
)

# DX8 GEX
add_custom_command(TARGET UselessImGUI_DX8 POST_BUILD
    COMMAND python -c "import json; d=json.load(open(r'${CMAKE_CURRENT_SOURCE_DIR}/UselessImGUI.gej')); d['name'] += 'DX8'; d['files'][0]['filename']='UselessImGUI_DX8.dll'; d['files'][0]['origname']='UselessImGUI_DX8.dll'; json.dump(d, open('UselessImGUI_DX8.gej', 'w'), indent=2)"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/api.gml"
        "${CMAKE_CURRENT_BINARY_DIR}/api.gml"
    COMMAND python "${CMAKE_SOURCE_DIR}/gm82gex.py" "UselessImGUI_DX8.gej"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    COMMENT "Created UselessImGUI_DX8.gex"
)
