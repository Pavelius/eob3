cmake_minimum_required(VERSION 3.16)

project(eob3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(EOB3_SOURCES
    eob3/answers.cpp
    eob3/appearance.cpp
    eob3/bin_border.cpp
    eob3/bin_chargen.cpp
    eob3/bin_chargenb.cpp
    eob3/bin_font6.cpp
    eob3/bin_font8.cpp
    eob3/bin_invent.cpp
    eob3/bin_itemgl.cpp
    eob3/bin_itemgs.cpp
    eob3/bin_items.cpp
    eob3/bin_menu.cpp
    eob3/bin_playfld.cpp
    eob3/bin_portm.cpp
    eob3/bin_scene.cpp
    eob3/bin_xspl.cpp
    eob3/bitmap.cpp
    eob3/collection.cpp
    eob3/color.cpp
    eob3/console.cpp
    eob3/creature.cpp
    eob3/dice.cpp
    eob3/draw.cpp
    eob3/draw_text.cpp
    eob3/dungeon.cpp
    eob3/dungeon_generate.cpp
    eob3/game.cpp
    eob3/game_action.cpp
    eob3/game_carousing.cpp
    eob3/game_city.cpp
    eob3/game_generate.cpp
    eob3/information.cpp
    eob3/item.cpp
    eob3/item_generate.cpp
    eob3/locale_en.cpp
    eob3/math.cpp
    eob3/mid_music.cpp
    eob3/midi.cpp
    eob3/monster.cpp
    eob3/npc.cpp
    eob3/point.cpp
    eob3/pointc.cpp
    eob3/pointca.cpp
    eob3/quest.cpp
    eob3/res.cpp
    eob3/scale.cpp
    eob3/shape.cpp
    eob3/stream.cpp
    eob3/stringbuilder.cpp
    eob3/stringvar.cpp
    eob3/view_controls.cpp
    eob3/view_debug.cpp
    eob3/view_focus.cpp
)

if(WIN32)
    list(APPEND EOB3_SOURCES
        eob3/win_draw.cpp
        eob3/win_io_file.cpp
        eob3/win_time.cpp
    )
elseif(UNIX AND NOT APPLE)
    list(APPEND EOB3_SOURCES
        eob3/nix_draw3x.cpp
        eob3/nix_io_file.cpp
        eob3/nix_time.cpp
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
