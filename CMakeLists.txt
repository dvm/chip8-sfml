cmake_minimum_required (VERSION 3.8)

project ("Chip8sfml" VERSION 0.1.0)

set (SFML_DIR C:/lib/SFML-2.5.1/x64/lib/cmake/SFML)
set (SFML_STATIC_LIBRARIES TRUE)
find_package (SFML 2.5 COMPONENTS graphics window)

set (CMAKE_CXX_STANDARD 17)
add_executable (${PROJECT_NAME} "Chip8.cpp" "Chip8.h"  "Screen.h" "Screen.cpp" "Keypad.h" "Keypad.cpp" "Cpu.h" "Cpu.cpp")
target_link_libraries (${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

# TODO: Add tests and install targets if needed.
