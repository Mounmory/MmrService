
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

#包含hvlib哪些头文件模块
MACRO(include_hv_lib_module)
	cmake_parse_arguments(
	MMR
    "" # no options
    "" # one value args
    "HV_MODULES" # multi value args
    ${ARGN}
    )
	
	foreach(module ${MMR_HV_MODULES})
		include_directories(${CMAKE_SOURCE_DIR}/common/include/libnet/${module})
	endforeach()
	
ENDMACRO()