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

