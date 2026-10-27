# CMake simple example

## [main]

# Almost all CMake files should start with this
# You should always specify a range with the newest
# and oldest tested versions of CMake. This will ensure
# you pick up the best policies.
cmake_minimum_required(VERSION 3.1...3.25)

# This is your project statement. You should always list languages;
# Listing the version is nice here since it sets lots of useful variables
project( AudIO
	VERSION 1.0
	LANGUAGES CXX
)

# Link each target with other targets or add options, etc.

# Adding something we can run - Output name matches target name
add_executable( DawExample 
	DawExampleMain.cpp
)

# Make sure you link your targets with this command. It can also link libraries and
# even flags, so linking a target that does not exist will not give a configure-time error.
target_link_libraries( DawExample 
	PRIVATE 
		AudIO_Core 
		DAW_Core 
		AirWindows_plugins
)

## [main]

# This part is so the Modern CMake book can verify this example builds. For your code,
# you'll probably want tests too
enable_testing()
add_test(NAME DawExample COMMAND DawExample)
