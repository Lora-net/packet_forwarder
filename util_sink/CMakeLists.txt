
add_executable(util_sink "")
target_sources(util_sink
        PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/src/util_sink.c
)

if(LORAGW_FOUND)
target_include_directories(util_sink
	PRIVATE
		${LORAGW_INCLUDE_DIRS}
)

target_link_libraries(util_sink
	PRIVATE
		${LORAGW_LINK_LIBRARIES}
)
endif()

if(loragw_FOUND)
target_link_libraries(util_sink
	PRIVATE
		Semtech::loragw
)
endif()

set_target_properties(util_sink PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# add the install targets
install (
        TARGETS util_sink
  	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT shlib
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
