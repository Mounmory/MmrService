﻿#工程名称
project(testAllocator)

build_app(${PROJECT_NAME}
	TARGET_LIBRARIES mmrUtil
	INSTALL_FLAG INSTALL_EXAMPLE #install flag
	TARGET_INSTALL_PATH example
	)
