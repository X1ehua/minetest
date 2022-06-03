cmake_minimum_required(VERSION 3.10)
project(IrrTest)

find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)
find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)

set(CMAKE_CXX_STANDARD 17)

set(PARENT_APP_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory("lib/irrlichtmt" EXCLUDE_FROM_ALL)

set(link_libs
    "-framework Cocoa -framework IOKit -framework CoreFoundation"
	${ZLIB_LIBRARY}
	${JPEG_LIBRARY}
	${PNG_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${FREETYPE_LIBRARY}
    IrrlichtMt::IrrlichtMt
)

file(GLOB includes "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h")
add_executable(IrrTest
    ${includes}
    main.cpp
)
target_link_libraries(IrrTest ${link_libs})

# add_compile_options(-Xobjective-c++ ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
# set_source_files_properties(main.cpp PROPERTIES COMPILE_FLAGS -Xobjective-c++)

set(dependencies
	IrrlichtMt::IrrlichtMt
)
add_dependencies(IrrTest ${dependencies})
