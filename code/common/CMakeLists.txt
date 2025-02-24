ADD_SUBDIRECTORY(source)

if(WIN32)
	set(3RD_INSTALL_PATH service)
	if(BUILD_EXAMPLES)
		set(3RD_INSTALL_PATH ${3RD_INSTALL_PATH} example)
	endif(BUILD_EXAMPLES)
	foreach(path ${3RD_INSTALL_PATH})
		INSTALL(FILES 
			${THIRD_PART_DIR}/bin/win64/sqlite3.dll #sqlite数据库动态库文件
			${THIRD_PART_DIR}/bin/win64/libprotobuf.dll#protbuf依赖
			DESTINATION bin/${path})
	endforeach()
else(UNIX)#linux直接将源码安装到系统
	#INSTALL(FILES ${THIRD_PART_DIR}/linux/bin/libsqlite3.so
	#	DESTINATION bin)
endif()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/common/include/
	DESTINATION include/common)