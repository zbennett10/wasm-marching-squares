# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/zach/dev/terra-incognita/wasm-gis-toolkit/wasm-marching-squares/include/jansson-2.13.1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/zach/dev/terra-incognita/wasm-gis-toolkit/wasm-marching-squares/include/jansson-2.13.1

# Include any dependencies generated for this target.
include CMakeFiles/test_copy.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_copy.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_copy.dir/flags.make

CMakeFiles/test_copy.dir/test/suites/api/test_copy.c.o: CMakeFiles/test_copy.dir/flags.make
CMakeFiles/test_copy.dir/test/suites/api/test_copy.c.o: test/suites/api/test_copy.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/zach/dev/terra-incognita/wasm-gis-toolkit/wasm-marching-squares/include/jansson-2.13.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/test_copy.dir/test/suites/api/test_copy.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/test_copy.dir/test/suites/api/test_copy.c.o -c /Users/zach/dev/terra-incognita/wasm-gis-toolkit/wasm-marching-squares/include/jansson-2.13.1/test/suites/api/test_copy.c

CMakeFiles/test_copy.dir/test/suites/api/test_copy.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test_copy.dir/test/suites/api/test_copy.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/zach/dev/terra-incognita/wasm-gis-toolkit/wasm-marching-squares/include/jansson-2.13.1/test/suites/api/test_copy.c > CMakeFiles/test_copy.dir/test/suites/api/test_copy.c.i

CMakeFiles/test_copy.dir/test/suites/api/test_copy.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test_copy.dir/test/suites/api/test_copy.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/zach/dev/terra-incognita/wasm-gis-toolkit/wasm-marching-squares/include/jansson-2.13.1/test/suites/api/test_copy.c -o CMakeFiles/test_copy.dir/test/suites/api/test_copy.c.s

# Object files for target test_copy
test_copy_OBJECTS = \
"CMakeFiles/test_copy.dir/test/suites/api/test_copy.c.o"

# External object files for target test_copy
test_copy_EXTERNAL_OBJECTS =

bin/test_copy: CMakeFiles/test_copy.dir/test/suites/api/test_copy.c.o
bin/test_copy: CMakeFiles/test_copy.dir/build.make
bin/test_copy: lib/libjansson.a
bin/test_copy: CMakeFiles/test_copy.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/zach/dev/terra-incognita/wasm-gis-toolkit/wasm-marching-squares/include/jansson-2.13.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable bin/test_copy"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_copy.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_copy.dir/build: bin/test_copy

.PHONY : CMakeFiles/test_copy.dir/build

CMakeFiles/test_copy.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_copy.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_copy.dir/clean

CMakeFiles/test_copy.dir/depend:
	cd /Users/zach/dev/terra-incognita/wasm-gis-toolkit/wasm-marching-squares/include/jansson-2.13.1 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/zach/dev/terra-incognita/wasm-gis-toolkit/wasm-marching-squares/include/jansson-2.13.1 /Users/zach/dev/terra-incognita/wasm-gis-toolkit/wasm-marching-squares/include/jansson-2.13.1 /Users/zach/dev/terra-incognita/wasm-gis-toolkit/wasm-marching-squares/include/jansson-2.13.1 /Users/zach/dev/terra-incognita/wasm-gis-toolkit/wasm-marching-squares/include/jansson-2.13.1 /Users/zach/dev/terra-incognita/wasm-gis-toolkit/wasm-marching-squares/include/jansson-2.13.1/CMakeFiles/test_copy.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_copy.dir/depend

