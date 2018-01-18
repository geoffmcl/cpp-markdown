# 2016-10-07 - cppmarkdown library project

cmake_minimum_required(VERSION 2.8)

project(cpp-markdown)

if (MSVC)
    set( CMAKE_DEBUG_POSTFIX "d" ) # little effect in unix
endif ()

### set(Boost_DEBUG ON)
set(Boost_USE_STATIC_LIBS ON) 
### set(Boost_USE_MULTITHREADED ON)  
### set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.62.0 COMPONENTS regex REQUIRED)
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
target_link_libraries(${name} ${add_LIBS})
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()

# eof
