project(ptcViewWidget)

#设置Qt库
set(QTCOMPONENT Core Gui Widgets AxContainer)

# 查找Qt库
find_package(Qt5 REQUIRED ${QTCOMPONENT})

foreach(QtComp ${QTCOMPONENT})
	include_directories(${Qt5${QtComp}_INCLUDE_DIRS})
	set(QT_LIBRARIES ${QT_LIBRARIES} ${Qt5${QtComp}_LIBRARIES})
endforeach()

#文件分组
file(GLOB COMMON_INTERFACE_FILES "${CMAKE_SOURCE_DIR}/creo/creoCommon/interface/*.h")
source_group("interface" FILES ${COMMON_INTERFACE_FILES})

file(GLOB FILE_HEADS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
source_group("include" FILES ${FILE_HEADS})

file(GLOB FILE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
source_group("source" FILES ${FILE_SOURCES})	

add_definitions(-DPTCVIEWWIDGET_LIB)

# 增加动态链接库
add_library(${PROJECT_NAME} SHARED
	${COMMON_INTERFACE_FILES} 
	${FILE_HEADS}
	${FILE_SOURCES})
	
target_link_libraries(${PROJECT_NAME} 
	${QT_LIBRARIES}
	netapi32
	psapi
	mpr
	wsock32
	)

