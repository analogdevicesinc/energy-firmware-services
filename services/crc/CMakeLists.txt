cmake_minimum_required(VERSION 3.22)

project(crc C)

add_library(crc INTERFACE)


target_sources(crc INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/source/adi_crc.c
	${CMAKE_CURRENT_LIST_DIR}/source/adi_crc_ccitt16.c
	${CMAKE_CURRENT_LIST_DIR}/source/adi_crc_lut.c
)

target_include_directories(crc INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include
)
