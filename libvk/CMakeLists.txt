cmake_minimum_required(VERSION 3.11)
project(libvk)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(SOURCES
	include/libvk.h
	include/loader/vkloader.h
	include/loader/vk.h
	include/utils/vkutils.h
	include/utils/vkwindow.h
	src/libvk.c
	src/loader/vkloader.c
	src/loader/vk.c
	src/utils/vkutils.c
	src/utils/vkwindow.c)

set(LIBVK_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include CACHE INTERNAL "")
add_library(libvk ${SOURCES})
target_compile_options(libvk PRIVATE /W4 /WX)
include_directories(${LIBVK_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/../thirdparty/vulkan)
add_definitions(-DVK_USE_PLATFORM_WIN32_KHR -DVK_NO_PROTOTYPES)

install(TARGETS libvk DESTINATION lib)