# -- Minimum required version
cmake_minimum_required (VERSION 3.2)

# -- Project name
project (packet_forwarder)

# -- Various includes
include (CMakePackageConfigHelpers)
include (GNUInstallDirs)
include (CheckFunctionExists)

# -- set c99 standard default
set(CMAKE_C_STANDARD 99)

# -- Required to build
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# -- Versioning with git tag
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
	execute_process(
		COMMAND git describe --tags --always
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		OUTPUT_VARIABLE "packet_forwarder_VERSION"
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)
		if(packet_forwarder_VERSION STREQUAL "")
			set(packet_forwarder_VERSION 0)
		endif(packet_forwarder_VERSION STREQUAL "")
		message( STATUS "Git full version: ${packet_forwarder_VERSION}" )
	execute_process(
		COMMAND /bin/bash -c "git describe --tags --abbrev=0 | cut --delimiter='v' --fields=2"
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		OUTPUT_VARIABLE "packet_forwarder_VERSION_SHORT"
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)
		if(packet_forwarder_VERSION_SHORT STREQUAL "")
			set(packet_forwarder_VERSION_SHORT 0)
		endif(packet_forwarder_VERSION_SHORT STREQUAL "")
		message( STATUS "Git version: ${packet_forwarder_VERSION_SHORT}" )
else(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
	set(packet_forwarder_VERSION_SHORT 0)
	set(packet_forwarder_VERSION 0)
endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)

# -- find packages
find_package(loragw QUIET)
if(NOT loragw_FOUND)
	find_package(PkgConfig)
	pkg_search_module(LORAGW loragw)

	if(LORAGW_FOUND)
		message("-- pkg_config: libloragw is found")
		message("-- libloragw include: ${LORAGW_INCLUDE_DIRS}")
		message("-- libloragw library: ${LORAGW_LINK_LIBRARIES}")
	else()
		message(FATAL_ERROR "-- pkg_config: libloragw is not found")
	endif()
else()
	message("-- CMake: libloragw is found")
endif()

# when building, don't use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
if (NOT (${CMAKE_INSTALL_PREFIX} STREQUAL "/usr" ) )
   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
endif()

# -- add the lora_pkt_fwd
add_subdirectory(lora_pkt_fwd)

# -- add the util_ack
add_subdirectory(util_ack)

# -- add the util_sink
add_subdirectory(util_sink)

# -- add the util_tx_test
add_subdirectory(util_tx_test)
