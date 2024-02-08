cmake_minimum_required(VERSION 3.10)
project(LicenseClient VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add your source files
set(SOURCES
    src/Client.cpp
    src/dllmain.cpp
    src/framework.h
    src/pch.cpp
    src/wrapper.cpp
    src/licfile/LicFile.cpp
    src/security/Security.cpp
    src/server/Server.cpp
    src/utils/utils.cpp
    src/validator/Validator.cpp
)

# Add headers for IDE support
include_directories(src)

# Add dependencies
include_directories(vcpkg/installed/x64-windows/include)

# Add a target for the library
add_library(LicenseClient SHARED ${SOURCES})

# Include version header
configure_file(src/Version.h.in Version.h)

# Specify directories to include when compiling
target_include_directories(LicenseClient PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# If on Windows, set specific properties for DLL
if(WIN32)
    set_target_properties(LicenseClient PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS TRUE
    )
endif()

# Define installation directories (if needed)
# For example, if you want to install your library
# into /usr/local/lib on Unix-like systems
#install(TARGETS LicenseClient DESTINATION /usr/local/lib)

# Define installation for headers (if needed)
#install(DIRECTORY src/ DESTINATION include
#        FILES_MATCHING PATTERN "*.h")
