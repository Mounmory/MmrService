﻿#工程名称
project(testTheadpool)

build_app(${PROJECT_NAME}
	TARGET_LIBRARIES mmrUtil 
	INSTALL_FLAG INSTALL_EXAMPLE #install flag
	TARGET_INSTALL_PATH example
	)
