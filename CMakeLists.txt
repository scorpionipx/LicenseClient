cmake_minimum_required(VERSION 3.10)

# Get Version from env variable.
# This will be set by Github Workflow.
# Otherwise set it to 0.0.0 for local builds
if(DEFINED ENV{VERSION})
    set(VERSION $ENV{VERSION})
else()
    set(VERSION 0.0.0)
endif()

# Set Project
project(
    LicClient
    VERSION ${VERSION}
    DESCRIPTION "Library for handling tool/package license validation"
    HOMEPAGE_URL "https://github.com/tov101/LicenseClient"
    LANGUAGES CXX
)

# Set Vars
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CPACK_SOURCE_GENERATOR "ZIP")
# For static linking
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT")

# Uncomment these lines for pdb generation
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
# set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

# Append library paths
list(APPEND CMAKE_PREFIX_PATH "$ENV{WORKSPACE}/vcpkg/installed/x64-windows-static/share")
list(APPEND CMAKE_LIBRARY_PATH "$ENV{WORKSPACE}/vcpkg/installed/x64-windows-static/lib")
list(APPEND CMAKE_INCLUDE_PATH "$ENV{WORKSPACE}/vcpkg/installed/x64-windows-static/include")

# Lib CURL will need this CF flag just for Windows
if(WIN32)
    add_definitions("-DCURL_STATICLIB")
endif()

find_package(CImg CONFIG REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)

file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add a target for the library
add_library(LicClient SHARED ${HEADERS} ${SOURCES} version.rc)

# Link dependencies
target_link_options(LicClient PRIVATE "/NODEFAULTLIB:LIBCMT")
target_link_libraries(
    LicClient PRIVATE 
    CImg::CImg 
    CURL::libcurl 
    nlohmann_json::nlohmann_json 
    PNG::PNG 
    ZLIB::ZLIB
)

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

# include(InstallRequiredSystemLibraries)

# Include DLL files in the CPack package
install(TARGETS LicClient RUNTIME DESTINATION bin)
include(CPack)