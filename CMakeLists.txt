project(searchfiles)

cmake_minimum_required(VERSION 3.24)

####################################
# src
####################################
file(GLOB_RECURSE searchfiles_sources
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")


### BOOST

# Find Boost libraries
find_package(Boost REQUIRED COMPONENTS filesystem system)

if(Boost_FOUND)
    message(STATUS "Found Boost: ${Boost_INCLUDE_DIRS}")
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(searchfiles Boost::filesystem Boost::system)
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
    include_directories(${LUCENE_INCLUDE_DIR})

    # Add the executable or library you are building
    add_executable(my_executable main.cpp)

    # Link against the Lucene++ library
    target_link_libraries(my_executable ${LUCENE_LIBRARY})

else()
    message(FATAL_ERROR "Could not find Lucene++ library")
endif()


####################################
# create executable target
####################################
add_executable(searchfiles
        ${searchfiles_sources})


####################################
# include directories
####################################
target_include_directories(searchfiles
        PRIVATE
        $<BUILD_INTERFACE:${lucene++_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${lucene++_SOURCE_DIR}/include/lucene++>
        ${Boost_INCLUDE_DIRS})


####################################
# dependencies
####################################
target_link_libraries(searchfiles
        PRIVATE
        Boost::boost
        Boost::date_time
        Boost::filesystem
        Boost::iostreams
        Boost::regex
        Boost::system
        Boost::thread
        ZLIB::ZLIB
        lucene++::lucene++
        lucene++::lucene++-contrib)
