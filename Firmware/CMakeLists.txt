project(OTPManagerTestApp CXX)
cmake_minimum_required(VERSION 2.8)

if(NOT((${CMAKE_GENERATOR} STREQUAL "Unix Makefiles") OR (${CMAKE_GENERATOR} STREQUAL "MSYS Makefiles") OR (${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles")))
  message(FATAL_ERROR "Chosen generator is not supported for this project")
endif()

message(STATUS "Building for ${CMAKE_SYSTEM_NAME}. Processor architecture is ${CMAKE_SYSTEM_PROCESSOR}")

#set some custom options and default values
set(ARCHSUFFIX ".${CMAKE_SYSTEM_PROCESSOR}")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

#set defaults
set(EXELINKFLAGS "")
set(OPTFLAGS "")

#setup RPATH and build directories
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_USE_RELATIVE_PATHS TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

set(CMAKE_INSTALL_RPATH "./lib${ARCHSUFFIX}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/lib${ARCHSUFFIX}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/lib${ARCHSUFFIX}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#print status
message(STATUS "Current build configuration:")
message(STATUS "CMAKE_GENERATOR=${CMAKE_GENERATOR}")
message(STATUS "CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_ARCHIVE_OUTPUT_DIRECTORY=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
message(STATUS "CMAKE_LIBRARY_OUTPUT_DIRECTORY=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

include_directories("${CMAKE_BINARY_DIR}")
include_directories("OTPManagerFirmware")
include_directories("StandaloneAppSrc")

#set flags for gcc
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
  #optional flags for g++ compiler
  set(OPTFLAGS "")
  #general compiler flags for g++ compiler
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -fpermissive -fno-exceptions -Wall -Wextra -Wshadow -Wstrict-overflow=5 -Wwrite-strings -Winit-self -Wmissing-include-dirs -Wcast-qual -Wcast-align -Wconversion -Wlogical-op -Wpacked -Wredundant-decls -Winline -Wdisabled-optimization -Woverlength-strings -Wfloat-equal -Wswitch-default -Wno-missing-field-initializers -Wno-gnu-statement-expression ${OPTFLAGS}")
  #set build-type dependent additional flags
  set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DDEBUG -ggdb")
  set(EXELINKFLAGS "-Wl,-rpath-link,'${CMAKE_LIBRARY_OUTPUT_DIRECTORY}'")
endif()

add_definitions(-DSTANDALONE_APP)

if(WIN32)
  set(BUILD_PLATFORM "WINDOWS")
elseif(LINUX)
  set(BUILD_PLATFORM "LINUX")
else()
  message(FATAL_ERROR "This platform is not supported!")
endif ()

#check include files
include(CheckIncludeFile)
include(CheckSymbolExists)

#save module configuration to config.h
configure_file(StandaloneAppSrc/config.h.in standalone_config.h)

#qt creator trick
if(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES)
  include_directories("${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES}")
endif(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES)

#emu util
add_executable(otptest StandaloneAppSrc/main.cpp StandaloneAppSrc/serial.cpp StandaloneAppSrc/gui.cpp OTPManagerFirmware/main_loop.cpp OTPManagerFirmware/crc8.cpp OTPManagerFirmware/comm_helper.cpp)
###target_link_libraries(TODO)
install(TARGETS otptest DESTINATION "otptest")
