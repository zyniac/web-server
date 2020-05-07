cmake_minimum_required(VERSION 3.16)
include(../../cmake/generalproc.cmake)
include(../../cmake/build.cmake)

add_definitions(-DBOOST_ERROR_CODE_HEADER_ONLY -DBOOST_SYSTEM_NO_DEPRECATED)

if(DEFINED $ENV{Boost_ROOT}) # Windows Boost Path
	cmake_policy(SET CMP0074 NEW)
endif()
#----------------

addproject("util")

#-Project-Values-
set(PROJECT_NAME z-web-server)
set(PROJECT_VERSION 0.1)
set(PROJECT_DESCRIPTION "Standalone Web Server for Zyniac")
set(PROJECT_TEST test_webserver)
set(PROJECT_LIB web-server)
add_definitions(-DBUFLEN=4000)
#----------------

#-------D--------

#----------------

project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} DESCRIPTION ${PROJECT_DESCRIPTION})

#-------V--------
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(IncludeDir "${CMAKE_CURRENT_SOURCE_DIR}/Include/")
set(SourceDir "${CMAKE_CURRENT_SOURCE_DIR}/src/")

if(WIN32 AND CMAKE_SYSTEM_VERSION)
	find_package(Boost REQUIRED)
else()
	find_package(Boost REQUIRED COMPONENTS thread system filesystem iostreams)
endif()

set(IncludeDirs ${IncludeDirs} ${IncludeDir} ${Boost_INCLUDE_DIRS})
set(LibraryDirs ${LibraryDirs} ${Boost_LIBRARY_DIRS})
set(Libraries ${Libraries} ${Boost_LIBRARIES})

file(GLOB_RECURSE IncludeFiles "${IncludeDir}*.h")
file(GLOB_RECURSE SourceFiles "${SourceDir}*.cpp")

set(MainFile "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

set(LibFiles ${SourceFiles})
set(ExeFiles ${IncludeFiles} ${SourceFiles})

list(LENGTH LibFiles liblength)
list(FIND LibFiles ${MainFile} LibContainsMain)
#----------------

#----Proc-Gen----
set(MADE_BUILD FALSE)
set(MADE_TEST_BUILD FALSE)
set(MADE_LIBRARY_BUILD FALSE)

incl_tools() # rapid json

genproc()
#----------------