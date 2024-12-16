# CMake版本约束
cmake_minimum_required(VERSION 3.0)
#工程名称
project(MmrService)
#工程放设置到文件夹中
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(THIRD_PART_DIR "${CMAKE_SOURCE_DIR}/../3rdParty" CACHE PATH "set third part depend directory")

if(NOT THIRD_PART_DIR)
	message(FATAL_ERROR "please set THIRD_PART_DIR value!")
endif()

message(STATUS "THIRD_PART_DIR value is ${THIRD_PART_DIR}")

option(BUILD_QT_PROJECT "build project with Qt" "OFF")

if(WIN32)
	#STRING(REPLACE "-O2" "-Od" CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})#取消优化
	if(MSVC)
		if(MSVC_VERSION LESS 1900) # 判断编译器版本是否低于VS2015
			message(FATAL_ERROR "MSVC version must be at least 14.0 (Visual Studio 2015)!")
		endif()
	endif()

	link_directories(${THIRD_PART_DIR}/lib/win64)
	#option(ENABLE_WINDUMP "Windows MiniDumpWriteDump" "OFF")
	add_definitions(-DOS_MMR_WIN)#添加window系统宏
	message("platform windows")
elseif(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")#c++11 do not has make_unique
	
	if(BUILD_QT_PROJECT)
		set(CMAKE_PREFIX_PATH "/opt/Qt5.12.0/5.12.0/gcc_64/" CACHE PATH "set path for find packet, multiple paths using ';' segmentation")
		set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH} ${CMAKE_PREFIX_PATH}/lib)#设置运行依赖库目录
	endif(BUILD_QT_PROJECT)
	
	set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH} ./ /usr/local/lib)#设置运行依赖库目录,sqlite等三方库安装在/usr/local/lib中
	
	set(CMAKE_INSTALL_PREFIX /mmr)#安装目录
	
	set(CMAKE_BUILD_TYPE Release)#设置构建类型
	
	#link_directories(${THIRD_PART_DIR}/lib/linux)#提前安装到系统
	#link_directories(${THIRD_PART_DIR}/bin/linux)
	add_definitions(-DOS_MMR_LINUX)#添加linux系统宏
	message("platform linux")
else()	
	message(FATAL_ERROR "Unsupported OS type!")
endif()

#包含目录
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)#可执行文件位置
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)#默认存放动态库的位置
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)#默认存放静态库的位置，以及MSVC中动态库的lib文件的位置。

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmakeModules)

include(MacroUtil)
include(MacroBuildSharedLib)
include(MacroBuildToolPlugin)
include(MacroBuildApp)
include(MacroBuildComponent)

SET(COMMON_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/common/include)
SET(COMMON_INCLUDE_UTIL_DIR ${CMAKE_SOURCE_DIR}/common/include/util)
SET(COMMON_INCLUDE_LIBNET_DIR ${CMAKE_SOURCE_DIR}/common/include/libnet)

#protobuf生成源文件路径
SET(PROTO_FILE_PATH ${CMAKE_SOURCE_DIR}/protoBuf)
SET(PROTO_GENERATE_FILE_PATH ${CMAKE_SOURCE_DIR}/protoBuf/generate)

include_directories(${THIRD_PART_DIR}/include)
include_directories(${COMMON_INCLUDE_DIR})
#include_directories(${CMAKE_SOURCE_DIR})
#include_directories(${COMMON_INCLUDE_UTIL_DIR})
#include_directories(${COMMON_INCLUDE_LIBNET_DIR})

#包含当前目录
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#输出可执行文件目录
#SET(TARGET_BIN_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

#ADD_SUBDIRECTORY(app)
ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(service)
#ADD_SUBDIRECTORY(tools)

if(WIN32 AND BUILD_QT_PROJECT)
	option(BUILD_CREO_LIB "build creo lib" "OFF")#是否构建Creo二次开发模块
	if(BUILD_CREO_LIB)
		#设置CreoTookit头文件、库文件路径
		set(CREO_TOOKIT_INCLUDE_PATH "" CACHE PATH "set Protookit include path")
		set(CREO_TOOKIT_LIB_PATH "" CACHE PATH "set Protookit include path")
		#设置安装路径
		set(CREO_TOOKIT_INSTALL_PATH "" CACHE PATH "set Protookit install path")
		
		if(NOT CREO_TOOKIT_INCLUDE_PATH)
			message(FATAL_ERROR "please set protookit include folder to CREO_TOOKIT_INCLUDE_PATH value! you can set BUILD_CREO_LIB OFF to cancel build Creo module.")
		endif()
		if(NOT CREO_TOOKIT_LIB_PATH)
			message(FATAL_ERROR "please set protookit library folder to CREO_TOOKIT_LIB_PATH value!")
		endif()
		
		ADD_SUBDIRECTORY(creo)
	endif(BUILD_CREO_LIB)
endif(WIN32 AND BUILD_QT_PROJECT)

#构建测试用例
option(BUILD_TESTS "build tests" "ON")
if(BUILD_TESTS)
	ADD_SUBDIRECTORY(test)
endif(BUILD_TESTS)