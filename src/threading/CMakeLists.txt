file(GLOB threading_INCS "./*.h")

set(JTHREAD_SRCS
	${threading_INCS}
	${CMAKE_CURRENT_SOURCE_DIR}/event.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/thread.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/semaphore.cpp
	PARENT_SCOPE
)

