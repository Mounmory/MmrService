#工程名称
project(exampleDealEvent)

build_app(${PROJECT_NAME}
	TARGET_LIBRARIES mmrUtil serviceFramework
	INSTALL_FLAG INSTALL_EXAMPLE #install flag
	)

if(INSTALL_EXAMPLE)
	INSTALL(FILES ${THIRD_PART_DIR}/config/fmServTest.json
	DESTINATION bin/config)
endif()