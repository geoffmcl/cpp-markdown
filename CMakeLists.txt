# 2016-10-07 - cppmarkdown library project

cmake_minimum_required(VERSION 2.8)

project(cpp-markdown)

# add a VERSION and DATE
file(READ version.txt versionFile)
if (NOT versionFile)
    message(FATAL_ERROR "Unable to determine cpp-markdown version. version.txt file is missing.")
endif()
string(STRIP "${versionFile}" VERSION_TEXT)
string(REGEX REPLACE "(.*)[\r\n|\n](.*)" "\\1" APP_VERSION ${VERSION_TEXT})
string(REGEX REPLACE "(.*)[\r\n|\n](.*)" "\\2" APP_DATE ${VERSION_TEXT})
# Establish version number
if (APP_VERSION)
    string(REPLACE "." ";" VERSION_LIST ${APP_VERSION})
    list(GET VERSION_LIST 0 MD_MAJOR)
    list(GET VERSION_LIST 1 MD_MINOR)
    list(GET VERSION_LIST 2 MD_POINT)
else ()
    message(FATAL_ERROR "*** FAILED to get a VERSION from version.txt!")
endif ()
# Establish version date
if (APP_DATE)
    string(REPLACE "." ";" DATE_LIST ${APP_DATE})
    list(GET DATE_LIST 0 APP_YEAR)
    list(GET DATE_LIST 1 APP_MONTH)
    list(GET DATE_LIST 2 APP_DAY)
else ()
    message(FATAL_ERROR "*** FAILED to get a DATE from version.txt!")
endif ()

add_definitions ( -DMD_VERSION="${APP_VERSION}" )
add_definitions ( -DMD_DATE="${APP_YEAR}/${APP_MONTH}/${APP_DAY}" )

if (MSVC)
    set( CMAKE_DEBUG_POSTFIX "d" ) # little effect in unix
endif ()

### set(Boost_DEBUG ON)
set(Boost_USE_STATIC_LIBS ON) 
### set(Boost_USE_MULTITHREADED ON)  
### set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost COMPONENTS iostreams regex REQUIRED)
message(STATUS "*** Found Boost inc ${Boost_INCLUDE_DIRS}, lib ${Boost_LIBRARIES}")
include_directories( ${Boost_INCLUDE_DIRS} )
list(APPEND add_LIBS ${Boost_LIBRARIES} )

##########################################################
### LIBRARY
set(name cppmarkdown)
set(dir src)
set(${name}_SRCS
    ${dir}/markdown.h
	${dir}/markdown.cpp
	${dir}/markdown-tokens.h
	${dir}/markdown-tokens.cpp
    )
add_library(${name} ${${name}_SRCS})
if (UNIX)
    set_property(TARGET ${name} PROPERTY CXX_STANDARD 11)
endif ()
target_include_directories(${name} PUBLIC src)
### target_link_libraries(${name} PUBLIC boost_regex)
list(APPEND add_LIBS ${name})

###########################################################
### EXECUTABLE
set(name cpp-markdown)
set(dir src)
set(${name}_SRCS 
    ${dir}/main.cpp
    )
add_executable(${name} ${${name}_SRCS})
if (UNIX)
    set_property(TARGET ${name} PROPERTY CXX_STANDARD 11)
endif ()
target_link_libraries(${name} ${add_LIBS})
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()

# eof
