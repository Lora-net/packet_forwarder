add_executable(util_tx_test
  src/base64.c
  src/util_tx_test.c
  )

target_include_directories(util_tx_test PRIVATE inc)

target_link_libraries(util_tx_test PRIVATE LoraGW::loragw)
