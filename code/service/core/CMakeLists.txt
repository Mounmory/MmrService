project(serviceFramework)

include_directories(${CMAKE_SOURCE_DIR}/common/include)
include_directories(${CMAKE_SOURCE_DIR}/service/core/include)

#文件分组
file(GLOB CORE_COMMON_HEADS 
	"${CMAKE_SOURCE_DIR}/common/include/*.h")
source_group("common" FILES ${CORE_COMMON_HEADS})

file(GLOB CORE_INTERFACE 
	"${CMAKE_SOURCE_DIR}/service/interface/icore/*.h")
source_group("interface/core" FILES ${CORE_INTERFACE})

#file(GLOB CORE_INTERFACE_COMM 
#	"${CMAKE_SOURCE_DIR}/service/interface/*.h")
#source_group("interface/common" FILES ${CORE_INTERFACE_COMM})

file(GLOB CORE_HEADS 
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
source_group("include" FILES ${CORE_HEADS})

file(GLOB CORE_SOURCE 
	"${CMAKE_CURRENT_SOURCE_DIR}/suorce/*.cpp")
source_group("source" FILES ${CORE_SOURCE})

add_definitions(-DMMR_COMPO_CORE_EXPORT)

build_shared_library( serviceFramework
	#project folder in VS
	FOLDER service/core
	#源文件
	SRC_FILES 
		${CORE_INTERFACE} 
		#${CORE_INTERFACE_COMM} 
		${CORE_COMMON_HEADS} 
		${CORE_HEADS} 
		${CORE_SOURCE}
	#依赖库
	TARGET_LIBRARIES mmrUtil hvNet
	TARGET_LINUX_LIBS dl
	)