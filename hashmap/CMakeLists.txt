cmake_minimum_required(VERSION 3.11)
project(hashmap)

set(SOURCES
	include/hash/chaining.h
	include/hash/robinhood.h
	include/hash/fnv1a.h
	include/pcg/pcg_basic.h
	include/test/dataset.h
	src/hash/chaining.c
	src/hash/robinhood.c
	src/hash/fnv1a.c
	src/pcg/pcg_basic.c
	src/test/dataset.c
	src/main.c)
	
include_directories("${PROJECT_BINARY_DIR}" ${PROJECT_SOURCE_DIR}/include)
add_executable(hashmap ${SOURCES})
install(TARGETS hashmap DESTINATION ${PROJECT_SOURCE_DIR}/bin)