add_executable(util_sink
  src/util_sink.c
  )

target_link_libraries(util_sink PRIVATE LoraGW::loragw)
