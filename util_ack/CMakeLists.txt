add_executable(util_ack
  src/util_ack.c
  )

target_link_libraries(util_ack PRIVATE LoraGW::loragw)
