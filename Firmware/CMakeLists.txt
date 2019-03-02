cmake_minimum_required(VERSION 3.0)

# you can override defaults from arduino-avr-toolchain.cmake with command line params
# example:
# -DARDUINO_MCU="atmega328p"
# -DARDUINO_F_CPU="8000000"
# -DARDUINO_MODEL="ARDUINO_AVR_PRO"
# -DARDUINO_VARIANT="eightanaloginputs"
# -DARDUINO_VER="10805"

set(CMAKE_TOOLCHAIN_FILE ArduinoCmakeBuilder/arduino-avr-toolchain.cmake)

if((${CMAKE_GENERATOR} STREQUAL "NMake Makefiles JOM") AND (NOT EXISTS "${CMAKE_MAKE_PROGRAM}"))
    set(CMAKE_MAKE_PROGRAM "${CMAKE_SOURCE_DIR}/Jom_Win32/jom.exe" CACHE INTERNAL "CMAKE_MAKE_PROGRAM")
endif()

project(OTPManagerFirmware C CXX ASM)

include(ArduinoCmakeBuilder/arduino-build-helper.cmake)

set_avr_compiler_flags()

add_arduino_library(SPI ${ARDUINO_LIBS_PATH})
add_arduino_library(Wire ${ARDUINO_LIBS_PATH})
add_arduino_library(EEPROM ${ARDUINO_LIBS_PATH})
add_arduino_library(U8g2_Arduino ${CMAKE_SOURCE_DIR}/Libraries SPI Wire)
add_arduino_library(Micro-AES-Arduino ${CMAKE_SOURCE_DIR}/Libraries)
add_arduino_library(SHA-Hash-Arduino ${CMAKE_SOURCE_DIR}/Libraries)
add_arduino_library(DS3231_Simple ${CMAKE_SOURCE_DIR}/Libraries Wire)
add_arduino_library(Low-Power ${CMAKE_SOURCE_DIR}/Libraries)

add_arduino_sketch(OTPManagerFirmware ${CMAKE_SOURCE_DIR} EEPROM U8g2_Arduino Micro-AES-Arduino SHA-Hash-Arduino DS3231_Simple Low-Power)
add_arduino_post_target(OTPManagerFirmware)
add_arduino_upload_target(OTPManagerFirmware)
