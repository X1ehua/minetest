cmake_minimum_required(VERSION 3.10)
project(IrrTest)

find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)
find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include_directories($ENV{HOME}/irrlichtmt/include)

set(link_libs
    "-framework Cocoa -framework IOKit -framework CoreFoundation"
	${ZLIB_LIBRARY}
	${JPEG_LIBRARY}
	${PNG_LIBRARY}
    ${OPENGL_LIBRARIES}
    /opt/homebrew/Cellar/glfw/3.3.5/lib/libglfw.dylib
    ${FREETYPE_LIBRARY}
    $ENV{HOME}/minetest/build/lib/irrlichtmt/lib/OSX/Debug/libIrrlichtMt.a
)
message( $ENV{HOME}/irrlichtmt/lib/OSX/libIrrlichtMt.a )

# add_executable(IrrTest main.cpp ${link_libs})
add_executable(IrrTest main.cpp)
target_link_libraries(IrrTest ${link_libs})

# target_compile_options(IrrTest PUBLIC -Wno-dev)
