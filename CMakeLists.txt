cmake_minimum_required(VERSION 3.21)

project(WaveForge
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(DEFINED ENV{WAVELINE_QT_ROOT})
    list(PREPEND CMAKE_PREFIX_PATH
        "$ENV{WAVELINE_QT_ROOT}")
endif()

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
    Concurrent
)

file(GLOB_RECURSE WAVEFORGE_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_executable(WaveForge
    ${WAVEFORGE_SOURCES}
)

target_include_directories(WaveForge
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(WaveForge
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Multimedia
        Qt6::Concurrent
)

if(MSVC)
    target_compile_options(WaveForge
        PRIVATE
            /W4
    )
endif()

set_target_properties(WaveForge
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)