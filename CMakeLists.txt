cmake_minimum_required(VERSION 2.8)

project( game_proj )

# Compiler options
SET( CMAKE_CCXX_FLAGS "${CMAKE_CCXX_FLAGS} -pedantic -finline-functions -fno-strict-aliasing -std=gnu++0x " )
SET( CMAKE_CCXX_FLAGS "${CMAKE_CCXX_FLAGS} -g " )

if(DEBUG)
    SET( CMAKE_CCXX_FLAGS "${CMAKE_CCXX_FLAGS} -D__DEBUG__" )
endif(DEBUG)

MACRO( SET_FLAGS ADDITIONAL_CXX_FLAGS )
    SET( CMAKE_CXX_FLAGS "${CMAKE_CCXX_FLAGS} ${ADDITIONAL_CXX_FLAGS}" )
    SET( CMAKE_C_FLAGS   "${CMAKE_CCXX_FLAGS} ${ADDITIONAL_CXX_FLAGS}" )
ENDMACRO( SET_FLAGS )

SET_FLAGS( "-Wall" )

# compile protocol as static library
SET( PROTO_DIR "Protocol" )
SET( PROTOC "protoc" )

SET( PROTO_COMPILED_DIR "proto/" )

FILE(GLOB PROTO_FILES ${PROTO_DIR}/*.proto)
file( MAKE_DIRECTORY ${PROTO_COMPILED_DIR} )


SET( COMPILED_PROTO_FILES "" )

# make list of compiled files
FOREACH( file ${PROTO_FILES} )  
    GET_FILENAME_COMPONENT( COMPILED_FILE ${file} NAME_WE ) 
    SET( COMPILED_FILE ${PROTO_COMPILED_DIR}${COMPILED_FILE}.pb.cc )
    SET( COMPILED_PROTO_FILES ${COMPILED_PROTO_FILES} ${COMPILED_FILE} )

    ADD_CUSTOM_COMMAND(
        OUTPUT ${COMPILED_FILE}
        COMMAND ${PROTOC} ${file} -I${CMAKE_CURRENT_SOURCE_DIR}/${PROTO_DIR} --cpp_out=${PROTO_COMPILED_DIR}
        COMMENT "Compiling ${file} to cpp"
        DEPENDS ${file}
    )

ENDFOREACH()

# proto lib build
SET_FLAGS( "-w" )
ADD_LIBRARY( proto STATIC ${COMPILED_PROTO_FILES} )
SET_TARGET_PROPERTIES( proto PROPERTIES PREFIX "" )
SET_TARGET_PROPERTIES( proto PROPERTIES OUTPUT_NAME build/proto )
SET_TARGET_PROPERTIES( proto PROPERTIES LINKER_LANGUAGE C )
SET_FLAGS( "-Wall" )

# network lib build
ADD_LIBRARY( network STATIC Network/network.cpp )
TARGET_LINK_LIBRARIES( network boost_system )
SET_TARGET_PROPERTIES( network PROPERTIES PREFIX "" )
SET_TARGET_PROPERTIES( network PROPERTIES OUTPUT_NAME build/network )
SET_TARGET_PROPERTIES( network PROPERTIES LINKER_LANGUAGE C )

# add includes
INCLUDE_DIRECTORIES( Network proto )

# add build folder for project
FILE( REMOVE         ${CMAKE_BINARY_DIR}/build )
FILE( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/build )

# server compile
FILE(GLOB SRV_SRC Server/*.cpp)

# include .proto into build
# SET( SRV_SRC ${SRV_SRC} ${COMPILED_PROTO_FILES} )

ADD_EXECUTABLE(        build/server ${SRV_SRC} )
TARGET_LINK_LIBRARIES( build/server network )
ADD_DEPENDENCIES( build/server network )

# client compile
FILE(GLOB CLI_SRC Client/*.cpp)

# include .proto into build
# SET( SRV_SRC ${SRV_SRC} ${COMPILED_PROTO_FILES} )

ADD_EXECUTABLE(        build/client ${CLI_SRC} )
TARGET_LINK_LIBRARIES( build/client network )
ADD_DEPENDENCIES( build/client network )

