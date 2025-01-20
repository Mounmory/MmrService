#工程名称
project(ptcViewDemo)

#设置Qt库
set(QTCOMPONENT Core Gui Widgets)

# 查找Qt库
find_package(Qt5 REQUIRED ${QTCOMPONENT})

foreach(QtComp ${QTCOMPONENT})
	include_directories(${Qt5${QtComp}_INCLUDE_DIRS})
	set(QT_LIBRARIES ${QT_LIBRARIES} ${Qt5${QtComp}_LIBRARIES})
endforeach()


#文件分组
file(GLOB COMMON_INCLUDE_FILES "${CMAKE_SOURCE_DIR}/creo/creoCommon/*.h")
source_group("include/common" FILES ${COMMON_INCLUDE_FILES})
#message(COMMON_INCLUDE_FILES is ${COMMON_INCLUDE_FILES})

file(GLOB COMMON_SUORCE_FILES "${CMAKE_SOURCE_DIR}/creo/creoCommon/*.cpp")
source_group("source/common" FILES ${COMMON_SUORCE_FILES})
#message(COMMON_SUORCE_FILES is ${COMMON_SUORCE_FILES})
	
file(GLOB CURRENT_INCLUDE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
source_group("include" FILES ${CURRENT_INCLUDE_FILE})

file(GLOB CURRENT_SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
source_group("source" FILES ${CURRENT_SOURCE_FILE})	

add_definitions(-DPRO_USE_VAR_ARGS)

#STRING(REPLACE "/O2" "/Od" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})

# 增加动态链接库
add_library(${PROJECT_NAME} SHARED
	${COMMON_INCLUDE_FILES} 
	${COMMON_SUORCE_FILES}
	${CURRENT_INCLUDE_FILE}
	${CURRENT_SOURCE_FILE})
	
target_link_libraries(${PROJECT_NAME} 
	${QT_LIBRARIES}
	netapi32
	psapi
	mpr
	wsock32
	protk_dll_NU
	protk_dllmd_NU
	protkmd_NU
	protoolkit_NU
	pt_asynchronous
	ptasyncmd
	ucore
	udata
	)
#使用UTF8编译
#add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")

target_compile_options(${PROJECT_NAME} PRIVATE "/MT$<$<CONFIG:RelWithDebInfo>:>")
#TARGET_COMPILE_OPTIONS(${PROJECT_NAME} PRIVATE "/MT$<$<CONFIG:RelWithDebInfo>:>")

