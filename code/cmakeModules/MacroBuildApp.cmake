#
# Depends on:
#  cmake_parse_arguments ( >= CMake 2.8.3)
#

MACRO(build_app targetName)
	cmake_parse_arguments(
	MMR #prifix
    "" # no options
    "FOLDER;INSTALL_FLAG" # one value args
    "TARGET_LIBRARIES;TARGET_WIN_LIBS;TARGET_LINUX_LIBS;TARGET_SRC_FILES" # multi value args
    ${ARGN}
    )
	
	message("build_app ${targetName}")
	
	include_directories(${CMAKE_SOURCE_DIR}/common/include)
	
	include_directories(${CMAKE_SOURCE_DIR}/service)
	
	include_directories(${CMAKE_SOURCE_DIR}/service/interface)
	
	include_directories(${CMAKE_SOURCE_DIR}/service/interface/iservice)
	
	include_directories(${CMAKE_SOURCE_DIR}/tools/include)
	
	#file(GLOB COMMON_HEADS "${CMAKE_SOURCE_DIR}/common/include/*.h")
	#source_group("include" FILES ${COMMON_HEADS})

	file(GLOB FILE_HEADS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
	source_group("include" FILES ${FILE_HEADS})

	file(GLOB FILE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/*.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
	source_group("source" FILES ${FILE_SOURCES})	


	# 增加动态链接库
	add_executable(${PROJECT_NAME} #WIN32
		${MMR_TARGET_SRC_FILES}
		#${COMMON_HEADS}
		${FILE_HEADS}
		${FILE_SOURCES})
		
		#conmmon target link libs 
	if(MMR_TARGET_LIBRARIES)
		target_link_libraries(${PROJECT_NAME} ${MMR_TARGET_LIBRARIES})
	endif()
	
	#windows only target link libs
	if(WIN32 AND MMR_TARGET_WIN_LIBS)
		target_link_libraries(${PROJECT_NAME} ${MMR_TARGET_WIN_LIBS})
	endif()

	#linux only target link libs
	if(UNIX AND MMR_TARGET_LINUX_LIBS)
		target_link_libraries(${PROJECT_NAME} ${MMR_TARGET_LINUX_LIBS})
	endif()

#[[
	#set build folders
	foreach(conf DEBUG MINSIZEREL RELWITHDEBINFO RELEASE)
		if(WIN32)
		set_target_properties(${targetName}  PROPERTIES
	  		RUNTIME_OUTPUT_DIRECTORY_${conf} ${CMAKE_BINARY_DIR}/bin
			)
		else(UNIX)
			set_target_properties(${targetName} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY_${conf} ${CMAKE_BINARY_DIR}/bin
			)
		endif()
	endforeach()
]]
	if(MMR_INSTALL_FLAG)
		INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "bin")
	endif(MMR_INSTALL_FLAG)
	
ENDMACRO()