cmake_minimum_required(VERSION 3.24)
project(searchfiles)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_BUILD_TYPE Debug)

####################################
# Find Boost Libraries
####################################
find_package(Boost 1.78.0 REQUIRED COMPONENTS filesystem system date_time iostreams regex thread)

if(Boost_FOUND)
    message(STATUS "Found Boost: ${Boost_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "Boost 1.78.0 not found!")
endif()

####################################
# src
####################################
file(GLOB_RECURSE searchfiles_sources
        "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

####################################
# create executable target
####################################
add_executable(searchfiles
        ${searchfiles_sources} SynoTerms.cpp SynoTerms.h SynoAnalyzer.cpp SynoAnalyzer.h SearchResults.cpp SearchResults.h)

### BOOST

# Add Boost directories and link libraries
target_include_directories(searchfiles PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(searchfiles PRIVATE Boost::boost Boost::filesystem Boost::system Boost::date_time Boost::iostreams Boost::regex Boost::thread)

### LUCENE

# Find the Lucene++ library
find_library(LUCENE_LIBRARY NAMES lucene++ HINTS /opt/local/lib)

# Find the Lucene++ Contrib library
find_library(LUCENE_CONTRIB_LIBRARY NAMES lucene++-contrib HINTS /opt/local/lib)

# Find the Lucene++ headers
find_path(LUCENE_INCLUDE_DIR NAMES lucene++/LuceneHeaders.h HINTS /opt/local/include)

# Check if the library and include path were found
if(LUCENE_LIBRARY AND LUCENE_INCLUDE_DIR AND LUCENE_CONTRIB_LIBRARY)
    message(STATUS "Found Lucene++: ${LUCENE_LIBRARY}")
    message(STATUS "Found Lucene++ Contrib: ${LUCENE_CONTRIB_LIBRARY}")

    # Include the Lucene++ headers
    target_include_directories(searchfiles PRIVATE ${LUCENE_INCLUDE_DIR})

    # Include the Lucene++ Contrib headers
    target_include_directories(searchfiles PRIVATE ${LUCENE_CONTRIB_INCLUDE_DIR})

    # Link the Lucene++ and Lucene++ Contrib libraries
    target_link_libraries(searchfiles PRIVATE ${LUCENE_LIBRARY} ${LUCENE_CONTRIB_LIBRARY})
else()
    if(NOT LUCENE_LIBRARY OR NOT LUCENE_INCLUDE_DIR)
        message(FATAL_ERROR "Could not find Lucene++ library")
    endif()

    if(NOT LUCENE_CONTRIB_LIBRARY)
        message(FATAL_ERROR "Could not find Lucene++ Contrib library")
    endif()
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
