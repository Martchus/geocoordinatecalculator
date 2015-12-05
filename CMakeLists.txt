cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

# meta data
set(META_PROJECT_NAME geocoordinatecalculator)
set(META_APP_NAME "geocoordinatecalculator")
set(META_APP_AUTHOR "Martchus")
set(META_APP_URL "https://github.com/${META_APP_AUTHOR}/${META_PROJECT_NAME}")
set(META APP_DESCRIPTION "Command line tool for basic calculations with geo coordinates such as format conversions and calculation of distance, bearing, mid point, destination and more.")
set(META_VERSION_MAJOR 1)
set(META_VERSION_MINOR 0)
set(META_VERSION_PATCH 1)

# define project
project(${META_PROJECT_NAME})

# stringification of meta data
set(META_PROJECT_NAME_STR "\"${META_PROJECT_NAME}\"")
set(META_APP_NAME_STR "\"${META_APP_NAME}\"")
set(META_APP_AUTHOR_STR "\"${META_APP_AUTHOR}\"")
set(META_APP_URL_STR "\"${META_APP_URL}\"")
set(APP_DESCRIPTION_STR "\"${APP_DESCRIPTION}\"")
set(META_APP_VERSION_STR "\"${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}\"")

# add configuration header
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)
include_directories("${PROJECT_BINARY_DIR}")

# add source and header files
file(GLOB PROJ_FILES "*.h" "*.cpp" "*.c")

# executable and linking
add_executable(${META_PROJECT_NAME} ${PROJ_FILES})
target_link_libraries(${META_PROJECT_NAME} c++utilities)

# enable C++11
set_property(TARGET ${META_PROJECT_NAME} PROPERTY CXX_STANDARD 11)

# add install target
install(TARGETS ${META_PROJECT_NAME} DESTINATION bin)
