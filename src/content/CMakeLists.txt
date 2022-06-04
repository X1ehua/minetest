file(GLOB content_INCS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

set(content_SRCS
	${content_INCS}
	${CMAKE_CURRENT_SOURCE_DIR}/content.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/mods.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/subgames.cpp
	PARENT_SCOPE
)
