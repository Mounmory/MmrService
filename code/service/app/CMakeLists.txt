include_directories(${CMAKE_SOURCE_DIR}/common/include)
include_directories(${CMAKE_SOURCE_DIR}/service/core/include)

# 查找所有子文件夹和文件夹
file(GLOB subdirectories RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
# 过滤出子文件夹
foreach(subdirectory ${subdirectories})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${subdirectory})
        list(APPEND subdirectories_list ${subdirectory})
		if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${subdirectory}/CMakeLists.txt")
			add_subdirectory(${subdirectory})
			if(TARGET ${subdirectory})
			 SET_PROPERTY(TARGET ${subdirectory} PROPERTY FOLDER "service/app")
			endif(TARGET ${subdirectory})
		endif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${subdirectory}/CMakeLists.txt")
    endif()
endforeach()

