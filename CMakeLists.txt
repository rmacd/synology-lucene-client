project(searchfiles)

cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

# Find Boost libraries
find_package(Boost REQUIRED COMPONENTS filesystem system date_time iostreams regex thread)

if(Boost_FOUND)
    message(STATUS "Found Boost: ${Boost_INCLUDE_DIRS}")
    target_include_directories(searchfiles PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(searchfiles PRIVATE Boost::filesystem Boost::system Boost::date_time Boost::iostreams Boost::regex Boost::thread)
else()
    message(FATAL_ERROR "Boost not found!")
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
