cmake_minimum_required (VERSION 3.8)

project ("Chip8sfml" LANGUAGES CXX VERSION 0.1.0)

if (WINDOWS)
set (SFML_DIR C:/lib/SFML-2.5.1/x64/lib/cmake/SFML)
set (SFML_STATIC_LIBRARIES TRUE)
endif(WINDOWS)
find_package (SFML 2.5 REQUIRED COMPONENTS graphics window)

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Wpointer-arith)
add_compile_options(-Wchar-subscripts)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

set (SRCS
    "Chip8.cpp"
    "Screen.cpp"
    "Keypad.cpp"
    "Cpu.cpp"
)

set (HDRS
    "Chip8.h"
    "Screen.h"
    "Keypad.h"
    "Cpu.h"
)

add_executable (${PROJECT_NAME} ${SRCS} ${HDRS})
target_link_libraries (${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

# TODO: Add tests and install targets if needed.
