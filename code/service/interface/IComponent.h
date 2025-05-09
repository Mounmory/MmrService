/**
 * @file IComponent.h
 * @brief 各模块组件类公共基类接口
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */

#ifndef ICOMPONENT_H
#define ICOMPONENT_H
#include <common/include/Common_def.h>
#include <common/include/util/json.hpp>

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
