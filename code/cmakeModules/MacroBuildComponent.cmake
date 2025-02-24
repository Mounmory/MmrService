#
# Depends on:
#  cmake_parse_arguments ( >= CMake 2.8.3)
#

MACRO(build_service_component targetName)
	cmake_parse_arguments(
	MMR
    "" # no options
    "NAME;EXPORT_DIRECTIVE;FOLDER;PLUGIN_DIR" # one value args
    "SRCS;MOC_SRCS;UI_FORMS;INCLUDE_DIRECTORIES;TARGET_LIBRARIES;TARGET_WIN_LIBS;TARGET_INSTALL_PATH;TARGET_LINUX_LIBS;RESOURCES;TARGET_SRC_FILES" # multi value args
    ${ARGN}
    )
	message("build_service_component ${targetName}")

	#文件分组
	#file(GLOB COPM_COMMON_HEADS "${CMAKE_SOURCE_DIR}/common/include/*.h")
	#source_group("common" FILES ${COPM_COMMON_HEADS})

	file(GLOB COPM_INTERFACE_COMMON_HEADS 
		"${CMAKE_SOURCE_DIR}/service/interface/*.h")
	source_group("common/interface" FILES ${COPM_INTERFACE_COMMON_HEADS})
	
	file(GLOB COPM_INTERFACE_HEADS 
		"${CMAKE_SOURCE_DIR}/service/interface/iservice/${targetName}/*.h")
	source_group("interface" FILES ${COPM_INTERFACE_HEADS})

	file(GLOB COPM_HEADS 
		"${CMAKE_CURRENT_SOURCE_DIR}/*.h")
	source_group("include" FILES ${COPM_HEADS})

	file(GLOB COPM_SOURCE 
		"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
	source_group("source" FILES ${COPM_SOURCE})

	add_definitions(-DMMR_COMPONENT_EXPORT)

	add_definitions(-DMMR_LOGGER_WRAP)

	# 增加动态链接库
	add_library(${targetName} SHARED
		${MMR_TARGET_SRC_FILES}
		#${COPM_COMMON_HEADS}
		${COPM_INTERFACE_COMMON_HEADS}
		${COPM_INTERFACE_HEADS}
		${COPM_HEADS}
		${COPM_SOURCE})
		
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
	
	#target_compile_definitions(${targetName} PRIVATE EXPORT_LIBRARY)#同时生成静态库

	#使用UTF8编译
	#add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
#[[
	if(WIN32)
		add_custom_command( TARGET ${targetName} PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory
		${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/component
		)
		add_custom_command( TARGET ${targetName} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/${targetName}.dll
		${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/component/${targetName}.dll
		)
	else(UNIX)
		add_custom_command( TARGET ${targetName} PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory
		${CMAKE_BINARY_DIR}/bin/component
		)
		add_custom_command( TARGET ${targetName} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_BINARY_DIR}/bin/${targetName}.so
		${CMAKE_BINARY_DIR}/bin/component/${targetName}.so
		)
	endif()
]]

	foreach(conf DEBUG MINSIZEREL RELWITHDEBINFO RELEASE)
		if(WIN32)
		set_target_properties(${targetName}  PROPERTIES
	  		RUNTIME_OUTPUT_DIRECTORY_${conf} ${CMAKE_BINARY_DIR}/bin/${conf}/component
			LIBRARY_OUTPUT_DIRECTORY_${conf} ${CMAKE_BINARY_DIR}/bin/${conf}/component #会生成.lib文件
			)
		else(UNIX)
			set_target_properties(${targetName} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY_${conf} ${CMAKE_BINARY_DIR}/bin/component
			LIBRARY_OUTPUT_DIRECTORY_${conf} ${CMAKE_BINARY_DIR}/bin/component)
		endif()
	endforeach()

	#set install folder
	foreach(path ${MMR_TARGET_INSTALL_PATH})
		install(TARGETS ${targetName} 
			RUNTIME DESTINATION bin/${path}/component #window下dll文件
			LIBRARY DESTINATION bin/${path}/component #linux下.so文件
			#ARCHIVE DESTINATION lib#静态库,这里用不上
			)
	endforeach()

ENDMACRO()