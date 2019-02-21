set(TARGET lora_pkt_fwd)

add_executable(${TARGET} "")

# --  add the compile options
target_compile_options(
	${TARGET}
	PRIVATE
	-Wall
	-Wextra
)

target_compile_definitions(
	${TARGET}
	PRIVATE
	VERSION_STRING="${packet_forwarder_VERSION_SHORT}"
)

target_sources(${TARGET}
	PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/src/base64.c
		${CMAKE_CURRENT_LIST_DIR}/src/jitqueue.c
		${CMAKE_CURRENT_LIST_DIR}/src/lora_pkt_fwd.c
		${CMAKE_CURRENT_LIST_DIR}/src/parson.c
		${CMAKE_CURRENT_LIST_DIR}/src/timersync.c
)

target_include_directories(${TARGET}
	PRIVATE
		${CMAKE_CURRENT_LIST_DIR}
		${CMAKE_CURRENT_LIST_DIR}/inc
)

target_link_libraries(${TARGET}
	PUBLIC
		Threads::Threads
		m
)

if(LORAGW_FOUND)
target_include_directories(${TARGET}
	PRIVATE
		${LORAGW_INCLUDE_DIRS}
)

target_link_libraries(${TARGET}
	PRIVATE
		${LORAGW_LINK_LIBRARIES}
)
endif()

if(loragw_FOUND)
target_link_libraries(${TARGET}
	PRIVATE
		Semtech::loragw
)
endif()

set_target_properties(${TARGET} PROPERTIES VERSION ${packet_forwarder_VERSION})
set_target_properties(${TARGET} PROPERTIES SOVERSION ${packet_forwarder_VERSION_SHORT})

# add the install targets
install (
	TARGETS ${TARGET}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT shlib
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
