cmake_minimum_required(VERSION 3.8)

project(DistributedSystems)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR})

set(MESSAGE_FILE msg)

ADD_CUSTOM_COMMAND(OUTPUT ${MESSAGE_FILE}.h ${MESSAGE_FILE}_clnt.c ${MESSAGE_FILE}_svc.c
        COMMAND "rpcgen"
        ARGS ${CMAKE_CURRENT_SOURCE_DIR}/${MESSAGE_FILE}.x
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${MESSAGE_FILE}.x
        COMMENT "Generating ${MESSAGE_FILE} from ${MESSAGE_FILE}.x")

add_executable(client hello_world_client.c
        ${MESSAGE_FILE}_clnt.c)

add_executable(server hello_world_server.c
        ${MESSAGE_FILE}_svc.c)
