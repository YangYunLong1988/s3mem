CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

add_library(commonLib
${CMAKE_CURRENT_SOURCE_DIR}/commonLib.c
${CMAKE_CURRENT_SOURCE_DIR}/getopt.c
${CMAKE_CURRENT_SOURCE_DIR}/tests.c)
# Windows Build environment setup
if(CMAKE_HOST_WIN32)
    add_subdirectory(win)
elseif(CMAKE_HOST_UNIX)
    add_subdirectory(unix)
endif()

