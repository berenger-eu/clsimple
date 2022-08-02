cmake_minimum_required (VERSION 3.0)
project (CLSIMPLE)

#################################################

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#################################################

option(USE_COVERAGE "Should be ON to enable code coverage" OFF)

if( USE_COVERAGE )
    message(STATUS "Use code coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")
    list(APPEND CLSIMPLE_LIBRARIES gcov)
    # Then one can use these commande:
    # lcov --directory CMakeFiles/ -c -o coverage-rapport.info
    # genhtml -o ./coverage -t "Coverage example" coverage-rapport.info
endif()

#################################################

add_executable(test.exe tests/test.cpp)
target_include_directories(test.exe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
if(CLSIMPLE_LIBRARIES)
    target_link_libraries(test.exe ${CLSIMPLE_LIBRARIES})
endif()

#################################################

add_executable(example.exe tests/example.cpp)
target_include_directories(example.exe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
if(CLSIMPLE_LIBRARIES)
    target_link_libraries(example.exe ${CLSIMPLE_LIBRARIES})
endif()


#################################################

option(USE_TESTING "Should be ON to enable unit tests" OFF)

if( USE_TESTING )
    MESSAGE(STATUS "Testing is enable")
    enable_testing()
    add_executable(utest.exe tests/utest.cpp)
    target_include_directories(utest.exe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
    if(CLSIMPLE_LIBRARIES)
        target_link_libraries(utest.exe ${CLSIMPLE_LIBRARIES})
    endif()
    add_test(utest.exe ${CMAKE_CURRENT_BINARY_DIR}/utest.exe)
endif()

#################################################

