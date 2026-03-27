cmake_minimum_required(VERSION 3.14)

# Auto-correct legacy MinGW (32-bit) toolchain selection in existing caches.
if(WIN32
	AND EXISTS "C:/msys64/ucrt64/bin/g++.exe"
	AND EXISTS "C:/msys64/ucrt64/bin/gcc.exe"
	AND EXISTS "C:/msys64/ucrt64/bin/mingw32-make.exe"
	AND (CMAKE_CXX_COMPILER MATCHES "^C:/MinGW/bin/g\\+\\+\\.exe$" OR CMAKE_C_COMPILER MATCHES "^C:/MinGW/bin/gcc\\.exe$"))
	set(CMAKE_C_COMPILER "C:/msys64/ucrt64/bin/gcc.exe" CACHE FILEPATH "C compiler" FORCE)
	set(CMAKE_CXX_COMPILER "C:/msys64/ucrt64/bin/g++.exe" CACHE FILEPATH "CXX compiler" FORCE)
	set(CMAKE_MAKE_PROGRAM "C:/msys64/ucrt64/bin/mingw32-make.exe" CACHE FILEPATH "Make program" FORCE)
endif()

project(FlappyBirdProject)

# Prefer the SFML package bundled in the workspace.
set(SFML_LOCAL_ROOT "${CMAKE_SOURCE_DIR}/../SFML-3.0.2")
if(MSVC AND EXISTS "${SFML_LOCAL_ROOT}/lib/cmake/SFML/SFMLConfig.cmake")
	list(PREPEND CMAKE_PREFIX_PATH "${SFML_LOCAL_ROOT}")
endif()

# For MinGW builds, prefer MSYS2 UCRT64 SFML package location.
if(MINGW)
	if(EXISTS "C:/msys64/ucrt64/lib/cmake/SFML/SFMLConfig.cmake")
		list(PREPEND CMAKE_PREFIX_PATH "C:/msys64/ucrt64")
	endif()
endif()

# SFML 3 package in this workspace is 64-bit; stop early on 32-bit MinGW.
if(MINGW AND CMAKE_SIZEOF_VOID_P EQUAL 4)
	message(FATAL_ERROR
		"Detected 32-bit MinGW toolchain (${CMAKE_CXX_COMPILER}). "
		"This project uses 64-bit SFML. Configure with a 64-bit compiler "
		"(for example: C:/msys64/ucrt64/bin/g++.exe)."
	)
endif()

# Bundled SFML package contains MSVC .lib import libraries, not MinGW .a files.
if(MINGW AND EXISTS "${SFML_LOCAL_ROOT}/lib/sfml-graphics.lib")
	if(NOT EXISTS "C:/msys64/ucrt64/lib/cmake/SFML/SFMLConfig.cmake")
		message(FATAL_ERROR
			"Detected MinGW toolchain (${CMAKE_CXX_COMPILER}) with MSVC SFML binaries in workspace. "
			"Install MinGW-compatible SFML first: pacman -S mingw-w64-ucrt-x86_64-sfml"
		)
	endif()
endif()

# 1. Add your source and include directories
include_directories(src/include)
if(WIN32)
        file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.rc")
else()
        file(GLOB_RECURSE SOURCES "src/*.cpp")
endif()

# 2. Create Executable
add_executable(FlappyBird ${SOURCES})

# 3. Discover and link SFML modules (including Audio for sounds)
find_package(SFML 3 CONFIG REQUIRED COMPONENTS Graphics Window System Audio)
target_link_libraries(FlappyBird PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)

# Set C++ Standard
target_compile_features(FlappyBird PRIVATE cxx_std_17)

# Copy only the exact runtime DLL dependency closure discovered by CMake.
# This avoids mixing incompatible architecture/ABI variants from wildcard copies.
if(WIN32)
	add_custom_command(TARGET FlappyBird POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_RUNTIME_DLLS:FlappyBird>
			$<TARGET_FILE_DIR:FlappyBird>
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/Assets"
			"$<TARGET_FILE_DIR:FlappyBird>/Flappy-Bird/Assets"
		COMMAND_EXPAND_LISTS
	)
endif()