include_directories(${CMAKE_SOURCE_DIR}/common/include)
include_directories(${PROTO_FILE_PATH}/include)
include_directories(${CMAKE_SOURCE_DIR}/service/core/include)
include_directories(${CMAKE_SOURCE_DIR}/service/interface)

SUBDIRLIST(SRC_DIR_LIST ${CMAKE_CURRENT_SOURCE_DIR})

# 查找所有子文件夹和文件夹
foreach(subdirectory ${SRC_DIR_LIST})
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${subdirectory}/CMakeLists.txt")
		add_subdirectory(${subdirectory})
		if(TARGET ${subdirectory})
		 SET_PROPERTY(TARGET ${subdirectory} PROPERTY FOLDER "service/component")
		endif(TARGET ${subdirectory})
	endif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${subdirectory}/CMakeLists.txt")
endforeach()