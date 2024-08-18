cmake_minimum_required(VERSION 3.24)
project(searchfiles)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

####################################
# Fetch Boost from GitHub
####################################
FetchContent_Declare(
        boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG boost-1.78.0
        SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/boost-src
        BINARY_DIR ${CMAKE_BINARY_DIR}/_deps/boost-build
)

# Optionally initialize submodules
set(FETCHCONTENT_QUIET NO)
FetchContent_MakeAvailable(boost)

# Set the Boost root directory to the fetched Boost
set(BOOST_ROOT ${CMAKE_BINARY_DIR}/_deps/boost-src)
set(BOOST_INCLUDEDIR ${BOOST_ROOT})
set(BOOST_LIBRARYDIR ${CMAKE_BINARY_DIR}/_deps/boost-build/stage/lib)

####################################
# src
####################################
file(GLOB_RECURSE searchfiles_sources
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

####################################
# create executable target
####################################
add_executable(searchfiles
        ${searchfiles_sources})

### BOOST

# Use find_package to find Boost components
set(Boost_NO_SYSTEM_PATHS TRUE)
find_package(Boost 1.78.0 REQUIRED COMPONENTS filesystem system date_time iostreams regex thread)

if(Boost_FOUND)
    message(STATUS "Found Boost: ${Boost_INCLUDE_DIRS}")
    target_include_directories(searchfiles PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(searchfiles PRIVATE Boost::filesystem Boost::system Boost::date_time Boost::iostreams Boost::regex Boost::thread)
else()
    message(FATAL_ERROR "Boost 1.78.0 not found!")
endif()

### LUCENE

# Find the Lucene++ library
find_library(LUCENE_LIBRARY NAMES lucene++ HINTS /opt/local/lib)

# Find the Lucene++ headers
find_path(LUCENE_INCLUDE_DIR NAMES lucene++/LuceneHeaders.h HINTS /opt/local/include)

# Check if the library and include path were found
if(LUCENE_LIBRARY AND LUCENE_INCLUDE_DIR)
    message(STATUS "Found Lucene++: ${LUCENE_LIBRARY}")
    target_include_directories(searchfiles PRIVATE ${LUCENE_INCLUDE_DIR})
    target_link_libraries(searchfiles PRIVATE ${LUCENE_LIBRARY})
else()
    message(FATAL_ERROR "Could not find Lucene++ library")
endif()

####################################
# Additional dependencies
####################################
find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
    target_link_libraries(searchfiles PRIVATE ZLIB::ZLIB)
else()
    message(FATAL_ERROR "ZLIB not found!")
endif()
