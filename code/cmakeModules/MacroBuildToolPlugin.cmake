#
# Depends on:
#  cmake_parse_arguments ( >= CMake 2.8.3)
#

MACRO(build_tool_plugin targetName)
	cmake_parse_arguments(
	MMR
    "" # no options
    "NAME;EXPORT_DIRECTIVE;FOLDER;PLUGIN_DIR" # one value args
    "SRCS;MOC_SRCS;UI_FORMS;INCLUDE_DIRECTORIES;TARGET_LIBRARIES;RESOURCES" # multi value args
    ${ARGN}
    )
	message("build_tool_plugin ${targetName}")

	#文件分组
	#file(GLOB PLUGIN_COMMON_HEADS "${CMAKE_SOURCE_DIR}/common/include/*.h")
	#source_group("common" FILES ${PLUGIN_COMMON_HEADS})

	file(GLOB PLUGIN_INTERFACE_HEADS 
		"${CMAKE_SOURCE_DIR}/tools/include/interface/*.hpp"
		"${CMAKE_SOURCE_DIR}/tools/include/interface/*.h")
	source_group("interface" FILES ${PLUGIN_INTERFACE_HEADS})

	file(GLOB PLUGIN_HEADS 
		"${CMAKE_SOURCE_DIR}/tools/include/plugins/*.h"
		"${CMAKE_SOURCE_DIR}/tools/include/plugins/${targetName}/*.h")
	source_group("include" FILES ${PLUGIN_HEADS})

	file(GLOB PLUGIN_SOURCE 
		"${CMAKE_SOURCE_DIR}/tools/source/plugins/${targetName}/*.cpp")
	source_group("source" FILES ${PLUGIN_SOURCE})

	add_definitions(-DMMR_TOOLPLUGIN_EXPORT)

	# 增加动态链接库
	add_library(${targetName} SHARED
		#${PLUGIN_COMMON_HEADS}
		${PLUGIN_INTERFACE_HEADS}
		${PLUGIN_HEADS}
		${PLUGIN_SOURCE})
		
	#target_compile_definitions(${targetName} PRIVATE EXPORT_LIBRARY)#同时生成静态库

	#使用UTF8编译
	#add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
#[[
	if(WIN32)
		add_custom_command( TARGET ${targetName} PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory
		${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/plugins
		)
		add_custom_command( TARGET ${targetName} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/${targetName}.dll
		${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>/plugins/${targetName}.dll
		)
	else(UNIX)
		add_custom_command( TARGET ${targetName} PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory
		${CMAKE_BINARY_DIR}/bin/plugins
		)
		add_custom_command( TARGET ${targetName} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_BINARY_DIR}/bin/${targetName}.so
		${CMAKE_BINARY_DIR}/bin/plugins/${targetName}.so
		)
	endif()
]]

	foreach(conf DEBUG MINSIZEREL RELWITHDEBINFO RELEASE)
		if(WIN32)
		set_target_properties(${targetName}  PROPERTIES
	  		RUNTIME_OUTPUT_DIRECTORY_${conf} ${CMAKE_BINARY_DIR}/bin/${conf}/plugins
			LIBRARY_OUTPUT_DIRECTORY_${conf} ${CMAKE_BINARY_DIR}/bin/${conf}/plugins #会生成.lib文件
			)
		else(UNIX)
			set_target_properties(${targetName} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY_${conf} ${CMAKE_BINARY_DIR}/bin/plugins
			LIBRARY_OUTPUT_DIRECTORY_${conf} ${CMAKE_BINARY_DIR}/bin/plugins)
		endif()
	endforeach()

	#set install folder
	install(TARGETS ${targetName} 
	RUNTIME DESTINATION bin/plugins #window下dll文件
	LIBRARY DESTINATION bin/plugins #linux下.so文件
	#ARCHIVE DESTINATION lib#静态库,这里用不上
	)
ENDMACRO()