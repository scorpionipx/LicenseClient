cmake_minimum_required(VERSION 3.10)
project(
    LicClient
    VERSION 1.0.0
    DESCRIPTION "Library for handling tool/package license validation"
    HOMEPAGE_URL "https://github.com/tov101/LicenseClient"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Uncomment these lines for pdb generation
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
# set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

list(APPEND CMAKE_PREFIX_PATH "$ENV{WORKSPACE}/vcpkg/installed/x64-windows/share")

find_package(nlohmann_json REQUIRED)

file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add a target for the library
add_library(LicClient SHARED ${HEADERS} ${SOURCES} version.rc)

# Link dependencies
target_link_libraries(LicClient PRIVATE nlohmann_json::nlohmann_json)

# Include version header
configure_file(src/Version.h.in Version.h)
configure_file(src/version.rc.in version.rc @ONLY)

# Specify directories to include when compiling
target_include_directories(LicClient PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# If on Windows, set specific properties for DLL
if(WIN32)
    set_target_properties(
        LicClient
        PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE
    )
endif()