ADD_SUBDIRECTORY(source)

if(WIN32)
	INSTALL(FILES 
		${THIRD_PART_DIR}/bin/win64/sqlite3.dll #sqlite数据库动态库文件
		${THIRD_PART_DIR}/bin/win64/libprotobuf.dll#protbuf依赖
		DESTINATION bin)
else(UNIX)#linux直接将源码安装到系统
	#INSTALL(FILES ${THIRD_PART_DIR}/linux/bin/libsqlite3.so
	#	DESTINATION bin)
endif()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/common/include/
	DESTINATION include/common)