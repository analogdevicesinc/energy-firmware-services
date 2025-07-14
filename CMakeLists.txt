cmake_minimum_required(VERSION 3.22)

project(firmware_services C)


add_library(firmware_services INTERFACE)


set(FW_SERVICES_DIR ${CMAKE_CURRENT_LIST_DIR}/services)

option(USE_UCOMM "Compile option to use UCOMM Service" OFF)
option(USE_CLI "Compile option to use CLI Service" OFF)
option(USE_IIO "Compile IIO  related sources" OFF)
option(USE_CF "Compile option to use CF Service" OFF)
option(USE_NVM_FLASH "Compile option to use NVM Service" OFF)
option(USE_NVM_FRAM "Compile option to use NVM Service" OFF)
option(USE_PROF "Compile option to use PROF service" OFF)
option(USE_SCOMM "Compile option to use PROF service" OFF)
option(USE_DISPLAY "Compile option to use Display Service" OFF)


if(USE_CLI)
	add_subdirectory(${FW_SERVICES_DIR}/cli)
	target_link_libraries(firmware_services INTERFACE cli)
endif()

if(USE_UCOMM)
	add_subdirectory(${FW_SERVICES_DIR}/ucomm)
	target_link_libraries(firmware_services INTERFACE ucomm)
endif()

if(USE_NVM_FLASH OR USE_NVM_FRAM)
	add_subdirectory(${FW_SERVICES_DIR}/nvm)
	target_link_libraries(firmware_services INTERFACE nvm)
endif()

if(USE_PROF)
	add_subdirectory(${FW_SERVICES_DIR}/prof)
	target_link_libraries(firmware_services INTERFACE prof)
endif()

if(USE_CF)
	add_subdirectory(${FW_SERVICES_DIR}/cf_pulse)
	target_link_libraries(firmware_services INTERFACE cf)
endif()

if(USE_SCOMM)
	add_subdirectory(${FW_SERVICES_DIR}/scomm)
	target_link_libraries(firmware_services INTERFACE scomm)
endif()

if(USE_DISPLAY)
	add_subdirectory(${FW_SERVICES_DIR}/display)
	target_link_libraries(firmware_services INTERFACE display)
endif()


target_include_directories(firmware_services INTERFACE
			${FW_SERVICES_DIR}/include
)
