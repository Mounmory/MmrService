include_directories(${CMAKE_SOURCE_DIR}/service/interface)

ADD_SUBDIRECTORY(app)
ADD_SUBDIRECTORY(core)
ADD_SUBDIRECTORY(compnets)

#[[
#install docker contrl files
if(UNIX)
	install(FILES ${THIRD_PART_DIR}/docker/Dockerfile 
		${THIRD_PART_DIR}/docker/buildDocker.sh 
		${THIRD_PART_DIR}/docker/startDocker.sh
		DESTINATION bin)
endif(UNIX)
]]