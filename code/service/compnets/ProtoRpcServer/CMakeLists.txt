project(ProtoRpcServer)

#包含网络库相关头文件
include_hv_lib_module(
	HV_MODULES cppnet hv	#要包含的文件夹
	)

file(GLOB PROTO_COMM_INCLUDE "${CMAKE_SOURCE_DIR}/common/include/protoBase/*.h" 
								"${CMAKE_SOURCE_DIR}/common/include/protoBase/*.hpp")
source_group("include/proto_base" FILES ${PROTO_COMM_INCLUDE})
	
#设置依赖库文件
set(LINK_LIBS serviceFramework hvNet mmrUtil)

if(WIN32)
	set(LINK_LIBS ${LINK_LIBS} libprotobuf)
else(WIN32)
	set(LINK_LIBS ${LINK_LIBS} protobuf)
endif(WIN32)

build_service_component(
	${PROJECT_NAME}
	TARGET_SRC_FILES ${PROTO_COMM_INCLUDE}
	TARGET_LIBRARIES ${LINK_LIBS}
)
