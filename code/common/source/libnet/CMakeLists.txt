project(hvNet)

#包含头文件
include_directories(${COMMON_INCLUDE_LIBNET_DIR}/base)
include_directories(${COMMON_INCLUDE_LIBNET_DIR}/cppnet)
include_directories(${COMMON_INCLUDE_LIBNET_DIR}/cpputil)
include_directories(${COMMON_INCLUDE_LIBNET_DIR}/event)
include_directories(${COMMON_INCLUDE_LIBNET_DIR}/http)
include_directories(${COMMON_INCLUDE_LIBNET_DIR}/util)
include_directories(${COMMON_INCLUDE_LIBNET_DIR}/hv)

if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS -D_WIN32_WINNT=0x0600)
    set(LIBS ${LIBS} secur32 crypt32 winmm iphlpapi ws2_32)
    if(ENABLE_WINDUMP)
        add_definitions(-DENABLE_WINDUMP)
        set(LIBS ${LIBS} dbghelp)
    endif()
else(UNIX)
 set(LIBS ${LIBS} pthread)
 add_definitions(-DHAVE_PTHREAD_H -DHAVE_SYS_TIME_H)
endif()

#文件分组
file(GLOB NET_BASE_HEADS "${COMMON_INCLUDE_LIBNET_DIR}/base/*.h" "${COMMON_INCLUDE_LIBNET_DIR}/base/*.hpp")
source_group("include/base" FILES ${NET_BASE_HEADS})
file(GLOB NET_BASE_SOURCE "${CMAKE_SOURCE_DIR}/common/source/libnet/base/*.c" "${CMAKE_SOURCE_DIR}/common/source/libnet/base/*.cpp")
source_group("source/base" FILES ${NET_BASE_SOURCE})

file(GLOB NET_CPPNET_HEADS "${COMMON_INCLUDE_LIBNET_DIR}/cppnet/*.h" "${COMMON_INCLUDE_LIBNET_DIR}/cppnet/*.hpp")
source_group("include/cppnet" FILES ${NET_CPPNET_HEADS})
file(GLOB NET_CPPNET_SOURCE "${CMAKE_SOURCE_DIR}/common/source/libnet/cppnet/*.c" "${CMAKE_SOURCE_DIR}/common/source/libnet/cppnet/*.cpp")
source_group("source/cppnet" FILES ${NET_CPPNET_SOURCE})

file(GLOB NET_CPPUTIL_HEADS "${COMMON_INCLUDE_LIBNET_DIR}/cpputil/*.h" "${COMMON_INCLUDE_LIBNET_DIR}/cpputil/*.hpp")
source_group("include/cpputil" FILES ${NET_CPPUTIL_HEADS})
file(GLOB NET_CPPUTIL_SOURCE "${CMAKE_SOURCE_DIR}/common/source/libnet/cpputil/*.c" "${CMAKE_SOURCE_DIR}/common/source/libnet/cpputil/*.cpp")
source_group("source/cpputil" FILES ${NET_CPPUTIL_SOURCE})

file(GLOB NET_EVENT_HEADS 
	"${COMMON_INCLUDE_LIBNET_DIR}/event/kcp/*.h" 
	"${COMMON_INCLUDE_LIBNET_DIR}/event/*.h" 
	"${COMMON_INCLUDE_LIBNET_DIR}/event/*.hpp")
file(GLOB NET_EVENT_SOURCE "${CMAKE_SOURCE_DIR}/common/source/libnet/event/*.c" 
	"${CMAKE_SOURCE_DIR}/common/source/libnet/event/kcp/*.c" 
	"${CMAKE_SOURCE_DIR}/common/source/libnet/event/*.cpp")
if(WIN32)
	file(GLOB NET_EVENT_HEADS ${NET_EVENT_HEADS} "${COMMON_INCLUDE_LIBNET_DIR}/event/wepoll/*.h")
	file(GLOB NET_EVENT_SOURCE ${NET_EVENT_SOURCE} "${CMAKE_SOURCE_DIR}/common/source/libnet/event/wepoll/*.c") 
endif()
source_group("include/event" FILES ${NET_EVENT_HEADS})
source_group("source/event" FILES ${NET_EVENT_SOURCE})

file(GLOB NET_HTTP_HEADS "${COMMON_INCLUDE_LIBNET_DIR}/http/*.h" "${COMMON_INCLUDE_LIBNET_DIR}/http/*.hpp")
source_group("include/http" FILES ${NET_HTTP_HEADS})
file(GLOB NET_HTTP_SOURCE "${CMAKE_SOURCE_DIR}/common/source/libnet/http/*.c" "${CMAKE_SOURCE_DIR}/common/source/libnet/http/*.cpp")
source_group("source/http" FILES ${NET_HTTP_SOURCE})

file(GLOB NET_UTIL_HEADS "${COMMON_INCLUDE_LIBNET_DIR}/util/*.h" "${COMMON_INCLUDE_LIBNET_DIR}/util/*.hpp")
source_group("include/util" FILES ${NET_UTIL_HEADS})
file(GLOB NET_UTIL_SOURCE "${CMAKE_SOURCE_DIR}/common/source/libnet/util/*.c" "${CMAKE_SOURCE_DIR}/common/source/libnet/util/*.cpp")
source_group("source/util" FILES ${NET_UTIL_SOURCE})

file(GLOB NET_SSL_HEADS "${COMMON_INCLUDE_LIBNET_DIR}/ssl/*.h" "${COMMON_INCLUDE_LIBNET_DIR}/ssl/*.hpp")
source_group("include/ssl" FILES ${NET_SSL_HEADS})
file(GLOB NET_SSL_SOURCE "${CMAKE_SOURCE_DIR}/common/source/libnet/ssl/*.c" "${CMAKE_SOURCE_DIR}/common/source/libnet/ssl/*.cpp")
source_group("source/ssl" FILES ${NET_SSL_SOURCE})
#add_definitions(-DMMR_COMMON_EXPORT)

build_shared_library( hvNet
	#project folder in VS
	FOLDER common
	#源文件
	SRC_FILES ${NET_BASE_HEADS} ${NET_BASE_SOURCE} 
		${NET_CPPNET_HEADS} ${NET_CPPNET_SOURCE}
		${NET_CPPUTIL_HEADS} ${NET_CPPUTIL_SOURCE}
		${NET_EVENT_HEADS} ${NET_EVENT_SOURCE}
		${NET_HTTP_HEADS} ${NET_HTTP_SOURCE}
		${NET_UTIL_HEADS} ${NET_UTIL_SOURCE}
		${NET_SSL_HEADS} ${NET_SSL_SOURCE}
	TARGET_WIN_LIBS ${LIBS}
	TARGET_LINUX_LIBS ${LIBS} #threadPool in linux OS link library
	)
#[[	
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
	DESTINATION include/common)

if(WIN32)
	INSTALL(FILES ${THIRD_PART_DIR}/bin/win64/sqlite3.dll 
		DESTINATION bin)
else(UNIX)#linux直接将源码安装到系统
	#INSTALL(FILES ${THIRD_PART_DIR}/linux/bin/libsqlite3.so
	#	DESTINATION bin)
endif()

]]	