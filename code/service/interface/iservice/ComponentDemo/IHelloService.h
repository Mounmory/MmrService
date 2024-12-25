﻿#ifndef IHELLOSERVICE_H
#define IHELLOSERVICE_H

#include "Common_def.h"
#include "ServiceCtrlPolicies.hpp"
//#include "iservice/InterfaceCommon.h"
#include "util/UtilFunc.h"

BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrComp)

class IHelloService
{
	IMPLEMENT_INDEXABLE_CLASS
	IMPLEMENT_GUID_CLASS("4EF84440-A747-49E9-993F-8CEDA873BA65")
public:
	IHelloService() = default;
	virtual ~IHelloService() = default;
	
	/*
		对于没有多态（虚成员函数）的基类，不需要typeid识别类型
		将析构函数定义为protected非虚函数，避免直接delete指针不调用派生类析构函数
		避免子类产生虚函数表，提升效率
	*/
//protected:
//	IHelloService() = default;
//	~IHelloService() = default;//


public:
	virtual void sayHello() = 0;
};

END_NAMESPACE(mmrService)
END_NAMESPACE(mmrComp)

#endif