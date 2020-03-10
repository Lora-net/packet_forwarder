##########################################################################
# Project Setup                                                          #
##########################################################################
cmake_minimum_required(VERSION 3.0.2)
project(LoRaPacketForwarder
  VERSION 4.0.1
  LANGUAGES C
  )
set(CMAKE_C_STANDARD 99)
# Generate `compile_commands.json` in build directory
# Can be used with static analyzers and code-completion tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

##########################################################################
# Dependencies                                                           #
##########################################################################
find_package(Threads REQUIRED)

option(WITH_VENDORED_HAL "Use git submodule to build libloragw from source")
if(${WITH_VENDORED_HAL})
  add_subdirectory(extern/lora_gateway)
else()
  find_package(LoraGW REQUIRED)
endif()

##########################################################################
# Static Analysis                                                        #
##########################################################################
find_program(CPPCHECK NAMES cppcheck)
if(CPPCHECK)
  message(STATUS "Found cppcheck: ${CPPCHECK}")
  set(CMAKE_C_CPPCHECK ${CPPCHECK}
    --enable=warning
    --inline-suppr
    --quiet
    --std=c${CMAKE_C_STANDARD}
    --suppress=missingIncludeSystem
    --suppress=unusedFunction
    --template=gcc
    )
endif()

##########################################################################
# Local                                                                  #
##########################################################################
add_subdirectory(lora_pkt_fwd)
add_subdirectory(util_ack)
add_subdirectory(util_sink)
