cmake_minimum_required(VERSION 3.22)

project(nvm C)

add_library(nvm INTERFACE)

option(USE_NVM_FLASH "Compile NVM sources" OFF)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

if(USE_NVM_FLASH)
set(NVM_SRC ${CMAKE_CURRENT_LIST_DIR}/source/nvm_flc_max32670.c)
else()
set(NVM_SRC ${CMAKE_CURRENT_LIST_DIR}/source/nvm_mb85rs.c)
endif()

target_sources(nvm INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/source/adi_nvm.c
	${CMAKE_CURRENT_LIST_DIR}/source/nvm_common.c
	${NVM_SRC}
)

target_include_directories(nvm INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include
)

