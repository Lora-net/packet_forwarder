add_executable(lora_pkt_fwd
  src/base64.c
  src/jitqueue.c
  src/lora_pkt_fwd.c
  src/parson.c
  src/timersync.c
  )

target_include_directories(lora_pkt_fwd PRIVATE inc)

target_link_libraries(lora_pkt_fwd
  PRIVATE LoraGW::loragw
  PRIVATE Threads::Threads
  )

##########################################################################
# Install                                                                #
##########################################################################
include(GNUInstallDirs)

install(TARGETS lora_pkt_fwd
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
