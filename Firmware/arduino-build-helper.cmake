if(NOT MCU)
    set(MCU "atmega328p" CACHE STRING "MCU model, used by compiler")
endif()
if(NOT F_CPU)
    set(F_CPU "16000000" CACHE STRING "Target clock speed")
endif()
if(NOT ARDUINO_VER)
    set(ARDUINO_VER "10805" CACHE STRING "Arduino version define, 10805 == 1.8.5")
endif()
if(NOT ARDUINO_MODEL)
    set(ARDUINO_MODEL "ARDUINO_AVR_PRO" CACHE STRING "Arduino board define")
endif()
if(NOT ARDUINO_VARIANT)
    set(ARDUINO_VARIANT "standard" CACHE STRING "Arduino board variant")
endif()

set(CXX_FLAGS
    "-g -Os -Wall -Wextra -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -flto"
    CACHE STRING "Arduino AVR C++ flags (from arduino IDE 1.8.5)")
set(CMAKE_CXX_FLAGS "${CXX_FLAGS} -mmcu=${MCU} -DF_CPU=${F_CPU} -DARDUINO=${ARDUINO_VER} -DARDUINO_ARCH_AVR -D${ARDUINO_MODEL}" CACHE INTERNAL "CMAKE_CXX_FLAGS")
set(CMAKE_CXX_FLAGS_RELEASE "" CACHE INTERNAL "CMAKE_CXX_FLAGS_RELEASE")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG" CACHE INTERNAL "CMAKE_CXX_FLAGS_DEBUG")

set(C_FLAGS
    "-g -Os -Wall -Wextra -std=gnu11 -ffunction-sections -fdata-sections -MMD -flto -fno-fat-lto-objects"
    CACHE STRING "Arduino AVR C flags (from arduino IDE 1.8.5)")

set(CMAKE_C_FLAGS "${C_FLAGS} -mmcu=${MCU} -DF_CPU=${F_CPU} -DARDUINO=${ARDUINO_VER} -DARDUINO_ARCH_AVR -D${ARDUINO_MODEL}" CACHE INTERNAL "CMAKE_C_FLAGS")
set(CMAKE_C_FLAGS_RELEASE "" CACHE INTERNAL "CMAKE_C_FLAGS_RELEASE")
set(CMAKE_C_FLAGS_DEBUG "-DDEBUG" CACHE INTERNAL "CMAKE_C_FLAGS_DEBUG")

set(ASM_FLAGS
    "-g -Os -Wall -Wextra -MMD -flto -fno-fat-lto-objects"
    CACHE STRING "Arduino AVR ASM flags")
set(CMAKE_ASM_FLAGS "-x assembler-with-cpp ${ASM_FLAGS} -mmcu=${MCU} -DF_CPU=${F_CPU} -DARDUINO=${ARDUINO_VER} -DARDUINO_ARCH_AVR -D${ARDUINO_MODEL}" CACHE INTERNAL "CMAKE_ASM_FLAGS")

set(EXE_LINKER_FLAGS "-Wall -Wextra -Os -g -flto -fuse-linker-plugin -Wl,--gc-sections"
    CACHE STRING "Arduino AVR GCC-linker flags")
set(CMAKE_EXE_LINKER_FLAGS "${EXE_LINKER_FLAGS} -mmcu=${MCU}" CACHE INTERNAL "CMAKE_EXE_LINKER_FLAGS")
