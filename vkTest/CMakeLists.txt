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
	

add_subdirectory(${PROJECT_SOURCE_DIR}/../libvk ${PROJECT_BINARY_DIR}/../lib/libvk)

include_directories(
	"${PROJECT_BINARY_DIR}" 
	${LIBVK_INCLUDE_DIR} 
	${PROJECT_SOURCE_DIR}/include 
	${PROJECT_SOURCE_DIR}/third_party/vulkan)
	
add_definitions(-DVK_USE_PLATFORM_WIN32_KHR -DVK_NO_PROTOTYPES)

add_executable(vkTest ${SOURCES})
target_link_libraries (vkTest libvk)
#set_property(TARGET vkTest APPEND_STRING PROPERTY LINK_FLAGS_RELEASE " ${flag}")
install(TARGETS vkTest DESTINATION bin)