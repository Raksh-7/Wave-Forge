cmake_minimum_required(VERSION 3.21)

project(Waveline
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# ============================================================
# Qt discovery
# ============================================================

# Allow the developer to specify the Qt installation through
# the WAVELINE_QT_ROOT environment variable.
#
# Example:
# WAVELINE_QT_ROOT=D:/Softwares/Qt/6.12.0/msvc2022_64

if(DEFINED ENV{WAVELINE_QT_ROOT})
    message(STATUS "WAVELINE_QT_ROOT: $ENV{WAVELINE_QT_ROOT}")

    list(PREPEND CMAKE_PREFIX_PATH
        "$ENV{WAVELINE_QT_ROOT}"
    )
endif()


# ============================================================
# Find Qt
# ============================================================

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)


# ============================================================
# Application
# ============================================================

add_executable(Waveline
    src/main.cpp
)


# ============================================================
# Qt libraries
# ============================================================

target_link_libraries(Waveline
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)