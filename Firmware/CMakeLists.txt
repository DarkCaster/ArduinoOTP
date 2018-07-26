cmake_minimum_required(VERSION 3.0)

set(CMAKE_TOOLCHAIN_FILE ArduinoCmakeBuilder/arduino-avr-toolchain.cmake)

if((${CMAKE_GENERATOR} STREQUAL "NMake Makefiles JOM") AND (NOT EXISTS "${CMAKE_MAKE_PROGRAM}"))
    set(CMAKE_MAKE_PROGRAM "${CMAKE_SOURCE_DIR}/Jom_Win32/jom.exe" CACHE INTERNAL "CMAKE_MAKE_PROGRAM")
endif()

project(OTPManagerTestApp C CXX ASM)

include(ArduinoCmakeBuilder/arduino-build-helper.cmake)

add_arduino_library(EEPROM ${ARDUINO_LIBS_PATH})
add_arduino_library(SPI ${ARDUINO_LIBS_PATH})
