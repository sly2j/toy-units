## Unit tests for LibPhysics v1.0


################################################################################
## Sources and headers
################################################################################
SET(SOURCES "test_unit.cc" 
            "test_vector.cc")

################################################################################
## CMAKE and Compiler Settings
################################################################################


################################################################################
## External Libraries
################################################################################
## Require the boost unit testing framework
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
## Use the dynamically linked version of the boost testing framework to minimize
## disk space
add_definitions(-DBOOST_TEST_DYN_LINK)
SET(EXT_LIBRARIES ${Boost_LIBRARIES})

################################################################################
## Compile and Link the tests
################################################################################
#if (NOT ${SOURCES} STREQUAL "")
  foreach(source ${SOURCES})
    get_filename_component(test_name ${source} NAME_WE)
    add_executable (${test_name} ${source})
    target_link_libraries(${test_name} ${EXT_LIBRARIES} ${LIBRARY})
    add_test(NAME "${test_name}" COMMAND ${test_name})
  endforeach()
#endif ()
