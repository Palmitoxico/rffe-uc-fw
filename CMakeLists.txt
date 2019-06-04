# Define minimal required version of CMake.
cmake_minimum_required(VERSION 2.8.8)

include(cmake/GetGitRevisionDescription.cmake)

# Project definition
project(rffe C CXX ASM)

include(${CMAKE_SOURCE_DIR}/cmake/toolchain-arm-none-eabi.cmake)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: none Debug Release."
    FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_ERROR_FLAGS "-Wall -Wextra -Wpointer-arith -Wno-packed-bitfield-compat -Wno-unused-parameter -Wno-missing-field-initializers")

# Build specific flags
set(CMAKE_C_FLAGS           "${CMAKE_C_FLAGS} ${CMAKE_ERROR_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG     "-Og -g3 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE   "-Os -g3")

# When we break up long strings in CMake we get semicolon
# separated lists, undo this here...
string(REGEX REPLACE ";" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REGEX REPLACE ";" " " CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
string(REGEX REPLACE ";" " " CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}" CACHE STRING "")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" CACHE STRING "")

set(PROJ_HDRS ${CMAKE_SOURCE_DIR})

add_subdirectory(port)
add_subdirectory(FreeRTOS)
add_subdirectory(boot)
add_subdirectory(uhal)

include_directories(${UCONTROLLER_HDRS} ${PROJ_HDRS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTARGET_CONTROLLER=${TARGET_CONTROLLER}")

if(BENCH_TEST)
  set(MODULES_FLAGS "${MODULES_FLAGS} -DBENCH_TEST")
  message(STATUS "Bench mode activated!")
endif()

# Get Git information
get_git_head_revision(GIT_REFSPEC GIT_SHA1 "--abbrev=4 --tags")
git_local_changes(GIT_DIRTY)
if(GIT_DIRTY STREQUAL "DIRTY")
  set(GIT_SHA1 ${GIT_SHA1}-dirty)
endif()
configure_file(uhal/GitSHA1.c.in GitSHA1.c @ONLY)
list(APPEND PROJ_SRCS "${PROJECT_BINARY_DIR}/GitSHA1.c" uhal/GitSHA1.h)

# Libraries path
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

## Create executable
add_executable(${CMAKE_PROJECT_NAME} ${UCONTROLLER_SRCS} ${PROJ_SRCS})
# set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${MODULES_FLAGS})

add_executable(bootloader ${BOOT_SRCS})

# Linker flags
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
  SUFFIX ".elf"
  LINK_FLAGS "-T ${APP_LINKER_SCRIPT} ${TARGET_LINK_FLAGS}"
  )

set_target_properties(bootloader PROPERTIES
  SUFFIX ".elf"
  LINK_FLAGS "-T ${BOOT_LINKER_SCRIPT} ${TARGET_LINK_FLAGS}"
  )

configure_file(openocd/openocd.cfg.in openocd.cfg)

# Headers path
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${PROJ_HDRS})
target_include_directories(bootloader PUBLIC ${PROJ_HDRS})
# Link libraries
target_link_libraries(${CMAKE_PROJECT_NAME} FreeRTOS c m ${TARGET_LIBS})
target_link_libraries(bootloader c m ${TARGET_LIBS})

##Generate binary file
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  COMMENT "Converting the ELF output to a binary file"
  )

add_custom_command(TARGET bootloader POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O binary bootloader.elf bootloader.bin
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  COMMENT "Converting the ELF output to a binary file"
  )

add_custom_target(program_boot
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/
  DEPENDS bootloader
  COMMAND openocd -f openocd.cfg -c "program bootloader.bin verify reset ${BOOT_START_ADDR}" -c shutdown
  )

add_custom_target(program_app
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/
  DEPENDS ${CMAKE_PROJECT_NAME}
  COMMAND openocd -f openocd.cfg -c "program ${CMAKE_PROJECT_NAME}.bin verify reset ${APP_START_ADDR}" -c shutdown
  )

add_custom_target(program_all
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/
  DEPENDS program_boot program_app
  )

add_custom_target(full_binary
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/
  DEPENDS ${CMAKE_PROJECT_NAME} bootloader
  #Create a pad file with the total size of the bootloader (0x2000)
  COMMAND dd if=/dev/zero bs=1 count=8192 | tr \"\\000\" \"\\377\" > bootloader_pad.bin
  #Add padding bytes to the bootloader binary
  COMMAND dd if=bootloader.bin of=bootloader_pad.bin conv=notrunc
  #Concatenate bootloader and app binaries
  COMMAND cat bootloader_pad.bin ${CMAKE_PROJECT_NAME}.bin > openMMC_full.bin
  )
