cmake_minimum_required(VERSION 3.11)
project(vkTest)

set(SOURCES
	include/vktMemory.h
	include/vktutils.h
	include/vktwindow.h
	src/vktMemory.c
	src/vktutils.c
	src/vktwindow.c
	src/vulkan.c
	src/main.c)
	
include_directories("${PROJECT_BINARY_DIR}" ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/third_party/vulkan)
add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
add_executable(vkTest ${SOURCES})
install(TARGETS vkTest DESTINATION ${PROJECT_SOURCE_DIR}/bin)