#工程名称
project(clientTestApp)

#设置protobuf源文件
add_definitions(-DPROTOBUF_USE_DLLS)

#包含的网络库文件夹
include_hv_lib_module(
	HV_MODULES cppnet cpputil hv	#要包含的文件夹
	)

include_directories(${PROTO_GENERATE_FILE_PATH}/AuthorityManager)

file(GLOB PROTO_HEADS "${PROTO_GENERATE_FILE_PATH}/AuthorityManager/*.h")
	source_group("include/proto" FILES ${PROTO_HEADS})
	
file(GLOB PROTO_SRC "${PROTO_GENERATE_FILE_PATH}/AuthorityManager/*.cc")
	source_group("source/proto" FILES ${PROTO_SRC})

if(WIN32)
set(LINK_LIBS hvNet mmrUtil libprotobuf)
else(WIN32)
set(LINK_LIBS hvNet mmrUtil protobuf)
endif(WIN32)

build_app(${PROJECT_NAME}
	TARGET_SRC_FILES ${PROTO_HEADS} ${PROTO_SRC}
	TARGET_LIBRARIES ${LINK_LIBS}
	)