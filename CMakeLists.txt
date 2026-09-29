cmake_minimum_required(VERSION 3.16)

project(eob3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(EOB3_SOURCES
    source/answers.cpp
    source/appearance.cpp
    source/bin_border.cpp
    source/bin_chargen.cpp
    source/bin_chargenb.cpp
    source/bin_font6.cpp
    source/bin_font8.cpp
    source/bin_invent.cpp
    source/bin_itemgl.cpp
    source/bin_itemgs.cpp
    source/bin_items.cpp
    source/bin_menu.cpp
    source/bin_playfld.cpp
    source/bin_portm.cpp
    source/bin_scene.cpp
    source/bin_xspl.cpp
    source/bitmap.cpp
    source/collection.cpp
    source/color.cpp
    source/console.cpp
    source/creature.cpp
    source/dice.cpp
    source/draw.cpp
    source/draw_text.cpp
    source/dungeon.cpp
    source/dungeon_generate.cpp
    source/game.cpp
    source/game_action.cpp
    source/game_city.cpp
    source/game_generate.cpp
    source/information.cpp
    source/item.cpp
    source/item_generate.cpp
    source/locale_en.cpp
    source/math.cpp
    source/mid_music.cpp
    source/midi.cpp
    source/monster.cpp
    source/npc.cpp
    source/point.cpp
    source/pointc.cpp
    source/pointca.cpp
    source/quest.cpp
    source/res.cpp
    source/scale.cpp
    source/shape.cpp
    source/stream.cpp
    source/stringbuilder.cpp
    source/stringvar.cpp
    source/view_controls.cpp
    source/view_debug.cpp
    source/view_focus.cpp
)

if(WIN32)
    list(APPEND EOB3_SOURCES
        source/win_draw.cpp
        source/win_io_file.cpp
        source/win_time.cpp
    )
elseif(UNIX AND NOT APPLE)
    list(APPEND EOB3_SOURCES
        source/nix_draw3x.cpp
        source/nix_io_file.cpp
        source/nix_time.cpp
    )
endif()

add_executable(eob3 ${EOB3_SOURCES})

target_include_directories(eob3 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/eob3
)

if(MSVC)
    target_compile_options(eob3 PRIVATE /W3)
    target_compile_definitions(eob3 PRIVATE
        WIN32
        _CONSOLE
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
    )
    target_link_libraries(eob3 PRIVATE winmm)
elseif(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_compile_options(eob3 PRIVATE -Wall)
    target_link_libraries(eob3 PRIVATE X11::X11)
endif()
