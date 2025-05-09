/**
 * @file InterfaceCommon.h
 * @brief 
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */

#ifndef IINTERFACECOMMON_H
#define IINTERFACECOMMON_H

//定义接口生成GUID函数
#define INTERFACE_GUID_DEFINE() \
public:\
	static const std::string& GetGUID()\
	{\
		static std::string strGUID = mmrUtil::generateGUID();\
		return strGUID;\
	}


#endif

