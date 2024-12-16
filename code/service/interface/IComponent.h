#ifndef ICOMPONENT_H
#define ICOMPONENT_H
#include <Common_def.h>
#include "util/json.hpp"

class IComponent 
{
public:
	IComponent() = default;
	virtual ~IComponent() = default;

	virtual const char* getName() = 0;//组件名称，与配置文件“Components”下子部件一致
	
	virtual bool initialise(const Json::Value& jsonConfig) = 0;

	virtual bool start() = 0;

	virtual void stop() = 0;
};


#endif