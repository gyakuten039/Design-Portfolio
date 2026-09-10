cmake_minimum_required(VERSION 3.16)
project(TypingHero)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(WIN32)
    set(CMAKE_SYSTEM_VERSION "10.0.18362.0")
endif()

if(DEFINED ENV{THIRDPARTY_DIR})
    set(CMAKE_PREFIX_PATH "$ENV{THIRDPARTY_DIR}/Qt6/lib/cmake")
else()
    set(CMAKE_PREFIX_PATH "D:/thirdparty_install/Qt6/lib/cmake")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/apple_game/apple.h
    src/apple_game/apple.cpp
    src/apple_game/apple_game_widget.h
    src/apple_game/apple_game_widget.cpp
    src/space_battle/enemy.h
    src/space_battle/enemy.cpp
    src/space_battle/space_battle_widget.h
    src/space_battle/space_battle_widget.cpp
    src/core/game_logic.h
    src/core/game_logic.cpp
    src/core/score_board.h
    src/core/score_board.cpp
    resources.qrc
)

add_executable(TypingHero ${PROJECT_SOURCES})

target_link_libraries(TypingHero PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../build/Debug")
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../build/Release")
endif()

file(MAKE_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
