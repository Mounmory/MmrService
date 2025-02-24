#[[
project(AppController)

build_service_component(
	AppController
	TARGET_LIBRARIES serviceFramework mmrUtil
	TARGET_INSTALL_PATH service
)
]]