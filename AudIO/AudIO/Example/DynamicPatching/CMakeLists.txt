# CMake simple example

## [main]

# Almost all CMake files should start with this
# You should always specify a range with the newest
# and oldest tested versions of CMake. This will ensure
# you pick up the best policies.
cmake_minimum_required(VERSION 3.1...3.25)

# This is your project statement. You should always list languages;
# Listing the version is nice here since it sets lots of useful variables
project( BasicExample
  VERSION 1.0
  LANGUAGES CXX
)

# If you set any CMAKE_ variables, that can go here.
# (But usually don't do this, except maybe for C++ standard)

# Find packages go here.

# Link each target with other targets or add options, etc.

# Adding something we can run - Output name matches target name
add_executable(DynamicPatch
  main.cpp
)

# Make sure you link your targets with this command. It can also link libraries and
# even flags, so linking a target that does not exist will not give a configure-time error.
target_link_libraries(DynamicPatch 
  PRIVATE AudIO_Core
)
