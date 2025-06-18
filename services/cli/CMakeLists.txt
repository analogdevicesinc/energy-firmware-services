cmake_minimum_required(VERSION 3.22)

project(cli C)

add_library(cli INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

option(USE_IIO "Compile IIO board_support sources" OFF)

# Add project symbols (macros)
target_compile_definitions(cli INTERFACE
	USE_HAL_DRIVER
	$<$<CONFIG:Debug>:DEBUG>
)
include(CMakePrintHelpers)

set (SOURCES
	${CMAKE_CURRENT_LIST_DIR}/source/adi_cli.c
	${CMAKE_CURRENT_LIST_DIR}/source/cli_dispatch.c
	${CMAKE_CURRENT_LIST_DIR}/source/cli_history.c
	${CMAKE_CURRENT_LIST_DIR}/source/cli_interface.c
	${CMAKE_CURRENT_LIST_DIR}/source/cli_utils.c
)

if(USE_IIO)
	set (SOURCES ${SOURCES} ${CMAKE_CURRENT_LIST_DIR}/source/adi_cli_iiod_xml.c)
endif()

target_sources(cli INTERFACE ${SOURCES} )

target_include_directories(cli INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include
)
