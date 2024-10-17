cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(CMAKE_BUILD_TYPE Release CACHE STRING "build type")

project(sbgECom)

#
# Compiler configuration
#
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# Project configuration
#
option(BUILD_EXAMPLES           "Build examples" OFF)
option(BUILD_TOOLS              "Build tools" OFF)
option(USE_DEPRECATED_MACROS    "Enable deprecated preprocessor defines and macros" ON)

if (USE_DEPRECATED_MACROS)
	message(NOTICE "deprecated definitions, macros and enum values enabled")
	add_compile_definitions(SBG_ECOM_USE_DEPRECATED_MACROS)
endif()

#
# sbgECom library
#
add_library(${PROJECT_NAME} STATIC)

file(GLOB_RECURSE COMMON_SRC ${PROJECT_SOURCE_DIR}/common/*.c)
file(GLOB_RECURSE ECOM_SRC ${PROJECT_SOURCE_DIR}/src/*.c)

# Exclude platform specific files
if (NOT MSVC)
	list(REMOVE_ITEM COMMON_SRC ${PROJECT_SOURCE_DIR}/common/interfaces/sbgInterfaceSerialWin.c)
else ()
	list(REMOVE_ITEM COMMON_SRC ${PROJECT_SOURCE_DIR}/common/interfaces/sbgInterfaceSerialUnix.c)
endif()

target_sources(${PROJECT_NAME} PRIVATE ${COMMON_SRC} ${ECOM_SRC})

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${PROJECT_SOURCE_DIR}/src
		${PROJECT_SOURCE_DIR}/common
	INTERFACE
		$<INSTALL_INTERFACE:include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>)

target_compile_definitions(${PROJECT_NAME} PUBLIC SBG_COMMON_STATIC_USE)

if (MSVC)
	target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)
	target_link_libraries(${PROJECT_NAME} PUBLIC Ws2_32)
endif()

#
# Examples
#
if (BUILD_EXAMPLES)
	add_executable(airDataInput ${PROJECT_SOURCE_DIR}/examples/airDataInput/src/airDataInput.c)
	target_link_libraries(airDataInput ${PROJECT_NAME})
	install(TARGETS airDataInput DESTINATION bin/examples COMPONENT executables)

	add_executable(ellipseMinimal ${PROJECT_SOURCE_DIR}/examples/ellipseMinimal/src/ellipseMinimal.c)
	target_link_libraries(ellipseMinimal ${PROJECT_NAME})
	install(TARGETS ellipseMinimal DESTINATION bin/examples COMPONENT executables)

	add_executable(ellipseOnboardMagCalib ${PROJECT_SOURCE_DIR}/examples/ellipseOnboardMagCalib/src/ellipseOnboardMagCalib.c)
	target_link_libraries(ellipseOnboardMagCalib ${PROJECT_NAME})
	install(TARGETS ellipseOnboardMagCalib DESTINATION bin/examples COMPONENT executables)

	add_executable(hpInsMinimal ${PROJECT_SOURCE_DIR}/examples/hpInsMinimal/src/hpInsMinimal.c)
	target_link_libraries(hpInsMinimal ${PROJECT_NAME})
	install(TARGETS hpInsMinimal DESTINATION bin/examples COMPONENT executables)

	add_executable(pulseMinimal ${PROJECT_SOURCE_DIR}/examples/pulseMinimal/src/pulseMinimal.c)
	target_link_libraries(pulseMinimal ${PROJECT_NAME})
	install(TARGETS pulseMinimal DESTINATION bin/examples COMPONENT executables)
endif(BUILD_EXAMPLES)

#
# Tools
#
if (BUILD_TOOLS)
	include(FetchContent)

	FetchContent_Declare(argtable3
		GIT_REPOSITORY https://github.com/argtable/argtable3.git
		GIT_TAG v3.1.5.1c1bb23
	)

	FetchContent_GetProperties(argtable3)

	if (NOT argtable3_POPULATED)
		FetchContent_Populate(argtable3)
		add_subdirectory(${argtable3_SOURCE_DIR} ${argtable3_BINARY_DIR} EXCLUDE_FROM_ALL)
	endif()

	#
	# sbgBasicLogger tool - log sbgECom logs to CSV like files
	#
	add_executable(sbgBasicLogger
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/helpers/imuDataMean.h
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryAidings.h
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryDvl.h
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryEkf.h
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryEkfRotAccel.h
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryEvent.h
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryGeneral.h
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryGnss.h	
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryImu.h
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryMag.h
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryShipMotion.h
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerManager/loggerContext.h
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerManager/loggerEntry.h
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerManager/loggerManager.h
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerManager/loggerSettings.h
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerApp.h
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/helpers/imuDataMean.cpp
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryAidings.cpp
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryDvl.cpp
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryEkf.cpp
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryEkfRotAccel.cpp
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryEvent.cpp
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryGeneral.cpp
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryGnss.cpp
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryImu.cpp
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryMag.cpp
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerEntry/loggerEntryShipMotion.cpp
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerManager/loggerContext.cpp
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerManager/loggerEntry.cpp
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerManager/loggerManager.cpp
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerManager/loggerSettings.cpp
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/loggerApp.cpp
		${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/main.cpp
	)
	target_include_directories(sbgBasicLogger
		PRIVATE ${argtable3_SOURCE_DIR}/src
		PRIVATE ${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src
	)
	target_link_libraries(sbgBasicLogger ${PROJECT_NAME} argtable3_static)
	install(TARGETS sbgBasicLogger DESTINATION bin/tools/sbgBasicLogger COMPONENT executables)
	install(FILES tools/sbgBasicLogger/README.md DESTINATION bin/tools/sbgBasicLogger COMPONENT executables)

	#
	# sbgEComApi tool - use sbgInsRestApi GET/POS method over a serial port
	#
	add_executable(sbgEComApi ${PROJECT_SOURCE_DIR}/tools/sbgEComApi/src/main.c)
	target_include_directories(sbgEComApi PRIVATE ${argtable3_SOURCE_DIR}/src)
	target_link_libraries(sbgEComApi ${PROJECT_NAME} argtable3_static)
	install(TARGETS sbgEComApi DESTINATION bin/tools/sbgEComApi COMPONENT executables)
	install(FILES tools/sbgEComApi/README.md DESTINATION bin/tools/sbgEComApi COMPONENT executables)
endif(BUILD_TOOLS)

#
# Install
#
install(TARGETS ${PROJECT_NAME} EXPORT sbgEComTargets)
install(EXPORT sbgEComTargets
        FILE sbgEComTargets.cmake
        NAMESPACE sbg::
        DESTINATION lib/cmake/sbg)
install(DIRECTORY common/ TYPE INCLUDE FILES_MATCHING REGEX ".*\\.h")
install(DIRECTORY src/ TYPE INCLUDE FILES_MATCHING REGEX ".*\\.h")
