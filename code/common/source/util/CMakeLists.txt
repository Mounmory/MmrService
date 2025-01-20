project(mmrUtil)

#generate_export_header(mmrUtil)

#文件分组
file(GLOB COMMON_HEADS "${CMAKE_SOURCE_DIR}/common/include/*.h" "${CMAKE_SOURCE_DIR}/common/include/*.hpp")
source_group("include" FILES ${COMMON_HEADS})

file(GLOB COMMON_SOURCE "${CMAKE_SOURCE_DIR}/common/source/*.cpp")
source_group("include" FILES ${COMMON_SOURCE})

file(GLOB FILE_HEADS "${CMAKE_SOURCE_DIR}/common/include/util/*.h" "${CMAKE_SOURCE_DIR}/common/include/util/*.hpp")
source_group("include/util" FILES ${FILE_HEADS})

file(GLOB FILE_SOURCES "${CMAKE_SOURCE_DIR}/common/source/util/*.cpp")
source_group("source/util" FILES ${FILE_SOURCES})	

add_definitions(-DMMR_COMMON_EXPORT)

build_shared_library( mmrUtil
	#project folder in VS
	FOLDER common
	#源文件
	SRC_FILES ${COMMON_HEADS} ${COMMON_SOURCE} ${FILE_HEADS} ${FILE_SOURCES}
	TARGET_LINUX_LIBS pthread #threadPool in linux OS link library
	)
	
