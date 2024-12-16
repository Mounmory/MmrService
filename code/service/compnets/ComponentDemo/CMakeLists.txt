project(ComponentDemo)

build_service_component(
	ComponentDemo
	TARGET_LIBRARIES serviceFramework mmrUtil hvNet
)
