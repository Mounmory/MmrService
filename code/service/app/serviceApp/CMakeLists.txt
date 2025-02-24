#工程名称
project(serviceApp)
	
build_app(${PROJECT_NAME}
	TARGET_LIBRARIES mmrUtil serviceFramework hvNet
	INSTALL_FLAG ON
	TARGET_INSTALL_PATH service
	)
	
INSTALL(FILES ${THIRD_PART_DIR}/config/serviceApp.json
	DESTINATION bin/service/config)