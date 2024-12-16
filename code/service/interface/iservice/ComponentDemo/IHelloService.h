#ifndef IHELLOSERVICE_H
#define IHELLOSERVICE_H

#include "Common_def.h"
#include "iservice/InterfaceCommon.h"
#include "util/UtilFunc.h"

BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrComp)

class IHelloService
{
	//INTERFACE_GUID_DEFINE();
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