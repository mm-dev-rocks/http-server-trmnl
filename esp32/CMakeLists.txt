# ESP-IDF 6.x needs at least 3.16
cmake_minimum_required(VERSION 3.16)
# Extra cmake stuff
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# Name the project
project(http_parse_tests)
