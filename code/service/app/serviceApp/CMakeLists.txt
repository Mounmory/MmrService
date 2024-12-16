#工程名称
project(serviceApp)
	
build_app(${PROJECT_NAME}
	TARGET_LIBRARIES mmrUtil serviceFramework hvNet
	INSTALL_FLAG ON
	)
	
INSTALL(FILES ${THIRD_PART_DIR}/config/serviceApp.json
	DESTINATION bin/config)