add_library(uhal_port STATIC
  uart_port.c
  i2c_port.c
  )

list(APPEND TARGET_LIBS "uhal_port")
set(TARGET_LIBS ${TARGET_LIBS} PARENT_SCOPE)
