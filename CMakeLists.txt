cmake_minimum_required(VERSION 3.5)

# Set policies up to 3.9 since we want to enable the IPO option
cmake_policy(VERSION 3.9)

# This can be read from ${PROJECT_NAME} after project() is called
project(minetest)
set(PROJECT_NAME_CAPITALIZED "Minetest")

set(PARENT_APP_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CLANG_MINIMUM_VERSION "3.5") # set(GCC_MINIMUM_VERSION 5.1) removed

# Also remember to set PROTOCOL_VERSION in network/networkprotocol.h when releasing
set(VERSION_MAJOR 5)
set(VERSION_MINOR 6)
set(VERSION_PATCH 0)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(DEVELOPMENT_BUILD TRUE) # Change to false for releases

set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}-dev")

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	set(VERSION_STRING "${VERSION_STRING}-debug") # Append "-debug" to version string
endif()
message(STATUS ">>> Will build version ${VERSION_STRING} <<<")

# Configuration options
set(RUN_IN_PLACE TRUE)

set(BUILD_CLIENT TRUE CACHE BOOL "Build client")
set(BUILD_SERVER FALSE CACHE BOOL "Build server")
set(BUILD_UNITTESTS FALSE) # CACHE BOOL "Build unittests")
set(BUILD_BENCHMARKS FALSE CACHE BOOL "Build benchmarks") # add_subdirectory(lib/catch2) while TRUE
message(">> BUILD_UNITTESTS: ${BUILD_UNITTESTS}")

set(WARN_ALL TRUE CACHE BOOL "Enable -Wall for Release build")

if(NOT CMAKE_BUILD_TYPE)
	# Default to release
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type: Debug or Release" FORCE)
endif()

# Included stuff
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/irrlichtmt")
message(STATUS "Using user-provided IrrlichtMt at subdirectory 'lib/irrlichtmt'")
if(BUILD_CLIENT)
	# tell IrrlichtMt to create a static library
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared library" FORCE)
	add_subdirectory(lib/irrlichtmt EXCLUDE_FROM_ALL)
	unset(BUILD_SHARED_LIBS CACHE)

	if(NOT TARGET IrrlichtMt)
		message(FATAL_ERROR "IrrlichtMt project is missing a CMake target?!")
	endif()
else()
	add_library(IrrlichtMt::IrrlichtMt INTERFACE IMPORTED)
	set_target_properties(IrrlichtMt::IrrlichtMt PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/lib/irrlichtmt/include")
endif()

if(TARGET IrrlichtMt::IrrlichtMt)
	message(STATUS "Found IrrlichtMt, version: ${IrrlichtMt_VERSION}")
endif()


# Installation

message(">> CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
if(WIN32) # removed
elseif(APPLE)
	# set(CMAKE_INSTALL_PREFIX "/opt/homebrew/Cellar/minetest/5.5.0")
	set(BUNDLE_NAME ${PROJECT_NAME}.app)
	set(BUNDLE_PATH "${BUNDLE_NAME}")
	set(BINDIR ${PARENT_APP_ROOT_DIR}/bin/${BUNDLE_NAME}/Contents/MacOS)
	# set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_INSTALL_PREFIX}/${BINDIR})
	# set(CMAKE_XCODE_SCHEME_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	set(CMAKE_XCODE_ATTRIBUTE_BUILT_PRODUCTS_DIR ${BINDIR})
	set(SHAREDIR ${BUNDLE_NAME}/Contents/Resources)
	set(DOCDIR "${SHAREDIR}/${PROJECT_NAME}")
	set(EXAMPLE_CONF_DIR ${DOCDIR})
	set(LOCALEDIR "${SHAREDIR}/locale")
	message(">> BINDIR: ${BINDIR} <<")
endif() # removed: elseif(UNIX) # Linux, BSD etc

# Deleted: CUSTOM_SHAREDIR _BINDIR _DOCDIR _MANDIR _EXAMPLE_CONF_DIR _XDG_APPS_DIR _ICONDIR _LOCALEDIR 

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/builtin" DESTINATION "${SHAREDIR}")
if(RUN_IN_PLACE)
	install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/mods/mods_here.txt" DESTINATION "${SHAREDIR}/mods")
	install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/textures/texture_packs_here.txt" DESTINATION "${SHAREDIR}/textures")
endif()

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/games/minetest_game" DESTINATION "${SHAREDIR}/games/"
	COMPONENT "SUBGAME_MINETEST_GAME" OPTIONAL PATTERN ".git*" EXCLUDE )
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/games/devtest" DESTINATION "${SHAREDIR}/games/"
	COMPONENT "SUBGAME_MINIMAL" OPTIONAL PATTERN ".git*" EXCLUDE )

if(BUILD_CLIENT)
	install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/client/shaders" DESTINATION "${SHAREDIR}/client")
	install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/textures/base/pack" DESTINATION "${SHAREDIR}/textures/base")
	if(RUN_IN_PLACE)
		install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/clientmods" DESTINATION "${SHAREDIR}")
		install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/client/serverlist" DESTINATION "${SHAREDIR}/client")
	endif()
endif()

# install(FILES README.md doc/*.txt minetest.conf.example) 7 lines deleted

if(APPLE)
	install(FILES "misc/minetest-icon.icns" DESTINATION "${SHAREDIR}")
	install(FILES "misc/Info.plist" DESTINATION "${BUNDLE_PATH}/Contents")
endif()

# Library pack
find_package(GMP REQUIRED)
find_package(Json REQUIRED)
find_package(Lua REQUIRED)
if(NOT USE_LUAJIT)
	set(LUA_BIT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/bitop)
	set(LUA_BIT_LIBRARY bitop)
	add_subdirectory(lib/bitop)
endif()

# Subdirectories. Be sure to add all relevant definitions above this
add_subdirectory(src)

# CPack 是 CMake 的一个内置工具，用于创建软件的二进制包和源代码包。https://zhuanlan.zhihu.com/p/141956373
# CPack 在整个 CMake 工具链的位置: 
# CMake [Project file generation(config -> generate)] -> Build -> CTest[Test] -> CPack[Package]
# 83 lines deleted about CPack
