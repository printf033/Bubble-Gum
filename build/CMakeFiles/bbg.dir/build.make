# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/aaa/code/myproj/Bubble-Gum

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/aaa/code/myproj/Bubble-Gum/build

# Include any dependencies generated for this target.
include CMakeFiles/bbg.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/bbg.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/bbg.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/bbg.dir/flags.make

CMakeFiles/bbg.dir/opengl/main.cpp.o: CMakeFiles/bbg.dir/flags.make
CMakeFiles/bbg.dir/opengl/main.cpp.o: /home/aaa/code/myproj/Bubble-Gum/opengl/main.cpp
CMakeFiles/bbg.dir/opengl/main.cpp.o: CMakeFiles/bbg.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/aaa/code/myproj/Bubble-Gum/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/bbg.dir/opengl/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/bbg.dir/opengl/main.cpp.o -MF CMakeFiles/bbg.dir/opengl/main.cpp.o.d -o CMakeFiles/bbg.dir/opengl/main.cpp.o -c /home/aaa/code/myproj/Bubble-Gum/opengl/main.cpp

CMakeFiles/bbg.dir/opengl/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/bbg.dir/opengl/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aaa/code/myproj/Bubble-Gum/opengl/main.cpp > CMakeFiles/bbg.dir/opengl/main.cpp.i

CMakeFiles/bbg.dir/opengl/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/bbg.dir/opengl/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aaa/code/myproj/Bubble-Gum/opengl/main.cpp -o CMakeFiles/bbg.dir/opengl/main.cpp.s

CMakeFiles/bbg.dir/mylog/src/AsyncHelper.cpp.o: CMakeFiles/bbg.dir/flags.make
CMakeFiles/bbg.dir/mylog/src/AsyncHelper.cpp.o: /home/aaa/code/myproj/Bubble-Gum/mylog/src/AsyncHelper.cpp
CMakeFiles/bbg.dir/mylog/src/AsyncHelper.cpp.o: CMakeFiles/bbg.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/aaa/code/myproj/Bubble-Gum/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/bbg.dir/mylog/src/AsyncHelper.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/bbg.dir/mylog/src/AsyncHelper.cpp.o -MF CMakeFiles/bbg.dir/mylog/src/AsyncHelper.cpp.o.d -o CMakeFiles/bbg.dir/mylog/src/AsyncHelper.cpp.o -c /home/aaa/code/myproj/Bubble-Gum/mylog/src/AsyncHelper.cpp

CMakeFiles/bbg.dir/mylog/src/AsyncHelper.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/bbg.dir/mylog/src/AsyncHelper.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aaa/code/myproj/Bubble-Gum/mylog/src/AsyncHelper.cpp > CMakeFiles/bbg.dir/mylog/src/AsyncHelper.cpp.i

CMakeFiles/bbg.dir/mylog/src/AsyncHelper.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/bbg.dir/mylog/src/AsyncHelper.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aaa/code/myproj/Bubble-Gum/mylog/src/AsyncHelper.cpp -o CMakeFiles/bbg.dir/mylog/src/AsyncHelper.cpp.s

CMakeFiles/bbg.dir/mylog/src/FileAppender.cpp.o: CMakeFiles/bbg.dir/flags.make
CMakeFiles/bbg.dir/mylog/src/FileAppender.cpp.o: /home/aaa/code/myproj/Bubble-Gum/mylog/src/FileAppender.cpp
CMakeFiles/bbg.dir/mylog/src/FileAppender.cpp.o: CMakeFiles/bbg.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/aaa/code/myproj/Bubble-Gum/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/bbg.dir/mylog/src/FileAppender.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/bbg.dir/mylog/src/FileAppender.cpp.o -MF CMakeFiles/bbg.dir/mylog/src/FileAppender.cpp.o.d -o CMakeFiles/bbg.dir/mylog/src/FileAppender.cpp.o -c /home/aaa/code/myproj/Bubble-Gum/mylog/src/FileAppender.cpp

CMakeFiles/bbg.dir/mylog/src/FileAppender.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/bbg.dir/mylog/src/FileAppender.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aaa/code/myproj/Bubble-Gum/mylog/src/FileAppender.cpp > CMakeFiles/bbg.dir/mylog/src/FileAppender.cpp.i

CMakeFiles/bbg.dir/mylog/src/FileAppender.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/bbg.dir/mylog/src/FileAppender.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aaa/code/myproj/Bubble-Gum/mylog/src/FileAppender.cpp -o CMakeFiles/bbg.dir/mylog/src/FileAppender.cpp.s

CMakeFiles/bbg.dir/mylog/src/FileManager.cpp.o: CMakeFiles/bbg.dir/flags.make
CMakeFiles/bbg.dir/mylog/src/FileManager.cpp.o: /home/aaa/code/myproj/Bubble-Gum/mylog/src/FileManager.cpp
CMakeFiles/bbg.dir/mylog/src/FileManager.cpp.o: CMakeFiles/bbg.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/aaa/code/myproj/Bubble-Gum/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/bbg.dir/mylog/src/FileManager.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/bbg.dir/mylog/src/FileManager.cpp.o -MF CMakeFiles/bbg.dir/mylog/src/FileManager.cpp.o.d -o CMakeFiles/bbg.dir/mylog/src/FileManager.cpp.o -c /home/aaa/code/myproj/Bubble-Gum/mylog/src/FileManager.cpp

CMakeFiles/bbg.dir/mylog/src/FileManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/bbg.dir/mylog/src/FileManager.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aaa/code/myproj/Bubble-Gum/mylog/src/FileManager.cpp > CMakeFiles/bbg.dir/mylog/src/FileManager.cpp.i

CMakeFiles/bbg.dir/mylog/src/FileManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/bbg.dir/mylog/src/FileManager.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aaa/code/myproj/Bubble-Gum/mylog/src/FileManager.cpp -o CMakeFiles/bbg.dir/mylog/src/FileManager.cpp.s

CMakeFiles/bbg.dir/mylog/src/LogMessage.cpp.o: CMakeFiles/bbg.dir/flags.make
CMakeFiles/bbg.dir/mylog/src/LogMessage.cpp.o: /home/aaa/code/myproj/Bubble-Gum/mylog/src/LogMessage.cpp
CMakeFiles/bbg.dir/mylog/src/LogMessage.cpp.o: CMakeFiles/bbg.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/aaa/code/myproj/Bubble-Gum/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/bbg.dir/mylog/src/LogMessage.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/bbg.dir/mylog/src/LogMessage.cpp.o -MF CMakeFiles/bbg.dir/mylog/src/LogMessage.cpp.o.d -o CMakeFiles/bbg.dir/mylog/src/LogMessage.cpp.o -c /home/aaa/code/myproj/Bubble-Gum/mylog/src/LogMessage.cpp

CMakeFiles/bbg.dir/mylog/src/LogMessage.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/bbg.dir/mylog/src/LogMessage.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aaa/code/myproj/Bubble-Gum/mylog/src/LogMessage.cpp > CMakeFiles/bbg.dir/mylog/src/LogMessage.cpp.i

CMakeFiles/bbg.dir/mylog/src/LogMessage.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/bbg.dir/mylog/src/LogMessage.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aaa/code/myproj/Bubble-Gum/mylog/src/LogMessage.cpp -o CMakeFiles/bbg.dir/mylog/src/LogMessage.cpp.s

CMakeFiles/bbg.dir/mylog/src/Logger.cpp.o: CMakeFiles/bbg.dir/flags.make
CMakeFiles/bbg.dir/mylog/src/Logger.cpp.o: /home/aaa/code/myproj/Bubble-Gum/mylog/src/Logger.cpp
CMakeFiles/bbg.dir/mylog/src/Logger.cpp.o: CMakeFiles/bbg.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/aaa/code/myproj/Bubble-Gum/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/bbg.dir/mylog/src/Logger.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/bbg.dir/mylog/src/Logger.cpp.o -MF CMakeFiles/bbg.dir/mylog/src/Logger.cpp.o.d -o CMakeFiles/bbg.dir/mylog/src/Logger.cpp.o -c /home/aaa/code/myproj/Bubble-Gum/mylog/src/Logger.cpp

CMakeFiles/bbg.dir/mylog/src/Logger.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/bbg.dir/mylog/src/Logger.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aaa/code/myproj/Bubble-Gum/mylog/src/Logger.cpp > CMakeFiles/bbg.dir/mylog/src/Logger.cpp.i

CMakeFiles/bbg.dir/mylog/src/Logger.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/bbg.dir/mylog/src/Logger.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aaa/code/myproj/Bubble-Gum/mylog/src/Logger.cpp -o CMakeFiles/bbg.dir/mylog/src/Logger.cpp.s

CMakeFiles/bbg.dir/mylog/src/Timestamp.cpp.o: CMakeFiles/bbg.dir/flags.make
CMakeFiles/bbg.dir/mylog/src/Timestamp.cpp.o: /home/aaa/code/myproj/Bubble-Gum/mylog/src/Timestamp.cpp
CMakeFiles/bbg.dir/mylog/src/Timestamp.cpp.o: CMakeFiles/bbg.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/aaa/code/myproj/Bubble-Gum/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/bbg.dir/mylog/src/Timestamp.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/bbg.dir/mylog/src/Timestamp.cpp.o -MF CMakeFiles/bbg.dir/mylog/src/Timestamp.cpp.o.d -o CMakeFiles/bbg.dir/mylog/src/Timestamp.cpp.o -c /home/aaa/code/myproj/Bubble-Gum/mylog/src/Timestamp.cpp

CMakeFiles/bbg.dir/mylog/src/Timestamp.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/bbg.dir/mylog/src/Timestamp.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aaa/code/myproj/Bubble-Gum/mylog/src/Timestamp.cpp > CMakeFiles/bbg.dir/mylog/src/Timestamp.cpp.i

CMakeFiles/bbg.dir/mylog/src/Timestamp.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/bbg.dir/mylog/src/Timestamp.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aaa/code/myproj/Bubble-Gum/mylog/src/Timestamp.cpp -o CMakeFiles/bbg.dir/mylog/src/Timestamp.cpp.s

CMakeFiles/bbg.dir/glad/src/glad.c.o: CMakeFiles/bbg.dir/flags.make
CMakeFiles/bbg.dir/glad/src/glad.c.o: /home/aaa/code/myproj/Bubble-Gum/glad/src/glad.c
CMakeFiles/bbg.dir/glad/src/glad.c.o: CMakeFiles/bbg.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/aaa/code/myproj/Bubble-Gum/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/bbg.dir/glad/src/glad.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/bbg.dir/glad/src/glad.c.o -MF CMakeFiles/bbg.dir/glad/src/glad.c.o.d -o CMakeFiles/bbg.dir/glad/src/glad.c.o -c /home/aaa/code/myproj/Bubble-Gum/glad/src/glad.c

CMakeFiles/bbg.dir/glad/src/glad.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/bbg.dir/glad/src/glad.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/aaa/code/myproj/Bubble-Gum/glad/src/glad.c > CMakeFiles/bbg.dir/glad/src/glad.c.i

CMakeFiles/bbg.dir/glad/src/glad.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/bbg.dir/glad/src/glad.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/aaa/code/myproj/Bubble-Gum/glad/src/glad.c -o CMakeFiles/bbg.dir/glad/src/glad.c.s

CMakeFiles/bbg.dir/net/src/peer.cpp.o: CMakeFiles/bbg.dir/flags.make
CMakeFiles/bbg.dir/net/src/peer.cpp.o: /home/aaa/code/myproj/Bubble-Gum/net/src/peer.cpp
CMakeFiles/bbg.dir/net/src/peer.cpp.o: CMakeFiles/bbg.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/aaa/code/myproj/Bubble-Gum/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/bbg.dir/net/src/peer.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/bbg.dir/net/src/peer.cpp.o -MF CMakeFiles/bbg.dir/net/src/peer.cpp.o.d -o CMakeFiles/bbg.dir/net/src/peer.cpp.o -c /home/aaa/code/myproj/Bubble-Gum/net/src/peer.cpp

CMakeFiles/bbg.dir/net/src/peer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/bbg.dir/net/src/peer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aaa/code/myproj/Bubble-Gum/net/src/peer.cpp > CMakeFiles/bbg.dir/net/src/peer.cpp.i

CMakeFiles/bbg.dir/net/src/peer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/bbg.dir/net/src/peer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aaa/code/myproj/Bubble-Gum/net/src/peer.cpp -o CMakeFiles/bbg.dir/net/src/peer.cpp.s

# Object files for target bbg
bbg_OBJECTS = \
"CMakeFiles/bbg.dir/opengl/main.cpp.o" \
"CMakeFiles/bbg.dir/mylog/src/AsyncHelper.cpp.o" \
"CMakeFiles/bbg.dir/mylog/src/FileAppender.cpp.o" \
"CMakeFiles/bbg.dir/mylog/src/FileManager.cpp.o" \
"CMakeFiles/bbg.dir/mylog/src/LogMessage.cpp.o" \
"CMakeFiles/bbg.dir/mylog/src/Logger.cpp.o" \
"CMakeFiles/bbg.dir/mylog/src/Timestamp.cpp.o" \
"CMakeFiles/bbg.dir/glad/src/glad.c.o" \
"CMakeFiles/bbg.dir/net/src/peer.cpp.o"

# External object files for target bbg
bbg_EXTERNAL_OBJECTS =

bbg: CMakeFiles/bbg.dir/opengl/main.cpp.o
bbg: CMakeFiles/bbg.dir/mylog/src/AsyncHelper.cpp.o
bbg: CMakeFiles/bbg.dir/mylog/src/FileAppender.cpp.o
bbg: CMakeFiles/bbg.dir/mylog/src/FileManager.cpp.o
bbg: CMakeFiles/bbg.dir/mylog/src/LogMessage.cpp.o
bbg: CMakeFiles/bbg.dir/mylog/src/Logger.cpp.o
bbg: CMakeFiles/bbg.dir/mylog/src/Timestamp.cpp.o
bbg: CMakeFiles/bbg.dir/glad/src/glad.c.o
bbg: CMakeFiles/bbg.dir/net/src/peer.cpp.o
bbg: CMakeFiles/bbg.dir/build.make
bbg: /usr/lib/x86_64-linux-gnu/libGL.so
bbg: CMakeFiles/bbg.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/aaa/code/myproj/Bubble-Gum/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Linking CXX executable bbg"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/bbg.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/bbg.dir/build: bbg
.PHONY : CMakeFiles/bbg.dir/build

CMakeFiles/bbg.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/bbg.dir/cmake_clean.cmake
.PHONY : CMakeFiles/bbg.dir/clean

CMakeFiles/bbg.dir/depend:
	cd /home/aaa/code/myproj/Bubble-Gum/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/aaa/code/myproj/Bubble-Gum /home/aaa/code/myproj/Bubble-Gum /home/aaa/code/myproj/Bubble-Gum/build /home/aaa/code/myproj/Bubble-Gum/build /home/aaa/code/myproj/Bubble-Gum/build/CMakeFiles/bbg.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/bbg.dir/depend

