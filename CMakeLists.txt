cmake_minimum_required(VERSION 3.21)

project(CarRental LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt auto tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 first, then fall back to Qt5
list(APPEND CMAKE_PREFIX_PATH /usr/lib/x86_64-linux-gnu/cmake/Qt6)
find_package(Qt6 QUIET COMPONENTS Widgets Sql)
if (Qt6_FOUND)
    message(STATUS "Found Qt6 version ${Qt6_VERSION}")
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets Sql)
    message(STATUS "Found Qt5 version ${Qt5_VERSION}")
endif()

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(RES_DIR ${CMAKE_SOURCE_DIR}/resources)

file(GLOB_RECURSE APP_SOURCES
    ${SRC_DIR}/*.cpp
    ${SRC_DIR}/*.h
)

add_executable(CarRental
    ${APP_SOURCES}
)

if (Qt6_FOUND)
    target_link_libraries(CarRental PRIVATE Qt6::Widgets Qt6::Sql)
else()
    target_link_libraries(CarRental PRIVATE Qt5::Widgets Qt5::Sql)
endif()

target_include_directories(CarRental PRIVATE ${SRC_DIR})

# Add resources
target_sources(CarRental PRIVATE ${RES_DIR}/resources.qrc)

# Place binary in bin/
set_target_properties(CarRental PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)


