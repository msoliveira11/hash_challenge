cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#project(Reader VERSION 1.0)
#
#file(GLOB_RECURSE SRC_FILES src/*.c)
#add_executable(Reader ${SRC_FILES})
#
#target_include_directories(Reader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

project(Reader VERSION 1.0)

SET(readerlibSRCS src/ber_tlv_reader.c src/stack.c)

ADD_LIBRARY(reader SHARED ${readerlibSRCS})
add_executable(Main src/main.c)

TARGET_LINK_LIBRARIES(Main reader)

target_include_directories(reader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(Main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
