project(minetest)

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)

# Add custom SemiDebug build mode
set(CMAKE_CXX_FLAGS_SEMIDEBUG "-O1 -g -W0" CACHE STRING # -Wall
	"Flags used by the C++ compiler during semidebug builds."
	FORCE
)
set(CMAKE_C_FLAGS_SEMIDEBUG "-O1 -g -W0 -pedantic" CACHE STRING # -Wall
	"Flags used by the C compiler during semidebug builds."
	FORCE
)
mark_as_advanced(
	CMAKE_CXX_FLAGS_SEMIDEBUG
	CMAKE_C_FLAGS_SEMIDEBUG
)
set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
	"Choose the type of build. Options are: None Debug SemiDebug RelWithDebInfo MinSizeRel."
	FORCE
)


# Set some random things default to not being visible in the GUI
# mark_as_advanced(EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH)


if(NOT (BUILD_CLIENT OR BUILD_SERVER))
	message(WARNING "Neither BUILD_CLIENT nor BUILD_SERVER is set! Setting BUILD_SERVER=true")
	set(BUILD_SERVER TRUE)
endif()


option(ENABLE_CURL "Enable cURL support for fetching media" TRUE)
set(USE_CURL FALSE)

if(ENABLE_CURL)
	find_package(CURL)
	if (CURL_FOUND)
		message(STATUS "cURL support enabled.")
		set(USE_CURL TRUE)
	endif()
else()
	mark_as_advanced(CLEAR CURL_LIBRARY CURL_INCLUDE_DIR)
endif()

if(NOT USE_CURL)
	if(BUILD_CLIENT)
		message(WARNING "cURL is required to load the server list")
	endif()
	if(BUILD_SERVER)
		message(WARNING "cURL is required to announce to the server list")
	endif()
endif()

# find_package(GLFW)

option(ENABLE_GETTEXT "Use GetText for internationalization" OFF) # ${BUILD_CLIENT})
set(USE_GETTEXT OFF)

if(ENABLE_GETTEXT)
	message(">> Removed 2022-05-29: ENABLE_GETTEXT #63")
else()
	mark_as_advanced(GETTEXT_INCLUDE_DIR GETTEXT_LIBRARY GETTEXT_MSGFMT)
	message(STATUS "GetText disabled.")
endif()


option(ENABLE_SOUND "Enable sound" FALSE)
set(USE_SOUND FALSE)

if(BUILD_CLIENT AND ENABLE_SOUND)
	message(">> Removed 2022-05-29: BUILD_CLIENT AND ENABLE_SOUND #74")
endif()

# TODO: this should be removed one day, we can enable it unconditionally
option(ENABLE_GLES "Enable extra support code for OpenGL ES" FALSE)
mark_as_advanced(ENABLE_GLES)

option(ENABLE_TOUCH "Enable Touchscreen support" FALSE)
if(ENABLE_TOUCH)
	add_definitions(-DHAVE_TOUCHSCREENGUI)
endif()

if(BUILD_CLIENT)
	find_package(Freetype REQUIRED)
endif()

option(ENABLE_CURSES "Enable ncurses console" TRUE)
set(USE_CURSES FALSE)

if(ENABLE_CURSES)
	find_package(Ncursesw)
	if(CURSES_FOUND)
		set(USE_CURSES TRUE)
		message(STATUS "ncurses console enabled.")
		include_directories(${CURSES_INCLUDE_DIRS})
	else()
		message(STATUS "ncurses not found!")
	endif()
endif(ENABLE_CURSES)

set(ENABLE_POSTGRESQL OFF)
set(USE_POSTGRESQL FALSE)

set(ENABLE_LEVELDB OFF)
set(USE_LEVELDB FALSE)

set(ENABLE_REDIS OFF)
set(USE_REDIS FALSE)

find_package(SQLite3 REQUIRED)

# Prometheus 是用於事件監視和警報的免費軟件應用程序。
# 它將實時指標記錄在使用HTTP拉模型構建的時間序列數據庫中，具有靈活的查詢和實時警報。
# message: Prometheus client disabled.
OPTION(ENABLE_PROMETHEUS "Enable prometheus client support" FALSE)
set(USE_PROMETHEUS FALSE)

OPTION(ENABLE_SPATIAL "Enable SpatialIndex AreaStore backend" FALSE)
set(USE_SPATIAL FALSE)

find_package(ZLIB REQUIRED)
find_package(Zstd REQUIRED)

if(NOT MSVC)
	set(USE_GPROF FALSE CACHE BOOL "Use -pg flag for g++")
endif()

# Haiku endian support
if(HAIKU)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_BSD_SOURCE")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_BSD_SOURCE")
endif()

# Use cmake_config.h
add_definitions(-DUSE_CMAKE_CONFIG_H)

if(WIN32)
	message(">> Removed 2022-05-29: WIN32 #220")
else()
	# Unix probably
	if(BUILD_CLIENT)
		if(NOT HAIKU AND NOT APPLE)
			find_package(X11 REQUIRED)
		endif(NOT HAIKU AND NOT APPLE)
	endif()

	set(PLATFORM_LIBS -lpthread ${CMAKE_DL_LIBS})
	if(APPLE)
		set(PLATFORM_LIBS "-framework CoreFoundation" ${PLATFORM_LIBS})
	else()
		check_library_exists(rt clock_gettime "" HAVE_LIBRT)
		if (HAVE_LIBRT)
			set(PLATFORM_LIBS -lrt ${PLATFORM_LIBS})
		endif(HAVE_LIBRT)
	endif(APPLE)

	# Prefer local iconv if installed
	find_library(ICONV_LIBRARY iconv)
	mark_as_advanced(ICONV_LIBRARY)
	if (ICONV_LIBRARY)
		set(PLATFORM_LIBS ${PLATFORM_LIBS} ${ICONV_LIBRARY})
	endif()
	if (HAIKU)
		set(PLATFORM_LIBS ${PLATFORM_LIBS} intl network)
	endif()

endif()

check_include_files(endian.h HAVE_ENDIAN_H)

configure_file(
	"${PROJECT_SOURCE_DIR}/cmake_config.h.in"
	"${PROJECT_BINARY_DIR}/cmake_config.h"
)


# Add a target that always rebuilds cmake_config_githash.h
add_custom_target(GenerateVersion
	COMMAND ${CMAKE_COMMAND}
	-D "GENERATE_VERSION_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
	-D "GENERATE_VERSION_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}"
	-D "VERSION_STRING=${VERSION_STRING}"
	-D "DEVELOPMENT_BUILD=${DEVELOPMENT_BUILD}"
	-P "${CMAKE_SOURCE_DIR}/cmake/Modules/GenerateVersion.cmake"
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)


add_subdirectory(threading)
add_subdirectory(content)
add_subdirectory(database)
add_subdirectory(gui)
add_subdirectory(mapgen)
add_subdirectory(network)
add_subdirectory(script)
add_subdirectory(unittest)
add_subdirectory(benchmark)
add_subdirectory(util)
add_subdirectory(irrlicht_changes)
add_subdirectory(server)

set(common_SRCS
	${database_SRCS}
	${mapgen_SRCS}
	${server_SRCS}
	${content_SRCS}
	ban.cpp
	chat.cpp
	clientiface.cpp
	collision.cpp
	content_mapnode.cpp
	content_nodemeta.cpp
	convert_json.cpp
	craftdef.cpp
	debug.cpp
	defaultsettings.cpp
	emerge.cpp
	environment.cpp
	face_position_cache.cpp
	filesys.cpp
	gettext.cpp
	httpfetch.cpp
	hud.cpp
	inventory.cpp
	inventorymanager.cpp
	itemdef.cpp
	itemstackmetadata.cpp
	light.cpp
	log.cpp
	main.cpp
	map.cpp
	map_settings_manager.cpp
	mapblock.cpp
	mapnode.cpp
	mapsector.cpp
	metadata.cpp
	modchannels.cpp
	nameidmapping.cpp
	nodedef.cpp
	nodemetadata.cpp
	nodetimer.cpp
	noise.cpp
	objdef.cpp
	object_properties.cpp
	particles.cpp
	pathfinder.cpp
	player.cpp
	porting.cpp
	profiler.cpp
	raycast.cpp
	reflowscan.cpp
	remoteplayer.cpp
	rollback.cpp
	rollback_interface.cpp
	serialization.cpp
	server.cpp
	serverenvironment.cpp
	serverlist.cpp
	settings.cpp
	staticobject.cpp
	terminal_chat_console.cpp
	texture_override.cpp
	tileanimation.cpp
	tool.cpp
	translation.cpp
	version.cpp
	voxel.cpp
	voxelalgorithms.cpp
	hud.cpp
	${common_network_SRCS}
	${JTHREAD_SRCS}
	${common_SCRIPT_SRCS}
	${UTIL_SRCS}
)

if(BUILD_UNITTESTS)
	set(common_SRCS ${common_SRCS} ${UNITTEST_SRCS})
endif()

if(BUILD_BENCHMARKS)
	set(common_SRCS ${common_SRCS} ${BENCHMARK_SRCS})
endif()

# This gives us the icon and file version information
if(WIN32)
	message(">> Removed 2022-05-29: WIN32 #367")
endif()


# Client sources
if (BUILD_CLIENT)
	add_subdirectory(client)
endif(BUILD_CLIENT)

set(client_SRCS
	${client_SRCS}
	${common_SRCS}
	${gui_SRCS}
	${client_network_SRCS}
	${client_irrlicht_changes_SRCS}
	${client_SCRIPT_SRCS}
)

if(BUILD_UNITTESTS)
	set(client_SRCS ${client_SRCS} ${UNITTEST_CLIENT_SRCS})
endif()

if(BUILD_BENCHMARKS)
	set(client_SRCS ${client_SRCS} ${BENCHMARK_CLIENT_SRCS})
endif()

list(SORT client_SRCS)

# Server sources
set(server_SRCS
	${common_SRCS}
)
list(SORT server_SRCS)

# Avoid source_group on broken CMake version.
# see issue #7074 #7075
if (CMAKE_VERSION VERSION_GREATER 3.8.1)
	source_group(TREE ${PROJECT_SOURCE_DIR} PREFIX "Source Client" FILES ${client_SRCS})
	source_group(TREE ${PROJECT_SOURCE_DIR} PREFIX "Source Server" FILES ${server_SRCS})
endif()

include_directories(
	${PROJECT_BINARY_DIR}
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/script
)
include_directories(SYSTEM
	${ZLIB_INCLUDE_DIR}
	${ZSTD_INCLUDE_DIR}
	${SQLITE3_INCLUDE_DIR}
	${LUA_INCLUDE_DIR}
	${GMP_INCLUDE_DIR}
	${JSON_INCLUDE_DIR}
	${LUA_BIT_INCLUDE_DIR}
)

if(USE_GETTEXT)
	include_directories(${GETTEXT_INCLUDE_DIR})
endif()

if(BUILD_CLIENT)
	include_directories(SYSTEM
		${FREETYPE_INCLUDE_DIRS}
		${SOUND_INCLUDE_DIRS}
		${X11_INCLUDE_DIR}
	)
endif()

if(USE_CURL)
	include_directories(${CURL_INCLUDE_DIR})
endif()

# message(">> MINETEST_ROOT_DIR: ${MINETEST_ROOT_DIR}")
include_directories(
	${MINETEST_ROOT_DIR}/lib/irrlichtmt/include
	${MINETEST_ROOT_DIR}/lib/irrlichtmt/source/Irrlicht
)

# When cross-compiling assume the user doesn't want to run the executable anyway,
# otherwise place it in <source dir>/bin/ since Minetest can only run from there.
if(NOT CMAKE_CROSSCOMPILING)
	set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_INSTALL_PREFIX}/${BINDIR})
	set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")
	# message(">> CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR} #451")
	# message(">> EXECUTABLE_OUTPUT_PATH: ${EXECUTABLE_OUTPUT_PATH}")
endif()

message(">> CMAKE_XCODE_SCHEME_WORKING_DIRECTORY: ${CMAKE_XCODE_SCHEME_WORKING_DIRECTORY}")
if(BUILD_CLIENT)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	add_executable(${PROJECT_NAME} ${client_SRCS} ${extra_windows_SRCS})
	add_dependencies(${PROJECT_NAME} GenerateVersion)
	target_link_libraries(
		${PROJECT_NAME}
		${ZLIB_LIBRARIES}
		# "${CMAKE_SOURCE_DIR}/lib/irrlichtmt/lib/OSX/libIrrlichtMt.a"
		IrrlichtMt::IrrlichtMt
		${ZSTD_LIBRARY}
		${X11_LIBRARIES}
		${SOUND_LIBRARIES}
		${SQLITE3_LIBRARY}
		${LUA_LIBRARY}
		${GMP_LIBRARY}
		${JSON_LIBRARY}
		${LUA_BIT_LIBRARY}
		${FREETYPE_LIBRARY}
		${PLATFORM_LIBS}
	)
	if(NOT USE_LUAJIT)
		set_target_properties(${PROJECT_NAME} PROPERTIES
			# This is necessary for dynamic Lua modules
			# to work when Lua is statically linked (issue #10806)
			ENABLE_EXPORTS 1
		)
	endif()

	if(USE_GETTEXT)
		target_link_libraries(
			${PROJECT_NAME}
			${GETTEXT_LIBRARY}
		)
	endif()
	if(USE_CURL)
		target_link_libraries(
			${PROJECT_NAME}
			${CURL_LIBRARY}
		)
	endif()
	if(FREETYPE_PKGCONFIG_FOUND)
		set_target_properties(${PROJECT_NAME}
			PROPERTIES
			COMPILE_FLAGS "${FREETYPE_CFLAGS_STR}"
	)
	endif()
	if (USE_CURSES)
		target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES})
	endif()
	if(BUILD_BENCHMARKS)
		target_link_libraries(${PROJECT_NAME} catch2)
	endif()
endif(BUILD_CLIENT)

# set_target_properties(${PROJECT_NAME}
#     PROPERTIES
#     # ARCHIVE_OUTPUT_DIRECTORY "/home/xy/cmake_practice/lib/"
#     # LIBRARY_OUTPUT_DIRECTORY "/home/xy/cmake_practice/lib/"
#     RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
# )

if(BUILD_SERVER)
	message(">> Removed 2022-05-29: BUILD_SERVER #516")
endif(BUILD_SERVER)

# Blacklisted locales that don't work.
# see issue #4638
set(GETTEXT_BLACKLISTED_LOCALES ar dv he hi kn ms_Arab th)

option(APPLY_LOCALE_BLACKLIST "Use a blacklist to avoid known broken locales" TRUE)

if (GETTEXTLIB_FOUND AND APPLY_LOCALE_BLACKLIST)
	set(GETTEXT_USED_LOCALES "")
	foreach(LOCALE ${GETTEXT_AVAILABLE_LOCALES})
		if (NOT "${LOCALE}" IN_LIST GETTEXT_BLACKLISTED_LOCALES)
			list(APPEND GETTEXT_USED_LOCALES ${LOCALE})
		endif()
	endforeach()
	message(STATUS "Locale blacklist applied; Locales used: ${GETTEXT_USED_LOCALES}")
elseif (GETTEXTLIB_FOUND)
	set(GETTEXT_USED_LOCALES ${GETTEXT_AVAILABLE_LOCALES})
endif()

# Set some optimizations and tweaks

include(CheckCSourceCompiles)

if(MSVC)
	message(">> Removed 2022-05-29: MSVC #542")
else()
	# GCC or compatible compilers such as Clang
	set(WARNING_FLAGS "-Wall -Wextra")
	set(WARNING_FLAGS "${WARNING_FLAGS} -Wno-unused-parameter -Wno-implicit-fallthrough")
	if(WARN_ALL)
		set(RELEASE_WARNING_FLAGS "${WARNING_FLAGS}")
	else()
		set(RELEASE_WARNING_FLAGS "")
	endif()

	if(APPLE AND USE_LUAJIT)
		# required per http://luajit.org/install.html
		SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -pagezero_size 10000 -image_base 100000000")
	elseif(UNIX AND USE_LUAJIT)
		check_c_source_compiles("#ifndef __aarch64__\n#error\n#endif\nint main(){}" IS_AARCH64)
		if(IS_AARCH64)
			# Move text segment below LuaJIT's 47-bit limit (see issue #9367)
			if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
				# FreeBSD uses lld, and lld does not support -Ttext-segment, suggesting
				# --image-base instead. Not sure if it's equivalent change for the purpose
				# but at least if fixes build on FreeBSD/aarch64
				# XXX: the condition should also be changed to check for lld regardless of
				# os, bit CMake doesn't have anything like CMAKE_LINKER_IS_LLD yet
				SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--image-base=0x200000000")
			else()
				SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Ttext-segment=0x200000000")
			endif()
		endif()
	endif()

	if(MINGW)
		set(OTHER_FLAGS "${OTHER_FLAGS} -mthreads -fexceptions")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWIN32_LEAN_AND_MEAN")
	endif()

	# Use a safe subset of flags to speed up math calculations:
	# - we don't need errno or math exceptions
	# - we don't deal with Inf/NaN or signed zero
	set(MATH_FLAGS "-fno-math-errno -fno-trapping-math -ffinite-math-only -fno-signed-zeros")

	# Enable SSE for floating point math on 32-bit x86 by default
	# reasoning see minetest issue #11810 and https://gcc.gnu.org/wiki/FloatingPointMath
	if(CMAKE_SIZEOF_VOID_P EQUAL 4)
		check_c_source_compiles("#ifndef __i686__\n#error\n#endif\nint main(){}" IS_I686)
		if(IS_I686)
			message(STATUS "Detected Intel x86: using SSE instead of x87 FPU")
			set(OTHER_FLAGS "${OTHER_FLAGS} -mfpmath=sse -msse")
		endif()
	endif()

	set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG ${RELEASE_WARNING_FLAGS} ${OTHER_FLAGS} -pipe -funroll-loops")
	if(CMAKE_SYSTEM_NAME MATCHES "(Darwin|BSD|DragonFly)")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")
	else()
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fomit-frame-pointer")
		if(CMAKE_SYSTEM_NAME STREQUAL "Linux"
				AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
				AND CMAKE_CXX_COMPILER_VERSION MATCHES "^9\\.")
			# Clang 9 has broken -ffast-math on glibc
		else()
			set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${MATH_FLAGS}")
		endif()
	endif()
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE} -g")
	set(CMAKE_CXX_FLAGS_SEMIDEBUG "-g -O1 ${WARNING_FLAGS} ${OTHER_FLAGS}")
	set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 ${WARNING_FLAGS} ${OTHER_FLAGS}")

	if(USE_GPROF)
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg")
	endif()

	if(MINGW)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mwindows")
	endif()
endif()


# Installation

if(WIN32)
	message(">> Removed 2022-05-29: WIN32 #623")
endif()

if(BUILD_CLIENT)
	# 下面全部注释掉似乎没有影响?

	# https://cmake.org/cmake/help/latest/command/install.html#targets
	# install(TARGETS ${PROJECT_NAME}
	# 	RUNTIME DESTINATION ${BINDIR} # .dll on Windows
	# 	LIBRARY DESTINATION ${BINDIR} # shared libs, except DLL & on macOS ma
	# 	ARCHIVE DESTINATION ${BINDIR} # .lib on Windows, 相比之下 macOS .a 使用 FRAMEWORK
	# 	BUNDLE DESTINATION .
	# )

	# message(">> CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
	if(APPLE)
		# CMAKE_INSTALL_PREFIX: default /usr/local, set to /opt/homebrew/Cellar/minetest/5.5.0 in ../CMakeLists.txt
		# BUNDLE_PATH: minetest.app
		# message(">> CMAKE_INSTALL_PREFIX/BUNDLE_PATH: ${CMAKE_INSTALL_PREFIX} / ${BUNDLE_PATH}")
		# BINDIR: minetest.app/Contents/MacOS
		# message(">> CMAKE_INSTALL_PREFIX/BINDIR: ${CMAKE_INSTALL_PREFIX} / ${BINDIR}")

		# install(CODE "
		# 	set(BU_CHMOD_BUNDLE_ITEMS ON)
		# 	include(BundleUtilities)
		# 	fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/${BUNDLE_PATH}\" \"\" \"\${CMAKE_INSTALL_PREFIX}/${BINDIR}\")
		# " COMPONENT Runtime)
	endif()
	# message(">> Installation Skipped!")

	if(USE_GETTEXT)
		message(">> Removed 2022-05-29: BUILD_CLIENT >> USE_GETTEXT #644")
	endif()

	# install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../fonts" DESTINATION "${SHAREDIR}"
	# 		FILES_MATCHING PATTERN "*.ttf" PATTERN "*.txt")
endif(BUILD_CLIENT)

# if(BUILD_SERVER)
#	install(TARGETS ${PROJECT_NAME}server DESTINATION ${BINDIR})
# endif()

if (USE_GETTEXT)
	message(">> Removed 2022-05-29: USE_GETTEXT #656")
endif()
