cmake_minimum_required(VERSION 3.8)

project(DistributedSystems)

# GLib
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0>=2.23)
include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR})

foreach(TASK psu_dist_lock_mgr)
    ADD_CUSTOM_COMMAND(OUTPUT ${TASK}_msg.h ${TASK}_msg_clnt.c ${TASK}_msg_svc.c
            COMMAND "rpcgen"
            ARGS ${CMAKE_CURRENT_SOURCE_DIR}/${TASK}_msg.x
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${TASK}_msg.x
            COMMENT "Generating ${TASK}_msg from ${TASK}_msg.x")

    add_executable(${TASK}_test ${TASK}_test.c
            ${TASK}_msg_clnt.c
            ${TASK}.c)

    add_executable(${TASK}_server ${TASK}_server.c
            ${TASK}_msg_svc.c)

    target_link_libraries(${TASK}_test ${GLIB_LIBRARIES})
    target_link_libraries(${TASK}_server ${GLIB_LIBRARIES})
endforeach()
