#工程名称
project(exampleSerInterface)
	
build_app(${PROJECT_NAME}
	TARGET_LIBRARIES serviceFramework
	INSTALL_FLAG INSTALL_EXAMPLE #install flag
	TARGET_INSTALL_PATH service
	)
