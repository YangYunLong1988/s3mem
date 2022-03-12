CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT (s3mem VERSION 0.0.1)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/config.h" @ONLY)
include_directories(${PROJECT_BINARY_BIN})

# Setup build folder
set(CMAKE_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add common source files
add_executable(s3mem main.c)

# Add Header files folder path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add Cmake module path
# set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/modules")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules")

if(CMAKE_HOST_WIN32)
    # Config WINDOWS SDK environment
    find_package(WindowsSDK)
    message("WINDOWSSDK_FOUND: ${WINDOWSSDK_FOUND}")
    message("WINDOWSSDK_LATEST_DIR: ${WINDOWSSDK_LATEST_DIR}")
    message("WINDOWSSDK_LATEST_NAME: ${WINDOWSSDK_LATEST_NAME}")
    message("WINDOWSSDK_FOUND_PREFERENCE: ${WINDOWSSDK_FOUND_PREFERENCE}")
    message("WINDOWSSDK_PREFERRED_DIR: ${WINDOWSSDK_PREFERRED_DIR}")
    message("WINDOWSSDK_PREFERRED_NAME: ${WINDOWSSDK_PREFERRED_NAME}")
    message("WINDOWSSDK_DIRS: ${WINDOWSSDK_DIRS}")
    message("WINDOWSSDK_PREFERRED_FIRST_DIRS: ${WINDOWSSDK_PREFERRED_FIRST_DIRS}")

    if(WINDOWSSDK_FOUND)
        # Set Windows SDK Include and library path
        set(WINDOWSSDK_INCLUDE_DIR)
        set(WINDOWSSDK_LIBRARY_DIR)
        get_windowssdk_include_dirs("${WINDOWSSDK_LATEST_DIR}" WINDOWSSDK_INCLUDE_DIR)
        get_windowssdk_library_dirs("${WINDOWSSDK_LATEST_DIR}" WINDOWSSDK_LIBRARY_DIR)
        message("WINDOWSSDK_INCLUDE_DIR: ${WINDOWSSDK_INCLUDE_DIR}")
        message("WINDOWSSDK_LIBRARY_DIR: ${WINDOWSSDK_LIBRARY_DIR}")

        # Set windows SDK binraries path
        set(WINDOWSSDK_BUILD)
        windowssdk_build_lookup("${WINDOWSSDK_LATEST_DIR}" WINDOWSSDK_BUILD)
        message("WINDOWSSDK_BUILD: ${WINDOWSSDK_BUILD}")
        set(WINDOWSSDK_BIN_PATH "${WINDOWSSDK_LATEST_DIR}/bin/${WINDOWSSDK_LATEST_DIR}.0")
        if("$ENV{PROCESSOR_ARCHITECTURE}" STREQUAL "AMD64")
            set(WINDOWSSDK_BIN_PATH "${WINDOWSSDK_BIN_PATH}/x64")
        elseif("$ENV{PROCESSOR_ARCHITECTURE}" STREQUAL "x86")
            set(WINDOWSSDK_BIN_PATH "${WINDOWSSDK_BIN_PATH}/x86")
        else()
            message(FATAL_ERROR "FATAL: Processor Architecture is incorrect")
        endif()

        LIST(APPEND CMAKE_PROGRAM_PATH  "${WINDOWSSDK_BIN_PATH}")

        #
    else()
        message(FATAL_ERROR "FATAL: Not found the windows SDK Installed.
            Please check your WINDOWS SDK Installation.")
    endif()

    include_directories(${WINDOWSSDK_INCLUDE_DIR})
    link_directories(${WINDOWSSDK_LIBRARY_DIR})
endif()

add_subdirectory(library)

if(STATIC_LINK)
	if(CMAKE_HOST_UNIX)
		set(CMAKE_EXE_LINKER_FLAGS "-static")
	endif()
endif()

target_link_libraries(s3mem
PUBLIC commonLib
PUBLIC osDepsLib)
