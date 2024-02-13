cmake_minimum_required(VERSION 3.10)
project(LicenseClient VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
list(APPEND CMAKE_PREFIX_PATH "$ENV{WORKSPACE}\\vcpkg\\installed\\x64-windows\\share")

find_package(nlohmann_json REQUIRED)

# Add your source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add a target for the library
add_library(LicenseClient SHARED ${SOURCES})

# Link dependencies
target_link_libraries(LicenseClient PRIVATE nlohmann_json::nlohmann_json)

# Include version header
configure_file(src/Version.h.in Version.h)

# Specify directories to include when compiling
target_include_directories(LicenseClient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# If on Windows, set specific properties for DLL
if(WIN32)
    set_target_properties(LicenseClient PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()