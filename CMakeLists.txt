cmake_minimum_required(VERSION 2.8)

project(DistributedSystems)

# GLib
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0>=2.23)
include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(psu_dist_lock_mgr_test psu_dist_lock_mgr_test.c
        psu_dist_lock_mgr_msg.h
        psu_dist_lock_mgr_msg_clnt.c
        psu_dist_lock_mgr_msg_xdr.c
        psu_dist_lock_mgr.c
        utility.c)

add_executable(psu_dist_lock_mgr_server psu_dist_lock_mgr_server.c
        psu_dist_lock_mgr_msg.h
        psu_dist_lock_mgr_msg_clnt.c
        psu_dist_lock_mgr_msg_svc.c
        psu_dist_lock_mgr_msg_xdr.c
        utility.c)

target_link_libraries(psu_dist_lock_mgr_test ${GLIB_LIBRARIES} pthread)
target_link_libraries(psu_dist_lock_mgr_server ${GLIB_LIBRARIES} pthread)


add_executable(psu_dsm_test psu_dsm_test.c psu_dsm_server.c
        psu_dsm_msg.h
        psu_dsm_msg_clnt.c
        psu_dsm_msg_xdr.c
        psu_dsm.c
        util.c)

add_executable(psu_dsm_server psu_dsm_server.c
        psu_dsm_msg.h
        psu_dsm_msg_clnt.c
        psu_dsm_msg_svc.c
        psu_dsm_msg_xdr.c
        util.c)

target_link_libraries(psu_dsm_test ${GLIB_LIBRARIES} pthread)
target_link_libraries(psu_dsm_server ${GLIB_LIBRARIES} pthread)


add_executable(psu_thread_test psu_thread_test.c psu_thread_server.c
        psu_thread_msg.h
        psu_thread_msg_svc.c
        psu_thread_msg_clnt.c
        psu_thread_msg_xdr.c
        psu_thread.c
        utility.c)
target_link_libraries(psu_thread_test ${GLIB_LIBRARIES} pthread)
