# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set your project name
project(KICKMIDI)

# Add the executable target
add_executable(KICKMIDI src/ttymidi.c)

# Find and link the pthread library
find_package(Threads REQUIRED)
find_package(ALSA REQUIRED)
target_link_libraries(KICKMIDI PRIVATE Threads::Threads)
target_link_libraries(KICKMIDI PRIVATE ALSA::ALSA)

