cmake_minimum_required(VERSION 3.0)

set(CMAKE_TOOLCHAIN_FILE arduino-avr-toolchain.cmake)

if((${CMAKE_GENERATOR} STREQUAL "NMake Makefiles JOM") AND (NOT EXISTS "${CMAKE_MAKE_PROGRAM}"))
    set(CMAKE_MAKE_PROGRAM "${CMAKE_SOURCE_DIR}/Jom_Win32/jom.exe" CACHE INTERNAL "CMAKE_MAKE_PROGRAM")
endif()

project(OTPManagerTestApp C CXX ASM)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

#qt creator trick
if(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES)
  include_directories("${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES}")
endif(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES)

include("arduino-build-helper.cmake")
