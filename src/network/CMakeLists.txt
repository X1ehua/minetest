file(GLOB common_network_INCS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
list(REMOVE_ITEM common_network_INCS
	${CMAKE_CURRENT_SOURCE_DIR}/clientopcodes.h
)

set(common_network_SRCS
	${common_network_INCS}
	${CMAKE_CURRENT_SOURCE_DIR}/address.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/connection.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/connectionthreads.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/networkpacket.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/serverpackethandler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/serveropcodes.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/socket.cpp
	PARENT_SCOPE
)

if (BUILD_CLIENT)
	set(client_network_SRCS
		${CMAKE_CURRENT_SOURCE_DIR}/clientopcodes.h
		${CMAKE_CURRENT_SOURCE_DIR}/clientopcodes.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/clientpackethandler.cpp
		PARENT_SCOPE
	)
endif()
