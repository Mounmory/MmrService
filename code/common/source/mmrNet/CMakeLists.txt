project(mmrNet)

#generate_export_header(mmrUtil)

#文件分组
file(GLOB COMMON_HEADS "${CMAKE_SOURCE_DIR}/common/include/*.h" "${CMAKE_SOURCE_DIR}/common/include/*.hpp")
source_group("include/common" FILES ${COMMON_HEADS})

file(GLOB COMMON_SOURCE "${CMAKE_SOURCE_DIR}/common/source/*.cpp")
source_group("source/common" FILES ${COMMON_SOURCE})

file(GLOB FILE_HEADS "${CMAKE_SOURCE_DIR}/common/include/mmrNet/*.h" "${CMAKE_SOURCE_DIR}/common/include/mmrNet/*.hpp")
source_group("include" FILES ${FILE_HEADS})

file(GLOB FILE_SOURCES "${CMAKE_SOURCE_DIR}/common/source/mmrNet/*.cpp")
source_group("source" FILES ${FILE_SOURCES})	

add_definitions(-DMMR_NET_EXPORT)

set(INSTALL_PATH service)
if(BUILD_EXAMPLES)
	set(INSTALL_PATH ${INSTALL_PATH} example)
endif(BUILD_EXAMPLES)

build_shared_library(${PROJECT_NAME}
	#project folder in VS
	FOLDER common
	#源文件
	SRC_FILES ${COMMON_HEADS} ${COMMON_SOURCE} ${FILE_HEADS} ${FILE_SOURCES}
	TARGET_LIBRARIES mmrUtil
	TARGET_LINUX_LIBS pthread #threadPool in linux OS link library
	TARGET_INSTALL_PATH ${INSTALL_PATH}
	)
	
