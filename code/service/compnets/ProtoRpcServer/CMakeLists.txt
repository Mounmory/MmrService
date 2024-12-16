project(ProtoRpcServer)

#包含网络库相关头文件
#include_directories(${COMMON_INCLUDE_LIBNET_DIR}/cpputil)

include_directories(${COMMON_INCLUDE_LIBNET_DIR}/cppnet)

include_directories(${COMMON_INCLUDE_LIBNET_DIR}/hv)


file(GLOB PROTO_COMM_INCLUDE "${COMMON_INCLUDE_DIR}/protoBase/*.h" "${COMMON_INCLUDE_DIR}/protoBase/*.hpp")
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
