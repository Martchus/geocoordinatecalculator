cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

# add project files
set(HEADER_FILES
    angle.h
    location.h
    main.h
    utils.h
)
set(SRC_FILES
    angle.cpp
    location.cpp
    main.cpp
    utils.cpp
)

set(DOC_FILES
    README.md
)

# meta data
set(META_PROJECT_NAME geocoordinatecalculator)
set(META_PROJECT_TYPE application)
set(META_APP_NAME "Geo coordinate calculator")
set(META_APP_AUTHOR "Martchus")
set(META_APP_URL "https://github.com/${META_APP_AUTHOR}/${META_PROJECT_NAME}")
set(META_APP_DESCRIPTION "Command line tool for basic calculations with geo coordinates such as format conversions and calculation of distance, bearing, mid point, destination and more")
set(META_VERSION_MAJOR 1)
set(META_VERSION_MINOR 1)
set(META_VERSION_PATCH 2)
set(META_APP_VERSION ${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH})

# find c++utilities
find_package(c++utilities 4.0.0 REQUIRED)
use_cpp_utilities()

# include modules to apply configuration
include(BasicConfig)
include(WindowsResources)
include(AppTarget)
include(ShellCompletion)
include(ConfigHeader)
