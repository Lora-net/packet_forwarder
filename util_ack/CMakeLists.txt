
add_executable(util_ack "")
target_sources(util_ack
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/util_ack.c
)

if(LORAGW_FOUND)
target_include_directories(util_ack
	PRIVATE
		${LORAGW_INCLUDE_DIRS}
)

target_link_libraries(util_ack
	PRIVATE
		${LORAGW_LINK_LIBRARIES}
)
endif()

if(loragw_FOUND)
target_link_libraries(util_ack
	PRIVATE
		Semtech::loragw
)
endif()
set_target_properties(util_ack PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# add the install targets
install (
        TARGETS util_ack
  	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT shlib
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
