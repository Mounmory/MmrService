#ifndef IHELLOSERVICE_H
#define IHELLOSERVICE_H

#include "Common_def.h"
#include "ServiceCtrlPolicies.hpp"
#include "iservice/InterfaceCommon.h"
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
		����û�ж�̬�����Ա�������Ļ��࣬����Ҫtypeidʶ������
		��������������Ϊprotected���麯��������ֱ��deleteָ�벻������������������
		������������麯��������Ч��
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