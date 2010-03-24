cmake_minimum_required (VERSION 2.6)
project (rl-learner) 

if(WIN32)
	set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/boost CACHE PATH "Location of the boost package")
	if    (NOT EXISTS "${BOOST_ROOT}" OR NOT IS_DIRECTORY "${BOOST_ROOT}")
		MESSAGE(FATAL_ERROR "BOOST_ROOT is not a valid directory")
	endif (NOT EXISTS "${BOOST_ROOT}" OR NOT IS_DIRECTORY "${BOOST_ROOT}")


	set(Boost_COMPILER "-vc80")
endif()


#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_MULTITHREADED OFF)

#set(Boost_FIND_COMPONENTS TRUE)

#FIND_PACKAGE( Boost )
#set(BOOST_ROOT "C:/Program Files/boost/boost_1_42") 
#set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib) 
#set(BOOST_INCLUDEDIR ${BOOST_ROOT}) 

FIND_PACKAGE( Boost COMPONENTS program_options REQUIRED  )

message(STATUS "Boost_LIBRARY_DIRS=${Boost_LIBRARY_DIRS}")
message(STATUS "Boost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS}")


link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} )

link_libraries(${Boost_LIBRARIES})

AUX_SOURCE_DIRECTORY(src SRCS)
add_executable (rl-learner ${SRCS})