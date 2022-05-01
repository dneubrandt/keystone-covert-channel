add_executable(eapp-trojan.riscv
  eapp-trojan.c
  edge_wrapper.c
  bht.c)

include_directories(include
  ${KEYSTONE_SDK_DIR}/include/app)
  
target_link_libraries(eapp-trojan.riscv
  ${KEYSTONE_LIB_EDGE}
  ${KEYSTONE_LIB_EAPP})
  
set_target_properties(eapp-trojan.riscv
  PROPERTIES LINK_FLAGS "-nostdlib -static -T ${CMAKE_CURRENT_SOURCE_DIR}/app.lds")