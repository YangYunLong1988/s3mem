CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

# Add source files for windows build
add_library(osDepsLib
${CMAKE_CURRENT_SOURCE_DIR}/systemAddress.c
${CMAKE_CURRENT_SOURCE_DIR}/systemSuspend.c)
