project(AuthorityManager)

add_definitions(-DPROTOBUF_USE_DLLS)

include_directories(${PROTO_GENERATE_FILE_PATH}/${PROJECT_NAME})

#包含网络库相关头文件
file(GLOB PROTO_INCLUDE "${PROTO_GENERATE_FILE_PATH}/${PROJECT_NAME}/*.h")
	source_group("include/proto" FILES ${PROTO_INCLUDE})
	
file(GLOB PROTO_SOURCE "${PROTO_GENERATE_FILE_PATH}/${PROJECT_NAME}/*.cc")
	source_group("source/proto" FILES ${PROTO_SOURCE})
	
#设置依赖库文件
set(LINK_LIBS serviceFramework mmrUtil)

if(WIN32)
	set(LINK_LIBS ${LINK_LIBS} libprotobuf sqlite3)
else(WIN32)
	set(LINK_LIBS ${LINK_LIBS} protobuf sqlite3)
endif(WIN32)

build_service_component(
	${PROJECT_NAME}
	TARGET_SRC_FILES ${PROTO_INCLUDE} ${PROTO_SOURCE}
	TARGET_LIBRARIES ${LINK_LIBS}
	TARGET_INSTALL_PATH service
)
