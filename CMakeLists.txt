## CMake project file for LibPhysics v1.0

################################################################################
## PROJECT: LibPhysics
################################################################################
cmake_minimum_required (VERSION 2.8)
project (LibPhysics)
set (LIBRARY "physics")
set (VERSION "1")
set (SOVERSION "1.0")

################################################################################
## Sources and headers
################################################################################
set (SOURCES "physics/util/configuration.cc"
             "physics/util/io.cc"
             "physics/util/logger.cc")
set (HEADERS "physics/unit/constants.hh"
             "physics/unit/detail.hh"
             "physics/unit/io.hh"
             "physics/unit/math.hh"
             "physics/unit/prefix.hh"
             "physics/unit/prototype.hh"
             "physics/unit/standard.hh"
             "physics/unit/type_traits.hh"
             "physics/unit.hh"
             "physics/util/assert.hh"
             "physics/util/configuration.hh"
             "physics/util/exception.hh"
             "physics/util/io.hh"
             "physics/util/logger.hh"
             "physics/util/math.hh"
             "physics/util/mixin.hh"
             "physics/util/root.hh"
             "physics/util/stringify.hh"
             "physics/util/type_traits.hh"
             "physics/vector/io.hh"
             "physics/vector/prototype.hh"
             "physics/vector.hh")

################################################################################
## CMAKE and Compiler Settings
################################################################################
## make sure that the default is RELEASE
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE RELEASE CACHE STRING
      "Choose the type of build, options are: None Debug Release."
      FORCE)
endif (NOT CMAKE_BUILD_TYPE)
## extra cmake modules
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} 
#    "${CMAKE_SOURCE_DIR}/cmake/Modules/")
## CXX FLAGS (currently only supports gcc (>=4.8) and clang)
## -O1 for debug to take care of a problem with constexpr in gcc
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++1y -fPIC")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O1 -std=c++1y -fPIC")
set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fPIC")
set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fPIC")
## Project source dir is in the include path
include_directories(BEFORE ${PROJECT_SOURCE_DIR})
## Unit testing for this project
enable_testing()
add_subdirectory(test)

################################################################################
## External Libraries
################################################################################
## require BOOST program_options library
find_package(Boost COMPONENTS program_options filesystem system REQUIRED)
include_directories(AFTER ${Boost_INCLUDE_DIRS})
#set(EXT_LIBRARIES "")

################################################################################
## Compile and Link (if there is anything to build)
################################################################################
#if (NOT ${SOURCES} STREQUAL "")
  add_library(${LIBRARY} SHARED ${SOURCES} ${HEADERS})
  target_link_libraries(${LIBRARY} ${EXT_LIBRARIES} ${Boost_LIBRARIES})
  set_target_properties(${LIBRARY} PROPERTIES VERSION ${VERSION} SOVERSION ${SOVERSION})
  #endif ()

################################################################################
## Install
################################################################################
## 1. install all headers
foreach (header ${HEADERS})
  get_filename_component(header_path ${header} PATH)
  install (FILES ${header} DESTINATION include/${header_path})
endforeach()
## 2. install the shared library if it was built
#if (NOT ${SOURCES} STREQUAL "")
  install (TARGETS ${LIBRARY} DESTINATION lib)
  #endif ()
