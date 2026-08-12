cmake_minimum_required(VERSION 3.16)

project(MiniPOS
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Sql)

include(CTest)

set(BOOST_ROOT "" CACHE PATH "Path to the extracted Boost directory")

if(BUILD_TESTING)
    if(NOT EXISTS "${BOOST_ROOT}/boost/test/included/unit_test.hpp")
        message(FATAL_ERROR
            "Boost headers were not found. Set BOOST_ROOT to the extracted Boost directory."
        )
    endif()
endif()

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()