set (BOOST_MIN_VERSION "1.64.0")
set (Boost_NO_BOOST_CMAKE ON)
#set (Boost_NO_SYSTEM_PATHS ON)
set (Boost_USE_STATIC_LIBS ON CACHE BOOL "use static libraries from Boost")
set (Boost_USE_STATIC_RUNTIME ON)
set (Boost_USE_MULTITHREADED ON)

find_package(Boost ${BOOST_MIN_VERSION} REQUIRED)

if (NOT Boost_FOUND)
  message(FATAL_ERROR "Fatal error: Boost (version >= 1.64) required.")
else()
  message(STATUS "Setting up BOOST")
  message(STATUS "BOOST_ROOT is ${BOOST_ROOT}")  
  message(STATUS " Includes - ${Boost_INCLUDE_DIRS}")
  message(STATUS " Library  - ${Boost_LIBRARY_DIRS}")
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
  link_directories(${BOOST_LIBRARYDIR})
endif (NOT Boost_FOUND)

# set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
# set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
# set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
# find_package(Boost 1.66.0 REQUIRED COMPONENTS system)
# if(Boost_FOUND)
#     message("Found Boost.")
#     include_directories(${Boost_INCLUDE_DIRS})
# endif() 