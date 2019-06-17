# uhal library

set(UHAL_INCDIR ${CMAKE_CURRENT_SOURCE_DIR})

set(LIBUHAL_SRCS
  main.c
  freertos-openocd.c
  pin_init.c
  uart.c
  )

add_library(uhal STATIC ${LIBUHAL_SRCS})
target_include_directories(uhal PUBLIC ${PROJ_HDRS} ${RTOSCONF_PATH} ${UCONTROLLER_HDRS} ${UHAL_PORT_PATH})

set(UHAL_INCDIR ${UHAL_INCDIR} PARENT_SCOPE)
