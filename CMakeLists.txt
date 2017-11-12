cmake_minimum_required(VERSION 2.8)

project(DistributedSystems)

# GLib
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0>=2.23)
include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR})

foreach(TASK psu_dist_lock_mgr)
    add_executable(${TASK}_test ${TASK}_test.c
            ${TASK}_msg.h
            ${TASK}_msg_clnt.c
            ${TASK}_msg_xdr.c
            ${TASK}.c
            utility.c)

    add_executable(${TASK}_server ${TASK}_server.c
            ${TASK}_msg.h
            ${TASK}_msg_clnt.c
            ${TASK}_msg_svc.c
            ${TASK}_msg_xdr.c
            utility.c)

    target_link_libraries(${TASK}_test ${GLIB_LIBRARIES} pthread)
    target_link_libraries(${TASK}_server ${GLIB_LIBRARIES} pthread)
endforeach()
