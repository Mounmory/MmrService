set(CMAKE_CREO_DIR ${CMAKE_SOURCE_DIR}/creo)

include_directories(${CMAKE_SOURCE_DIR}/common/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/creoCommon)

include_directories(${CREO_TOOKIT_INCLUDE_PATH})#包含creo头文件

include_directories(${CMAKE_CREO_DIR}/creoCommon)#包含公共头文件

link_directories(${CREO_TOOKIT_LIB_PATH})#Creo库目录

# 开启Qt MOC
set(CMAKE_AUTOMOC ON)
#开启Qt ROC
set(CMAKE_AUTORCC ON)
# 开启Qt UIC
set(CMAKE_AUTOUIC ON)



# 查找所有子文件夹和文件夹
file(GLOB subdirectories RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
# 过滤出子文件夹
foreach(subdirectory ${subdirectories})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${subdirectory})
        list(APPEND subdirectories_list ${subdirectory})
		if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${subdirectory}/CMakeLists.txt")
			add_subdirectory(${subdirectory})
			if(TARGET ${subdirectory})
				#文件夹分组
				SET_PROPERTY(TARGET ${subdirectory} PROPERTY FOLDER "creo")
				#安装路径
				if(CREO_TOOKIT_INSTALL_PATH)
					install(TARGETS ${subdirectory} RUNTIME DESTINATION ${CREO_TOOKIT_INSTALL_PATH}/creo)
				else()
					install(TARGETS ${subdirectory} RUNTIME DESTINATION bin/creo/)
				endif()
			endif(TARGET ${subdirectory})
		endif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${subdirectory}/CMakeLists.txt")
    endif()
endforeach()

if(CREO_TOOKIT_INSTALL_PATH)
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/text/	DESTINATION ${CREO_TOOKIT_INSTALL_PATH}/creo/text)
else()
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/text/	DESTINATION bin/creo/text)
endif()
