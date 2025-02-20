cmake_minimum_required(VERSION 3.24)

if(DEFINED ENV{VCPKG_ROOT})
  include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

project(vAmigaSDL C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

# Set the optimization level (if not specified on the command line)
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Find and optionally link zlib
find_package(ZLIB QUIET)
if (ZLIB_FOUND)
  message(STATUS "Found ZLIB: ${ZLIB_LIBRARIES}")
else()
  message(STATUS "ZLIB not found. Compression support will be disabled.")
endif()

# Add the emulator library
add_library(vAmigaCore
  vAmiga/Emulator/VAmiga.cpp
  vAmiga/Emulator/VAmigaConfig.cpp
)

# Specify compile options
target_compile_definitions(vAmigaCore PUBLIC _USE_MATH_DEFINES)
if(MSVC)
  target_compile_options(vAmigaCore PUBLIC /W4 /bigobj /Zc:preprocessor) # /WX disabled for now
  target_compile_options(vAmigaCore PUBLIC /wd4100 /wd4201 /wd4324 /wd4458 /wd4091 /wd4430)
  set_source_files_properties(vAmiga/Emulator/VAmiga/Components/CPU/Moira/Moira.cpp PROPERTIES COMPILE_FLAGS "/wd4127")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(vAmigaCore PUBLIC -Wall -Werror)
  target_compile_options(vAmigaCore PUBLIC -Wno-restrict)
  target_compile_options(vAmigaCore PUBLIC -Wno-unused-parameter)
# target_compile_options(vAmigaCore PUBLIC -Wno-array-bounds)
  target_compile_options(vAmigaCore PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fconcepts>)
else()
  target_compile_options(vAmigaCore PUBLIC -Wall -Werror)
  target_compile_options(vAmigaCore PUBLIC -Wno-unused-parameter)
  target_compile_options(vAmigaCore PUBLIC -Wno-gnu-anonymous-struct)
  target_compile_options(vAmigaCore PUBLIC -Wno-nested-anon-types)
endif()

# Add include paths
target_include_directories(vAmigaCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vAmiga/Emulator)

# Add subdirectories
add_subdirectory(vAmiga/Emulator/VAmiga)

# Add libraries
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(vAmigaCore
  INTERFACE xdms
  INTERFACE Threads::Threads
)

# vAmigaSDL
add_executable(vAmigaSDL
  debug.c
  main.cpp
  scancodes.cpp
)

if (MSVC)
  set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Supported build configurations" FORCE)

  target_compile_options(vAmigaCore PUBLIC /MP /analyze /permissive- /Zi /Zo /sdl /W4)
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /analyze /permissive- /Zi /Zo /sdl /W4 /wd4100 /wd4201 /wd4324 /wd4458 /wd4146")

  if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /JMC")
  endif()
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
  # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:10000000") # To allow ADFFile etc. on stack
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
  add_definitions("-D_CRT_SECURE_NO_WARNINGS")
else()
  set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -ggdb")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ggdb")
  #set(COMMON_FLAGS "-fsanitize=address -ggdb")
  #set(COMMON_FLAGS "-fsanitize=thread -ggdb")

  if (WIN32)
      # For MinGW
      set(COMMON_FLAGS "-Wa,-mbig-obj")
  else()
      # Bogus warning at HDFFile.cpp:468
      # set(COMMON_FLAGS "-Wno-error=array-bounds")
  endif()

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(vAmigaSDL PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fconcepts>)
  endif()
endif()

if (WIN32)
  add_definitions("-DWIN32 -D_WIN32 -DUNICODE -D_UNICODE -D_WIN32_WINNT=0x600")
elseif (${CMAKE_HOST_SYSTEM_VERSION} MATCHES ".*-WSL2$")
  message(WARNING "Adding WSL2 mouse hack")
  add_definitions("-DWSL2_MOUSE_HACK")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")
endif()

if (WIN32 AND NOT DEFINED ENV{VCPKG_ROOT})
# Avoid warnings in CMake 3.24.0: DOWNLOAD_EXTRACT_TIMESTAMP
  if (CMAKE_VERSION VERSION_GREATER "3.24.0")
    cmake_policy(SET CMP0135 NEW)
  endif()

  include(ExternalProject)
  ExternalProject_Add(SDL3_LIB
    URL "https://www.libsdl.org/release/SDL3-devel-3.2.4-VC.zip"
    URL_HASH SHA256=56537a0840bf3b9f328370670497aa15221e60bff4b1fbe0450e3e60c44a4ba2
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )

  ExternalProject_Get_property(SDL3_LIB SOURCE_DIR)
  set(SDL3_INCLUDE_DIRS "${SOURCE_DIR}/include")
  add_library(SDL3 STATIC IMPORTED GLOBAL)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set_target_properties(SDL3 PROPERTIES IMPORTED_LOCATION "${SOURCE_DIR}/lib/x64/SDL3.lib")
  else()
    set_target_properties(SDL3 PROPERTIES IMPORTED_LOCATION "${SOURCE_DIR}/lib/x86/SDL3.lib")
  endif()
  set (SDL3_LIBRARIES SDL3)
else()
  find_package(SDL3 REQUIRED)
endif()

target_include_directories(vAmigaSDL PRIVATE ${SDL3_INCLUDE_DIRS})
target_link_libraries(vAmigaSDL
  PRIVATE vAmigaCore
  PRIVATE ${SDL3_LIBRARIES}
  PRIVATE Threads::Threads
)
if (WIN32)
#  target_link_libraries(vAmigaSDL ws2_32)
else()
#  target_link_libraries(vAmigaSDL pthread)
endif()
