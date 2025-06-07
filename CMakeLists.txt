cmake_minimum_required(VERSION 3.30)

project(zust)

file(GLOB_RECURSE Zust-src ${PROJECT_SOURCE_DIR}/src/*)
file(GLOB_RECURSE Zust-inc ${PROJECT_SOURCE_DIR}/inc/*)

add_executable(zust ${Zust-src} ${Zust-inc})
target_include_directories(zust PRIVATE inc)