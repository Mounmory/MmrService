#工程名称
project(exampleDealEvent)

build_app(${PROJECT_NAME}
	TARGET_LIBRARIES mmrUtil serviceFramework
	INSTALL_FLAG INSTALL_EXAMPLE #install flag
	TARGET_INSTALL_PATH service
	)

