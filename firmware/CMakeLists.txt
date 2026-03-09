# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Buzhi C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

set(PICO_CXX_ENABLE_RTTI 1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(Buzhi Buzhi.cpp
                kinematics.cpp
                commandQ.cpp
                commands.cpp
                
                modules/mfrc522.c
                modules/slice.cpp
                modules/servo.cpp
                modules/motor.cpp
                modules/debug.cpp
                #modules/tcp_client.cpp
                )

pico_set_program_name(Buzhi "Buzhi")
pico_set_program_version(Buzhi "0.1")

# Generate PIO header
pico_generate_pio_header(Buzhi ${CMAKE_CURRENT_LIST_DIR}/stepper.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Buzhi 0)
pico_enable_stdio_usb(Buzhi 1)

# Add the standard library to the build
target_link_libraries(Buzhi
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(Buzhi PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(Buzhi 
        hardware_gpio
        hardware_pwm
        hardware_spi
        pico_multicore
        )

# In case using board supports WiFi - Debugging
if (PICO_CYW43_SUPPORTED)
    target_link_libraries(Buzhi pico_cyw43_arch_lwip_threadsafe_background
                          pico_lwip_nosys
                          )
endif()

pico_add_extra_outputs(Buzhi)

