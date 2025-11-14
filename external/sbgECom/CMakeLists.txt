cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Set project metadata
project(sbgECom VERSION 5.3.2276 LANGUAGES C;CXX)

#
# Compiler configuration
#
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

#
# Project configuration options
#
option(BUILD_EXAMPLES           "Build examples" OFF)
option(BUILD_TOOLS              "Build tools" OFF)
option(USE_DEPRECATED_MACROS    "Enable deprecated preprocessor defines and macros" ON)

# Display chosen options
message(STATUS "C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Examples: ${BUILD_EXAMPLES}")
message(STATUS "Build Tools: ${BUILD_TOOLS}")
message(STATUS "Use Deprecated Macros: ${USE_DEPRECATED_MACROS}")

#
# sbgECom library
#
if (USE_DEPRECATED_MACROS)
    add_compile_definitions(SBG_ECOM_USE_DEPRECATED_MACROS)
endif()

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
# Fetch and make available dependencies for both Tools and Examples code
#
if (BUILD_EXAMPLES OR BUILD_TOOLS)
    include(FetchContent)

    # Set options for argtable3 before fetching it
    set(BUILD_SHARED_LIBS           OFF CACHE BOOL "Build shared library")
    set(ARGTABLE3_ENABLE_CONAN      OFF CACHE BOOL "Enable Conan dependency manager")
    set(ARGTABLE3_ENABLE_TESTS      OFF CACHE BOOL "Enable unit tests")
    set(ARGTABLE3_ENABLE_EXAMPLES   OFF CACHE BOOL "Enable examples")

    FetchContent_Declare(
        argtable3
        GIT_REPOSITORY https://github.com/argtable/argtable3.git
        GIT_TAG v3.2.2.f25c624
    )
    
    FetchContent_MakeAvailable(argtable3)
endif()

#
# Examples
#
if (BUILD_EXAMPLES)
    # Build airDataInput example
    add_executable(airDataInput ${PROJECT_SOURCE_DIR}/examples/airDataInput/src/main.c)
    target_link_libraries(airDataInput PRIVATE ${PROJECT_NAME})
    install(TARGETS airDataInput DESTINATION bin/examples COMPONENT executables)

    # Build ellipseLegacy example
    add_executable(ellipseLegacy ${PROJECT_SOURCE_DIR}/examples/ellipseLegacy/src/main.c)
    target_link_libraries(ellipseLegacy PRIVATE ${PROJECT_NAME})
    install(TARGETS ellipseLegacy DESTINATION bin/examples COMPONENT executables)

    # Build ellipseOnboardMagCalib example
    add_executable(ellipseOnboardMagCalib ${PROJECT_SOURCE_DIR}/examples/ellipseOnboardMagCalib/src/main.c)
    target_link_libraries(ellipseOnboardMagCalib PRIVATE ${PROJECT_NAME})
    install(TARGETS ellipseOnboardMagCalib DESTINATION bin/examples COMPONENT executables)

    # Build sbgEComExample
    add_executable(sbgEComExample
        ${PROJECT_SOURCE_DIR}/examples/sbgEComExample/src/cJSON.c
        ${PROJECT_SOURCE_DIR}/examples/sbgEComExample/src/main.c
        ${PROJECT_SOURCE_DIR}/examples/sbgEComExample/src/restApiHelper.c)
    
    target_include_directories(sbgEComExample PRIVATE ${argtable3_SOURCE_DIR}/src)
    target_link_libraries(sbgEComExample PRIVATE ${PROJECT_NAME} argtable3)
    install(TARGETS sbgEComExample DESTINATION bin/examples COMPONENT executables)
endif()

#
# Tools
#
if (BUILD_TOOLS)
    # List all source/header files for sbgBasicLogger tool
    file(GLOB_RECURSE SBGBASICLOGGER_SRC
        ${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/*.cpp
        ${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src/*.h
    )

    # Build sbgBasicLogger tool
    add_executable(sbgBasicLogger ${SBGBASICLOGGER_SRC})
    target_include_directories(sbgBasicLogger
        PRIVATE ${argtable3_SOURCE_DIR}/src
        PRIVATE ${PROJECT_SOURCE_DIR}/tools/sbgBasicLogger/src
    )
    target_link_libraries(sbgBasicLogger PRIVATE ${PROJECT_NAME} argtable3)

    install(TARGETS sbgBasicLogger DESTINATION bin/tools/sbgBasicLogger COMPONENT executables)
    install(FILES tools/sbgBasicLogger/README.md DESTINATION bin/tools/sbgBasicLogger COMPONENT executables)

    # Build sbgEComApi tool
    add_executable(sbgEComApi ${PROJECT_SOURCE_DIR}/tools/sbgEComApi/src/main.c)
    target_include_directories(sbgEComApi PRIVATE ${argtable3_SOURCE_DIR}/src)
    target_link_libraries(sbgEComApi PRIVATE ${PROJECT_NAME} argtable3)
    
    install(TARGETS sbgEComApi DESTINATION bin/tools/sbgEComApi COMPONENT executables)
    install(FILES tools/sbgEComApi/README.md DESTINATION bin/tools/sbgEComApi COMPONENT executables)
endif()

#
# Install the main library target
#

# Export the target so it can be used by other projects
install(TARGETS ${PROJECT_NAME}
    EXPORT sbgEComTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include)

# Export CMake targets for use in other projects
install(EXPORT sbgEComTargets
        FILE sbgEComTargets.cmake
        NAMESPACE sbgECom::
        DESTINATION lib/cmake/sbgECom)

# Create a version file for compatibility checks
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/sbgEComConfigVersion.cmake"
    VERSION 5.3.2276
    COMPATIBILITY SameMajorVersion
)

# Create a config file to describe the package
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/sbgEComConfig.in.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/sbgEComConfig.cmake"
    INSTALL_DESTINATION lib/cmake/sbgECom
)

# Install cmake config  and version files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/sbgEComConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/sbgEComConfigVersion.cmake"
    DESTINATION lib/cmake/sbgECom
)

# Install header files from common and src directories
install(DIRECTORY common/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY src/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")
